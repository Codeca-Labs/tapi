/**
 * @author Sean Hobeck
 * @date 2026-01-25
 */
#include "det.h"

/*! @uses bool, true, false. */
#include <stdbool.h>

/*! @uses memcpy. */
#include <string.h>

/*! @uses cs_insn, csh, cs_open. */
#include <capstone/capstone.h>

/*! @uses internal. */
#include "intt.h"

/*! @uses arch_t, get_arch. */
#include "arch.h"

/**
 * @brief is the instruction specified used to end a function?
 *
 * @param insn the instruction to be inspected.
 * @param handle the capstone handle used.
 * @return if the instruction is a function end type.
 */
internal bool
is_end_inst(cs_insn* insn, csh handle) {
    if (cs_insn_group(handle, insn, CS_GRP_RET) || cs_insn_group(handle, insn, CS_GRP_IRET) \
        || cs_insn_group(handle, insn, CS_GRP_BRANCH_RELATIVE)) {
        return true;
    }
    return false;
}

/**
 * @brief is the instruction specified a call at the end of a function to somewhere else?
 *
 * @param insn the instruction to be inspected.
 * @return if the instruction is a tail call.
 */
internal bool
is_tail_call(cs_insn* insn) {
    if (insn->id == X86_INS_JMP || insn->id == ARM_INS_B \
        || insn->id == ARM_INS_BX) {
        return true;
    }
    return false;
}

/**
 * @brief find the size of the function in memory.
 *
 * @param address the address of the function to analyze.
 * @param max_size the max size to search.
 * @return size of the function in memory.
 */
size_t
det_function_size(void* address, size_t max_size) {
    /* open capstone to reading at the specified address with the native architecture. */
    csh handle;
    size_t size = 0;
    arch_t architecture = get_arch();
    cs_open(architecture.arch, architecture.mode, &handle);
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

    /* get the bytes at the address, and create an iterator. */
    const uint8_t* bytes = (unsigned char*) address;
    uint64_t iter = (uint64_t) address;
    size_t code_size = max_size;
    cs_insn* insn = cs_malloc(handle);
    if (!insn) {
        cs_close(&handle);
        fprintf(stderr, "cs_malloc failed; could not allocate memory for instructions.");
        return 0;
    }

    /* start iterating. */
    while (cs_disasm_iter(handle, &bytes, &code_size, &iter, insn)) {
        size += insn->size;

        /* are any of these function end instructions? */
        if (is_end_inst(insn, handle)) {
            if (!is_tail_call(insn)) {
                break;
            }
        }

        /* sanity bounds. */
        if (size >= max_size) {
            fprintf(stderr, "warning; hit max search size of %zu bytes\n", max_size);
            break;
        }
    }

    /* free and close. */
    cs_free(insn, 1);
    cs_close(&handle);
    return size;
}

/**
 * @brief search through an x86 instructions opt. info to find if it is a call to an
 *  immediate/ relative address within the memory; for x86|x86_64 only.
 *
 * @param target the target address to search within the instruction for.
 * @param call the call information structure to be used.
 * @param insn the instruction to be searched.
 * @param mode the mode of the architecture (x86 vs. x86_64).
 * @return ref. to intt.h for enum.
 */
internal e_intt_result_t
find_call_bx86(const void* target, det_call_t* call, const cs_insn* insn, const cs_mode mode) {
    /* we look for the target address. */
    uint64_t address = 0u;
    cs_x86* ops = &insn->detail->x86;
    for (size_t i = 0; i < ops->op_count; i++) {
        /* iterate until we find the immediate value used in the call. */
        cs_x86_op* op = &ops->operands[i];
        if (op->type == X86_OP_IMM) {
            if (mode == CS_MODE_64) {
                /* relative call. */
                if (op->imm <= UINT32_MAX && ops->disp == 0u) {
                    address = insn->address + insn->size + op->imm;
                    call->is_rel = true;
                    call->orig_off = (int32_t) op->imm;
                }
                else if (op->size == 8u) {
                    /* absolute call, funny enough, we can still modify these. */
                    address = op->imm;
                    call->is_rel = true;
                }
                /* we currently don't support rip-rel calls. */
            }
            else {
                /* relative call. */
                if (op->size == 4u) {
                    address = insn->address + insn->size + op->imm;
                    call->is_rel = true;
                }
                else {
                    /* absolute call... */
                    address = op->imm;
                    call->is_rel = false;
                }
            }

            /* check if it is our target. */
            bool is_target = address == (uint64_t) target;
            if (is_target) {
                call->call = (void*) insn->address;
                call->dest = (void*) target;
                call->size = insn->size;

                /* copy and return. */
                memcpy(call->bytes, insn->bytes, insn->size < 32u ? insn->size : 32u);
                return E_INTT_RESULT_SUCCESS;
            }
        }
    }
    return E_INTT_RESULT_FAILURE;
}

/**
 * @brief search through an arm instructions opt. info to find if it is a call to an immediate/
 *  relative address within memory; for aarch32|thumb only.
 *
 * @param target the target address to search within the instruction for.
 * @param call the call information structure to be used.
 * @param insn the instruction to be searched.
 * @return ref. to intt.h for enum.
 */
