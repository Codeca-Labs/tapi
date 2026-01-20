/**
 * @author Sean Hobeck
 * @date 2026-01-20
 */
#include "det.h"

/*! @uses bool, true, false. */
#include <stdbool.h>

/*! @uses cs_insn, csh, cs_open. */
#include <capstone/capstone.h>

/*! @uses internal. */
#include "guard.h"

/*! @uses arch_t, get_arch. */
#include <string.h>

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
 * @brief search through an instructions optional information to find if it is a call to an
 *  immediate/ relative address within the memory; for x86|x86_64 architectures only.
 *
 * @param target the target address to search all calls for.
 * @param call the call information structure to be used.
 * @param insn the instruction to be searched.
 * @param mode the mode of the architecture (x86 vs. x86_64).
 * @return 1 if successful in finding the call to target, o.w. 0.
 */
internal size_t
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
                    call->orig_off = op->imm;
                }
                else if (op->size == 8u) {
                    /* absolute call... */
                    address = op->imm;
                    call->is_rel = false;
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
                memcpy(call->bytes, insn->bytes, insn->size < 32u ? insn->size : 32u);
                return 1;
            }
        }
    }
    return 0;
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

    /* set up address and size for the iteration. */
    const uint8_t* bytes = source;
    uint64_t address = (uint64_t) source;
    size_t size = det_function_size(source, 0x1000);
    cs_insn* insn = cs_malloc(handle);
    if (!insn) {
        cs_close(&handle);
        fprintf(stderr, "cs_malloc failed; could not allocate memory for instructions.");
        return 0x0;
    }

    /* iterating. */
    while (cs_disasm_iter(handle, &bytes, &size, &address, insn)) {
        /* is this a call instruction. */
        if (cs_insn_group(handle, insn, CS_GRP_CALL)) {
            /* get the target address */
            switch (architecture.arch) {
                case (CS_ARCH_X86): {
                    /* check if we can find the target in the insn. */
                    if (find_call_bx86(target, call, insn, architecture.mode) == 1u) {
                        return call;
                    }
                }
                default: {
                    fprintf(stderr, "unknown architecture; corrupted?");
                    return 0x0;
                }
            }
        }
    }
    return 0x0;
}