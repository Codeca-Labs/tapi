/**
 * @author Sean Hobeck
 * @date 2026-01-16
 */
#include "det.h"

/*! @uses bool, true, false. */
#include <stdbool.h>

/*! @uses cs_insn, csh, cs_open. */
#include <capstone/capstone.h>

/*! @uses internal. */
#include "guard.h"

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