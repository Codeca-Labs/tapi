/**
 * @author Sean Hobeck
 * @date 2026-01-21
 */
#ifndef TAPI_STUB_H
#define TAPI_STUB_H

/* stub function definition. */
typedef void* (*tapi_stub_t)(void*, ...);

/**
 * @brief ...
 *
 * @param src
 * @param dest
 * @return
 */
void*
tapi_create_stub(void* src, tapi_stub_t dest);

/* macro for making a stub/ mock result. */
#define tapi_make_stub(name, result) \
    void* name(void* original, ...) { \
        return result; \
    }
#endif /* TAPI_STUB_H */