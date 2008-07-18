/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M‰nner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl‰ﬂ
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
//     - class implements the trackfinding algorithm
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-05-14 15:48:25 $
// $Revision: 1.5 $
//
// *******************************************************************/

#include "../../MiscLIB/include/terminal.h"
#include "../include/trackfinderError.h"
#include "../include/trackfinder.h"
#include <stdio.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinder::trackfinder() {

	houghTransform = new houghTransformation();

#ifdef ERASERPEAKFINDING

	histoTransform = new eraser();

#else

	histoTransform = new maxMorphSearch();

#endif

	analyser       = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinder::trackfinder( trackfinderInputData** eventData, histogramData** histogram,
						  trackData** tracks, tables** ratings, lutImplementation** lut,
						  unsigned short firstFilterDim1ClearRadius,
						  unsigned short firstFilterDim2ClearRadius,
						  unsigned short secondFilterDim1ClearRadius,
						  unsigned short secondFilterDim2ClearRadius,
						  unsigned short secondFilterDim3ClearRadius) {

	houghTransform = new houghTransformation(eventData, histogram, lut);

#ifdef ERASERPEAKFINDING

	histoTransform = new eraser(histogram, tracks, ratings, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim3ClearRadius);

#else

	histoTransform = new maxMorphSearch(histogram, tracks, ratings, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim1ClearRadius, secondFilterDim2ClearRadius, secondFilterDim3ClearRadius);

#endif

	analyser       = NULL;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinder::~trackfinder() {

	if(histoTransform != NULL) {

		delete histoTransform;
		histoTransform = NULL;
	}

	if(houghTransform != NULL) {

		delete houghTransform;
		houghTransform = NULL;
	}

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void trackfinder::init(trackfinderInputData** eventData, histogramData** histogram,
					   trackData** tracks, tables** ratings, lutImplementation** lut,
					   unsigned short firstFilterDim1ClearRadius,
					   unsigned short firstFilterDim2ClearRadius,
					   unsigned short secondFilterDim1ClearRadius,
					   unsigned short secondFilterDim2ClearRadius,
					   unsigned short secondFilterDim3ClearRadius) {

	houghTransform->init(eventData, histogram, lut);

#ifdef ERASERPEAKFINDING

	((eraser*)histoTransform)->init(histogram, tracks, ratings, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim3ClearRadius);

#else

	((maxMorphSearch*)histoTransform)->init(histogram, tracks, ratings, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim1ClearRadius, secondFilterDim2ClearRadius, secondFilterDim3ClearRadius);

#endif

}

/****************************************************************
 * This method sets the object for analysing the trackfinding	*
 * algorithm													*
 ****************************************************************/

void trackfinder::setAnalyser(analysis* analyser) {

	this->analyser = analyser;

}

/****************************************************************
 * This applies the track-finding algorithm						*
 ****************************************************************/

void trackfinder::evaluate(std::streambuf* terminal) {

	terminalSequence statusSequence;

	if (houghTransform == NULL)
		throw cannotAccessHoughTransformError();
	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	houghTransform->resetHistogram();
	histoTransform->resetTracks();
	houghTransform->createBorders(terminal);

	createTerminalStatusSequence(&statusSequence, terminal, "Process layers:\t\t\t", houghTransform->getNumberOfHistogramLayers());
	terminalInitialize(statusSequence);

	for (unsigned short i = 0; i < houghTransform->getNumberOfHistogramLayers(); i++) {

		houghTransform->createHistogramLayer(i);
		if (analyser != NULL) {
			if (analyser->isWritingCreatedHistogramLayerEnabled()) {
				if (analyser->isWritingJustOneCreatedHistogramLayerEnabled()) {
					if (analyser->getWritingHistogramLayerIndex() == i) {
						analyser->createdHistogramLayerWrite(i);
						if (analyser->isShowingCreatedHistogramLayerEnabled())
							analyser->createdHistogramLayerShow(i);
					}
				}
				else {
					analyser->createdHistogramLayerWrite(i);
					if (analyser->getWritingHistogramLayerIndex() == i) {
						if (analyser->isShowingCreatedHistogramLayerEnabled())
							analyser->createdHistogramLayerShow(i);
					}
				}
			}
		}
		histoTransform->encodeHistogramLayer(i);
		if (analyser != NULL) {
			if (analyser->isWritingEncodedHistogramLayerEnabled()) {
				if (analyser->isWritingJustOneEncodedHistogramLayerEnabled()) {
					if (analyser->getWritingHistogramLayerIndex() == i) {
						analyser->encodedHistogramLayerWrite(i);
						if (analyser->isShowingEncodedHistogramLayerEnabled())
							analyser->encodedHistogramLayerShow(i);
					}
				}
				else {
					analyser->encodedHistogramLayerWrite(i);
					if (analyser->getWritingHistogramLayerIndex() == i) {
						if (analyser->isShowingEncodedHistogramLayerEnabled())
							analyser->encodedHistogramLayerShow(i);
					}
				}
			}
		}
		histoTransform->diagonalizeHistogramLayer(i);
		histoTransform->filterHistogramLayer(i);
		if (analyser != NULL) {
			if (analyser->isWritingFilteredHistogramLayerEnabled()) {
				if (analyser->isWritingJustOneFilteredHistogramLayerEnabled()) {
					if (analyser->getWritingHistogramLayerIndex() == i) {
						analyser->filteredHistogramLayerWrite(i);
						if (analyser->isShowingFilteredHistogramLayerEnabled())
							analyser->filteredHistogramLayerShow(i);
					}
				}
				else {
					analyser->filteredHistogramLayerWrite(i);
					if (analyser->getWritingHistogramLayerIndex() == i) {
						if (analyser->isShowingFilteredHistogramLayerEnabled())
							analyser->filteredHistogramLayerShow(i);
					}
				}
			}
		}

		histoTransform->finalizeHistogramLayer(i, terminal, &statusSequence, houghTransform->getNumberOfHistogramLayers());

		houghTransform->resetHistogramLayer();

		terminalOverwrite(statusSequence, i + 1);

	}

	terminalFinalize(statusSequence);

	if (analyser != NULL) {

		if (analyser->isNumberOfTracksInAllColumnsAnalysisEnabled())
			analyser->evaluateNumberOfTracksInAllColumns();
		if (analyser->isNumberOfTracksInAllLayersAnalysisEnabled())
			analyser->evaluateNumberOfTracksInAllLayers();

	}

	histoTransform->filterHistogram(terminal);

}
