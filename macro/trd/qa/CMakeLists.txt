GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/qa/QAsim_monolithic_trd.C)
add_test(sim_monolithic_trd ${CBMROOT_BINARY_DIR}/macro/trd/qa/QAsim_monolithic_trd.sh)
SET_TESTS_PROPERTIES(sim_monolithic_trd PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(sim_monolithic_trd PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/qa/QAsim_quasi_monolithic_trd.C)
add_test(sim_quasi_monolithic_trd ${CBMROOT_BINARY_DIR}/macro/trd/qa/QAsim_quasi_monolithic_trd.sh)
SET_TESTS_PROPERTIES(sim_quasi_monolithic_trd PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(sim_quasi_monolithic_trd PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/qa/QAsim_rectangular_segmented_trd.C)
add_test(sim_rectangular_segmented_trd ${CBMROOT_BINARY_DIR}/macro/trd/qa/QAsim_rectangular_segmented_trd.sh)
SET_TESTS_PROPERTIES(sim_rectangular_segmented_trd PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(sim_rectangular_segmented_trd PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/qa/QAsim_squared_segmented_trd.C)
add_test(sim_squared_segmented_trd ${CBMROOT_BINARY_DIR}/macro/trd/qa/QAsim_squared_segmented_trd.sh)
SET_TESTS_PROPERTIES(sim_squared_segmented_trd PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(sim_squared_segmented_trd PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
