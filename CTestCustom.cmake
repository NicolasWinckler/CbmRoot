# -*- mode: cmake -*-

message(" -- Read CTestCustom.cmake --")

# -----------------------------------------------------------
# -- Number of warnings to display
# -----------------------------------------------------------

set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS "500" )

# -----------------------------------------------------------
# -- Number of errors to display
# -----------------------------------------------------------

set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS   "50" )

# -----------------------------------------------------------
# -- Warning execptions
# -----------------------------------------------------------

set(CTEST_CUSTOM_WARNING_EXCEPTION
	${CTEST_CUSTOM_WARNING_EXCEPTION}

	# -- doxygen warnings
# 	"of command \@param is not found in the argument list of" 
#	"for \\link command"
#	"for \\ref command"
#	"\\class statement"
#	"\\file statement"
#	"are not documented:"
#	"Skipping documentation"
#	"has a brief description"

 	# -- CLHEP and Pluto warnings
        "/include/CLHEP/"
        "PParticle.h"

 	# -- htrack warnings
        # should be switched on, but there are so many
        "/htrack/"
  )

# -----------------------------------------------------------
# -- Warning addon's
# -----------------------------------------------------------
set(CTEST_CUSTOM_WARNING_MATCH	${CTEST_CUSTOM_WARNING_MATCH}
	)
