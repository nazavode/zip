
// Set ASAN default options
// https://github.com/google/sanitizers/wiki/AddressSanitizerFlags
extern "C" const char *__asan_default_options() {
    return "detect_stack_use_after_return=true:"
           "check_initialization_order=true:"
           "detect_invalid_pointer_pairs=1:"
           // FIXME: gtest on macOS arm64 seems to be angry at
           // REGISTER_TYPED_TEST_SUITE_P, needs to be investigated.
           "detect_container_overflow=0";
}
