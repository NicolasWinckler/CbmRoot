set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${GEANT3_INCLUDE_DIR}
${Boost_INCLUDE_DIR}
${CBMROOT_SOURCE_DIR}/analysis
${CBMROOT_SOURCE_DIR}/analysis/hyperon
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmbase/draw
${CBMROOT_SOURCE_DIR}/cbmbase/report
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/much
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/global
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/global
${CBMROOT_SOURCE_DIR}/trackbase
${CBMROOT_SOURCE_DIR}/geane
${CBMROOT_SOURCE_DIR}/field
${CBMROOT_SOURCE_DIR}/mvd 
${CBMROOT_SOURCE_DIR}/sts 
${CBMROOT_SOURCE_DIR}/KF 
${CBMROOT_SOURCE_DIR}/KF/Interface 
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/rich
${CBMROOT_SOURCE_DIR}/littrack
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(ANALYSIS_SRCS_RULECHECKER
hyperon/CbmAnaHypYPtGenerator.cxx
hyperon/CbmAnaHypTools.cxx
hyperon/CbmAnaHypSecFinder.cxx
hyperon/CbmAnaHypSecFinderQa.cxx
hyperon/CbmAnaHypPair.cxx
hyperon/CbmAnaHypLambdaCandidate.cxx
hyperon/CbmAnaHypTrack.cxx
hyperon/CbmAnaHypRecEff.cxx
hyperon/CbmAnaHypRecLambda.cxx
hyperon/CbmAnaHypSelectLambda.cxx
hyperon/CbmAnaHypRecCascade.cxx
hyperon/CbmAnaHypMix.cxx

#CbmD0Analysis.cxx
#V0/CbmV0Analysis.cxx

#msh/CbmMSHConverter.cxx
#msh/CbmMSHAnalysis.cxx

much/CbmAnaMuch.cxx
much/CbmAnaMuchJpsiPolarization.cxx
much/CbmPolarizedGenerator.cxx
much/CbmAnaMuonCandidate.cxx
much/CbmAnaDimuonCandidate.cxx
much/CbmAnaDimuonAnalysis.cxx
much/CbmAnaDimuonHisto.cxx
much/CbmAnaAngularAcceptance.cxx
much/CbmAnaMuchBgd.cxx

#timing/CbmAnaTimingAnalysis.cxx 
#timing/CbmAnaEpochTest.cxx
#timing/CbmAnaTimingMuchDigitizer.cxx
#timing/CbmAnaTimingMuchHitFinder.cxx
#timing/CbmAnaTimingStsDigitizer.cxx
#timing/CbmAnaTimingStsHitFinder.cxx

dielectron/CbmAnaDielectronTask.cxx
dielectron/CbmAnaDielectronTaskDraw.cxx
dielectron/CbmAnaDielectronTaskDrawAll.cxx
dielectron/CbmAnaLmvmDrawStudy.cxx
dielectron/CbmAnaLmvmNames.cxx
dielectron/CbmAnaDielectronStudyReportAll.cxx
dielectron/CbmAnaDielectronReports.cxx

#photon/CbmTrdPhotonAnalysis.cxx
)


SET(ANALYSIS_SRCS ${ANALYSIS_SRCS_RULECHECKER})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${ANALYSIS_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" ANALYSIS_RULES)
endif(RULE_CHECKER_FOUND)

set(ANALYSIS_HEADERS
hyperon/CbmAnaHypYPtGenerator.h
hyperon/CbmAnaHypTools.h
hyperon/CbmAnaHypSecFinder.h
hyperon/CbmAnaHypSecFinderQa.h
hyperon/CbmAnaHypPair.h
hyperon/CbmAnaHypLambdaCandidate.h
hyperon/CbmAnaHypTrack.h
hyperon/CbmAnaHypRecEff.h
hyperon/CbmAnaHypRecLambda.h
hyperon/CbmAnaHypSelectLambda.h
hyperon/CbmAnaHypRecCascade.h
hyperon/CbmAnaHypMix.h

#CbmD0Analysis.h
#V0/CbmV0Analysis.h

#msh/CbmMSHConverter.h
#msh/CbmMSHTrackInfo.h
#msh/CbmMSHAnalysis.h

much/CbmAnaMuch.h
much/CbmAnaMuchJpsiPolarization.h
much/CbmPolarizedGenerator.h
much/CbmAnaMuonCandidate.h
much/CbmAnaDimuonCandidate.h
much/CbmAnaDimuonAnalysis.h
much/CbmAnaDimuonHisto.h
much/CbmAnaAngularAcceptance.h
much/CbmAnaMuchBgd.h

#timing/CbmAnaTimingAnalysis.h 
#timing/CbmAnaEpochTest.h
#timing/CbmAnaTimingMuchDigitizer.h
#timing/CbmAnaTimingMuchHitFinder.h
#timing/CbmAnaTimingStsDigitizer.h
#timing/CbmAnaTimingStsHitFinder.h

dielectron/CbmAnaDielectronTask.h
dielectron/CbmAnaDielectronTaskDraw.h
dielectron/CbmAnaDielectronTaskDrawAll.h
dielectron/CbmAnaLmvmDrawStudy.h
dielectron/CbmAnaDielectronReports.h

#photon/CbmTrdPhotonAnalysis.h
)

# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h ANALYSIS_HEADERS "${ANALYSIS_SRCS}")

set(ANALYSIS_LINKDEF  AnalysisLinkDef.h)
set(ANALYSIS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmAnalysisDict.cxx) 

ROOT_GENERATE_DICTIONARY("${ANALYSIS_HEADERS}" "${ANALYSIS_LINKDEF}" "${ANALYSIS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(ANALYSIS_SRCS ${ANALYSIS_SRCS} ${ANALYSIS_DICTIONARY})

add_library(Analysis SHARED ${ANALYSIS_SRCS})
target_link_libraries(Analysis ${ROOT_LIBRARIES})
set_target_properties(Analysis PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Analysis DESTINATION ${CMAKE_BINARY_DIR}/lib)

