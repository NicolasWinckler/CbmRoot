# Create a library called "libTrd" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${Boost_INCLUDE_DIR}

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
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
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
digi/CbmTrdDigitizer.cxx
#digi/CbmTrdDigiWriteoutBuffer.cxx

qa/CbmTrdHitProducerQa.cxx  
qa/CbmTrdTracksPidQa.cxx
#qa/CbmTrdQa.cxx
#qa/CbmTrdHitDensityQa.cxx
qa/CbmTrdHitRateQa.cxx
#qa/CbmTrdOccupancyQa.cxx
qa/CbmTrdDigitizerPRFQa.cxx
qa/CbmTrdClusterizerFastQa.cxx
qa/CbmTrdHitProducerClusterQa.cxx
qa/CbmTrdRecoQa.cxx

reco/CbmTrdHitProducerIdeal.cxx    
reco/CbmTrdHitProducerSmearing.cxx
reco/CbmTrdHitProducerDigi.cxx
#reco/CbmTrdClusterFinder.cxx
reco/CbmTrdClusterFinderFast.cxx
reco/CbmTrdHitProducerCluster.cxx
#reco/CbmTrdSimpleHitProducerCluster.cxx

tools/CbmTrdElectronsTrainAnn.cxx
tools/CbmTrdGeoHandler.cxx

data/CbmTrdRadiator.cxx  
data/CbmTrdGas.cxx
data/CbmTrdModule.cxx

CbmTrdFindTracks.cxx  
CbmTrdTrackFinderIdeal.cxx  
CbmTrdFitTracks.cxx  
CbmTrdTrackFitterIdeal.cxx  
CbmTrdMatchTracks.cxx
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
