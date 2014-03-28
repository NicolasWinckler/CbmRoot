# Create a library called "libCbmBeamtime" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
  ${Boost_INCLUDE_DIR}
  ${CBMROOT_SOURCE_DIR}/cbmbase
  ${CBMROOT_SOURCE_DIR}/cbmdata
#  ${CBMROOT_SOURCE_DIR}/trd/param
  ${CBMROOT_SOURCE_DIR}/cbmdata/sts
  ${CBMROOT_SOURCE_DIR}/cbmdata/much
  ${CBMROOT_SOURCE_DIR}/cbmdata/beamtime
  ${CBMROOT_SOURCE_DIR}/cbmdata/beamtime/fhodo
  ${CBMROOT_SOURCE_DIR}/sts/setup
  ${CBMROOT_BINARY_DIR}/roc/roc/roclib
  ${CBMROOT_SOURCE_DIR}/beamtime
  ${CBMROOT_SOURCE_DIR}/beamtime/fhodo
  ${CBMROOT_SOURCE_DIR}/beamtime/reco_tasks
  ${CBMROOT_SOURCE_DIR}/beamtime/param
  ${CBMROOT_SOURCE_DIR}/sts/setup
  ${CBMROOT_SOURCE_DIR}/beamtime/reco
)

Include_Directories( 
  ${INCLUDE_DIRECTORIES}
)

Set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
)
 
Link_Directories( 
  ${LINK_DIRECTORIES}
)

Set(SRCS
  CbmDaqMap.cxx
  CbmMuchDigiTb.cxx
  CbmSourceLmd.cxx
  CbmTbDaqBuffer.cxx

  fhodo/CbmFiberHodoClusterFinder.cxx
  fhodo/CbmFiberHodoBLCalibrator.cxx

#  reco_tasks/CbmBeamTimeExampleTask.cxx
#  reco_tasks/CbmBeamTimeExampleTask2.cxx
#  reco_tasks/CbmBeamTimeExampleTask3.cxx

  param/CbmBeamTimeContFact.cxx
  param/CbmBeamTimeFirstPar.cxx

  reco/CbmStsBLCalibrator.cxx
)

# Add artificial dependency which delays the compilation of the defined file
# until the libRoc.so is produced which is needed for the linking of the
# beamtime library. I couldn't find any better solution for the problem up
# to now.
Set_Source_Files_Properties(PROPERTIES CbmRocSource.cxx PROPERTIES
                            OBJECT_DEPENDS ${LIBRARY_OUTPUT_PATH}/libRoc.so
)

Set(LINKDEF  CbmBeamtimeLinkDef.h)
Set(LIBRARY_NAME CbmBeamtime)
Set(DEPENDENCIES
    ${LIBRARY_OUTPUT_PATH}/libDabcRoc.so 
    ${LIBRARY_OUTPUT_PATH}/libDabcMbs.so 
    ${LIBRARY_OUTPUT_PATH}/libDabcBase.so 
    ${LIBRARY_OUTPUT_PATH}/libRoc.so 
    Sts
    CbmData CbmBase
#    Base Trd
    Base 
)

GENERATE_LIBRARY()
