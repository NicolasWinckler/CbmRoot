# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${SIMPATH}/basics/gsl/include
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/passive
${CBMROOT_SOURCE_DIR}/mvd 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${SIMPATH}/basics/gsl/lib
)

link_directories( ${LINK_DIRECTORIES})

set(MVD_SRCS
CbmMvd.cxx 
CbmMvdContFact.cxx 
CbmMvdGeo.cxx 
CbmMvdGeoPar.cxx 
CbmMvdHit.cxx 
CbmMvdHitMatch.cxx 
CbmMvdHitProducer.cxx 
CbmMvdDigitiser.cxx  
CbmMvdPileupManager.cxx  
CbmMvdPoint.cxx
CbmMvdDigi.cxx  
CbmMvdFindHits.cxx
CbmMvdCluster.cxx 
CbmMvdStation.cxx
MyG4UniversalFluctuationForSi.cxx
CbmMvdPixelCharge.cxx    
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MVD_HEADERS "${MVD_SRCS}")

set(MVD_LINKDEF  MvdLinkDef.h)
set(MVD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmMvdDict.cxx) 

ROOT_GENERATE_DICTIONARY("${MVD_HEADERS}" "${MVD_LINKDEF}" "${MVD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(MVD_SRCS ${MVD_SRCS} ${MVD_DICTIONARY})


add_library(Mvd SHARED ${MVD_SRCS})
target_link_libraries(Mvd -lgsl ${ROOT_LIBRARIES}) 
set_target_properties(Mvd PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Mvd DESTINATION ${CMAKE_BINARY_DIR}/lib)
