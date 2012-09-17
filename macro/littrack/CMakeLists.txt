File(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/events_electron)
File(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/results_electron)
File(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/events_muon)
File(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/results_muon)

# Define the different setups to be tested
List(APPEND  setup  electron  muon)

ForEach(__setup IN  LISTS  setup)

  Set(testname global_sim_${__setup})

  Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test.sh ${__setup} sim)
  Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "300")
  Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Set(_dep ${testname})
  Set(testname global_reco_${__setup})
  
  Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test.sh ${__setup} reco)
  Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "300")
  Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  Set_Tests_Properties(${testname} PROPERTIES DEPENDS ${_dep})
 
EndForEach(__setup IN LISTS setup)



#GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test_mvd.sh)
#
#Add_Test(littrack_mvd_delta "${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_mvd.sh" "delta")
#Set_Tests_Properties(littrack_mvd_delta PROPERTIES TIMEOUT "1800")
#Set_Tests_Properties(littrack_mvd_delta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#
#Add_Test(littrack_mvd_nodelta "${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_mvd.sh" "nodelta")
#Set_Tests_Properties(littrack_mvd_nodelta PROPERTIES TIMEOUT "1800")
#Set_Tests_Properties(littrack_mvd_nodelta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
#

# Additional nightly tests
If(BUILD_LITTRACK_EXPERIMENTAL)
  GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_reco.sh)
  
  Add_Test(littrack_electron_auau_25gev_centr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_electron_auau_25gev_centr" "electron" "auau" "25gev" "centr")
  Set_Tests_Properties(littrack_electron_auau_25gev_centr PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_electron_auau_25gev_centr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Add_Test(littrack_electron_auau_25gev_mbias "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_electron_auau_25gev_mbias" "electron" "auau" "25gev" "mbias")
  Set_Tests_Properties(littrack_electron_auau_25gev_mbias PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_electron_auau_25gev_mbias PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Add_Test(littrack_muon_auau_25gev_centr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_auau_25gev_centr" "muon" "auau" "25gev" "centr")
  Set_Tests_Properties(littrack_muon_auau_25gev_centr PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_muon_auau_25gev_centr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Add_Test(littrack_muon_auau_25gev_mbias "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_auau_25gev_mbias" "muon" "auau" "25gev" "mbias")
  Set_Tests_Properties(littrack_muon_auau_25gev_mbias PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_muon_auau_25gev_mbias PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Add_Test(littrack_muon_radial_auau_25gev_centr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_radial_auau_25gev_centr" "muon_radial" "auau" "25gev" "centr")
  Set_Tests_Properties(littrack_muon_radial_auau_25gev_centr PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_muon_radial_auau_25gev_centr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Add_Test(littrack_muon_radial_auau_25gev_mbias "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh" "littrack_global_reco_muon_radial_auau_25gev_mbias" "muon_radial" "auau" "25gev" "mbias")
  Set_Tests_Properties(littrack_muon_radial_auau_25gev_mbias PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_muon_radial_auau_25gev_mbias PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh)
  
  Add_Test(littrack_mvd_auau_25gev_centr_nodelta "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_centr_nodelta" "auau" "25gev" "centr" "nodelta")
  Set_Tests_Properties(littrack_mvd_auau_25gev_centr_nodelta PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_mvd_auau_25gev_centr_nodelta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Add_Test(littrack_mvd_auau_25gev_mbias_nodelta "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_mbias_nodelta" "auau" "25gev" "mbias" "nodelta")
  Set_Tests_Properties(littrack_mvd_auau_25gev_mbias_nodelta PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_mvd_auau_25gev_mbias_nodelta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Add_Test(littrack_mvd_auau_25gev_centr_delta "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_centr_delta" "auau" "25gev" "centr" "delta")
  Set_Tests_Properties(littrack_mvd_auau_25gev_centr_delta PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_mvd_auau_25gev_centr_delta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Add_Test(littrack_mvd_auau_25gev_mbias_delta "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh" "littrack_mvd_reco_auau_25gev_mbias_delta" "auau" "25gev" "mbias" "delta")
  Set_Tests_Properties(littrack_mvd_auau_25gev_mbias_delta PROPERTIES TIMEOUT "36000")
  Set_Tests_Properties(littrack_mvd_auau_25gev_mbias_delta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_field.sh)
  Add_Test(littrack_field "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_field.sh")
  Set_Tests_Properties(littrack_field PROPERTIES TIMEOUT "3600")
  Set_Tests_Properties(littrack_field PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_fieldapr.sh)
  Add_Test(littrack_fieldapr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_fieldapr.sh")
  Set_Tests_Properties(littrack_fieldapr PROPERTIES TIMEOUT "3600")
  Set_Tests_Properties(littrack_fieldapr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

EndIf(BUILD_LITTRACK_EXPERIMENTAL)
