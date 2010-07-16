SET (CTEST_SOURCE_DIRECTORY $ENV{SOURCEDIR})
SET (CTEST_BINARY_DIRECTORY $ENV{BUILDDIR})
SET (CTEST_SITE $ENV{SITE})
SET (CTEST_BUILD_NAME $ENV{LABEL})
SET (CTEST_CMAKE_GENERATOR "Unix Makefiles")
SET (CTEST_PROJECT_NAME "CBMROOT")

SET (CTEST_UPDATE_COMMAND "svn")
SET (CTEST_UPDATE_CHECKOUT  " ${CTEST_UPDATE_COMMAND} update ")
SET (BUILD_COMMAND "make")
SET (CTEST_BUILD_COMMAND "${BUILD_COMMAND} -j$ENV{number_of_processors}")


if($ENV{ctest_model} MATCHES Nightly)
  # get the information about conflicting or localy modified files
  # from svn, extract the relavant information about the file name
  # and put the result in the output variable
  execute_process(COMMAND svn stat -u  
                  COMMAND grep ^[CM]
                  COMMAND cut -c21- 
                  OUTPUT_VARIABLE FILELIST
                  )

  # create out of the output a cmake list. This step is done to convert the
  # stream into seperated filenames.
  # The trick is to exchange an "\n" by an ";" which is the separartor in
  # a list created by cmake 
  STRING(REGEX REPLACE "\n" ";" _result "${FILELIST}")

  FOREACH(_file ${_result})
    SET (CTEST_NOTES_FILES ${CTEST_NOTES_FILES} "${CTEST_SOURCE_DIRECTORY}/${_file}")
  ENDFOREACH(_file ${_result})

  CTEST_EMPTY_BINARY_DIRECTORY(${CTEST_BINARY_DIRECTORY})

endif($ENV{ctest_model} MATCHES Nightly)

configure_file(${CTEST_SOURCE_DIRECTORY}/CTestCustom.cmake
               ${CTEST_BINARY_DIRECTORY}/CTestCustom.cmake
              )
ctest_read_custom_files("${CTEST_BINARY_DIRECTORY}")

CTEST_START ($ENV{ctest_model})
CTEST_UPDATE (SOURCE "${CTEST_SOURCE_DIRECTORY}")
CTEST_CONFIGURE (BUILD "${CTEST_BINARY_DIRECTORY}")
CTEST_BUILD (BUILD "${CTEST_BINARY_DIRECTORY}")
CTEST_TEST (BUILD "${CTEST_BINARY_DIRECTORY}")
CTEST_SUBMIT ()
 