internal e_intt_result_t
find_call_barm32(const void* target, det_call_t* call, const cs_insn* insn) {
    bool branch_link = insn->id == ARM_INS_BL;
    bool blx = insn->id == ARM_INS_BLX;
    if (branch_link || blx) {
        cs_arm* arm = &insn->detail->arm;

        /* iterate through every operand. */
        for (int i = 0; i < arm->op_count; i++) {
            cs_arm_op* op = &arm->operands[i];
            if (op->type == ARM_OP_IMM) {
                uint64_t target_addr = (uint64_t)op->imm;
                call->is_rel = true;

                /* is it a branch with link insn? */
                if (branch_link) {
                    /* calculate original offset. */
                    if (call->is_thumb) {
                        /* thumb bl/blx; 2-instruction sequence
                         *  bl encodes a 21-bit signed offset. */
                        call->orig_off = (int32_t)(op->imm - (insn->address + 4));
                    } else {
                        /* arm bl; encodes 24-bit signed offset. */
                        call->orig_off = (int32_t)(op->imm - (insn->address + 8));
                    }
                }

                /* is this the target address? */
                if ((void*)target_addr == target) {
                    call->call = (void*)insn->address;
                    call->dest = (void*)target_addr;
                    call->size = insn->size;

                    /* is this a blx interworking call? */
                    if (blx) {
                        call->orig_off = (int32_t)(op->imm - (insn->address + (call->is_thumb ?
                            4u : 8u)));
                    }

                    /* copy bytes and return. */
                    memcpy(call->bytes, (void*)insn->address,
                           insn->size < 32u ? insn->size : 32u);
                    return E_INTT_RESULT_SUCCESS;
                }
            }
        }
    }
    return E_INTT_RESULT_FAILURE;
}

/**
 * @brief search through an arm64 instructions opt. info to find if it is a call to an immediate/
 *  relative address within memory; for aarch64 only.
 *
 * @param target the target address to search within the instruction for.
 * @param call the call info structure to be used.
 * @param insn the instruction to be searched.
 * @return ref. to intt.h for enum.
 */
internal e_intt_result_t
find_call_barm64(const void* target, det_call_t* call, const cs_insn* insn) {
    /* is this a branch with link insn? */
    if (insn->id == ARM64_INS_BL) {
        cs_arm64* arm64 = &insn->detail->arm64;

        /* iterate. */
        for (int i = 0; i < arm64->op_count; i++) {
            cs_arm64_op* op = &arm64->operands[i];

            /* are we dealing with the immediate value? */
            if (op->type == ARM64_OP_IMM) {
                uint64_t target_addr = op->imm;

                // ARM64 BL is always relative
                // Encodes a 26-bit signed offset
                call->is_rel = true;

                /* calculate offset, target = pc + (imm << 2). */
                call->orig_off = (int32_t)(target_addr - insn->address);

                /* is this the target address? */
                if ((void*)target_addr == target) {
                    call->call = (void*)insn->address;
                    call->dest = (void*)target_addr;
                    call->size = insn->size;

                    /* copy bytes and return. */
                    memcpy(call->bytes, (void*)insn->address,
                           insn->size < 32u ? insn->size : 32u);
                    return E_INTT_RESULT_SUCCESS;
                }
            }
        }
    }
    /* blr for indirect calls via register. */
    if (insn->id == ARM64_INS_BLR) {
        /* this is unsupported at the moment. */
        return E_INTT_RESULT_FAILURE;
    }
    return E_INTT_RESULT_FAILURE;
}

/**
 * @brief ...
 *
 * @param source the source...
 * @param target the destination..
 * @return ...
 */
det_call_t*
det_call_target(void* source, const void* target) {
    /* open memory for the compile-time architecture. */
    csh handle;
    arch_t architecture = get_arch();
    cs_open(architecture.arch, architecture.mode, &handle);
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

    /* allocate the pointer. */
    det_call_t* call = calloc(1, sizeof *call);
    if (call == 0x0) {
        fprintf(stderr, "calloc failed; could not allocate memory for det_call_t*.\n");
        return 0x0;
    }

    /* set up address and size for the iteration. */
    const uint8_t* bytes = source;
    uint64_t address = (uint64_t) source;
    size_t size = det_function_size(source, 0x1000);
    cs_insn* insn = cs_malloc(handle);
    if (!insn) {
        cs_close(&handle);
        fprintf(stderr, "cs_malloc failed; could not allocate memory for instructions.\n");
        return 0x0;
    }

    /* iterating. */
    while (cs_disasm_iter(handle, &bytes, &size, &address, insn)) {
        /* is this a call instruction? */
        if (cs_insn_group(handle, insn, CS_GRP_CALL)) {
            /* get the target address */
            switch (architecture.arch) {
                case (CS_ARCH_X86): {
                    /* check if we can find the target in the insn. */
                    if e_intt_passed(find_call_bx86(target, call, insn, architecture.mode)) {
                        cs_free(insn, 1u);
                        cs_close(&handle);
                        return call;
                    }
                }
                /* same for both arm32 and arm64. */
                case (CS_ARCH_ARM): {
                    /* this is really aarch32 + thumb. */
                    if e_intt_passed(find_call_barm32(target, call, insn)) {
                        cs_free(insn, 1u);
                        cs_close(&handle);
                        return call;
                    }
                }
                case (CS_ARCH_ARM64): {
                    if e_intt_passed(find_call_barm64(target, call, insn)) {
                        cs_free(insn, 1u);
                        cs_close(&handle);
                        return call;
                    }
                }
                default: break;
            }
        }
    }

    /* free & return. */
    cs_free(insn, 1u);
    cs_close(&handle);
    return 0x0;
}