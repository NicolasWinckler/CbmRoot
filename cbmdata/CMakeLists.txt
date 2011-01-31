# Create a library called "libCbmData" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(CBMDATA_SRCS
CbmBaseHit.cxx
CbmDigi.cxx
CbmHit.cxx
CbmPixelHit.cxx
CbmStack.cxx
CbmStripHit.cxx
CbmTrack.cxx
CbmTrackMatch.cxx

CbmMCEpoch.cxx
CbmMCEventHeader.cxx
CbmMCTrack.cxx

mvd/CbmMvdCluster.cxx 
mvd/CbmMvdDetectorId.cxx
mvd/CbmMvdDigi.cxx  
mvd/CbmMvdHit.cxx 
mvd/CbmMvdHitMatch.cxx 
mvd/CbmMvdPoint.cxx

sts/CbmStsCluster.cxx 
sts/CbmStsDetectorId.cxx
sts/CbmStsDigi.cxx 
sts/CbmStsDigiMatch.cxx 
sts/CbmStsHit.cxx 
sts/CbmStsPoint.cxx 
sts/CbmStsTrack.cxx 

rich/CbmRichRing.cxx	
rich/CbmRichHit.cxx 
rich/CbmRichPoint.cxx
rich/CbmRichRingMatch.cxx 

much/CbmMuchPixelHit.cxx 
much/CbmMuchPoint.cxx
much/CbmMuchCluster.cxx 
much/CbmMuchDigi.cxx
much/CbmMuchDigiMatch.cxx
much/CbmMuchTrack.cxx 
much/CbmMuchStrawHit.cxx

trd/CbmTrdHit.cxx		 
trd/CbmTrdTrack.cxx		 
trd/CbmTrdPoint.cxx	
trd/CbmTrdDigi.cxx  
trd/CbmTrdDigiMatch.cxx  
trd/CbmTrdDetectorId.cxx
trd/CbmTrdCluster.cxx

tof/CbmTofHit.cxx 
tof/CbmTofPoint.cxx

ecal/CbmEcalHit.cxx 
ecal/CbmEcalPoint.cxx
ecal/CbmEcalPointLite.cxx 
ecal/CbmEcalCluster.cxx 
ecal/CbmEcalHitFastMC.cxx 

zdc/CbmZdcPoint.cxx  
zdc/CbmZdcEvent.cxx

stt/CbmSttPoint.cxx
stt/CbmSttHit.cxx
stt/CbmSttTrack.cxx
stt/CbmSttTrackMatch.cxx

global/CbmGlobalTrack.cxx
global/CbmVertex.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${CBMDATA_SRCS}" "${INCLUDE_DIRECTORIES}" CBMDATA_RULES)
endif(RULE_CHECKER_FOUND)
 
# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h CBMDATA_HEADERS "${CBMDATA_SRCS}")

set(CBMDATA_LINKDEF  DataLinkDef.h)
set(CBMDATA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmDataDict.cxx) 

ROOT_GENERATE_DICTIONARY("${CBMDATA_HEADERS}" "${CBMDATA_LINKDEF}" "${CBMDATA_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(CBMDATA_SRCS ${CBMDATA_SRCS} ${CBMDATA_DICTIONARY})


add_library(CbmData SHARED ${CBMDATA_SRCS})
target_link_libraries(CbmData ${ROOT_LIBRARIES})
set_target_properties(CbmData PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS CbmData DESTINATION ${CMAKE_BINARY_DIR}/lib)
