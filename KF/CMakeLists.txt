# Create a library called "libKF" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 

${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/ecal
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface
${CBMROOT_SOURCE_DIR}/KF/KFModelParameters
${CBMROOT_SOURCE_DIR}/KF/KFQA
${CBMROOT_SOURCE_DIR}/KF/KFParticle
${CBMROOT_SOURCE_DIR}/KF/KFParticlesFinder

${CBMROOT_SOURCE_DIR}/sts 

${CBMROOT_SOURCE_DIR}/L1
${CBMROOT_SOURCE_DIR}/L1/L1Algo
${CBMROOT_SOURCE_DIR}/L1/ParticleFinder

${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/passive 
${CBMROOT_SOURCE_DIR}/mvd 
${CBMROOT_SOURCE_DIR}/rich 
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/trd/param
${CBMROOT_SOURCE_DIR}/tof/TofParam
${CBMROOT_SOURCE_DIR}/ecal
${CBMROOT_SOURCE_DIR}/stt

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmKF.cxx 
CbmKFFieldMath.cxx 
CbmKFHit.cxx 
CbmKFMaterial.cxx 
CbmKFMath.cxx 
CbmKFPixelMeasurement.cxx 
CbmKFPrimaryVertexFinder.cxx 
CbmKFSecondaryVertexFinder.cxx 
CbmKFTrackInterface.cxx 
CbmKFUMeasurement.cxx 
CbmKFVertexInterface.cxx 
Interface/CbmEcalTrackExtrapolationKF.cxx
Interface/CbmKFStsHit.cxx 
Interface/CbmKFTrack.cxx 
Interface/CbmKFTrdHit.cxx 
Interface/CbmPVFinderKF.cxx 
Interface/CbmStsFitPerformanceTask.cxx 
Interface/CbmStsKFTrackFitter.cxx 
Interface/CbmStsKFSecondaryVertexFinder.cxx 
Interface/CbmTrdTrackFitterKF.cxx 
Interface/CbmGlobalTrackFitterKF.cxx
CbmKFParticle.cxx
CbmKFParticle_simd.cxx
CbmKFParticleInterface.cxx
KFModelParameters/CbmThermalModelNoFlow.cxx
KFQA/CbmKFTrErrMCPoints.cxx
KFQA/CbmKFTrackFitQA.cxx
KFQA/KFParticleMatch.cxx
KFQA/KFMCParticle.cxx
KFQA/CbmKFParticlesFinderQA.cxx
KFParticle/KFParticleBase.cxx
KFParticle/KFParticle.cxx
KFParticle/KFParticleBaseSIMD.cxx
KFParticle/KFParticleSIMD.cxx
KFParticle/KFParticleFinder.cxx
KFParticlesFinder/CbmKFParticlesFinder.cxx
KFParticlesFinder/CbmKFParticleDatabase.cxx
)


set(HEADERS
CbmKF.h 
CbmKFFieldMath.h
CbmKFHit.h
CbmKFMaterial.h
CbmKFMath.h
CbmKFPixelMeasurement.h
CbmKFPrimaryVertexFinder.h
CbmKFSecondaryVertexFinder.h
CbmKFTrackInterface.h
CbmKFUMeasurement.h 
CbmKFVertexInterface.h
Interface/CbmEcalTrackExtrapolationKF.h
Interface/CbmKFStsHit.h
Interface/CbmKFTrack.h 
Interface/CbmKFTrdHit.h 
Interface/CbmPVFinderKF.h
Interface/CbmStsFitPerformanceTask.h
Interface/CbmStsKFTrackFitter.h
Interface/CbmStsKFSecondaryVertexFinder.h
Interface/CbmTrdTrackFitterKF.h
Interface/CbmGlobalTrackFitterKF.h
CbmKFParticle.h
KFModelParameters/CbmThermalModelNoFlow.h
KFQA/CbmKFTrErrMCPoints.h
KFQA/CbmKFTrackFitQA.h
KFQA/KFParticleMatch.h
KFQA/KFMCParticle.h
KFQA/CbmKFParticlesFinderQA.h
KFQA/CbmKFPartEfficiencies.h
KFParticle/KFParticleBase.h
KFParticle/KFParticle.h
KFParticlesFinder/CbmKFParticlesFinder.h
KFParticlesFinder/CbmKFParticleDatabase.h
)

ADD_DEFINITIONS(-Wall -Wabi -Wsign-promo  -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel -Wno-non-template-friend ) # -Weffc++ -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast   : wait for other parts of cbmroot\root.

IF (SSE_FOUND)
  Message(STATUS "KF will be compiled with SSE support")
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
ELSE (SSE_FOUND)
  MESSAGE(STATUS "KF will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_FOUND)

set(LINKDEF KFLinkDef.h)
Set(LIBRARY_NAME KF)
Set(DEPENDENCIES
    Ecal CbmBase CbmData Base
)

GENERATE_LIBRARY()
