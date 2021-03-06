#
# Build the documentation
#
# INCLUDE (Documentation)




IF (BUILD_DOXYGEN)

  FIND_PACKAGE(UnixCommands)
  FIND_PACKAGE(Doxygen)
  FIND_PACKAGE(Gnuplot)
  FIND_PACKAGE(HTMLHelp)
  FIND_PACKAGE(Perl)
  FIND_PACKAGE(Wget)

  SET(DOXYGEN_PROJECT_NAME "CbmRoot")

  SET(DOXYGEN_SOURCE_DIR "${CBMROOT_SOURCE_DIR}/doxygen")
  SET(DOXYGEN_PROJECT_SOURCE_DIR "${CBMROOT_SOURCE_DIR}")
  SET(CBMROOT_DOXYGEN_HOME "${CBMROOT_SOURCE_DIR}/doxygen")
  SET(DOXYGEN_BINARY_LOCATIONS_DIR "${CBMROOT_BINARY_DIR}")
  SET(DOXYGEN_BINARY_LOCATIONS
       "")
  SET(DOXYGEN_IGNORE_PREFIX "svn")

  #
  # Configure the script and the doxyfile, then add target
  #
  IF(NOT DOT_PATH)
    GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
  ENDIF(NOT DOT_PATH)

  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY IMMEDIATE)

  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/doc_makeall.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/doc_makeall.sh @ONLY IMMEDIATE)

  ADD_CUSTOM_TARGET(${DOXYGEN_PROJECT_NAME}DoxygenDoc ALL
    ${BASH}
    ${CMAKE_CURRENT_BINARY_DIR}/doc_makeall.sh)

ENDIF (BUILD_DOXYGEN)
