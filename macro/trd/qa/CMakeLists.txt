GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/qa/QAsim_root_trd.C)
add_test(sim_root_trd ${CBMROOT_BINARY_DIR}/macro/trd/qa/QAsim_root_trd.sh)
SET_TESTS_PROPERTIES(sim_root_trd PROPERTIES TIMEOUT "700")
SET_TESTS_PROPERTIES(sim_root_trd PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
