/**
 * @author Sean Hobeck
 * @date 2026-01-14
 */
#include <tapi/tapi.h>

/** @brief library entry point. */
__attribute__((constructor))
void entry() {
    /* setup with the native compiled architecture. */
    tapi_setup(E_TAPI_ARCH_NATIVE, 0x0);
}

/** @brief library exit point. */
__attribute__((destructor))
void exit() {}