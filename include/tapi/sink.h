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

/* for clarity. */
typedef FILE* tapi_stream_t;

/**
 * a data structure ...
 */
typedef struct {
    struct {
        char* data; size_t length; /* a pointer to data and length. */
    } buffer; /* a pre-allocated buffer. */
    enum {
        E_TAPI_SINK_TYPE_NONE = 0x0, /* no dest. set. */
        E_TAPI_SINK_TYPE_BUF = 0x1, /* a pre-allocated buffer. */
        E_TAPI_SINK_TYPE_STR = 0x2, /* a pre-allocated stream. */
    } type; /* the type of sink. */
    /* a pointer to a stream. */
    tapi_stream_t stream;
} tapi_sink_t;

/**
 * @brief make a sink structure to be written to.
 *
 * @return a pointer to an allocated sink.
 */
tapi_sink_t*
tapi_sink_make();

/**
 * @brief set a pre-allocated buffer to the destination of the sink.
 *
 * @param sink the sink to set the destination to.
 * @param buffer the pre-allocated buffer to be used in the sink.
 * @param length the size of the pre-allocated buffer.
 */
void
tapi_sink_setdbf(tapi_sink_t* sink, char* buffer, size_t length);

/**
 * @brief set a pre-allocated stream to the destination of the sink.
 *
 * @param sink the sink to set the destination to.
 * @param stream the pre-allocated or opened stream.
 */
void
tapi_sink_setdfp(tapi_sink_t* sink, tapi_stream_t stream);

/**
 * @brief free a sink.
 *
 * @param sink the sink to be freed.
 */
void
tapi_sink_destroy(tapi_sink_t* sink);
#endif /* TAPI_SINK_H */