file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/events_electron)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/results_electron)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/events_muon)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/results_muon)

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/global_sim_new.C)

add_test(global_sim_electron ${CBMROOT_BINARY_DIR}/macro/littrack/global_sim_new.sh 1 \"electron\")
SET_TESTS_PROPERTIES(global_sim_electron PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(global_sim_electron PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

add_test(global_sim_muon ${CBMROOT_BINARY_DIR}/macro/littrack/global_sim_new.sh 1 \"muon\")
SET_TESTS_PROPERTIES(global_sim_muon PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(global_sim_muon PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/global_reco_new.C)

add_test(global_reco_electron ${CBMROOT_BINARY_DIR}/macro/littrack/global_reco_new.sh 1 \"all\")
SET_TESTS_PROPERTIES(global_reco_electron PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(global_reco_electron PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

#add_test(global_sim_muon ${CBMROOT_BINARY_DIR}/macro/littrack/global_sim_new.sh 1 \"muon\")
#SET_TESTS_PROPERTIES(global_sim_muon PROPERTIES TIMEOUT "300")
#SET_TESTS_PROPERTIES(global_sim_muon PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;#All ok")



#GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test_electron.sh)
#ADD_TEST(littrack_electron "${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_electron.sh")
#SET_TESTS_PROPERTIES(littrack_electron PROPERTIES TIMEOUT "1800")
#SET_TESTS_PROPERTIES(littrack_electron PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test_muon.sh)
#ADD_TEST(littrack_muon ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_muon.sh)
#SET_TESTS_PROPERTIES(littrack_muon PROPERTIES TIMEOUT "1800")
#SET_TESTS_PROPERTIES(littrack_muon PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test_mvd.sh)
#
#ADD_TEST(littrack_mvd_delta "${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_mvd.sh" "delta")
#SET_TESTS_PROPERTIES(littrack_mvd_delta PROPERTIES TIMEOUT "1800")
#SET_TESTS_PROPERTIES(littrack_mvd_delta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#ADD_TEST(littrack_mvd_nodelta "${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_mvd.sh" "nodelta")
#SET_TESTS_PROPERTIES(littrack_mvd_nodelta PROPERTIES TIMEOUT "1800")
#SET_TESTS_PROPERTIES(littrack_mvd_nodelta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
## Additional nightly tests
#If(BUILD_LITTRACK_EXPERIMENTAL)
#GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_reco.sh)
#
#ADD_TEST(littrack_electron_auau_25gev_centr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_electron_auau_25gev_centr" "electron" "auau" "25gev" "centr")
#SET_TESTS_PROPERTIES(littrack_electron_auau_25gev_centr PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_electron_auau_25gev_centr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#ADD_TEST(littrack_electron_auau_25gev_mbias "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_electron_auau_25gev_mbias" "electron" "auau" "25gev" "mbias")
#SET_TESTS_PROPERTIES(littrack_electron_auau_25gev_mbias PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_electron_auau_25gev_mbias PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#ADD_TEST(littrack_muon_auau_25gev_centr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_auau_25gev_centr" "muon" "auau" "25gev" "centr")
#SET_TESTS_PROPERTIES(littrack_muon_auau_25gev_centr PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_muon_auau_25gev_centr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#ADD_TEST(littrack_muon_auau_25gev_mbias "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_auau_25gev_mbias" "muon" "auau" "25gev" "mbias")
#SET_TESTS_PROPERTIES(littrack_muon_auau_25gev_mbias PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_muon_auau_25gev_mbias PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#ADD_TEST(littrack_muon_radial_auau_25gev_centr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_radial_auau_25gev_centr" "muon_radial" "auau" "25gev" "centr")
#SET_TESTS_PROPERTIES(littrack_muon_radial_auau_25gev_centr PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_muon_radial_auau_25gev_centr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#ADD_TEST(littrack_muon_radial_auau_25gev_mbias "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_radial_auau_25gev_mbias" "muon_radial" "auau" "25gev" "mbias")
#SET_TESTS_PROPERTIES(littrack_muon_radial_auau_25gev_mbias PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_muon_radial_auau_25gev_mbias PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh)
#
#ADD_TEST(littrack_mvd_auau_25gev_centr_nodelta "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_centr_nodelta" "auau" "25gev" "centr" "nodelta")
#SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_centr_nodelta PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_centr_nodelta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#ADD_TEST(littrack_mvd_auau_25gev_mbias_nodelta "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_mbias_nodelta" "auau" "25gev" "mbias" "nodelta")
#SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_mbias_nodelta PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_mbias_nodelta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#ADD_TEST(littrack_mvd_auau_25gev_centr_delta "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_centr_delta" "auau" "25gev" "centr" "delta")
#SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_centr_delta PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_centr_delta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#ADD_TEST(littrack_mvd_auau_25gev_mbias_delta "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_mbias_delta" "auau" "25gev" "mbias" "delta")
#SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_mbias_delta PROPERTIES TIMEOUT "36000")
#SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_mbias_delta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_field.sh)
#ADD_TEST(littrack_field "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_field.sh")
#SET_TESTS_PROPERTIES(littrack_field PROPERTIES TIMEOUT "3600")
#SET_TESTS_PROPERTIES(littrack_field PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_fieldapr.sh)
#ADD_TEST(littrack_fieldapr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_fieldapr.sh")
#SET_TESTS_PROPERTIES(littrack_fieldapr PROPERTIES TIMEOUT "3600")
#SET_TESTS_PROPERTIES(littrack_fieldapr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#EndIf(BUILD_LITTRACK_EXPERIMENTAL)
