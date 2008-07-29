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
///     - class implements the trackfinding algorithm
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-10-19 14:34:55 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDER_H
#define _TRACKFINDER_H


#include "../../DataRootObjectLIB/include/trackfinderInputData.h"
#include "../../HoughTransformationLIB/include/houghTransformation.h"
#include "../../HistogramTransformationLIB/include/eraser.h"
#include "../../HistogramTransformationLIB/include/maxMorphSearch.h"
#include "../../AnalysisLIB/include/analysis.h"


//#define ERASERPEAKFINDING		/**< Uses eraser-object instead of maxMorphSearch-object for the peakfinding */
#undef ERASERPEAKFINDING


/* **************************************************************
 * CLASS trackfinder							 				*
 * **************************************************************/

class trackfinder {

private:

	houghTransformation*     houghTransform;				/**< Object which fills the histogram by transforming the hits */
	histogramTransformation* histoTransform;				/**< Object which evaluates the tracks basing on the histogram */

#ifndef NOANALYSIS

	analysis*                analyser;						/**< Object to analyse the trackfinder internally. */

#endif

public:

/**
 * Default constructor
 */

	trackfinder();

/**
 * Constructor
 * @param eventData is an object for accessing the input data
 * @return histogram is an object for accessing the histogram
 * @return tracks is an object for accessing the tracks
 * @param ratings is the object to access the tables for the ratings
 * @param lut is an object for accessing both look-up-tables
 * @param firstFilterDim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param firstFilterDim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param secondFilterDim1ClearRadius is the region of interest for the second filter in the first dimension
 * @param secondFilterDim2ClearRadius is the region of interest for the second filter in the second dimension
 * @param secondFilterDim3ClearRadius is the region of interest for the second filter in the third dimension
 */

	trackfinder(trackfinderInputData** eventData, histogramData** histogram,
			    trackData** tracks, tables** ratings, lutImplementation** lut,
				unsigned short firstFilterDim1ClearRadius,
				unsigned short firstFilterDim2ClearRadius,
				unsigned short secondFilterDim1ClearRadius,
				unsigned short secondFilterDim2ClearRadius,
				unsigned short secondFilterDim3ClearRadius);


/**
 * Destructor
 */

	virtual ~trackfinder();

/**
 * Method inits the variables based on the detector stations.
 * @param eventData is an object for accessing the input data
 * @return histogram is an object for accessing the histogram
 * @return tracks is an object for accessing the tracks
 * @param ratings is the object to access the tables for the ratings
 * @param lut is an object for accessing both look-up-tables
 * @param firstFilterDim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param firstFilterDim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param secondFilterDim1ClearRadius is the region of interest for the second filter in the first dimension
 * @param secondFilterDim2ClearRadius is the region of interest for the second filter in the second dimension
 * @param secondFilterDim3ClearRadius is the region of interest for the second filter in the third dimension
 */

	void init(trackfinderInputData** eventData, histogramData** histogram,
			  trackData** tracks, tables** ratings, lutImplementation** lut,
			  unsigned short firstFilterDim1ClearRadius,
			  unsigned short firstFilterDim2ClearRadius,
			  unsigned short secondFilterDim1ClearRadius,
			  unsigned short secondFilterDim2ClearRadius,
			  unsigned short secondFilterDim3ClearRadius);

/**
 * This method sets the object for analysing the trackfinding algorithm
 * @param analyser is a pointer to the analysis object which has the ability to analyze the algorithm
 * @warning The analysis object must have the functionality of: writeCreatedHistogramLayer(), writeEncodedHistogramLayer() and writeFilteredHistogramLayer()
 */

	void setAnalyser(analysis* analyser = NULL);

/**
 * This applies the track-finding algorithm
 * @param terminal is a buffer to place the process information
 */

	void evaluate(std::streambuf* terminal = NULL);

/**
 * This method returns the trackIndex of the debugged track,
 * if DEBUGJUSTONEGOODTRACK is enabled.
 */

	int getDebugTrackIndex();

};

#endif
