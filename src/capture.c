/**
 * @author Sean Hobeck
 * @date 2026-01-28
 */
#include <tapi/capture.h>

/** todo; format this using a membuf for sink_buf_t, and keep this for the sink_file_t (very old
 * code).

    // allocate the structure.
    tapi_output_capture_t* capture = calloc(1, sizeof *capture);

    // open the streams.
    int fds[2];
    if (pipe(fds) == -1) {
        fprintf(stderr, "pipe failed; could not create pipe for stdout and stderr.\n");
        exit(EXIT_FAILURE);
    }
    capture->pipe_rd = fds[0];
    capture->pipe_wr = fds[1];

    // flush stdout.
    fflush(stream);
    capture->saved_fd = dup(fileno(stream));
    if (capture->saved_fd == -1) {
        fprintf(stderr, "pipe failed; could not create pipe for stdout and stderr.\n");
        exit(EXIT_FAILURE);
    }
    if (dup2(capture->pipe_wr, fileno(stream)) == -1) {
        fprintf(stderr, "dup2 failed; could not copy over pipe_wr fd to stdout.\n");
        exit(EXIT_FAILURE);
    }

    // stdout is now the pipe write end, close the copy
    close(capture->pipe_wr);
    capture->pipe_wr = -1;
    setvbuf(stream, 0x0, _IONBF, 0);
    return capture;


===================================================================================


    // flush both stdout stream.
    fflush(stream);
    if (dup2(capture->saved_fd, fileno(stream)) == -1) {
        fprintf(stderr, "dup2 failed; could not copy saved fd over to stdout.\n");
        exit(EXIT_FAILURE);
    }
    close(capture->saved_fd);
    capture->saved_fd = -1;

    // read all data from pipe.
    char buf[4096];
    size_t capacity = 0, len = 0;
    capture->data = 0x0;
    capture->size = 0;

    // close writer side so pipe will hit EOF
    ssize_t n;
    while ((n = read(capture->pipe_rd, buf, 4096)) > 0) {
        // if len is greater we realloc the string.
        if (len + n > capacity) {
            capacity = capacity ? capacity * 2 : 8192;

            // calculate size, plus null terminator.
            if (capacity < len + n + 1) capacity = len + n + 1;
            capture->data = realloc(capture->data, capacity);
            if (!capture->data) {
                fprintf(stderr, "realloc failed; could not allocate memory for stdout capture.\n");
                close(capture->pipe_rd);
                exit(EXIT_FAILURE);
            }
        }
        memcpy(capture->data + len, buf, n);
        len += n;
    }
    close(capture->pipe_rd);
    capture->pipe_rd = -1;
    if (n == -1) {
        fprintf(stderr, "read failed; could not read from stdout pipe.\n");
        exit(EXIT_FAILURE);
    }

    // terminate the string/data.
    if (capture->data) capture->data[len] = '\0';
    capture->size = len;

 */