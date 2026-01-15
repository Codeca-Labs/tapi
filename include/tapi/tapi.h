/**
 * @author Sean Hobeck
 * @date 2026-01-14
 */
#ifndef TAPI_H
#define TAPI_H

/*! @uses dyna_t. */
#include <dyna.h>

/** enum for different types of results from a test. */
typedef enum {
    E_TAPI_TEST_RESULT_PASSED = 0x1, /* test passed. */
    E_TAPI_TEST_RESULT_FAILED, /* test failed. */
    E_TAPI_TEST_RESULT_SKIPPED, /* test skipped. */
} e_tapi_test_result_t;

/* a function pointer type for test functions. */
typedef e_tapi_test_result_t (*tapi_test_func_t)(void);

/* a general function pointer type for setup and teardown functions. */
typedef void (*tapi_gen_func_t)(void);

/**
 * a data structure for tests within the tapi. this contains a name, description, setup, and
 *  teardown functions, as well as other mocking and output capture information.
 */
typedef struct {
    const char* name, *description; /* name and description of the test. */
    tapi_test_func_t function; /* pointer to the test function. */
    tapi_gen_func_t setup, teardown; /* pointer to the setup and teardown functions. */
    dyna_t mocks; /* dynamic array of mock pointers. */
} tapi_test_t;

/** enum for different supported architectures. */
typedef enum {
    E_TAPI_ARCH_NATIVE = 0x0, /* let tapi use your native architecture. */
    E_TAPI_ARCH_X86 = 0x1, /* 32-bit systems. */
    E_TAPI_ARCH_X64, /* 64-bit systems. */
    E_TAPI_ARCH_ARM, /* arm 32-bit systems. */
    E_TAPI_ARCH_ARM64, /* arm 64-bit systems. */
    E_TAPI_ARCH_RISCV, /* risc-v systems. */
} e_tapi_arch_t;

/**
 * @brief set up tapi to do runtime patches and testing with a log file as well as specific
 *  architecture.
 *
 * @param arch the architecture that is to be used in this test or 0x0 for native.
 * @param log_path the path to a .log file, or 0x0 if none.
 */
void
tapi_setup(e_tapi_arch_t arch, const char* log_path);

/** @return the architecture specified in tapi_setup. */
e_tapi_arch_t
tapi_get_arch(void);
#endif /* TAPI_H */