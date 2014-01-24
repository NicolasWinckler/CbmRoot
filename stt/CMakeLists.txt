# Create a library called "libStt" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/stt
${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/stt
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles
CbmStt.cxx
CbmSttContFact.cxx
CbmGeoStt.cxx
CbmGeoSttPar.cxx
CbmSttHitProducer.cxx
CbmSttMatchTracks.cxx
)

set(LINKDEF SttLinkDef.h)
Set(LIBRARY_NAME Stt)
Set(DEPENDENCIES
    CbmBase CbmData Base 
)

GENERATE_LIBRARY()
