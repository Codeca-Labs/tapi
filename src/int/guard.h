/**
 * @author Sean Hobeck
 * @date 2026-01-14
 */
#ifndef PGUARD_H
#define PGUARD_H

/*! @uses size_t. */
#include <stddef.h>

/** a data structure for a memory protection guard. */
typedef struct {
    void* address; /* address in memory to be written to. */
    size_t length, flags; /* length and old flags for memory (winapi). */
} guard_t;

/**
 * @brief create a write-protect guard for an address in memory.
 *
 * @param address the address to be given write protection in memory.
 * @param length the length of bytes to be protected.
 * @return a allocated pguard_t structure.
 */
guard_t*
guard_create(void* address, size_t length);

/**
 * @brief close/ restore the write-protect guard.
 *
 * @param guard the guard to be closed/ restored.
 */
void
guard_close(guard_t* guard);
#endif /* PGUARD_H */