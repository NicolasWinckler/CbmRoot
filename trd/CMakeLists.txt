# Create a library called "libTrd" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/trd
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/passive
${CBMROOT_SOURCE_DIR}/global

${CBMROOT_SOURCE_DIR}/cbmdata/sts
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})


#FILE(GLOB TRD_HEADERS ${CMAKE_HOME_DIRECTORY}/trd *.h)

set(TRD_SRCS
CbmGeoTrd.cxx 
CbmGeoTrdPar.cxx 
CbmTrdContFact.cxx   
CbmTrd.cxx 		 
CbmTrdHitProducerIdeal.cxx    
CbmTrdHitProducerSmearing.cxx	 
CbmTrdHitProducerQa.cxx  
CbmTrdRadiator.cxx  
CbmTrdFindTracks.cxx  
CbmTrdFindTracksQa.cxx  
CbmTrdTrackFinderIdeal.cxx  
CbmTrdFitTracks.cxx  
CbmTrdFitTracksQa.cxx  
CbmTrdTrackFitterIdeal.cxx  
CbmTrdMatchTracks.cxx
CbmTrdSetTracksPidWkn.cxx       
CbmTrdSetTracksPidLike.cxx     
CbmTrdSetTracksPidANN.cxx       
CbmTrdCreatePidLike.cxx 
CbmTrdTracksPidQa.cxx
CbmTrdGas.cxx
#CbmTrdDigitizer.cxx
#CbmTrdDigi.cxx
#CbmTrdDigiPar.cxx
#CbmTrdDigiMatch.cxx
#CbmTrdStringToken.cxx
#CbmTrdModule.cxx
#CbmTrdCreateDigiPar.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TRD_SRCS}" "${INCLUDE_DIRECTORIES}" TRD_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TRD_HEADERS "${TRD_SRCS}")

set(TRD_LINKDEF TrdLinkDef.h)
set(TRD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmTrdDict.cxx) 

ROOT_GENERATE_DICTIONARY("${TRD_HEADERS}" "${TRD_LINKDEF}" "${TRD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
#ROOT_GENERATE_DICTIONARY(${TRD_HEADERS} ${TRD_DICTIONARY} ${INCLUDE_DIRECTORIES})

set(TRD_SRCS ${TRD_SRCS} ${TRD_DICTIONARY})

add_library(Trd SHARED ${TRD_SRCS})
target_link_libraries(Trd ${ROOT_LIBRARIES})
set_target_properties(Trd PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Trd  DESTINATION ${CMAKE_BINARY_DIR}/lib)
