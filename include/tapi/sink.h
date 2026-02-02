/**
 * @author Sean Hobeck
 * @date 2026-02-02
 */
#ifndef TAPI_SINK_H
#define TAPI_SINK_H

/* we have to define this to use fileno as stream->_fileno is not universal across different libc. */
#define _POSIX_C_SOURCE 200809L

/*! @uses FILE. */
#include <stdio.h>

/**
 * a data structure ...
 */
typedef struct {
    /* either a pointer to a sink buffer, or file. */
    FILE* stream;
    char* orig_buf;
    size_t bytes;
} tapi_sink_t;

/**
 * enum differentiating ...
 */
typedef enum {
    E_SINK_TY_BUFFER = 0x1, /* a sink_buf_t structure. */
    E_SINK_TY_FILE = 0x2, /* a sink_file_t structure. */
} e_tapi_sink_ty_t;

tapi_sink_t*
tapi_make_sink(e_tapi_sink_ty_t type);

void
tapi_sink_write(const char* buffer);

void
tapi_sink_flush(void);

void
tapi_sink_close(void);

void
tapi_destroy_sink(tapi_sink_t* sink);
#endif /* TAPI_SINK_H */