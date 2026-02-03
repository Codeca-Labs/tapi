#include <tapi/tapi.h>
#include <tapi/capture.h>
#include <tapi/sink.h>

#include <string.h>

void function(float percentage) {
    int result1 = 0x10;
    double result2 = (float)result1 * percentage;
    printf("%.2f\n", result2);
}

e_tapi_test_result_t test_function() {
    /* arrange. */
    tapi_sink_t* sink = tapi_sink_make();
    char buffer[4096u];
    tapi_sink_setdbf(sink, buffer, sizeof(buffer));
    tapi_capture_t* capture = tapi_capture_make(sink, stdout);

    /* act. */
    function(0.1f);

    /* assert. */
    tapi_capture_end(capture);
    tapi_assert(strcmp(sink->buffer.data, "1.60"));
    return E_TAPI_TEST_RESULT_PASSED;
}

int main() {
    tapi_test_t* test = tapi_make_test("test_function", test_function);
    tapi_test_t* tests[] = { test };
    tapi_setup_tests(tests, 1u);
    tapi_run_tests();
    return 0;
}