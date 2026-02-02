/**
 * @author Sean Hobeck
 * @date 2026-02-02
 */
#include <tapi/capture.h>

/*! @uses calloc, close. */
#include <stdlib.h>

/*! @uses pipe, dup, dup2. */
#include <unistd.h>

tapi_capture_t*
tapi_capture_start(tapi_sink_t* sink, FILE* stream) {
    /* allocate the structure. */
    tapi_capture_t* capture = calloc(1u, sizeof *capture);
    capture->sink = sink;

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
