# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/psd
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/psd
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmPsd.cxx  
CbmGeoPsd.cxx  
CbmGeoPsdPar.cxx  
CbmPsdv1.cxx 
CbmPsdIdealDigitizer.cxx
CbmPsdHitProducer.cxx
)

set(LINKDEF PsdLinkDef.h)
Set(LIBRARY_NAME Psd)
Set(DEPENDENCIES
    CbmData Base 
)

GENERATE_LIBRARY()
