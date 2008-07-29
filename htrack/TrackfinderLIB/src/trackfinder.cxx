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
// $Date: 2008-06-26 13:01:08 $
// $Revision: 1.8 $
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

#ifndef NOANALYSIS

	analyser       = NULL;

#endif

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

#ifndef NOANALYSIS

	analyser       = NULL;

#endif

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

#ifndef NOANALYSIS

	this->analyser = analyser;

#endif

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


#ifndef NOANALYSIS

	if (analyser != NULL) {
		if (analyser->isTimeAnalysisEnabled()) {
			analyser->borderCreationTimerReset();
			analyser->histogramCreationTimerReset();
			analyser->histogramEncodingTimerReset();
			analyser->histogramDiagonalizingTimerReset();
			analyser->histogramPeakfindingTimerReset();
			analyser->histogramFinalizingTimerReset();
			analyser->histogramResettingTimerReset();
			analyser->trackPeakfindingTimerReset();
		}
	}

#endif

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isTimeAnalysisEnabled())
			analyser->borderCreationTimerStart(false);

#endif

#ifndef NOANALYSIS

	houghTransform->createBorders(analyser, terminal);

#else

	houghTransform->createBorders(terminal);

#endif

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isTimeAnalysisEnabled())
			analyser->borderCreationTimerStop();

#endif

	createTerminalStatusSequence(&statusSequence, terminal, "Process layers:\t\t\t\t\t", houghTransform->getNumberOfHistogramLayers());
	terminalInitialize(statusSequence);

	for (unsigned short i = 0; i < houghTransform->getNumberOfHistogramLayers(); i++) {

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramCreationTimerStart(false);

#endif

		houghTransform->createHistogramLayer(i);

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramCreationTimerStop();

#endif

#ifndef NOANALYSIS

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

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramEncodingTimerStart(false);

#endif

		histoTransform->encodeHistogramLayer(i);

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramEncodingTimerStop();

#endif

#ifndef NOANALYSIS

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

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramDiagonalizingTimerStart(false);

#endif

		histoTransform->diagonalizeHistogramLayer(i);

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramDiagonalizingTimerStop();

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramPeakfindingTimerStart(false);

#endif

		histoTransform->filterHistogramLayer(i);

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramPeakfindingTimerStop();

#endif

#ifndef NOANALYSIS

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

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramFinalizingTimerStart(false);

#endif

		histoTransform->serializeHistogramLayer(i, terminal, &statusSequence, houghTransform->getNumberOfHistogramLayers());

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramFinalizingTimerStop();

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramResettingTimerStart(false);

#endif

		houghTransform->resetHistogramLayer();

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramResettingTimerStop();

#endif

		terminalOverwrite(statusSequence, i + 1);

	}

	terminalFinalize(statusSequence);

#ifndef NOANALYSIS

	if (analyser != NULL) {

		if (analyser->isNumberOfTracksInAllColumnsAnalysisEnabled())
			analyser->evaluateNumberOfTracksInAllColumns(terminal);
		if (analyser->isNumberOfTracksInAllRowsAnalysisEnabled())
			analyser->evaluateNumberOfTracksInAllRows(terminal);
		if (analyser->isNumberOfTracksInAllLayersAnalysisEnabled())
			analyser->evaluateNumberOfTracksInAllLayers(terminal);

	}

#endif

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isMemoryAnalysisEnabled())
			analyser->evaluateSizeOfLBufferData();

#endif

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isTimeAnalysisEnabled())
			analyser->trackPeakfindingTimerStart(false);

#endif

	histoTransform->filterHistogram(terminal);

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isTimeAnalysisEnabled())
			analyser->trackPeakfindingTimerStop();

#endif

}

/****************************************************************
 * This method returns the trackIndex of the debugged track,	*
 * if DEBUGJUSTONEGOODTRACK is enabled.							*
 ****************************************************************/

int trackfinder::getDebugTrackIndex() {

	int returnValue;

	if (houghTransform != NULL)
		returnValue = houghTransform->getDebugTrackIndex();
	else
		returnValue = INVALIDTRACKINDEX;

	return returnValue;

}
