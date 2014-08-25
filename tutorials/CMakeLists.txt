
set(Fles_ipc_Headers 
    TimesliceArchiveDescriptor.hpp TimesliceReceiver.hpp MicrosliceDescriptor.hpp TimesliceCompletion.hpp TimesliceSource.hpp
    TimesliceComponentDescriptor.hpp TimesliceDescriptor.hpp TimesliceView.hpp
    StorableTimeslice.hpp TimesliceWorkItem.hpp TimesliceInputArchive.hpp System.hpp TimesliceOutputArchive.hpp Timeslice.hpp
)

set(SEARCH_FLES_INCLUDE_DIR ${CBMROOT_SOURCE_DIR}/tutorials/MQTest/fles_ipc)
find_path(FLES_INCLUDE_DIR StorableTimeslice.hpp PATHS ${SEARCH_FLES_INCLUDE_DIR})

if(FLES_INCLUDE_DIR)
MESSAGE(STATUS "fles_ipc include directory found in : ${FLES_INCLUDE_DIR}")
else(FLES_INCLUDE_DIR)
MESSAGE(STATUS "fles_ipc include directory not found in path(s) : ${SEARCH_FLES_INCLUDE_DIR}")
endif(FLES_INCLUDE_DIR)

set(flesfilemissing false)
set(flesmissingfiles )

FOREACH(FileName ${Fles_ipc_Headers})
    set(FlesFile ${SEARCH_FLES_INCLUDE_DIR}/${FileName})
    #MESSAGE(STATUS "Process file: ${FileName}")
    find_file(havefile_${FileName} NAMES ${FileName} PATHS ${FLES_INCLUDE_DIR} NO_DEFAULT_PATH)
    if(havefile_${FileName})
    MESSAGE(STATUS "found file ${havefile_${FileName}}")
    else(havefile_${FileName})
    MESSAGE(STATUS "file ${FileName} is missing in fles_ipc directory")
    set(flesfilemissing true)
    set(flesmissingfiles ${flesmissingfiles} ${FileName})
    endif(havefile_${FileName})
ENDFOREACH(FileName)


if(flesfilemissing)
    MESSAGE(STATUS "The following header file(s) is (are) missing in fles_ipc directory : ")
    MESSAGE(STATUS "${flesmissingfiles}")
    MESSAGE(STATUS "MQTest example will not be built.")
else(flesfilemissing)
    add_subdirectory(MQTest)
endif(flesfilemissing)

#add_subdirectory (Tutorial1/src)
#add_subdirectory (Tutorial2/src)

#

#add_subdirectory (Tutorial1/src)
#add_subdirectory (Tutorial2/src)
