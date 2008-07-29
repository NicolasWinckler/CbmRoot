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
///       with regard to the range for the correct layer
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-12-13 13:47:19 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/prelutRangeAnalysisDef.h"
#include "../include/prelutRangeCorrectLayerAnalysis.h"


/* **************************************************************
 * CLASS prelutRangeCorrectLayerAnalysis						*
 * **************************************************************/

/****************************************************************
 * This method returns the display name							*
 ****************************************************************/

std::string prelutRangeCorrectLayerAnalysis::getDisplayName() {

	return PRELUTRANGECORRETLAYERNAME;

}

/****************************************************************
 * This method returns the display title						*
 ****************************************************************/

	std::string prelutRangeCorrectLayerAnalysis::getDisplayTitle() {

	return PRELUTRANGECORRECTLYERTITLE;

}

/****************************************************************
 * This method returns the window name							*
 ****************************************************************/

std::string prelutRangeCorrectLayerAnalysis::getWindowName() {

	return PRELUTRANGECORRECTLAYERWINDOWNAME;

}

/****************************************************************
 * This method returns the window name							*
 ****************************************************************/

std::string prelutRangeCorrectLayerAnalysis::getWindowTitle() {

	return PRELUTRANGECORRECTLAYERWINDOWTITLE;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelutRangeCorrectLayerAnalysis::prelutRangeCorrectLayerAnalysis() : prelutRangeAnalysis() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutRangeCorrectLayerAnalysis::prelutRangeCorrectLayerAnalysis(unsigned short numberOfDisplays) : prelutRangeAnalysis(numberOfDisplays) {

}
prelutRangeCorrectLayerAnalysis::prelutRangeCorrectLayerAnalysis(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) : prelutRangeAnalysis(prelutRangeCut, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax) {

}
prelutRangeCorrectLayerAnalysis::prelutRangeCorrectLayerAnalysis(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) : prelutRangeAnalysis(prelutRangeCut, numberOfDisplays, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutRangeCorrectLayerAnalysis::~prelutRangeCorrectLayerAnalysis() {

}
