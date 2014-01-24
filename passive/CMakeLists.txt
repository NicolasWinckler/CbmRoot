# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/passive
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmCave.cxx            
CbmGeoCave.cxx         
CbmGeoMagnet.cxx       
CbmGeoPassivePar.cxx   
CbmGeoPipe.cxx         
CbmGeoPlatform.cxx
CbmGeoStart.cxx        
CbmMagnet.cxx          
CbmPassiveContFact.cxx 
CbmPipe.cxx
CbmPlatform.cxx
CbmShield.cxx
CbmShieldGeo.cxx         
CbmTarget.cxx 
)

set(LINKDEF PassiveLinkDef.h)
Set(LIBRARY_NAME Passive)
Set(DEPENDENCIES
    CbmBase Base 
)

GENERATE_LIBRARY()
