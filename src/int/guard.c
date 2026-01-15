/**
 * @author Sean Hobeck
 * @date 2026-01-14
 */
#include "guard.h"

/*! @uses calloc. */
#include <stdlib.h>

/*! @uses uintptr_t. */
#include <stdint.h>

/*! @uses sysconf, _SC_PAGE_SIZE. */
#include <unistd.h>

/*! @uses mprotect. */
#include <sys/mman.h>

/*! @uses fprintf, stderr. */
#include <stdio.h>

/** @return page size on the given architecture, winapi and posix. */
internal size_t
get_page_size() {
#ifndef _WIN32
    long value = sysconf(_SC_PAGESIZE);
    return value > 0 ? (size_t)value : 4096;
#else
    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    return (size_t) si.dwPageSize;
#endif
}

/**
 * @brief align a page down and cast to an address for usage.
 *
 * @param page the page to be aligned down.
 * @return an address aligned down to the nearest page.
 */
internal void*
page_align_down(void* page, size_t size) {
    return (void*)((uintptr_t)page & ~(size - 1));
}

/**
 * @brief round up a page size if required.
 *
 * @param address the address to be rounded up.
 * @param page_size the architectures page size.
 * @return an address rounded up to the nearest page.
 */
internal size_t
page_round_up(uintptr_t address, size_t page_size) {
    return (address + (page_size - 1)) & ~(page_size - 1);
}

/**
 * @brief create a write-protect guard for an address in memory.
 *
 * @param address the address to be given write protection in memory.
 * @param length the length of bytes to be protected.
 * @return an allocated pguard_t structure.
 */
guard_t*
guard_create(void* address, size_t length) {
    /* allocate. */
    guard_t* guard = calloc(1, sizeof *guard);

    /* we find the relative page bounds. */
    guard->address = page_align_down(address, get_page_size());
    uintptr_t delta = (uintptr_t)guard->address - (uintptr_t)address;
    guard->length = page_round_up(length + delta, get_page_size());

    /* allow write protection on those pages. */
#ifndef _WIN32
    if (mprotect(guard->address, guard->length, PROT_READ | PROT_WRITE | PROT_EXEC) != 0x0) {
        fprintf(stderr, "mprotect failed; could not allocate memory for pguard.");
#else
    if (VirtualProtect(guard->address, guard->length, PAGE_EXECUTE_READWRITE, &guard->flags) !=
        0x0) {
        fprintf(stderr, "VirtualProtect failed; could not allocate memory for pguard.");
#endif
        return 0x0;
    }

    return guard;
}

/**
 * @brief close/ restore the write-protect guard.
 *
 * @param guard the guard to be closed/ restored.
 */
void
guard_close(guard_t* guard) {
    /* remove write protection on the pages. */
#ifndef _WIN32
    if (mprotect(guard->address, guard->length, PROT_READ | PROT_EXEC) != 0x0)
        fprintf(stderr, "mprotect failed; could not close pguard.");
#else
    DWORD tmp;
    if (VirtualProtect(guard->address, guard->length, guard->flags, &tmp) != 0x0)
        fprintf(stderr, "VirtualProtect failed; could not close pguard.");
#endif
}