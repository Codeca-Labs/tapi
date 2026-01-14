/**
 * @author Sean Hobeck
 * @date 2026-01-13
 */
#include <stdio.h>

/** @brief library entry point. */
__attribute__((constructor)) void entry() {};

/** @brief library exit point. */
__attribute__((destructor)) void exit() {};