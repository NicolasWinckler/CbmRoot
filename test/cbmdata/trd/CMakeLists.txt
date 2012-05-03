set(INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${GTEST_INCLUDE_DIRS} 
 ${CMAKE_SOURCE_DIR}/cbmdata/trd
 ${CMAKE_SOURCE_DIR}/cbmbase
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
)

Set ( Sources
    _GTestCbmTrdDetectorId.cxx
    ${CMAKE_SOURCE_DIR}/cbmdata/trd/CbmTrdDetectorId.cxx
    )  
 
link_directories( ${LINK_DIRECTORIES})

############### build the test #####################
add_executable(_GTestCbmTrdDetectorId ${Sources})
target_link_libraries(_GTestCbmTrdDetectorId ${ROOT_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
add_test(_GTestCbmTrdDetectorId ${PROJECT_BINARY_DIR}/bin/_GTestCbmTrdDetectorId)




