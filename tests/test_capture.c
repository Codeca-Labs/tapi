/**
 * @author Sean Hobeck
 * @date 2026-02-23
 */
#include <tapi/tapi.h>

/*! @uses tapi_capture_t, etc... */
#include <tapi/capture.h>

/*! @uses tapi_sink_t, etc... */
#include <tapi/sink.h>

/*! @uses printf. */
#include <stdio.h>

/*! @uses strcmp, strlen. */
#include <string.h>

/*! @uses calloc, free. */
#include <stdlib.h>

e_tapi_test_result_t test_capture_empty() {
    /* arrange + act. */
    tapi_quick_capture(stdout, 16u);
    tapi_quick_end_capture();

    /* assert. */
    tapi_assert(strlen(sink->buffer.data) == 0);
    tapi_quick_destroy_capture();
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_capture_single_line() {
    /* arrange. */
    tapi_quick_capture(stdout, 16u);

    /* act. */
    printf("hello world");
    tapi_quick_end_capture();

    /* assert. */
    tapi_assert(strcmp(sink->buffer.data, "hello world") == 0);
    tapi_quick_destroy_capture();
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_capture_multiline() {
    /* arrange. */
    tapi_quick_capture(stdout, 32u);

    /* act. */
    printf("line 1\n");
    printf("line 2\n");
    printf("line 3\n");
    tapi_quick_end_capture();

    /* assert. */
    tapi_assert(strcmp(sink->buffer.data, "line 1\nline 2\nline 3\n") == 0);
    tapi_quick_destroy_capture();
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_capture_stderr() {
    /* arrange. */
    tapi_quick_capture(stderr, 32u);

    /* act. */
    fprintf(stderr, "error message");
    tapi_quick_end_capture();

    /* assert. */
    tapi_assert(strcmp(sink->buffer.data, "error message") == 0);
    tapi_quick_destroy_capture();
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_capture_both_streams() {
    /* arrange. */
    tapi_sink_t* sink1 = tapi_make_sink();
    tapi_sink_t* sink2 = tapi_make_sink();
    tapi_sink_setdbf(sink1, 256);
    tapi_sink_setdbf(sink2, 256);
    tapi_capture_t* cap1 = tapi_make_capture(sink1, stdout);
    tapi_capture_t* cap2 = tapi_make_capture(sink2, stderr);

    /* act. */
    printf("stdout output");
    fprintf(stderr, "stderr output");
    tapi_end_capture(cap1);
    tapi_end_capture(cap2);

    /* assert. */
    tapi_assert(strcmp(sink1->buffer.data, "stdout output") == 0);
    tapi_assert(strcmp(sink2->buffer.data, "stderr output") == 0);
    tapi_destroy_capture(cap1);
    tapi_destroy_capture(cap2);
    tapi_destroy_sink(sink1);
    tapi_destroy_sink(sink2);
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_capture_large_output() {
    /* arrange. */
    tapi_quick_capture(stdout, 1024u);

    for (int i = 0; i < 100; i++) {
        printf("0123456789");
    }
    tapi_quick_end_capture();

    /* assert. */
    tapi_assert(strlen(sink->buffer.data) == 1000);
    tapi_quick_destroy_capture();
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_capture_overflow() {
    /* arrange. */
    tapi_sink_t* small_sink = tapi_make_sink();
    tapi_sink_setdbf(small_sink, 16u);

    /* act. */
    tapi_capture_t* cap = tapi_make_capture(small_sink, stdout);
    printf("this is a very long string that exceeds the buffer");
    tapi_end_capture(cap);

    /* assert. */
    tapi_assert(strlen(small_sink->buffer.data) <= 16);
    tapi_destroy_capture(cap);
    tapi_destroy_sink(small_sink);
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_capture_formats() {
    /* arrange. */
    tapi_quick_capture(stdout, 128u);

    /* act. */
    printf("int: %d, float: %.2f, str: %s, hex: 0x%x",
           42, 3.14f, "hello", 0xDEADBEEF);
    tapi_quick_end_capture();

    /* assert. */
    tapi_assert(strstr(sink->buffer.data, "int: 42") != 0x0);
    tapi_assert(strstr(sink->buffer.data, "float: 3.14") != 0x0);
    tapi_assert(strstr(sink->buffer.data, "str: hello") != 0x0);
    tapi_assert(strstr(sink->buffer.data, "hex: 0xdeadbeef") != 0x0);
    tapi_quick_destroy_capture();
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_capture_special_chars() {
    /* arrange. */
    tapi_quick_capture(stdout, 64u);

    /* act. */
    printf("tab:\t newline:\n null: quote:\"");
    tapi_quick_end_capture();

    /* assert. */
    tapi_assert(strstr(sink->buffer.data, "tab:\t") != 0x0);
    tapi_assert(strstr(sink->buffer.data, "newline:\n") != 0x0);
    tapi_quick_destroy_capture();
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_capture_whitespace() {
    /* arrange. */
    tapi_quick_capture(stdout, 32u);

    /* act. */
    printf("   \n\t\n   ");
    tapi_quick_end_capture();

    /* assert. */
    tapi_assert(strcmp(sink->buffer.data, "   \n\t\n   ") == 0x0);
    tapi_quick_destroy_capture();
    return E_TAPI_TEST_RESULT_PASSED;
}

int main() {
    tapi_test_t* test_empty = tapi_make_test("test_capture_empty", test_capture_empty);
    tapi_test_t* test_single_line = tapi_make_test("test_single_line", test_capture_single_line);
    tapi_test_t* test_multiline = tapi_make_test("test_multiline", test_capture_multiline);
    tapi_test_t* test_stderr = tapi_make_test("test_stderr", test_capture_stderr);
    tapi_test_t* test_both_streams = tapi_make_test("test_both_streams", test_capture_both_streams);
    tapi_test_t* test_large_output = tapi_make_test("test_large_output", test_capture_large_output);
    tapi_test_t* test_overflow = tapi_make_test("test_overflow", test_capture_overflow);
    tapi_test_t* test_formats = tapi_make_test("test_formats", test_capture_formats);
    tapi_test_t* test_special_chars = tapi_make_test("test_special_chars", test_capture_special_chars);
    tapi_test_t* test_whitespace = tapi_make_test("test_whitespace", test_capture_whitespace);
    tapi_test_t* tests[] = { \
        test_empty, test_single_line, test_multiline, \
        test_stderr, test_both_streams, test_large_output, \
        test_overflow, test_formats, test_special_chars, \
        test_whitespace
    };
    tapi_setup_tests(tests, 10u);
    tapi_run_tests();
    return 0;
}