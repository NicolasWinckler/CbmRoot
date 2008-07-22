SET(MODEL Experimental)
IF(${CTEST_SCRIPT_ARG} MATCHES Nightly)
  SET(MODEL Nightly)
ENDIF(${CTEST_SCRIPT_ARG} MATCHES Nightly)

SET (CTEST_COMMAND
    "ctest -D ${MODEL}Start"
    "ctest -D ${MODEL}Build"
    "make install" 
    "ctest -D ${MODEL}Test" 
    "ctest -D ${MODEL}Submit"
)

SET (CTEST_SOURCE_DIRECTORY $ENV{SOURCEDIR})
SET (CTEST_BINARY_DIRECTORY $ENV{BUILDDIR})

SET (CTEST_UPDATE_COMMAND "/usr/bin/svn")
SET (CTEST_UPDATE_CHECKOUT  " ${CTEST_UPDATE_COMMAND} update ")


# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND 
  "cmake"
  )


####################################################################
# The values in this section are optional you can either
# have them or leave them commented out
####################################################################

# should ctest wipe the binary tree before running
#SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)
#CTEST_EMPTY_BINARY_DIRECTORY(${CTEST_BINARY_DIRECTORY})

# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
SET (CTEST_INITIAL_CACHE "
BUILDNAME:STRING=$ENV{LABEL}
SITE:STRING=$ENV{SITE}
")

