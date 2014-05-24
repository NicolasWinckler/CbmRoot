# CMakeList file for library libSts.
# Last update: V. Friese, 17.02.2014


# -----   Specify library name   --------------------------
Set(LIBRARY_NAME Sts)
# ---------------------------------------------------------


# -----  Specify sources for compilation   ----------------
# --- Sources in setup
set(SRCS_SETUP
#setup/CbmStsAddress.cxx
setup/CbmStsElement.cxx
setup/CbmStsModule.cxx
setup/CbmStsSenzor.cxx
setup/CbmStsSensorPoint.cxx
setup/CbmStsSensorType.cxx
setup/CbmStsSensorTypeDssd.cxx
setup/CbmStsSetup.cxx
)
# --- Sources in mc
set(SRCS_MC
mc/CbmStsMC.cxx
)
# --- Sources in digitize
set (SRCS_DIGITIZE
digitize/CbmStsDigitizeIdeal.cxx
digitize/CbmStsSensorTypeDssdIdeal.cxx;
)
# --- All sources
set(SRCS
${SRCS_SETUP}
${SRCS_MC}
${SRCS_DIGITIZE}
CbmGeoSts.cxx 
CbmGeoStsPar.cxx 
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
# -----  End of sources   ---------------------------------



# ----  Specify include directories -----------------------
set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/sts/setup
${CBMROOT_SOURCE_DIR}/sts/mc
${CBMROOT_SOURCE_DIR}/sts/digitize

# needed by MatchHits???
${CBMROOT_SOURCE_DIR}/passive 

${CBMROOT_SOURCE_DIR}/cbmbase 

${CBMROOT_SOURCE_DIR}/run 

${CBMROOT_SOURCE_DIR}/cbmdata 
${CBMROOT_SOURCE_DIR}/cbmdata/sts 

# needed by run/CbmMCBuffer.h
${CBMROOT_SOURCE_DIR}/cbmdata/mvd 
${CBMROOT_SOURCE_DIR}/cbmdata/rich 
${CBMROOT_SOURCE_DIR}/cbmdata/much 
${CBMROOT_SOURCE_DIR}/cbmdata/trd 
${CBMROOT_SOURCE_DIR}/cbmdata/tof 
${CBMROOT_SOURCE_DIR}/cbmdata/ecal 
${CBMROOT_SOURCE_DIR}/cbmdata/psd 
)
# ----  End of include directories ------------------------


# ----  Specify link directories --------------------------
set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
# -----  End of link directories   ------------------------


# -----   Specify library dependences   -------------------
Set(DEPENDENCIES
    Passive Run CbmBase CbmData Base 
)
# ---------------------------------------------------------


# -----  Specify LinkDef file   ---------------------------
set(LINKDEF ${LIBRARY_NAME}LinkDef.h)
# ---------------------------------------------------------


# -----  Let cmake do the job   ---------------------------
include_directories( ${INCLUDE_DIRECTORIES})
link_directories( ${LINK_DIRECTORIES})
GENERATE_LIBRARY()
# ---------------------------------------------------------
