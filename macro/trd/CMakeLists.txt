GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/trd_sim.C)
add_test(a_trd_sim ${CBMROOT_BINARY_DIR}/macro/trd/trd_sim.sh)
SET_TESTS_PROPERTIES(a_trd_sim PROPERTIES TIMEOUT "600")
SET_TESTS_PROPERTIES(a_trd_sim PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/trd_digi1.C)
add_test(a_trd_digi1 ${CBMROOT_BINARY_DIR}/macro/trd/trd_digi1.sh)
SET_TESTS_PROPERTIES(a_trd_digi1 PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(a_trd_digi1 PROPERTIES DEPENDS a_trd_sim)
SET_TESTS_PROPERTIES(a_trd_digi1 PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/trd_digi2.C)
add_test(a_trd_digi2 ${CBMROOT_BINARY_DIR}/macro/trd/trd_digi2.sh)
SET_TESTS_PROPERTIES(a_trd_digi2 PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(a_trd_digi2 PROPERTIES DEPENDS a_trd_sim)
SET_TESTS_PROPERTIES(a_trd_digi2 PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

# run benchmark only on selected nodes
# GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/trd_sim_benchmark.C)
# add_test(a_trd_sim_benchmark ${CBMROOT_BINARY_DIR}/macro/trd/trd_sim_benchmark.sh)
# SET_TESTS_PROPERTIES(a_trd_sim_benchmark PROPERTIES TIMEOUT "3500")
# SET_TESTS_PROPERTIES(a_trd_sim_benchmark PROPERTIES DEPENDS a_trd_digi2)
# SET_TESTS_PROPERTIES(a_trd_sim_benchmark PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

