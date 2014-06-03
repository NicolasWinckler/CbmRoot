# Create a library called "libTrd" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${Boost_INCLUDE_DIR}

# needed by run/FairMCBuffer.h
${CBMROOT_SOURCE_DIR}/run 
${CBMROOT_SOURCE_DIR}/cbmdata/mvd 
${CBMROOT_SOURCE_DIR}/cbmdata/rich 
${CBMROOT_SOURCE_DIR}/cbmdata/much 
${CBMROOT_SOURCE_DIR}/cbmdata/trd 
${CBMROOT_SOURCE_DIR}/cbmdata/tof 
${CBMROOT_SOURCE_DIR}/cbmdata/ecal 
${CBMROOT_SOURCE_DIR}/cbmdata/psd 

${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmbase/draw

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/trd
${CBMROOT_SOURCE_DIR}/trd/digi
${CBMROOT_SOURCE_DIR}/trd/param
${CBMROOT_SOURCE_DIR}/trd/reco
${CBMROOT_SOURCE_DIR}/trd/tools
${CBMROOT_SOURCE_DIR}/trd/data
${CBMROOT_SOURCE_DIR}/trd/beamtime
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
#${ROCSYS}/lib
#${GO4SYS}/include
#${GO4SYS}/Go4Event
#${GO4SYS}/Go4StatusBase
#${ROCSYS}/include
#${ROCSYS}/include/spadic
#${ROCSYS}/include/sp605
#${ROCSYS}/include/roc
#${ROCSYS}/onlinemonitor/framework
#${ROCSYS}/onlinemonitor/epicsmonitor
#${ROCSYS}/onlinemonitor/rocmonitor
#${ROCSYS}/onlinemonitor/spadicmonitor
#${ROCSYS}/onlinemonitor/spadicV10monitor
#${ROCSYS}/onlinemonitor/trbmonitor
#${ROCSYS}/beamtime/cern-oct12/go4
#${ROCSYS}/beamtime/cern-oct11/go4/RICH
#${ROCSYS}/beamtime/cern-oct11/go4/MSFFM
#${ROCSYS}/beamtime/cern-oct11/go4/BUCHAREST
#${ROCSYS}/beamtime/cern-oct11/go4/
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
mc/CbmGeoTrd.cxx 
mc/CbmTrd.cxx 		 

param/CbmGeoTrdPar.cxx 
param/CbmTrdContFact.cxx   
param/CbmTrdDigiPar.cxx
param/CbmTrdCreateDigiPar.cxx

#digi/CbmTrdDigitizerMathieson.cxx 
digi/CbmTrdDigitizerPRF.cxx
digi/CbmTrdDigitizerPRF_TB.cxx
digi/CbmTrdDigitizer.cxx
#digi/CbmTrdDigiWriteoutBuffer.cxx

reco/CbmTrdSPADIC.cxx

qa/CbmTrdHitProducerQa.cxx  
qa/CbmTrdTracksPidQa.cxx
qa/CbmTrdQa.cxx
qa/CbmTrdHitDensityQa.cxx
qa/CbmTrdHitRateQa.cxx
qa/CbmTrdHitRateFastQa.cxx
qa/CbmTrdOccupancyQa.cxx
qa/CbmTrdDigitizerPRFQa.cxx
qa/CbmTrdClusterizerFastQa.cxx
qa/CbmTrdHitProducerClusterQa.cxx
qa/CbmTrdRecoQa.cxx

reco/CbmTrdHitProducerIdeal.cxx    
reco/CbmTrdHitProducerSmearing.cxx
reco/CbmTrdHitProducerDigi.cxx
#reco/CbmTrdClusterFinder.cxx
reco/CbmTrdClusterFinderFast.cxx
reco/CbmTrdClusterFinderDPB.cxx
reco/CbmTrdHitProducerCluster.cxx
#reco/CbmTrdSimpleHitProducerCluster.cxx

tools/CbmTrdUtils.cxx
tools/CbmTrdElectronsTrainAnn.cxx
tools/CbmTrdGeoHandler.cxx

data/CbmTrdRadiator.cxx  
data/CbmTrdGas.cxx
data/CbmTrdModule.cxx
data/CbmTrdAsic.cxx
#beamtime/CbmTrdRawToDigiSpadic03.cxx


#${ROCSYS}/include/spadic/Message.h
#${ROCSYS}/include/roc/Message.h
#${ROCSYS}/onlinemonitor/rocmonitor/TRocEvent.cxx
#${ROCSYS}/onlinemonitor/spadicmonitor/TSpadicEvent.cxx
#${ROCSYS}/onlinemonitor/framework/TTriglogEvent.cxx
#${ROCSYS}/beamtime/cern-oct12/go4/TMbsCrateEvent.cxx
#${ROCSYS}/beamtime/cern-oct12/go4/TCernOct12UnpackEvent.cxx
#${ROCSYS}/beamtime/cern-oct12/go4/TCernOct12DetectorEvent.cxx
#${ROCSYS}/beamtime/cern-oct12/go4/TBeamMonitorEvent.cxx

CbmTrdFindTracks.cxx  
CbmTrdTrackFinderIdeal.cxx  
CbmTrdFitTracks.cxx  
CbmTrdTrackFitterIdeal.cxx  
#CbmTrdMatchTracks.cxx
CbmTrdSetTracksPidWkn.cxx       
CbmTrdSetTracksPidLike.cxx     
CbmTrdSetTracksPidANN.cxx       
CbmTrdCreatePidLike.cxx 
#CbmTrdDigiSorterTask.cxx
#CbmTrdDigiRingSorter.cxx

)

set(LINKDEF TrdLinkDef.h)
Set(LIBRARY_NAME Trd)
Set(DEPENDENCIES
    CbmBase CbmData Base TMVA
)

GENERATE_LIBRARY()
