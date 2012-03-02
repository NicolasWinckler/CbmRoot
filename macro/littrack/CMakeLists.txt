GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test_electron.sh)
ADD_TEST(littrack_electron "${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_electron.sh par")
SET_TESTS_PROPERTIES(littrack_electron PROPERTIES TIMEOUT "900")
SET_TESTS_PROPERTIES(littrack_electron PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test_muon.sh)
ADD_TEST(littrack_muon ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_muon.sh)
SET_TESTS_PROPERTIES(littrack_muon PROPERTIES TIMEOUT "900")
SET_TESTS_PROPERTIES(littrack_muon PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

# Additional nightly tests
If($ENV{ctest_model} MATCHES Experimental)
GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_reco.sh)

ADD_TEST(littrack_electron_auau_25gev_centr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_electron_auau_25gev_centr" "electron" "auau" "25gev" "centr")
SET_TESTS_PROPERTIES(littrack_electron_auau_25gev_centr PROPERTIES TIMEOUT "300000")
SET_TESTS_PROPERTIES(littrack_electron_auau_25gev_centr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

ADD_TEST(littrack_electron_auau_25gev_mbias "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_electron_auau_25gev_mbias" "electron" "auau" "25gev" "mbias")
SET_TESTS_PROPERTIES(littrack_electron_auau_25gev_mbias PROPERTIES TIMEOUT "300000")
SET_TESTS_PROPERTIES(littrack_electron_auau_25gev_mbias PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

ADD_TEST(littrack_muon_auau_25gev_centr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_auau_25gev_centr" "muon" "auau" "25gev" "centr")
SET_TESTS_PROPERTIES(littrack_muon_auau_25gev_centr PROPERTIES TIMEOUT "300000")
SET_TESTS_PROPERTIES(littrack_muon_auau_25gev_centr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

ADD_TEST(littrack_muon_auau_25gev_mbias "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_auau_25gev_mbias" "muon" "auau" "25gev" "mbias")
SET_TESTS_PROPERTIES(littrack_muon_auau_25gev_mbias PROPERTIES TIMEOUT "300000")
SET_TESTS_PROPERTIES(littrack_muon_auau_25gev_mbias PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh)

ADD_TEST(littrack_mvd_auau_25gev_centr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_centr" "auau" "25gev" "centr")
SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_centr PROPERTIES TIMEOUT "300000")
SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_centr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

ADD_TEST(littrack_mvd_auau_25gev_mbias "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_mbias" "auau" "25gev" "mbias")
SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_mbias PROPERTIES TIMEOUT "300000")
SET_TESTS_PROPERTIES(littrack_mvd_auau_25gev_mbias PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
EndIf($ENV{ctest_model} MATCHES Experimental)
