# Create a library called "libRun" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 

${CBMROOT_SOURCE_DIR}/cbmbase

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/much
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/ecal
${CBMROOT_SOURCE_DIR}/cbmdata/psd

${CBMROOT_SOURCE_DIR}/mvd
${CBMROOT_SOURCE_DIR}/sts/mc
${CBMROOT_SOURCE_DIR}/rich
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/trd/mc
${CBMROOT_SOURCE_DIR}/tof/TofMC
${CBMROOT_SOURCE_DIR}/ecal
${CBMROOT_SOURCE_DIR}/psd
${CBMROOT_SOURCE_DIR}/passive
${CBMROOT_SOURCE_DIR}/field



${CBMROOT_SOURCE_DIR}/run
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS

CbmDaq.cxx
CbmDaqQa.cxx
CbmDaqBuffer.cxx
CbmMCBuffer.cxx
CbmMCTimeSim.cxx
CbmTimeSlice.cxx

CbmMCStreamer.cxx
CbmRunAna.cxx
CbmMCStreamerQa.cxx

CbmSetup.cxx

)

set(LINKDEF RunLinkDef.h)
Set(LIBRARY_NAME Run)
Set(DEPENDENCIES
    CbmData Base CbmBase Field
)

GENERATE_LIBRARY()
