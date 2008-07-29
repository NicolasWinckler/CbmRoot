//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M‰nner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl‰ﬂ
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - implements methods to display the prelut ranges analysis results
///       with regard to the range for the main layer
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-12-13 13:47:20 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/prelutRangeAnalysisDef.h"
#include "../include/prelutRangeMainLayerAnalysis.h"


/* **************************************************************
 * CLASS prelutRangeMainLayerAnalysis							*
 * **************************************************************/

/****************************************************************
 * This method returns the display name							*
 ****************************************************************/

std::string prelutRangeMainLayerAnalysis::getDisplayName() {

	return PRELUTRANGEMAINLAYERNAME;

}

/****************************************************************
 * This method returns the display title						*
 ****************************************************************/

std::string prelutRangeMainLayerAnalysis::getDisplayTitle() {

	return RELUTRANGEMAINLAYERTITLE;

}

/****************************************************************
 * This method returns the window name							*
 ****************************************************************/

std::string prelutRangeMainLayerAnalysis::getWindowName() {

	return PRELUTRANGEMAINLAYERWINDOWNAME;

}

/****************************************************************
 * This method returns the window name							*
 ****************************************************************/

std::string prelutRangeMainLayerAnalysis::getWindowTitle() {

	return PRELUTRANGEMAINLAYERWINDOWTITLE;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelutRangeMainLayerAnalysis::prelutRangeMainLayerAnalysis() : prelutRangeAnalysis() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutRangeMainLayerAnalysis::prelutRangeMainLayerAnalysis(unsigned short numberOfDisplays) : prelutRangeAnalysis(numberOfDisplays) {

}
prelutRangeMainLayerAnalysis::prelutRangeMainLayerAnalysis(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) : prelutRangeAnalysis(prelutRangeCut, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax) {

}
prelutRangeMainLayerAnalysis::prelutRangeMainLayerAnalysis(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) : prelutRangeAnalysis(prelutRangeCut, numberOfDisplays, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutRangeMainLayerAnalysis::~prelutRangeMainLayerAnalysis() {

}
