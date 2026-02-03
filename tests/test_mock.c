#include <tapi/tapi.h>
#include <tapi/mock.h>

int target_function(int x) {
    return ~x + 1u;
}

int function() {
    int result1 = 0x10;
    int result2 = -0x1;
    int result3 = result1 + result2;
    int result4 = target_function(result3);
    return result4 + 1u;
}

tapi_mock_return(tested_function_target, int, 0u);

e_tapi_test_result_t test_function() {
    /* act & assert. */
    int result = function();
    tapi_assert(result == 1u);
    return E_TAPI_TEST_RESULT_PASSED;
};

int main(int argc, char** argv) {
    tapi_test_t* test = tapi_make_test("test_function", test_function);
    tapi_add_mock_to_test(test, function, target_function, tested_function_target);
    tapi_test_t* tests[] = { test };
    tapi_setup_tests(tests, 1u);
    tapi_run_tests();
    return 0;
}