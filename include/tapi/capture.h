/**
 * @author Sean Hobeck
 * @date 2026-02-02
 */
#ifndef TAPI_CAPTURE_H
#define TAPI_CAPTURE_H

/*! @uses size_t, FILE, sink_t, ... */
#include <tapi/sink.h>

/**
 * a data structure for ...
 */
typedef struct {
    int dst_fd, piperd, pipewr; /* file descriptor for dest., and pipe read & write. */
    tapi_sink_t* sink; /* sink to write data to. */
    tapi_stream_t stream; /* the stream to capture. */
} tapi_capture_t;

/**
 * @brief start capturing data written to a specific stream and re-route to a specified sink.
 *
 * @param sink the sink to capture the re-routed data (see @ref<tapi/sink.h>["tapi_sink_t"].
 * @param stream the stream to re-route data from.
 * @return a pointer to an allocated capture structure.
 */
tapi_capture_t*
tapi_capture_make(tapi_sink_t* sink, tapi_stream_t stream);

/**
 * @brief stop capturing data from a stream.
 *
 * @param capture the capture to be ended.
 */
void
tapi_capture_end(tapi_capture_t* capture);

/**
 * @brief free a capture structure.
 *
 * @param capture the capture to be freed.
 */
void
tapi_capture_destroy(tapi_capture_t* capture);
#endif /* TAPI_CAPTURE_H */