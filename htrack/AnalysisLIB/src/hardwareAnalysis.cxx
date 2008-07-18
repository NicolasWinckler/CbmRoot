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
// $Date: 2007-04-26 15:40:46 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/hardwareAnalysis.h"
#include <stdio.h>


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 


/****************************************************************
 * Default Constructor											*
 ****************************************************************/

hardwareAnalysis::hardwareAnalysis() {

	isTracksPerColumnEnabled         = false;
	numberOfColumnAnalysis           = 0;
	numberOfTracksInAllColumnEntries = 0;
	minimalNumberOfTracksPerColumn   = NULL;
	averageNumberOfTracksPerColumn   = NULL;
	maximalNumberOfTracksPerColumn   = NULL;
	isTracksPerLayerEnabled          = false;
	numberOfLayerAnalysis            = 0;
	numberOfTracksInAllLayerEntries  = 0;
	numberOfTracksPerLayer           = NULL;
	numberOfTrackDensitiesPerLayer   = NULL;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hardwareAnalysis::~hardwareAnalysis() {

	if (minimalNumberOfTracksPerColumn != NULL) {
		free(minimalNumberOfTracksPerColumn);
		minimalNumberOfTracksPerColumn = NULL;
	}

	if (averageNumberOfTracksPerColumn != NULL) {
		free(averageNumberOfTracksPerColumn);
		averageNumberOfTracksPerColumn = NULL;
	}

	if (maximalNumberOfTracksPerColumn != NULL) {
		free(maximalNumberOfTracksPerColumn);
		maximalNumberOfTracksPerColumn = NULL;
	}

	if (numberOfTracksPerLayer != NULL) {
		free(numberOfTracksPerLayer);
		numberOfTracksPerLayer = NULL;
	}

	if (numberOfTrackDensitiesPerLayer != NULL) {
		free(numberOfTrackDensitiesPerLayer);
		numberOfTrackDensitiesPerLayer = NULL;
	}

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all columns of the histogram layers is				*
 * enabled or not												*
 ****************************************************************/

bool hardwareAnalysis::isNumberOfTracksInAllColumnsAnalysisEnabled() {

	return isTracksPerColumnEnabled;

}

/****************************************************************
 * method returns the number of actually made column anaylsis	*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfColumnAnaylsis() {

	return numberOfColumnAnalysis;

}

/****************************************************************
 * method returns the number of entries in the distribution		*
 * for all number of tracks per column in all layers			*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfTracksInAllColumnEntries() {

	return numberOfTracksInAllColumnEntries;

}

/****************************************************************
 * method returns the distribution for the minimal number		*
 * of tracks per column in all layers							*
 ****************************************************************/

unsigned int* hardwareAnalysis::getMinimalNumberOfTracksInAllColumnsDistribution() {

	return minimalNumberOfTracksPerColumn;

}

/****************************************************************
 * method returns the distribution for the average number		*
 * of tracks per column in all layers							*
 ****************************************************************/

double* hardwareAnalysis::getAverageNumberOfTracksInAllColumnsDistribution() {

	return averageNumberOfTracksPerColumn;

}

/****************************************************************
 * method returns the distribution for the maximal number		*
 * of tracks per column in all layers							*
 ****************************************************************/

unsigned int* hardwareAnalysis::getMaximalNumberOfTracksInAllColumnsDistribution() {

	return maximalNumberOfTracksPerColumn;

}

/****************************************************************
 * method initializes the hardwareTracksPerColumn-Analysis.		*
 ****************************************************************/

void hardwareAnalysis::initHardwareTracksPerColumnAnalysis(unsigned int numberOfTracksInAllColumnEntries, bool enable) {

	this->numberOfColumnAnalysis           = 0;
	this->numberOfTracksInAllColumnEntries = 0;

	if (isTracksPerColumnEnabled) {

		if (minimalNumberOfTracksPerColumn != NULL) {
			free(minimalNumberOfTracksPerColumn);
			minimalNumberOfTracksPerColumn = NULL;
		}

		if (averageNumberOfTracksPerColumn != NULL) {
			free(averageNumberOfTracksPerColumn);
			averageNumberOfTracksPerColumn = NULL;
		}

		if (maximalNumberOfTracksPerColumn != NULL) {
			free(maximalNumberOfTracksPerColumn);
			maximalNumberOfTracksPerColumn = NULL;
		}

	}

	isTracksPerColumnEnabled = enable;

	if (isTracksPerColumnEnabled) {

		this->numberOfTracksInAllColumnEntries = numberOfTracksInAllColumnEntries;

		minimalNumberOfTracksPerColumn = (unsigned int*)calloc(this->numberOfTracksInAllColumnEntries, sizeof(unsigned int));
		if (minimalNumberOfTracksPerColumn == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		averageNumberOfTracksPerColumn = (double*)calloc(this->numberOfTracksInAllColumnEntries, sizeof(double));
		if (averageNumberOfTracksPerColumn == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		maximalNumberOfTracksPerColumn = (unsigned int*)calloc(this->numberOfTracksInAllColumnEntries, sizeof(unsigned int));
		if (maximalNumberOfTracksPerColumn == NULL)
			throw memoryAllocationError(ANALYSISLIB);

	}

}

/****************************************************************
 * This method evaluates the minimal, the maximal and			*
 * the average number of tracks in all columns of the histogram.*
 ****************************************************************/

void hardwareAnalysis::evaluateNumberOfTracksInAllColumns(trackData* tracks) {

	unsigned int  j;
	unsigned int* actualNumberOfTracks;
	unsigned int* averageNumberOfTracks;

	if (tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);
	
	if (numberOfTracksInAllColumnEntries > 0) {
		actualNumberOfTracks  = (unsigned int*)calloc(numberOfTracksInAllColumnEntries, sizeof(unsigned int));
		averageNumberOfTracks = (unsigned int*)calloc(numberOfTracksInAllColumnEntries, sizeof(unsigned int));
	}
	else{
		actualNumberOfTracks  = NULL;
		averageNumberOfTracks = NULL;
	}
	if (actualNumberOfTracks == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	if (averageNumberOfTracks == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (unsigned short i = 0; i < tracks->getNumberOfLayers(); i++) {

		for (j = 0; j < numberOfTracksInAllColumnEntries; j++)
			actualNumberOfTracks[j] = 0;

		for (trackLayer::iterator actualTrack = tracks->getTrackIteratorBeginOfHistogramLayer(i); actualTrack != tracks->getTrackIteratorEndOfHistogramLayer(i); actualTrack++)
			if (actualTrack->position.get(DIM1) < numberOfTracksInAllColumnEntries)
				actualNumberOfTracks[actualTrack->position.get(DIM1)]++;

		for (j = 0; j < numberOfTracksInAllColumnEntries; j++) {

			if (i == 0) {

				averageNumberOfTracks[j]              = actualNumberOfTracks[j];
				
				if (numberOfColumnAnalysis == 0) {

					minimalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

					maximalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

				}
				else {

					if (minimalNumberOfTracksPerColumn[j] > actualNumberOfTracks[j])
						minimalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

					if (maximalNumberOfTracksPerColumn[j] < actualNumberOfTracks[j])
						maximalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

				}

			}
			else {

				averageNumberOfTracks[j]              += actualNumberOfTracks[j];

				if (minimalNumberOfTracksPerColumn[j] > actualNumberOfTracks[j])
					minimalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

				if (maximalNumberOfTracksPerColumn[j] < actualNumberOfTracks[j])
					maximalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

			}

		}

	}

	for (j = 0; j < numberOfTracksInAllColumnEntries; j++) {

		if (numberOfColumnAnalysis == 0)
			averageNumberOfTracksPerColumn[j]  = (double)averageNumberOfTracks[j] / (double)tracks->getNumberOfLayers();
		else
			averageNumberOfTracksPerColumn[j] += (double)averageNumberOfTracks[j] / (double)tracks->getNumberOfLayers();

	}

	if (actualNumberOfTracks != NULL) {
		free(actualNumberOfTracks);
		actualNumberOfTracks = NULL;
	}
	if (averageNumberOfTracks != NULL) {
		free(averageNumberOfTracks);
		averageNumberOfTracks = NULL;
	}

	numberOfColumnAnalysis++;

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all histogram layers is enabled or not				*
 ****************************************************************/

bool hardwareAnalysis::isNumberOfTracksInAllLayersAnalysisEnabled() {

	return isTracksPerLayerEnabled;

}

/****************************************************************
 * method returns the number of actually made column anaylsis	*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfLayerAnaylsis() {

	return numberOfLayerAnalysis;

}

/****************************************************************
 * method returns the number of entries in the distribution		*
 * for all number of tracks in layers							*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfTracksInAllLayersEntries() {

	return numberOfTracksInAllLayerEntries;

}

/****************************************************************
 * method returns the distribution for the number				*
 * of tracks per layers											*
 ****************************************************************/

unsigned int* hardwareAnalysis::getNumberOfTracksPerLayerDistribution() {

	return numberOfTracksPerLayer;

}

/****************************************************************
 * method returns the distribution for the number				*
 * of track densities per layer									*
 ****************************************************************/

int* hardwareAnalysis::getNumberOfTrackDensitiesPerLayerDistribution() {

	return numberOfTrackDensitiesPerLayer;

}

/****************************************************************
 * method initializes the hardwareTracksPerLayer-Analysis.		*
 ****************************************************************/

void hardwareAnalysis::initHardwareTracksPerLayerAnalysis(unsigned int numberOfTracksInAllLayerEntries, bool enable) {

	this->numberOfLayerAnalysis           = 0;
	this->numberOfTracksInAllLayerEntries = 0;

	if (isTracksPerLayerEnabled) {

		if (numberOfTracksPerLayer != NULL) {
			free(numberOfTracksPerLayer);
			numberOfTracksPerLayer = NULL;
		}

		if (numberOfTrackDensitiesPerLayer != NULL) {
			free(numberOfTrackDensitiesPerLayer);
			numberOfTrackDensitiesPerLayer = NULL;
		}

	}

	isTracksPerLayerEnabled = enable;

	if (isTracksPerLayerEnabled) {

		this->numberOfTracksInAllLayerEntries = numberOfTracksInAllLayerEntries;

		numberOfTracksPerLayer = (unsigned int*)calloc(this->numberOfTracksInAllLayerEntries, sizeof(unsigned int));
		if (numberOfTracksPerLayer == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		numberOfTrackDensitiesPerLayer = (int*)calloc(this->numberOfTracksInAllLayerEntries, sizeof(int));
		if (numberOfTrackDensitiesPerLayer == NULL)
			throw memoryAllocationError(ANALYSISLIB);

	}

}

/****************************************************************
 * This method evaluates the distribution of the number of		*
 * tracks in all layers of the histogram.						*
 ****************************************************************/

void hardwareAnalysis::evaluateNumberOfTracksInAllLayers(trackData* tracks) {

	if (tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);

	for (unsigned int i = 0; i < min(numberOfTracksInAllLayerEntries, tracks->getNumberOfLayers()); i++) {

		if (numberOfLayerAnalysis == 0)
			numberOfTracksPerLayer[i]          = tracks->getNumberOfTracksOfHistogramLayer(i);
		else
			numberOfTracksPerLayer[i]         += tracks->getNumberOfTracksOfHistogramLayer(i);

		if (i == 0)
			numberOfTrackDensitiesPerLayer[i]  = (int)numberOfTracksPerLayer[i] - 0;
		else
			numberOfTrackDensitiesPerLayer[i]  = (int)numberOfTracksPerLayer[i] - (int)numberOfTracksPerLayer[i - 1];

	}

	numberOfLayerAnalysis++;

}
