/**
 * @author Sean Hobeck
 * @date 2026-01-21
 */
#include "patch.h"

/*! @uses fprintf, stderr. */
#include <stdio.h>

/*! @uses guard_create, guard_close. */
#include "guard.h"

/**
 * @brief binary patch a call to a target using
 *
 * @param call the call structure info representing the call to be patched.
 * @param new_target the new target address to set the new call to.
 * @return 1 if successful, and 0 o.w.
 */
int32_t
patch_call_target(det_call_t* call, void* new_target) {
    /* create a protect guard for an entire page. */
    guard_t* guard = guard_create(call->call, call->size);
    if (call->is_rel) {
        int32_t new_rel_addr =  (int32_t)((uintptr_t)new_target - (uintptr_t)(call->call +
            call->size));

        /* verify new offset. */
        if (new_rel_addr > INT32_MAX || new_rel_addr < INT32_MIN) {
            fprintf(stderr, "new target address is too far for rel. call!");
            return 0u;
        }

        /* patch for a specific backend. */
    } else {
        fprintf(stderr, "cannot patch a non-relative call!\n");
        return 0u;
    }

    /* close the guard and flush insn. cache. */
    guard_close(guard);
    __builtin___clear_cache(call->call, call->call + call->size);
    return 1u;
}