/**
 * @author Sean Hobeck
 * @date 2026-02-02
 */
#include <tapi/sink.h>

/*! @uses calloc, free. */
#include <stdlib.h>

/*! @uses errno. */
#include <errno.h>

/**
 * @brief make a sink structure to be written to.
 *
 * @return a pointer to an allocated sink.
 */
tapi_sink_t*
tapi_make_sink() {
	/* allocate and return. */
	tapi_sink_t* sink = calloc(1u, sizeof *sink);
	return sink;
}

/**
 * @brief set a pre-allocated buffer to the destination of the sink.
 *
 * @param sink the sink to set the destination to.
 * @param buffer the pre-allocated buffer to be used in the sink.
 * @param length the size of the pre-allocated buffer.
 */
void
tapi_sink_setdbf(tapi_sink_t* sink, char* buffer, size_t length) {
	/* set the type and then the buffer. */
	sink->buffer.data = buffer;
	sink->buffer.length = length;
	sink->type = E_TAPI_SINK_TYPE_BUF;

	/* set the stream and then we are done. */
	tapi_stream_t stream = fmemopen(sink->buffer.data, sink->buffer.length, "w");
	if (stream == 0x0) {
		fprintf(stderr, "fmemopen failed; could not open memory stream, was it allocated? errno: %d\n", errno);
		return;
	}
	sink->stream = stream;
}

/**
 * @brief set a pre-allocated stream to the destination of the sink.
 *
 * @param sink the sink to set the destination to.
 * @param stream the pre-allocated or opened stream.
 */
void
tapi_sink_setdfp(tapi_sink_t* sink, tapi_stream_t stream) {
	/* set the type and then the stream, and we are done. */
	sink->stream = stream;
	sink->type = E_TAPI_SINK_TYPE_STR;
}

/**
 * @brief free a sink.
 *
 * @param sink the sink to be freed.
 */
void
tapi_destroy_sink(tapi_sink_t* sink) {
	/* if it is a memory stream, we do not free the buffer. */
	free(sink);
};