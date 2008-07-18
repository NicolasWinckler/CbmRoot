add_test(run ${ROOTSYS}/bin/root -b -l  ${CBMROOT_SOURCE_DIR}/macro/run/run.C)
SET_TESTS_PROPERTIES (run PROPERTIES TIMEOUT "150")
SET_TESTS_PROPERTIES (run PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

add_test(run_sim ${ROOTSYS}/bin/root -b -l  ${CBMROOT_SOURCE_DIR}/macro/run/run_sim.C)
set_tests_properties(run_sim PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES (run_sim PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

add_test(run_reco ${ROOTSYS}/bin/root -b -l  ${CBMROOT_SOURCE_DIR}/macro/run/run_reco.C)
set_tests_properties(run_reco PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES (run_reco PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")
