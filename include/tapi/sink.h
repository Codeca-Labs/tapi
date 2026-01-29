/**
 * @author Sean Hobeck
 * @date 2026-01-28
 */
#ifndef TAPI_SINK_H
#define TAPI_SINK_H

#include <stddef.h>

#include <stdio.h>

typedef struct {
    char* buffer;
    size_t size;
} sink_buf_t;

typedef struct {
    FILE* file;
} sink_file_t;

typedef union {
    /* either a pointer to a sink buffer, or file. */
    sink_buf_t* buf;
    sink_file_t* file;
} sink_t;

typedef enum {
    E_SINK_TY_BUFFER = 0x1, /* a sink_buf_t structure. */
    E_SINK_TY_FILE = 0x2, /* a sink_file_t structure. */
} e_sink_ty_t;

sink_t*
tapi_make_sink(e_sink_ty_t type);

void
tapi_sink_write(const char* buffer);

void
tapi_sink_flush(void);

void
tapi_sink_close(void);

void
tapi_destroy_sink(sink_t* sink);

#endif /* TAPI_SINK_H */