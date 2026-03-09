/**
 * @author Sean Hobeck
 * @date 2026-02-23
 */
#include <tapi/tapi.h>

/*! @uses tapi_mock_return. */
#include <tapi/mock.h>

/* region for all of the test call targets and assembly specific fuctions. */
#pragma region test call targets
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

#if defined(__x86_64__) || defined(__i386__)
int asm_target_x86(int x) {
    int result;
    __asm__ volatile(
        "movl %1, %0\n"
        "negl %0\n"
        : "=r"(result)
        : "r"(x)
        : "cc"
    );
    return result;
}

int asm_caller_x86() {
    int val = 0x42;
    /* direct call to test relative call detection and patching. */
    int result = asm_target_x86(val);
    return result + 1;
}
#elif defined(__aarch64__)
int asm_target_aarch64(int x) {
    int result;
    __asm__ volatile(
        "neg %w0, %w1\n"
        : "=r"(result)
        : "r"(x)
    );
    return result;
}

int asm_caller_aarch64() {
    int val = 0x42;
    int result = asm_target_aarch64(val);
    return result + 1;
}
#elif defined(__arm__)
int asm_target_arm32(int x) {
    int result;
    __asm__ volatile(
        "rsb %0, %1, #0\n"
        : "=r"(result)
        : "r"(x)
    );
    return result;
}

int asm_caller_arm32() {
    int val = 0x42;
    int result = asm_target_arm32(val);
    return result + 1;
}

__attribute__((target("thumb")))
int asm_target_thumb(int x) {
    int result;
    __asm__ volatile(
        "rsb %0, %1, #0\n"
        : "=r"(result)
        : "r"(x)
    );
    return result;
}

__attribute__((target("thumb")))
int asm_caller_thumb() {
    int val = 0x42;
    int result = asm_target_thumb(val);
    return result + 1;
}
#endif

int nested_target(int x) {
    return x * 2;
}

int nested_middle(int x) {
    return nested_target(x) + 5;
}

int nested_caller() {
    return nested_middle(10);
}

int conditional_target(int x) {
    return x + 100;
}

int conditional_caller(int use_target) {
    if (use_target) {
        return conditional_target(50);
    }
    return 0;
}
#pragma endregion

/* region for all of the setups and mock return values. */
#pragma region mock return values
tapi_mock_return(tested_function_target, int, 0u);
#if defined(__x86_64__) || defined(__i386__)
tapi_mock_return(mock_asm_target_x86, int, 0x100);
#elif defined(__aarch64__)
tapi_mock_return(mock_asm_target_aarch64, int, 0x100);
#elif defined(__arm__)
tapi_mock_return(mock_asm_target_arm32, int, 0x100);
tapi_mock_return(mock_asm_target_thumb, int, 0x100);
#endif
tapi_mock_return(mock_nested_target, int, 42);
tapi_mock_return(mock_conditional_target, int, 999);
#pragma endregion

/* region for all of the tests. */
#pragma region tests
e_tapi_test_result_t test_basic_mock() {
    /* act & assert. */
    int result = function();
    tapi_assert(result == 1u);
    return E_TAPI_TEST_RESULT_PASSED;
}

#if defined(__x86_64__) || defined(__i386__)
e_tapi_test_result_t test_asm_x86_mock() {
    /* act & assert. */
    int result = asm_caller_x86();
    tapi_assert(result == 0x101);
    return E_TAPI_TEST_RESULT_PASSED;
}
#elif defined(__aarch64__)
e_tapi_test_result_t test_asm_aarch64_mock() {
    /* act & assert. */
    int result = asm_caller_aarch64();
    tapi_assert(result == 0x101);
    return E_TAPI_TEST_RESULT_PASSED;
}
#elif defined(__arm__)
e_tapi_test_result_t test_asm_arm32_mock() {
    /* act & assert. */
    int result = asm_caller_arm32();
    tapi_assert(result == 0x101);
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_asm_thumb_mock() {
    /* act & assert. */
    int result = asm_caller_thumb();
    tapi_assert(result == 0x101);
    return E_TAPI_TEST_RESULT_PASSED;
}
#endif

e_tapi_test_result_t test_nested_mock() {
    /* act & assert. */
    int result = nested_caller();
    tapi_assert(result == 47);
    return E_TAPI_TEST_RESULT_PASSED;
}

e_tapi_test_result_t test_conditional_mock() {
    /* act & assert. */
    int result = conditional_caller(1);
    tapi_assert(result == 999);
    return E_TAPI_TEST_RESULT_PASSED;
}
#pragma endregion

int main() {
    /* basic test. */
    tapi_test_t* test1 = tapi_test_make("test_basic_mock", test_basic_mock);
    tapi_test_add_mock(test1, function, target_function, tested_function_target);

    /* architecture-specific tests. */
#if defined(__x86_64__) || defined(__i386__)
    tapi_test_t* test2 = tapi_test_make("test_asm_x86_mock", test_asm_x86_mock);
    tapi_test_add_mock(test2, asm_caller_x86, asm_target_x86, mock_asm_target_x86);
#elif defined(__aarch64__)
    tapi_test_t* test2 = tapi_test_make("test_asm_aarch64_mock", test_asm_aarch64_mock);
    tapi_test_add_mock(test2, asm_caller_aarch64, asm_target_aarch64, mock_asm_target_aarch64);
#elif defined(__arm__)
    tapi_test_t* test2 = tapi_test_make("test_asm_arm32_mock", test_asm_arm32_mock);
    tapi_test_add_mock(test2, asm_caller_arm32, asm_target_arm32, mock_asm_target_arm32);
    tapi_test_t* test4 = tapi_test_make("test_asm_thumb_mock", test_asm_thumb_mock);
    tapi_test_add_mock(test4, asm_caller_thumb, asm_target_thumb, mock_asm_target_thumb);
#endif
    tapi_test_t* test_nested = tapi_test_make("test_nested_mock", test_nested_mock);
    tapi_test_add_mock(test_nested, nested_middle, nested_target, mock_nested_target);
    tapi_test_t* test_cond = tapi_test_make("test_conditional_mock", test_conditional_mock);
    tapi_test_add_mock(test_cond, conditional_caller, conditional_target, mock_conditional_target);

    /* setup test array. */
#if defined(__arm__)
    tapi_test_t* tests[] = { test1, test2, test4, test_nested, test_cond };
    tapi_test_setup(tests, 5u);
#else
    tapi_test_t* tests[] = { test1, test2, test_nested, test_cond };
    tapi_test_setup(tests, 4u);
#endif
    tapi_test_run();
    return 0;
}