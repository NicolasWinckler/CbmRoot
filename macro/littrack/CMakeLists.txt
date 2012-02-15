GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test_electron.sh)
add_test(littrack_electron ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_electron.sh)
SET_TESTS_PROPERTIES(littrack_electron PROPERTIES TIMEOUT "900")
SET_TESTS_PROPERTIES(littrack_electron PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test_muon.sh)
add_test(littrack_muon ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_muon.sh)
SET_TESTS_PROPERTIES(littrack_muon PROPERTIES TIMEOUT "900")
SET_TESTS_PROPERTIES(littrack_muon PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

# Additional nightly tests
If($ENV{ctest_model} MATCHES Experimental)
GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_electron.sh)
add_test(littrack_electron ${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_electron.sh)
SET_TESTS_PROPERTIES(littrack_electron PROPERTIES TIMEOUT "20000")
SET_TESTS_PROPERTIES(littrack_electron PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_muon.sh)
add_test(littrack_muon ${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_muon.sh)
SET_TESTS_PROPERTIES(littrack_muon PROPERTIES TIMEOUT "20000")
SET_TESTS_PROPERTIES(littrack_muon PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
EndIf($ENV{ctest_model}  MATCHES Experimental)
