/**
 * @author Sean Hobeck
 * @date 2026-01-26
 */
#ifndef TAPI_H
#define TAPI_H

/*! @uses dyna_t. */
#include <tapi/dyna.h>

/* for functions that are exported by tapi. */
#if (defined(__GNUC__) || defined(__IBMC__))
#define TAPI_EXPORT __attribute__((visibility("default")))
#else
/* if we are using msvc toolchain (winapi).*/
#if (defined(_MSC_VER))
#define TAPI_EXPORT __declspec(dllexport)
#else
#define TAPI_EXPORT
#endif
#endif

/** enum for different types of results from a test. */
typedef enum {
    E_TAPI_TEST_RESULT_PASSED = 0x1, /* test passed. */
    E_TAPI_TEST_RESULT_FAILED, /* test failed. */
    E_TAPI_TEST_RESULT_SKIPPED, /* test skipped. */
} e_tapi_test_result_t;

/* a function pointer type for test functions. */
typedef e_tapi_test_result_t (*tapi_test_func_t)(void);

/* a function pointer type for setup and teardown functions. */
typedef void (*tapi_gen_func_t)(void);

/**
 * a data structure for tests within the tapi. this contains a name, description, setup, and
 *  teardown functions, as well as other mocking and output capture information.
 */
typedef struct {
    char* name; /* name of the test. */
    tapi_test_func_t function; /* pointer to the test function. */
    tapi_gen_func_t setup, teardown; /* pointer to the setup and teardown functions. */
    dyna_t* mocks; /* dynamic array of mock pointers. */
    e_tapi_test_result_t result; /* result of calling the test. */
} tapi_test_t;

/**
 * @brief set up many tests to be run in concession.
 *
 * @param tests the array of tests to be set up for a test file.
 * @param count the number of tests to be set up.
 */
TAPI_EXPORT void
tapi_setup_tests(tapi_test_t** tests, size_t count);

/**
 * @brief add a test to your testing suite.
 *
 * @param test the test to be added.
 */
TAPI_EXPORT void
tapi_add_test(tapi_test_t* test);

/** @brief run all the tests set up in concession. */
TAPI_EXPORT void
tapi_run_tests(void);

/**
 * @brief make a new test given minimal information.
 *
 * @param name the name of the test.
 * @param function the test function to be used.
 */
TAPI_EXPORT tapi_test_t*
tapi_make_test(const char* name, tapi_test_func_t function);

/**
 * @brief add a mock to a certain test.
 *
 * @param test the test to be altered.
 * @param tested the tested function to search through.
 * @param target the target address to redirect to mock.
 * @param mocked the mocked result to be redirected to.
 */
TAPI_EXPORT void
tapi_add_mock_to_test(tapi_test_t* test, void* tested, void* target, void* mocked);

/**
 * @brief free and destroy a list of tests after they have been ran.
 *
 * @param tests the tests to be freed (this also frees all of its elements, including mocks).
 * @param length the number of tests to be freed.
 */
TAPI_EXPORT void
tapi_destroy_tests(tapi_test_t** tests, size_t length);

/* assert on a condition and fail a given test if not met. */
#define tapi_assert(cond) if (!(cond)) return E_TAPI_TEST_RESULT_FAILED;
#endif /* TAPI_H */