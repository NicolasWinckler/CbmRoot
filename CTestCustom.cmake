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

        # -- warnings from ubuntu systems which are a little to much
        # -- probably defined warn-unused-result. ignoring the result
        # -- of fgets is common practice. A work around would be to
        # -- store the return value in a dummy variable
        "ignoring return value of 'char* fgets(char*, int, FILE*)'"
        "ignoring return value of 'char* fscanf(char*, int, FILE*)'"

        # -- boost warnings
        "/include/boost/exception/exception.hpp:"
        "/include/boost/smart_ptr/detail/sp_convertible.hpp:"
        "/include/boost/smart_ptr/shared_ptr.hpp:"  

        # -- Root warnings which should not show up in the test setup
        "/include/G__ci.h:"
        "/include/TAttImage.h:"
        "/include/TCollectionProxyInfo.h"
        "/include/TCut.h:"
        "/include/TEveBoxSet.h:"
        "/include/TEveTrackPropagator.h:"
        "/include/TEveTrackPropagator.h:"
        "/include/TEveVector.h:"
        "/include/TFcnAdapter.h:"
        "/include/TFitterMinuit.h:"
        "/include/TGeoMatrix.h:"
        "/include/TGeoPainter.h:"
        "/include/TList.h:"
        "/include/TMap.h:"
        "/include/TMatrixT.h:"
        "/include/TMatrixTSym.h:"
        "/include/TObjArray.h:"
        "/include/TRefArray.h:"
        "/include/Minuit2/BasicFunctionGradient.h:"
        "/include/Minuit2/MnUserParameterState.h:"
        "/include/Minuit2/StackAllocator.h:"
        "/include/TMVA/ClassInfo.h:"
        "/include/TMVA/Config.h:"
        "/include/TMVA/Configurable.h:"
        "/include/TMVA/DataInputHandler.h:"
        "/include/TMVA/DataSet.h:"
        "/include/TMVA/DataSetInfo.h:"
        "/include/TMVA/DataSetManager.h:"
        "/include/TMVA/Event.h:"
        "/include/TMVA/Factory.h:"
        "/include/TMVA/KDEKernel.h:"
        "/include/TMVA/Option.h:"
        "/include/TMVA/PDF.h:"
        "/include/TMVA/Reader.h:"
        "/include/TMVA/Types.h:"

        # -- Geant3 warnings
        "/geant3/TGeant3/TGeant3.h:"
        "/geant3/TGeant3/TGeant3TGeo.h:"

        # ignore warnings from Fair base classes for the timebeeing
	"FairAsciiGenerator.h:"
	"FairPrimaryGenerator.h:"
	"FairGeoMedium.h:"
	"FairDetParAsciiFileIo.h:"
	"FairParamList.h:"
	"FairDetParRootFileIo.h:"
	"FairIonGenerator.cxx:"
	"FairParIo.h:"
	"FairRtdbRun.h:"
	"FairBoxGenerator.cxx:"
	"FairDetParAsciiFileIo.cxx:"
	"FairIonGenerator.h:"
	"FairParRootFileIo.h:"
	"FairLink.cxx:"
	"FairParAsciiFileIo.h:"
	"FairShieldGenerator.h:"
	"FairShieldGenerator.cxx:"	
  )

# -----------------------------------------------------------
# -- Warning addon's
# -----------------------------------------------------------
set(CTEST_CUSTOM_WARNING_MATCH	${CTEST_CUSTOM_WARNING_MATCH}
	)
