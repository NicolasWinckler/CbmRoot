


If(NOT CMAKE_CXX_FLAGS)
  Message(STATUS "No C++11 support found. MQTest example will not be build.")
Else()
  String(FIND ${CMAKE_CXX_FLAGS} "-std=c++11" POS_C++11)
  If(${POS_C++11} EQUAL -1)
    Message(STATUS "No C++11 support found. MQTest example will not be build.")
  Else()
    Message(STATUS "C++11 support found. Build MQTest example.")
    add_subdirectory(MQTest)
  EndIf()
EndIf()

#add_subdirectory (Tutorial1/src)
#add_subdirectory (Tutorial2/src)
