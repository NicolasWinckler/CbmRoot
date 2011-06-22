GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_electron.sh)
add_test(littrack_electron ${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_electron.sh)
SET_TESTS_PROPERTIES(littrack_electron PROPERTIES TIMEOUT "600")
SET_TESTS_PROPERTIES(littrack_electron PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

