/**
 * @author Sean Hobeck
 * @date 2026-01-19
 */
#ifndef STUB_H
#define STUB_H

/* stub function destination... */
typedef void* (*stub_function_t)(void*, ...);

/**
 * @brief ...
 *
 * @param src
 * @param dest
 * @return
 */
void*
create_stub(void* src, stub_function_t dest);
#endif /* STUB_H */