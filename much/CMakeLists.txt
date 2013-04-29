# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed
# here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 

${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata 
${CBMROOT_SOURCE_DIR}/cbmdata/much 
${CBMROOT_SOURCE_DIR}/much
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/much/reco
${CBMROOT_SOURCE_DIR}/much/tracking
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
geo/CbmGeoMuch.cxx  
geo/CbmGeoMuchPar.cxx 
geo/CbmMuch.cxx
geo/CbmMuchContFact.cxx  
geo/CbmMuchGeoScheme.cxx
geo/CbmMuchLayer.cxx
geo/CbmMuchLayerSide.cxx
geo/CbmMuchModule.cxx
geo/CbmMuchModuleGem.cxx
geo/CbmMuchModuleGemRadial.cxx
geo/CbmMuchModuleGemRectangular.cxx
geo/CbmMuchModuleStraws.cxx
geo/CbmMuchPad.cxx
geo/CbmMuchPadRadial.cxx
geo/CbmMuchPadRectangular.cxx
geo/CbmMuchSector.cxx
geo/CbmMuchSectorRadial.cxx
geo/CbmMuchSectorRectangular.cxx
geo/CbmMuchSegmentAuto.cxx
geo/CbmMuchSegmentManual.cxx
geo/CbmMuchSegmentSector.cxx
geo/CbmMuchStation.cxx
reco/CbmMuchDigitizeGem.cxx
reco/CbmMuchDigitizeStraws.cxx
reco/CbmMuchFindHitsGem.cxx
reco/CbmMuchFindHitsStraws.cxx
reco/CbmMuchHitFinderQa.cxx
reco/CbmMuchHitProducerIdeal.cxx
reco/CbmMuchPointInfo.cxx
reco/CbmMuchStrawHitFinderQa.cxx
#reco/CbmMuchClusterAnalysis.cxx

tracking/CbmMuchFindTracks.cxx
tracking/CbmMuchMatchTracks.cxx 
tracking/CbmMuchTrackFinderIdeal.cxx
)

set(LINKDEF MuchLinkDef.h)
Set(LIBRARY_NAME Much)
Set(DEPENDENCIES
    CbmBase CbmData Base 
)

GENERATE_LIBRARY()

