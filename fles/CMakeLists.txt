# Build the fles_ipc library which is tacken from
# https://github.com/cbm-fles/fles_ipc.git
# This library provides the interface to the FLIB board or to files in tca
# format written by the FLIB
#
# This file is only called if the compiler provides c++11 support

Set(CMAKE_CXX_FLAGS_BAK ${CMAKE_CXX_FLAGS})

# Add some extra compile warnings
Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Winit-self -Wundef -Wold-style-cast -Woverloaded-virtual -Wwrite-strings -Wnon-virtual-dtor")

Set(INCLUDE_DIRECTORIES
${CBMROOT_CURRENT_SOURCE_DIR}/ipc
)

Set(SYSTEM_INCLUDE_DIRECTORIES
${Boost_INCLUDE_DIR}
)

Include_Directories( ${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})


Set(LINK_DIRECTORIES
${Boost_LIBRARY_DIRS}
)

Link_Directories( ${LINK_DIRECTORIES})

Set(SRCS
ipc/StorableTimeslice.cpp
ipc/System.cpp
ipc/TimesliceInputArchive.cpp
ipc/TimesliceReceiver.cpp
ipc/TimesliceView.cpp
)


Set(LIBRARY_NAME fles_ipc)
If(UNIX AND NOT APPLE)
  Set(DEPENDENCIES boost_thread boost_system boost_serialization rt)
Else()
  Set(DEPENDENCIES boost_thread boost_system boost_serialization)
EndIf()


GENERATE_LIBRARY()
