/**
 * @author Sean Hobeck
 * @date 2026-01-15
 */
#ifndef DET_H
#define DET_H

/*! @uses size_t. */
#include <stddef.h>

/**
 * @brief find the size of the function in memory.
 *
 * @param address the address of the function to analyze.
 * @param max_size the max size to search.
 * @return size of the function in memory.
 */
size_t
det_function_size(void* address, size_t max_size);
#endif /* DET_H */