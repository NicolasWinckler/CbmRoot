# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${CLHEP_INCLUDE_DIR}
${GSL_INCLUDE_DIRS}
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/mvd 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${GSL_LIBRARY_DIRS}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmMvd.cxx 
CbmMvdContFact.cxx 
CbmMvdGeo.cxx 
CbmMvdGeoPar.cxx 
CbmMvdHitProducer.cxx 
CbmMvdDigitize.cxx   
CbmMvdDigitizeL.cxx   
CbmMvdPileupManager.cxx  
CbmMvdPixelCharge.cxx  
CbmMvdFindHits.cxx 
CbmMvdStation.cxx 
MyG4UniversalFluctuationForSi.cxx   
)
#CbmMvdMatchTracks.cxx 


set(LINKDEF MvdLinkDef.h)
Set(LIBRARY_NAME Mvd)
Set(DEPENDENCIES
    CbmBase CbmData Base 
)

GENERATE_LIBRARY()
