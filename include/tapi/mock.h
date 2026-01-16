/**
 * @author Sean Hobeck
 * @date 2026-01-15
 */
#ifndef TAPI_MOCK_H
#define TAPI_MOCK_H

/*! @uses size_t. */
#include <stddef.h>

/**
 * a data structure ...
 */
typedef struct {
    void* orig, *mocked, *stub; /* original, mocked, and stub/ trampoline functions. */
    void* call; /* address of the call in the original function. */
    size_t size; /* size of the patch. */
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
tapi_mock_t*
tapi_mock_create(void* orig, void* target, void* mocked);

/**
 * @brief apply the mocks patch in memory; write stub to route to
 *  the given mocked function pointer.
 *
 * @param mock the mock to be applied.
 */
void
tapi_mock_apply(tapi_mock_t* mock);

/**
 * @brief restore the contents of a function, and free the mock.
 *
 * @param mock the mock structure to be freed and restored.
 */
void
tapi_mock_destroy(tapi_mock_t* mock);
#endif /* TAPI_MOCK_H */