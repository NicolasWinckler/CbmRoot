# Create a library called "libDilept" which includes the source files
# given in the array . The extension is already found.  Any number of
# sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 

${CMAKE_SOURCE_DIR}/cbmbase

${CMAKE_SOURCE_DIR}/cbmdata
${CMAKE_SOURCE_DIR}/cbmdata/sts
${CMAKE_SOURCE_DIR}/cbmdata/rich
${CMAKE_SOURCE_DIR}/cbmdata/trd
${CMAKE_SOURCE_DIR}/cbmdata/tof
${CMAKE_SOURCE_DIR}/cbmdata/global

${CMAKE_SOURCE_DIR}/KF
${CMAKE_SOURCE_DIR}/KF/Interface

${CMAKE_SOURCE_DIR}/dilept
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR} 
)
 
link_directories( ${LINK_DIRECTORIES})


set(SRCS
CbmDileptonTrack.cxx
CbmDileptonTrackReal.cxx
CbmDileptonTrackRealCollection.cxx
CbmDileptonAssignPid.cxx
CbmDileptonTrackSim.cxx
CbmDileptonTrackSimCollection.cxx
CbmDileptonAssignMCid.cxx
)


set(LINKDEF DileptonLinkDef.h)
Set(LIBRARY_NAME Dilept)
Set(DEPENDENCIES
    KF CbmBase CbmData Base 
)

GENERATE_LIBRARY()
