# Copyright 2013 Jan de Cuveland <cmail@cuveland.de>
#file(GLOB FLES_LIB_SOURCES *.cpp)
#file(GLOB FLES_LIB_HEADERS *.hpp)
#add_library(fles_ipc ${FLES_LIB_SOURCES} ${FLES_LIB_HEADERS})
##############################################################

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${CBMROOT_SOURCE_DIR}/tutorials/MQTest/fles_ipc
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${Boost_LIBRARY_DIRS}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
StorableTimeslice.cpp
System.cpp
TimesliceInputArchive.cpp
TimesliceReceiver.cpp
TimesliceView.cpp
)


Set(LIBRARY_NAME fles_ipc)
Set(DEPENDENCIES boost_thread boost_system boost_serialization)

GENERATE_LIBRARY()


