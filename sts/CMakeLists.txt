# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/sts

# needed by MatchHits???
${CBMROOT_SOURCE_DIR}/passive 

${CBMROOT_SOURCE_DIR}/cbmbase 

${CBMROOT_SOURCE_DIR}/run 

${CBMROOT_SOURCE_DIR}/cbmdata 
${CBMROOT_SOURCE_DIR}/cbmdata/sts 

# needed by run/FairMCBuffer.h
${CBMROOT_SOURCE_DIR}/cbmdata/mvd 
${CBMROOT_SOURCE_DIR}/cbmdata/rich 
${CBMROOT_SOURCE_DIR}/cbmdata/much 
${CBMROOT_SOURCE_DIR}/cbmdata/trd 
${CBMROOT_SOURCE_DIR}/cbmdata/tof 
${CBMROOT_SOURCE_DIR}/cbmdata/ecal 
${CBMROOT_SOURCE_DIR}/cbmdata/psd 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS_SETUP
setup/CbmStsAddress.cxx
setup/CbmStsElement.cxx
setup/CbmStsModule.cxx
setup/CbmStsSenzor.cxx
setup/CbmStsSensorPoint.cxx
setup/CbmStsSensorType.cxx
setup/CbmStsSensorTypeDssd.cxx
setup/CbmStsSetup.cxx
)


set(SRCS
${SRCS_SETUP}
CbmGeoSts.cxx 
CbmGeoStsPar.cxx 
CbmSts.cxx 
CbmStsContFact.cxx 
CbmStsDigiPar.cxx 
CbmStsDigiScheme.cxx 
CbmStsIdealDigitize.cxx 
CbmStsDigitize.cxx 
CbmStsDigitizeTb.cxx 
CbmStsIdealFindHits.cxx 
CbmStsClusterFinder.cxx 
CbmStsFindHits.cxx 
CbmStsFindTracks.cxx 
CbmStsFindTracksQa.cxx 
CbmStsFitTracks.cxx 
CbmStsHitProducerIdeal.cxx 
CbmStsIdealMatchHits.cxx 
CbmStsMatchHits.cxx 
CbmStsMatchTracks.cxx 
CbmStsParAsciiFileIo.cxx 
CbmStsParRootFileIo.cxx 
CbmStsRadTool.cxx
CbmStsSensor.cxx 
CbmStsSensorDigiPar.cxx 
CbmStsSector.cxx 
CbmStsSectorDigiPar.cxx 
CbmStsStation.cxx 
CbmStsStationDigiPar.cxx 
CbmStsTrackFinderIdeal.cxx 
CbmStsTrackFitterIdeal.cxx 
CbmStsSimulationQa.cxx 
CbmStsFindHitsQa.cxx
CbmStsReconstructionQa.cxx
)

set(LINKDEF StsLinkDef.h)
Set(LIBRARY_NAME Sts)
Set(DEPENDENCIES
    Passive Run CbmBase CbmData Base 
)

GENERATE_LIBRARY()
