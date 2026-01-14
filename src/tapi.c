/**
 * @author Sean Hobeck
 * @date 2026-01-13
 */
#include <tapi/tapi.h>

/*! @uses strdup. */
#define _GNU_SOURCE 0
#include <string.h>

/* global variables used. */
static e_tapi_arch_t architecture;
static const char* logger_path = 0x0;

/**
 * @brief set up tapi to do runtime patches and testing with a log file as well as specific
 *  architecture.
 *
 * @param arch the architecture that is to be used in this test or 0x0 for native.
 * @param log_path the path to a .log file, or 0x0 if none.
 */
void
tapi_setup(e_tapi_arch_t arch, const char* log_path) {
    /* if architecture is 'native' we use pre-processor. */
    if (arch != E_TAPI_ARCH_NATIVE)
        architecture = arch;
    else {
#ifdef __amd64__
        architecture = E_TAPI_ARCH_AMD64;
#endif
#ifdef __i386__
        architecture = E_TAPI_ARCH_X86;
#endif
#ifdef __aarch64__
        architecture = E_TAPI_ARCH_ARM64;
#endif
#ifdef __arm__
        architecture = E_TAPI_ARCH_ARM;
#endif
#ifdef __riscv__
        architecture = E_TAPI_ARCH_RISCV;
#endif
    }

    /* set the log path. */
    if (log_path != 0x0)
        logger_path = strdup(log_path);
    else logger_path = 0x0;
}