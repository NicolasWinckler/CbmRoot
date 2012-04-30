set(INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${GTEST_INCLUDE_DIRS} 
 ${CMAKE_SOURCE_DIR}/trd
 ${CMAKE_SOURCE_DIR}/cbmdata/trd
 ${CMAKE_SOURCE_DIR}/cbmbase
)

# Boost is needed for the regular expression handling
#find_package(Boost COMPONENTS regex)

If(Boost_FOUND)
  Set(INCLUDE_DIRECTORIES
      ${INCLUDE_DIRECTORIES}
      ${Boost_INCLUDE_DIRS}
     )
EndIf(Boost_FOUND)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
)

Set ( Sources
    _GTestTrdGeoHandler.cxx
    )  
 
link_directories( ${LINK_DIRECTORIES})
############### build the test #####################
If(Boost_FOUND)
  add_executable(_GTestTrdGeoHandler ${Sources})
  target_link_libraries(_GTestTrdGeoHandler ${ROOT_LIBRARIES} Geom GeomPainter VMC  ${GTEST_BOTH_LIBRARIES} FairTools FairDB GeoBase ParBase Base CbmBase CbmData Trd)
  add_test(_GTesTrdGeoHandler ${PROJECT_BINARY_DIR}/bin/_GTestTrdGeoHandler)
Else(Boost_FOUND)
  Message(STATUS "Could not build the test executable, because the Boost libraries are misssing.")
EndIf(Boost_FOUND)




