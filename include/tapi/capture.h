/**
 * @author Sean Hobeck
 * @date 2026-01-28
 */
#ifndef TAPI_CAPTURE_H
#define TAPI_CAPTURE_H

/*! @uses size_t, FILE, sink_t, ... */
#include <tapi/sink.h>

typedef struct {
    size_t dest_fd, p_read, p_write; /* file descriptor for dest., and pipe read & write. */
    sink_t* sink;
} tapi_capture_t;

tapi_capture_t*
tapi_capture_start(sink_t* sink);

void
tapi_capture_end(tapi_capture_t* capture);

#endif /* TAPI_CAPTURE_H */