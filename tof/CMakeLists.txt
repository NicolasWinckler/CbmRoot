# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.



set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/tof
${CBMROOT_SOURCE_DIR}/tof/TofMC
${CBMROOT_SOURCE_DIR}/tof/TofParam
${CBMROOT_SOURCE_DIR}/tof/TofReco
${CBMROOT_SOURCE_DIR}/tof/TofTools
${CBMROOT_SOURCE_DIR}/tof/TofData

${CBMROOT_SOURCE_DIR}/cbmbase 

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/tof
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
TofMC/CbmGeoTof.cxx 
TofMC/CbmTof.cxx 

TofParam/CbmGeoTofPar.cxx 
TofParam/CbmTofContFact.cxx 
TofParam/CbmTofDigiPar.cxx 
TofParam/CbmTofCreateDigiPar.cxx

TofReco/CbmTofHitProducer.cxx 
TofReco/CbmTofHitProducerIdeal.cxx 
TofReco/CbmTofHitProducerNew.cxx 

TofTools/CbmTofGeoHandler.cxx

TofData/CbmTofCell.cxx
)

set(LINKDEF TofLinkDef.h)
Set(LIBRARY_NAME Tof)
Set(DEPENDENCIES
    CbmBase CbmData Base 
)

GENERATE_LIBRARY()
