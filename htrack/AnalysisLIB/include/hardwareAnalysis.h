/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - implements methods to analyze hardware prerequisites
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-04-26 12:50:06 $
// $Revision: 1.2 $
//
// *******************************************************************/


#ifndef _HARDWAREANALYSIS_H
#define _HARDWAREANALYSIS_H


#include "../../DataRootObjectLIB/include/trackData.h"


/* **************************************************************
 * CLASS hardwareAnalysis				 						*
 * **************************************************************/

class hardwareAnalysis {

protected:

	bool          isTracksPerColumnEnabled;				/**< Variable for enabling the evaluation of the number of tracks per column in all histogram layers. */
	unsigned int  numberOfColumnAnalysis;				/**< Variable to store the number of analysis which are made to evaluate the number of tracks per column in all histogram layers. */
	unsigned int  numberOfTracksInAllColumnEntries;		/**< Varibale to store the number of entries for all column distributions. */
	unsigned int* minimalNumberOfTracksPerColumn;		/**< Variable to store the column distribution for the minimal number of tracks in all histogram layers. */
	double*       averageNumberOfTracksPerColumn;		/**< Variable to store the column distribution for the average number of tracks in all histogram layers. */
	unsigned int* maximalNumberOfTracksPerColumn;		/**< Variable to store the column distribution for the maximal number of tracks in all histogram layers. */
	bool          isTracksPerLayerEnabled;				/**< Variable for enabling the evaluation of the number of tracks per histogram layer. */
	unsigned int  numberOfLayerAnalysis;				/**< Variable to store the number of analysis which are made to evaluate the number of tracks per histogram layer. */
	unsigned int  numberOfTracksInAllLayerEntries;		/**< Varibale to store the number of entries for all layer distributions. */
	unsigned int* numberOfTracksPerLayer;				/**< Variable to store the distribution of the number of tracks per layer. */
	int*          numberOfTrackDensitiesPerLayer;		/**< Variable to store the distribution of the track densities for the number of tracks per layer. */

public:

/**
 * Default constructor
 */

	hardwareAnalysis();

/**
 * Destructor
 */

	virtual ~hardwareAnalysis();

/**
 * method returns if the analysis for the number of
 * tracks in all columns of the histogram layers is
 * enabled or not
 */

	bool isNumberOfTracksInAllColumnsAnalysisEnabled();

/**
 * method returns the number of actually made column anaylsis
 */

	unsigned int getNumberOfColumnAnaylsis();

/**
 * method returns the number of entries in the distribution
 * for all number of tracks per column in all layers
 */

	unsigned int getNumberOfTracksInAllColumnEntries();

/**
 * method returns the distribution for the minimal number
 * of tracks per column in all layers
 */

	unsigned int* getMinimalNumberOfTracksInAllColumnsDistribution();

/**
 * method returns the distribution for the average number
 * of tracks per column in all layers
 */

	double* getAverageNumberOfTracksInAllColumnsDistribution();

/**
 * method returns the distribution for the maximal number
 * of tracks per column in all layers
 */

	unsigned int* getMaximalNumberOfTracksInAllColumnsDistribution();

/**
 * method initializes the hardwareTracksPerColumnAnalysis.
 */

	void initHardwareTracksPerColumnAnalysis(unsigned int numberOfTracksInAllColumnEntries, bool enable = true);

/**
 * This method evaluates the minimal, the maximal and
 * the average number of tracks in all columns of the histogram.
 */

	void evaluateNumberOfTracksInAllColumns(trackData* tracks);

/**
 * method returns if the analysis for the number of
 * tracks in all histogram layers is enabled or not
 */

	bool isNumberOfTracksInAllLayersAnalysisEnabled();

/**
 * method returns the number of actually made layer anaylsis
 */

	unsigned int getNumberOfLayerAnaylsis();

/**
 * method returns the number of entries in the distribution
 * for all number of tracks in layers
 */

	unsigned int getNumberOfTracksInAllLayersEntries();

/**
 * method returns the distribution for the number
 * of tracks per layers
 */

	unsigned int* getNumberOfTracksPerLayerDistribution();

/**
 * method returns the distribution for the number
 * of track densities per layer
 */

	int* getNumberOfTrackDensitiesPerLayerDistribution();

/**
 * method initializes the hardwareTracksPerLayerAnalysis.
 */

	void initHardwareTracksPerLayerAnalysis(unsigned int numberOfTracksInAllLayerEntries, bool enable = true);

/**
 * This method evaluates the distribution of the number of tracks
 * in all layers of the histogram.
 */

	void evaluateNumberOfTracksInAllLayers(trackData* tracks);

};

#endif
