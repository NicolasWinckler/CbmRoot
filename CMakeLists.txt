# The name of our project is "CBMROOT".  CMakeLists files in this project can
# refer to the root source directory of the project as ${CBMROOT_SOURCE_DIR} or
# as ${CMAKE_SOURCE_DIR} and to the root binary directory of the project as 
# ${CBMROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}.
# This difference is is important for the base classes which are in CBMROOT 
# and PANDAROOT.

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)

# Set name of our project to "CBMROOT". Has to be done
# after check of cmake version                        
project(CBMROOT)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ 
# is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")


# Load some basic macros which are needed later on
include(FairMacros)
include(WriteConfigFile)
include(CTest)
include(CheckCompiler)

#Check the compiler and set the compile and link flags
Check_Compiler()

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")

Option(USE_PATH_INFO "Information from PATH and LD_LIBRARY_PATH are used."OFF)
If(USE_PATH_INFO)
  Set(PATH $ENV{PATH})
  If (APPLE)
    Set(LD_LIBRARY_PATH $ENV{DYLD_LIBRARY_PATH})
  Else (APPLE)
    Set(LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
  EndIf (APPLE)
EndIf(USE_PATH_INFO)

# Check if the user wants to build the project in the source
# directory
CHECK_OUT_OF_SOURCE_BUILD()

# Check if we are on an UNIX system. If not stop with an error
# message
IF(NOT UNIX)
  MESSAGE(FATAL_ERROR "You're not on an UNIX system. The project was up to now only tested on UNIX systems, so we break here. If you want to go on please edit the CMakeLists.txt in the source directory.")
ENDIF(NOT UNIX)  

#Check if necessary environment variables are set
#If not stop execution
if(NOT CBMROOT_MINIMAL)
  IF(NOT DEFINED ENV{SIMPATH})
    MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
  ENDIF(NOT DEFINED ENV{SIMPATH})
  SET(SIMPATH $ENV{SIMPATH})
else(NOT CBMROOT_MINIMAL)
  if(NOT DEFINED ENV{ROOTSYS})
    MESSAGE(FATAL_ERROR "You did not define the environment variable ROOTSYS which is nedded for the installation of the minimal version of CbmRoot. Please set this variable and execute cmake again.")
  endif(NOT DEFINED ENV{ROOTSYS})
endif(NOT CBMROOT_MINIMAL)

STRING(REGEX MATCHALL "[^:]+" PATH $ENV{PATH})


# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is
# mandatory
if(CBMROOT_MINIMAL)
  MESSAGE("******* Install minimal CbmRoot Version ********")
  find_package(ROOT REQUIRED)
else(CBMROOT_MINIMAL)
  find_package(ROOT REQUIRED)
  find_package(PLUTO REQUIRED)
  find_package(GENERATORS REQUIRED)
  find_package(GEANT3 REQUIRED)
  find_package(GEANT4)
  find_package(GEANT4VMC)
  find_package(CLHEP)
  find_package(RuleChecker)
  find_package(TBB)
  find_package(SSE)

  Set(Boost_NO_SYSTEM_PATHS TRUE)
  Set(Boost_NO_BOOST_CMAKE TRUE)
#  set(Boost_DEBUG TRUE)
  If(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
    set(BOOST_ROOT ${SIMPATH})
    set(GSL_DIR ${SIMPATH})
  Else(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
    set(BOOST_ROOT ${SIMPATH}/basics/boost)
    set(GSL_DIR ${SIMPATH}/basics/gsl)
  EndIf(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
  Message("-- Looking for Boost ...")
  # If an older version of boost is found both of the variables below are
  # cached and in a second cmake run, a good boost version is found even 
  # if the version is to old. 
  # To overcome this problem both variables are cleared before checking
  # for boost.
  Unset(Boost_INCLUDE_DIR CACHE)
  Unset(Boost_LIBRARY_DIRS CACHE)
  find_package(Boost 1.41)
  If (Boost_FOUND)
    Set(Boost_Avail 1)
  Else (Boost_FOUND)
    Set(Boost_Avail 0)
  EndIf (Boost_FOUND)
  Message("-- Looking for GSL ...")
  find_package(GSL)

endif(CBMROOT_MINIMAL)


# Set the library version in the main CMakeLists.txt
SET(CBMROOT_MAJOR_VERSION 0)
SET(CBMROOT_MINOR_VERSION 0)
SET(CBMROOT_PATCH_VERSION 0)
SET(CBMROOT_VERSION "${CBMROOT_MAJOR_VERSION}.${CBMROOT_MINOR_VERSION}.${CBMROOT_PATCH_VERSION}")
SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
    VERSION "${CBMROOT_VERSION}"
    SOVERSION "${CBMROOT_MAJOR_VERSION}"
    SUFFIX ".so"
)

SET(CBMLIBDIR ${CMAKE_BINARY_DIR}/lib)  
SET(LD_LIBRARY_PATH  ${CBMLIBDIR} ${Boost_LIBRARY_DIRS} ${LD_LIBRARY_PATH})

Set(INCLUDE_DIRECTORIES 
  ${ROOT_INCLUDE_DIR}
  ${CBMROOT_SOURCE_DIR}/fairtools
)

Generate_Version_Info()

# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

add_subdirectory (fairtools)
add_subdirectory (base)
add_subdirectory (dbase)
add_subdirectory (geobase)
add_subdirectory (parbase) 
add_subdirectory (cbmbase)
add_subdirectory (cbmdata)
add_subdirectory (field)  
add_subdirectory (generators)
add_subdirectory (passive)
add_subdirectory (rich) 
add_subdirectory (mvd) 
add_subdirectory (sts) 
add_subdirectory (tof) 
add_subdirectory (trd) 
add_subdirectory (ecal) 
add_subdirectory (zdc) 
add_subdirectory (much)
#add_subdirectory (much_feb07)
#add_subdirectory (lhetrack) 
add_subdirectory (littrack) 
add_subdirectory (htrack)
add_subdirectory (global) 
add_subdirectory (KF)
add_subdirectory (L1)
add_subdirectory (macro)
add_subdirectory (dilept)
add_subdirectory (stt)
add_subdirectory (geane)
add_subdirectory (trackbase)
add_subdirectory (trdcatrack)
#add_subdirectory (tutorials/Tutorial1/src)
#add_subdirectory (tutorials/Tutorial2/src)
add_subdirectory (cbmgenerators)
add_subdirectory (run)
add_subdirectory (hadron)
add_subdirectory (analysis)

Option(BUILD_UNITTESTS "Build all unittests and add them as new tests" OFF)
If(BUILD_UNITTESTS)

  Message("Build the unit tests.")

  Set(GTEST_ROOT ${SIMPATH})
  Find_Package(GTest)

  If(GTEST_FOUND)
    Message("Add tests library and tests.")
    Add_Subdirectory(test)
  EndIf(GTEST_FOUND)
EndIf(BUILD_UNITTESTS)

Execute_Process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-opengl
                OUTPUT_VARIABLE EveIsBuild
                OUTPUT_STRIP_TRAILING_WHITESPACE
               )

If(${EveIsBuild} STREQUAL yes)
  Message("Build the eventdisplay.")
  add_subdirectory (eventdisplay)
  add_subdirectory (cbmdisplay)
Else(${EveIsBuild} STREQUAL yes)
  Message("The eventdisplay is not build, there is no OpenGL support.")
EndIf(${EveIsBuild} STREQUAL yes)

set(BUILD_ROCLIB FALSE CACHE BOOL "Build DABC and RocLib which are needed
    to read testbeam data.")
If(BUILD_ROCLIB)
  configure_file(${PROJECT_SOURCE_DIR}/cmake/scripts/CMakeLists.txt_roc
                 ${PROJECT_SOURCE_DIR}/roc/CMakeLists.txt COPYONLY)
  add_subdirectory (roc)
EndIf(BUILD_ROCLIB)


Option(BUILD_DOXYGEN "Build Doxygen" OFF)
if(BUILD_DOXYGEN)
  MESSAGE(STATUS "*** Building the Doxygen documentaion ***")
  ADD_SUBDIRECTORY(doxygen)
endif(BUILD_DOXYGEN)

if(RULE_CHECKER_FOUND)

  ADD_CUSTOM_TARGET(RULES 
     COMMAND ${RULE_CHECKER_SCRIPT1} ${CMAKE_BINARY_DIR} viol > violations.html
     DEPENDS $ENV{ALL_RULES})

endif(RULE_CHECKER_FOUND)

if(NOT CBMROOT_MINIMAL)
  WRITE_CONFIG_FILE(config.sh)
  WRITE_CONFIG_FILE(config.csh)
else(NOT CBMROOT_MINIMAL)
  SET(FULL_CONFIG_FILE "false")
  WRITE_CONFIG_FILE(config.sh)
endif(NOT CBMROOT_MINIMAL)
