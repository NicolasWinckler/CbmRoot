# Create a library called "libL1" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 

${CBMROOT_SOURCE_DIR}/cbmbase 

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/rich/fitter

${CBMROOT_SOURCE_DIR}/L1
${CBMROOT_SOURCE_DIR}/L1/L1Algo
${CBMROOT_SOURCE_DIR}/L1/OffLineInterface
${CBMROOT_SOURCE_DIR}/L1/ParticleFinder

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/KFQA
${CBMROOT_SOURCE_DIR}/KF/KFParticlesFinder
${CBMROOT_SOURCE_DIR}/KF/Interface

${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/trd/param
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})


set(SRCS
#L1Algo/L1CATrackFinder.cxx 
#CbmL1Performance.cxx 
#CbmL1ReadEvent.cxx 
CbmL1CATrdTrackFinderSA.cxx 
CbmL1TrdTracklet.cxx 
CbmL1TrdTracklet4.cxx 
CbmL1.cxx 
CbmL1TrdTrackFinderSts.cxx 
CbmL1TrackMerger.cxx 
CbmL1TofMerger.cxx 
OffLineInterface/CbmL1RichENNRingFinder.cxx
OffLineInterface/CbmL1RichENNRingFinderParallel.cxx 
OffLineInterface/CbmL1StsTrackFinder.cxx 
#OffLineInterface/CbmL1MuchFinder.cxx 
#OffLineInterface/CbmL1MuchHit.cxx 
#OffLineInterface/CbmL1MuchTrack.cxx 
#OffLineInterface/CbmL1MuchFinderQa.cxx
OffLineInterface/CbmL1RichRingQa.cxx 
#OffLineInterface/CbmL1SttHit.cxx 
#OffLineInterface/CbmL1SttTrackFinder.cxx 
#OffLineInterface/CbmL1SttTrack.cxx 

L1Algo/L1Algo.cxx
L1Algo/L1CATrackFinder.cxx
L1Algo/L1TrackExtender.cxx
L1Algo/L1TrackFitter.cxx
L1Algo/L1CAMergeClones.cxx
L1Algo/L1HitsSortHelper.cxx
CbmL1Performance.cxx 
CbmL1ReadEvent.cxx

CbmL1MCTrack.cxx

CbmL1ParticlesFinder.cxx
ParticleFinder/CbmL1PFFitter.cxx
ParticleFinder/CbmL1PFMCParticle.cxx
)

set(HEADERS
CbmL1CATrdTrackFinderSA.h
CbmL1.h
CbmL1MCTrack.h
CbmL1MCPoint.h
CbmL1StsHit.h
CbmL1TofMerger.h
CbmL1Track.h
CbmL1TrackMerger.h
CbmL1TrackPar.h
CbmL1TrdHit.h
CbmL1TrdTrackFinderSts.h
CbmL1TrdTracklet4.h
CbmL1TrdTracklet.h
CbmL1Vtx.h
#OffLineInterface/CbmL1MuchFinder.h
#OffLineInterface/CbmL1MuchFinderQa.h
#OffLineInterface/CbmL1MuchHit.h
#OffLineInterface/CbmL1MuchTrack.h
OffLineInterface/CbmL1RichENNRingFinder.h
OffLineInterface/CbmL1RichRingQa.h
OffLineInterface/CbmL1StsTrackFinder.h
#OffLineInterface/CbmL1SttHit.h
#OffLineInterface/CbmL1SttTrackFinder.h
#OffLineInterface/CbmL1SttTrack.h
# L1Algo/L1Algo.h
CbmL1PartEfficiencies.h
)

ADD_DEFINITIONS(-Wall -Wabi -Wsign-promo  -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel -Wno-non-template-friend  ) #   -Weffc++ -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast : wait for other parts of cbmroot\root. 

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
  Message(STATUS "L1 will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "L1 will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_FOUND)

set(LINKDEF L1LinkDef.h)
Set(LIBRARY_NAME L1)
Set(DEPENDENCIES
    #Rich 
    KF Trd Sts CbmBase CbmData Base 
)

GENERATE_LIBRARY()

