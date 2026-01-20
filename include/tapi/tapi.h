/**
 * @author Sean Hobeck
 * @date 2026-01-19
 */
#ifndef TAPI_H
#define TAPI_H

/*! @uses dyna_t. */
#include <tapi/dyna.h>

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


#endif /* TAPI_H */