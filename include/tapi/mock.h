/**
 * @author Sean Hobeck
 * @date 2026-01-25
 */
#ifndef TAPI_MOCK_H
#define TAPI_MOCK_H

/*! @uses TAPI_EXPORT. */
#include <tapi/tapi.h>

/*! @uses size_t. */
#include <stddef.h>

/**
 * a data structure for mocking the result of a call within a tested function. this is mainly
 *  used for isolation from other functions, and to primarily test your functions logic,
 *  arithmetic, and functionality. mocks/ stub functions can be as simple as returning a
 *  value, or as complex as you need them to be; complexity is completely left up to the user.
 */
typedef struct {
    void* orig, *mocked, *target; /* original, mocked, and target functions. */
    void* call; /* address of the call in the original function. */
    size_t size, fun_size; /* size of the patch & function. */
    unsigned char orig_bytes[32u], mocked_bytes[32u];
    /* first 32 bytes of original and mocked functions */
} tapi_mock_t;

/**
 * @brief mock the first call occurrence to a target with a call
 *  to a mocked function instead.
 *
 * @param orig the original function to search for target in.
 * @param target the target address to be replaced.
 * @param mocked the function to replace the target call with.
 * @return an allocated mock structure with all data, ready to be applied.
 */
TAPI_EXPORT tapi_mock_t*
tapi_mock_create(void* orig, void* target, void* mocked);

/**
 * @brief apply the mocks patch in memory; write stub to route to
 *  the given mocked function pointer.
 *
 * @param mock the mock to be applied.
 */
TAPI_EXPORT void
tapi_mock_apply(tapi_mock_t* mock);

/**
 * @brief restore the contents of a function and free the mock.
 *
 * @param mock the mock structure to be freed and restored.
 */
TAPI_EXPORT void
tapi_mock_restore(tapi_mock_t* mock);

/* create a simple mock to return a given value. */
#define tapi_mock_return(func_name, return_type, return_value) \
    return_type func_name() { return return_value; }

/* create a mock to return a given integer. */
#define tapi_mock_return_int(func_name, return_value) \
    tapi_mock_return(func_name, int, return_value)

/* create a mock to return a given pointer. */
#define tapi_mock_return_ptr(func_name, return_value) \
    tapi_mock_return(func_name, void*, return_value)

/* create a mock to return null. */
#define tapi_mock_return_null(func_name) \
    tapi_mock_return(func_name, void*, null)

/* create a mock to return a string/ string literal. */
#define tapi_mock_return_strl(func_name, return_value) \
    tapi_mock_return(func_name, char*, return_value)
#endif /* TAPI_MOCK_H */