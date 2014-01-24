## Create a library called "libtrdcatrack" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 

${CBMROOT_SOURCE_DIR}/cbmbase 

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/trd/param

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface

${CBMROOT_SOURCE_DIR}/trdcatrack
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmL1CATrdTrackFinderSA.cxx
CbmL1TrdTracklet4.cxx
CbmL1TrdTracklet.cxx
CbmTrdMatchTracksMK.cxx
CbmTrdTestEventsWithMom.cxx
CbmTrdTrackFitterKF_CA.cxx
)

set(LINKDEF TrdCaTrackLinkDef.h)
Set(LIBRARY_NAME trdcatrack)
Set(DEPENDENCIES
    KF Trd CbmBase CbmData Base 
)

GENERATE_LIBRARY()

