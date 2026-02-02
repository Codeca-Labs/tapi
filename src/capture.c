/**
 * @author Sean Hobeck
 * @date 2026-02-02
 */
#include <tapi/capture.h>

/*! @uses calloc, close. */
#include <stdlib.h>

/*! @uses pipe, dup, dup2. */
#include <unistd.h>
/*! @uses errno. */
#include <errno.h>

/**
 * @brief start capturing data written to a specific stream and re-route to a specified sink.
 *
 * @param sink the sink to capture the re-routed data (see @ref<tapi/sink.h>["tapi_sink_t"].
 * @param stream the stream to re-route data from.
 * @return a pointer to an allocated capture structure.
 */
tapi_capture_t*
tapi_capture_make(tapi_sink_t* sink, tapi_stream_t stream) {
    /* allocate the structure. */
    tapi_capture_t* capture = calloc(1u, sizeof *capture);
    capture->sink = sink;
    capture->stream = stream;

    // open the streams.
    int fds[2];
    if (pipe(fds) == -1) {
        fprintf(stderr, "pipe failed; could not create pipe for stdout and stderr.\n");
        exit(EXIT_FAILURE);
    }
    capture->piperd = fds[0];
    capture->pipewr = fds[1];

    /* flush the stream. */
    fflush(stream);
    capture->dst_fd = dup(fileno(stream));
    if (capture->dst_fd == -1) {
        fprintf(stderr, "pipe failed; could not create pipe for stdout and stderr.\n");
        exit(EXIT_FAILURE);
    }
    if (dup2(capture->pipewr, fileno(stream)) == -1) {
        fprintf(stderr, "dup2 failed; could not copy over pipe_wr fd to stdout.\n");
        exit(EXIT_FAILURE);
    }

    /* pass the pipe write end, close the copy. */
    close(capture->pipewr);
    capture->pipewr = -1;
    setvbuf(stream, 0x0, _IONBF, 0);
    return capture;
}

/**
 * @brief stop capturing data from a stream.
 *
 * @param capture the capture to be ended.
 */
void
tapi_capture_end(tapi_capture_t* capture) {
    /* flush the stream which we capture from. */
    fflush(capture->stream);
    if (dup2(capture->dst_fd, fileno(capture->stream)) == -1) {
        fprintf(stderr, "dup2 failed; could not copy saved fd over to stdout. errno: %d\n", errno);
        return;
    }
    close(capture->dst_fd);
    capture->dst_fd = -1;

    /* read all data from pipe, and write it to sink. */
    char buf[4096u];

    /* close writer side so pipe will hit EOF. */
    ssize_t n;
    while ((n = read(capture->piperd, buf, 4096u)) > 0u) {
        /* todo; all we do is simply write the data to the stream, if it fails then we stop. */
        fwrite(buf, n, 1u, capture->sink->stream);
    }

    /* close our read pipe since we are done. */
    close(capture->piperd);
    capture->piperd = -1;
    if (n == -1) {
        fprintf(stderr, "read failed; could not read from stdout pipe. errno: %d\n", errno);
    }
}

/**
 * @brief free a capture structure.
 *
 * @param capture the capture to be freed.
 */
void
tapi_capture_destroy(tapi_capture_t* capture) {
    free(capture);
}