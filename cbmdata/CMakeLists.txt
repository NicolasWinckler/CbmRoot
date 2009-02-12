# Create a library called "libCbmData" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
#${CBMROOT_SOURCE_DIR}/geobase 
#${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
#${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata 
#${CBMROOT_SOURCE_DIR}/ecal
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(CBMDATA_SRCS
CbmEcalHit.cxx 
CbmEcalPoint.cxx
CbmEcalPointLite.cxx 
CbmEcalCluster.cxx 
CbmEcalHitFastMC.cxx 

CbmMuchHit.cxx 
CbmMuchPoint.cxx
CbmMuchCluster.cxx 
CbmMuchDigi.cxx
CbmMuchDigiMatch.cxx
CbmMuchTrack.cxx 
CbmMuchTrackMatch.cxx

CbmMvdHit.cxx 
CbmMvdHitMatch.cxx 
CbmMvdPoint.cxx
CbmMvdDigi.cxx  
CbmMvdCluster.cxx 

CbmRichRing.cxx	
CbmRichHit.cxx 
CbmRichPoint.cxx
CbmRichRingMatch.cxx 

CbmStsCluster.cxx 
CbmStsDigi.cxx 
CbmStsDigiMatch.cxx 
CbmStsHit.cxx 
CbmStsPoint.cxx 
CbmStsTrack.cxx 
CbmStsTrackMatch.cxx

CbmSttPoint.cxx
CbmSttHit.cxx
CbmSttTrack.cxx
CbmSttTrackMatch.cxx

CbmTofHit.cxx 
CbmTofPoint.cxx

CbmTrdHit.cxx		 
CbmTrdTrack.cxx		 
CbmTrdPoint.cxx	
CbmTrdTrackMatch.cxx  

CbmZdcPoint.cxx  
CbmZdcEvent.cxx
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
