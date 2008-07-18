/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�ner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl�
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
//     - implements methods to analyze the trackfinding algorithm's quality
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-06-21 13:20:04 $
// $Revision: 1.25 $
//
// *******************************************************************/

#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/terminal.h"
#include "../../DataObjectLIB/include/analyticFormula.h"
#include "../../DataObjectLIB/include/histogramCellSignatures.h"
#include "../include/analysisError.h"
#include "../include/analysisWarningMsg.h"
#include "../include/analysisDef.h"
#include "../include/analysis.h"
#include <malloc.h>
#include <stdio.h>


#define NUMBEROFDIFFERENTMOMENTAS    100			/**< This definition allows to look in Hough transform goodness to the percentage of wrong momentas. */
#define NUMBEROFDIFFERENTCORRECTHITS 10				/**< This definition allows to look in prelut goodness to the percentage of hits, which meet the correct border in the histogram. */
#define STARTSLOPE                   0.003			/**< This definition sets for the prelut goodness the allowed slope range for the first station */
#define INCRSLOPE                    0.001			/**< This definition sets for the prelut goodness the incremental slope range for the stations after the first one */


#define sqr(a)  ((a) * (a)) 


typedef struct {

	unsigned short frequency;
	double         value;

} stationMinimum;


typedef struct {

	unsigned short border;
	unsigned short frequency;
	double         distance;

} hitsInHistogramBorderFrequency;


/****************************************************************
 * method initializes the showAnalysis-object.					*
 ****************************************************************/

void analysis::initializeShowAnalysis() {

	if (showAnalyser == NULL)
		showAnalyser = new showAnalysis();

	if (showAnalyser == NULL)
		throw memoryAllocationError(ANALYSISLIB);


}


/****************************************************************
 * method initializes the totalAnalysis-object.					*
 ****************************************************************/

void analysis::initializeTotalAnalysis() {

	if (totalAnalyser == NULL)
		totalAnalyser = new totalAnalysis();

	if (totalAnalyser == NULL)
		throw memoryAllocationError(ANALYSISLIB);


}

/****************************************************************
 * method initializes the algorithm-evaluation.					*
 ****************************************************************/

void analysis::initializeAlgorithmEvaluation() {

	if (qualityAnalyser != NULL)
		qualityAnalyser->initializeEvaluation();

	if (momentumAnalyser != NULL)
		momentumAnalyser->initializeEvaluation();

	if (projectionAnalyser != NULL)
		projectionAnalyser->initializeEvaluation();

}

/****************************************************************
 * method initializes the magnetfield-evaluation.				*
 ****************************************************************/

void analysis::initializeMagnetfieldEvaluation() {

	if (magnetfieldAnalyser != NULL)
		magnetfieldAnalyser->initializeEvaluation();


}

/****************************************************************
 * method finalizes the algorithm-evaluation.					*
 ****************************************************************/

void analysis::finalizeAlgorithmEvaluation() {

	if (qualityAnalyser != NULL)
		qualityAnalyser->finalizeEvaluation(getNumberOfTracksWithP());

	if (momentumAnalyser != NULL)
		momentumAnalyser->finalizeEvaluation();

	if (projectionAnalyser != NULL)
		projectionAnalyser->finalizeEvaluation();

}

/****************************************************************
 * method finalizes the magnetfieldAnalyser-evaluation.			*
 ****************************************************************/

void analysis::finalizeMagnetfieldEvaluation() {

	if (magnetfieldAnalyser != NULL)
		magnetfieldAnalyser->finalizeEvaluation();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::peakToFakeTrack(trackCoordinates& coordinates, trackParameter& parameter) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (qualityAnalyser != NULL)
		qualityAnalyser->peakToFakeTrack();

	if (momentumAnalyser != NULL)
		momentumAnalyser->peakToFakeTrack(parameter, *(*space));

	if (projectionAnalyser != NULL)
		projectionAnalyser->peakToFakeTrack(coordinates);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::peakToNoTrack(trackCoordinates& coordinates, trackParameter& parameter) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (qualityAnalyser != NULL)
		qualityAnalyser->peakToNoTrack();

	if (momentumAnalyser != NULL)
		momentumAnalyser->peakToNoTrack(parameter, *(*space));

	if (projectionAnalyser != NULL)
		projectionAnalyser->peakToNoTrack(coordinates);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::peakToOneTrack(trackCoordinates& coordinates, trackParameter& parameter, bool isClone, bool isWrong) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (qualityAnalyser != NULL)
		qualityAnalyser->peakToOneTrack(isClone, isWrong);

	if (momentumAnalyser != NULL)
		momentumAnalyser->peakToOneTrack(parameter, *(*space), isClone, isWrong);

	if (projectionAnalyser != NULL)
		projectionAnalyser->peakToOneTrack(coordinates, isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::peakToMoreTracks(trackCoordinates& coordinates, trackParameter& parameter, bool isClone, bool isWrong) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (qualityAnalyser != NULL)
		qualityAnalyser->peakToMoreTracks(isClone, isWrong);

	if (momentumAnalyser != NULL)
		momentumAnalyser->peakToMoreTracks(parameter, *(*space), isClone, isWrong);

	if (projectionAnalyser != NULL)
		projectionAnalyser->peakToMoreTracks(coordinates, isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::trackToNoPeak(trackfinderInputTrack& actualTrack) {

	trackCoordinates actualCoordinates;

	if (qualityAnalyser != NULL)
		qualityAnalyser->trackToNoPeak();

	if (momentumAnalyser != NULL)
		momentumAnalyser->trackToNoPeak(actualTrack);

	if (projectionAnalyser != NULL) {

		if (computeCoordinates(actualTrack, &actualCoordinates))
			projectionAnalyser->trackToNoPeak(actualCoordinates);
	
	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::trackToOnePeak(trackfinderInputTrack& actualTrack) {

	if (qualityAnalyser != NULL)
		qualityAnalyser->trackToOnePeak();

	if (momentumAnalyser != NULL)
		momentumAnalyser->trackToOnePeak(actualTrack);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::trackToMorePeaks(trackfinderInputTrack& actualTrack) {

	if (qualityAnalyser != NULL)
		qualityAnalyser->trackToMorePeaks();

	if (momentumAnalyser != NULL)
		momentumAnalyser->trackToMorePeaks(actualTrack);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::wellFoundTracks(trackfinderInputTrack& actualTrack) {

	if (qualityAnalyser != NULL)
		qualityAnalyser->wellFoundTracks();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::wrongFoundTracks(trackfinderInputTrack& actualTrack) {

	if (qualityAnalyser != NULL)
		qualityAnalyser->wrongFoundTracks();

}

/****************************************************************
 * This method evaluates if the given pattern is a track		*
 * pattern and returns the peakclass.							*
 ****************************************************************/

bitArray analysis::evaluateTrackPatternGradingP(bitArray& pattern) {

	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	return (*ratings)->getGradingPTableClassification(pattern);

}

/****************************************************************
 * This method evaluates if the given pattern is a track		*
 * pattern and returns the peakclass.							*
 ****************************************************************/

bitArray analysis::evaluateTrackPatternGradingR(bitArray& pattern) {

	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	return (*ratings)->getGradingRTableClassification(pattern);

}

/****************************************************************
 * This method evaluates if the given pattern is a track		*
 * pattern and returns the peakclass.							*
 ****************************************************************/

bitArray analysis::evaluateTrackPatternCoding(bitArray& pattern) {

	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	return (*ratings)->getCodingTableClassification(pattern);

}

/****************************************************************
 * This method evaluates the number of hits for a given real	*
 * track id.													*
 ****************************************************************/

unsigned short analysis::evaluateNumberOfRealTrackHits(trackfinderInputTrack* actualTrack) {

	trackfinderInputHit* hit;
	bitArray             trackPattern;
	unsigned short       returnValue;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (actualTrack == NULL)
		throw cannotAccessHitsOrTracksError(ANALYSISLIB);

	trackPattern.reset();
	actualTrack->resetHitPointer();
	for (unsigned short i = 0; i < actualTrack->getNumberOfHits(); i++) {

		hit = actualTrack->getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (hit->getStation() == NULL)
			throw cannotAccessStationError(ANALYSISLIB);

		trackPattern[hit->getStation()->getIndex()] = true;

	}

	returnValue = evaluateNumberOfTrackHits(trackPattern);

	return returnValue;

}

/****************************************************************
 * This method evaluates the number of hits for a given pattern	*
 * which represents a track.									*
 ****************************************************************/

unsigned short analysis::evaluateNumberOfTrackHits(bitArray pattern) {

	return (unsigned short)pattern.count();

}

/****************************************************************
 * This method evaluates if the track is a  real track which	*
 * should be found.												*
 ****************************************************************/

bool analysis::evaluateFoundTrackIsRealTrack(trackfinderInputTrack* actualTrack) {

	bitArray             trackPattern;
	unsigned short       i;
	trackfinderInputHit* hit;
	bool                 returnValue = true;

#ifndef FOUNDTRACKISREALTRACKCOMPTABILITYMODE

	bitArray comparator;

#endif

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (actualTrack == NULL)
		throw cannotAccessHitsOrTracksError(ANALYSISLIB);

	trackPattern.reset();
	actualTrack->resetHitPointer();
	for (i = 0; i < actualTrack->getNumberOfHits(); i++) {

		hit = actualTrack->getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (hit->getStation() == NULL)
			throw cannotAccessStationError(ANALYSISLIB);

		trackPattern[hit->getStation()->getIndex()] = true;

	}

#ifdef FOUNDTRACKISREALTRACKCOMPTABILITYMODE

	for (i = 1; i < actualTrack->getNumberOfHits() - 2; i++)
		if (!trackPattern[i])
			returnValue = false;

#else

	comparator = bitArray(0);

	if (evaluateTrackPatternGradingP(trackPattern) == comparator)
		returnValue = false;

#endif

	return returnValue;

}

/****************************************************************
 * This method evaluates the track distribution of a peak.		*
 ****************************************************************/

void analysis::evaluatePeakDistribution(trackDigitalInformation& track, specialListMem<trackHitMem>* possibleTracks) {

#ifndef NOANALYSIS

	trackHitMem          actualTrack;
	trackfinderInputHit* hit;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	for (unsigned int i = 0; i < track.hits.getNumberOfMemories(); i++) {

		track.hits[i].resetActiveObject();
		for(unsigned short j = 0; j < track.hits[i].getNumberOfEntries(); j++) {

			actualTrack.removeAllHits();

			hit = track.hits[i].readActiveObjectAndMakeNextOneActive();

			actualTrack.addHit(hit);
		
			if (possibleTracks->isFound(actualTrack, true))		/* track is old track */
				possibleTracks->getActiveObject()->addHit(hit);
			else												/* track not found, so add it; fakes have no track => they are always not found */
				possibleTracks->push(actualTrack);

		}

	}

	actualTrack.removeAllHits();

#endif

}

/****************************************************************
 * This method evaluates the category of a found track.			*
 ****************************************************************/

void analysis::evaluateFoundTracks(trackfinderInputTrack* track, unsigned short maximumHits, bool* isClone, bool* isWrong) {

	bool allowWrong = true;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (track == NULL)
		throw cannotAccessHitsOrTracksError(ANALYSISLIB);

	/* peak is a clone or not */
	if ((track->getMomZ() > minimumP) && (evaluateFoundTrackIsRealTrack(track))) {

		allowWrong = false;
		*isWrong   = false;
				
		if ((track->getFrequency() > 0) && (*isClone))
			*isClone = true;	/* mark peak as clone (priority 2) */
		else
			*isClone = false;	/* mark peak as real track (priority 1) */
			
	}
	else
		if (allowWrong)
			*isWrong = true;	/* mark peak as wrong track (priority 3) */
			
	/* save track */
	track->incrementFrequency();
	track->modifyMaximumNumberOfHits(maximumHits);

}

/****************************************************************
 * This method evaluates how many tracks contribute to a peak.	*
 ****************************************************************/

unsigned short analysis::evaluatePeakContribution(trackDigitalInformation& track, bool* isClone, bool* isWrong, bool* isFake) {

	specialListMem<trackHitMem> possibleTracks;
	trackHitMem                 actualTrack;
	bitArray                    trackPattern;
	trackfinderInputTrack*      realTrack;
	bitArray                    actualTrackPattern;
	bitArray                    actualClass;
	bitArray                    minimumClass;
	unsigned short              maximumHits;
	unsigned short              returnValue = 0;

#ifdef PEAKCONTRIBUTIONCOMPTABILITYMODE

	trackfinderInputTrack*      maximumTrack;
	bitArray                    maximumClass;
	bool                        maximumFound = false;

#endif

	*isClone   = true;
	*isWrong   = false;
	realTrack  = NULL;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	possibleTracks.clear();
	evaluatePeakDistribution(track, &possibleTracks);

	trackPattern.reset();
	possibleTracks.resetActiveObject();
	minimumClass = bitArray(0);
	for (unsigned long i = 0; i < possibleTracks.getNumberOfEntries(); i++) {

		actualTrack        = possibleTracks.readActiveObjectAndMakeNextOneActive();

		actualTrackPattern = actualTrack.getTrackPattern();

		actualClass        = evaluateTrackPatternGradingR(actualTrackPattern);

		realTrack          = actualTrack.getRealTrack();

		if (realTrack != NULL)
			trackPattern |= actualTrackPattern;

		if (actualClass > minimumClass) {

#ifdef PEAKCONTRIBUTIONCOMPTABILITYMODE

			if (maximumFound) {

				if (actualClass > maximumClass) {

					maximumClass = actualClass;
					maximumTrack = realTrack;
					maximumHits  = evaluateNumberOfTrackHits(actualTrackPattern);

				}

			}
			else {

				maximumClass = actualClass;
				maximumTrack = realTrack;
				maximumHits  = evaluateNumberOfTrackHits(actualTrackPattern);
				returnValue  = 1;
				maximumFound = true;

			}

#else

			returnValue++;
	
			maximumHits = evaluateNumberOfTrackHits(actualTrackPattern);
			if (realTrack != NULL)
				evaluateFoundTracks(realTrack, maximumHits, isClone, isWrong);
			else
				throw fakePassingGradingRError();

#endif

		}

	}

	if (evaluateTrackPatternCoding(trackPattern) > minimumClass)
		*isFake = false;
	else
		*isFake = true;

#ifdef PEAKCONTRIBUTIONCOMPTABILITYMODE

	if (returnValue == 1) {

		if (maximumTrack != NULL)
			evaluateFoundTracks(maximumTrack, maximumHits, isClone, isWrong);
		else
			throw fakePassingGradingRError();

	}

#endif

	return returnValue;

}

/****************************************************************
 * method computes the coordinates in the Hough space for a		*
 * track.														*
 ****************************************************************/

bool analysis::computeCoordinates(trackfinderInputTrack& track, trackCoordinates* coordinates) {
	
	bool                   returnValue;
	trackMomentum          actualMomentum;
	analyticFormula        formula;
	trackParameter         actualParameter;
	trackCoordinates       actualPosition;
	trackCoordinates       estimatedPosition;
	trackCoordinates       bestPosition;
	double                 estimatedMomentumDifferenceQuad;
	double                 bestMomentumDifferenceQuad;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (coordinates == NULL)
		throw cannotAccessTrackCoordinatesError();

	returnValue = false;

	/* setup the correct momentum */
	actualMomentum.set(track.getMomX(), PX);
	actualMomentum.set(track.getMomY(), PY);
	actualMomentum.set(track.getMomZ(), PZ);

	/* compute the coordinates for the correct momentum */
	formula.evaluateHough(actualMomentum, track.getCharge(), *(*space), &actualPosition);

	/* CAUTION:
	 * The actualPosition is always rounded to the next value in each dimension above. So a
	 * correction of the position must be applied because one cannot round each dimension independant of the others.
	 */

	/* compute the estimated momentum for the computed coordinates */
	formula.evaluateP(actualPosition, *(*space), &actualMomentum);

	/* compute the quadrat of the phytagoras difference for the correct and the estimated momentum */
	bestMomentumDifferenceQuad = sqr(track.getMomX() - actualMomentum.get(DIM1)) + sqr(track.getMomY() - actualMomentum.get(DIM2)) + sqr(track.getMomZ() - actualMomentum.get(DIM3));

	/* set the estimated position with the actual minimal difference */
	bestPosition = actualPosition;

	/* search the neighbourhood for a smaller difference */
	for (short neighbourhoodDim1 = -1; neighbourhoodDim1 <= 1; neighbourhoodDim1++) {
		for (short neighbourhoodDim2 = -1; neighbourhoodDim2 <= 1; neighbourhoodDim2++) {
			for (short neighbourhoodDim3 = -1; neighbourhoodDim3 <= 1; neighbourhoodDim3++) {

				estimatedPosition.set(actualPosition.get(DIM1) + neighbourhoodDim1, DIM1);
				estimatedPosition.set(actualPosition.get(DIM2) + neighbourhoodDim2, DIM2);
				estimatedPosition.set(actualPosition.get(DIM3) + neighbourhoodDim3, DIM3);

				if ((estimatedPosition.get(DIM1) < (*space)->getStep(DIM1)) && (estimatedPosition.get(DIM2) < (*space)->getStep(DIM2)) && (estimatedPosition.get(DIM3) < (*space)->getStep(DIM3))) {

					/* compute the estimated neighbourhood momentum for the estimated coordinates */
					formula.evaluateP(estimatedPosition, *(*space), &actualMomentum);

					/* compute the quadrat of the phytagoras difference for the correct and the estimated neighbourhood momentum */
					estimatedMomentumDifferenceQuad = sqr(track.getMomX() - actualMomentum.get(DIM1)) + sqr(track.getMomY() - actualMomentum.get(DIM2)) + sqr(track.getMomZ() - actualMomentum.get(DIM3));

					if (estimatedMomentumDifferenceQuad < bestMomentumDifferenceQuad) {

						/* set the quadrat of the phytagoras difference for the correct and the new estimated momentum */
						bestMomentumDifferenceQuad = estimatedMomentumDifferenceQuad;

						/* set the estimated position with the actual minimal difference */
						bestPosition               = estimatedPosition;

					}

				}

			}
		}
	}

	if ((bestPosition.get(DIM1) < (*space)->getStep(DIM1)) && (bestPosition.get(DIM2) < (*space)->getStep(DIM2)) && (bestPosition.get(DIM3) < (*space)->getStep(DIM3))) {

		*coordinates = bestPosition;
		returnValue  = true;

	}
	else {

		positionIsOutOfSpaceWarningMsg* positionIsOutOfSpace = new positionIsOutOfSpaceWarningMsg(*space, &estimatedPosition);
		positionIsOutOfSpace->warningMsg();
		if(positionIsOutOfSpace != NULL) {
			delete positionIsOutOfSpace;
			positionIsOutOfSpace = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * method searches all tracks to find the findable ones and to	*
 * computer their position in the Hough space.					*
 ****************************************************************/

void analysis::setupFindableTracks(std::list<findableTrack>* findableTracks) {

	trackfinderInputTrack* actualMCTrack;
	trackCoordinates       estimatedPosition;
	findableTrack          actualFindableTrack;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	if (findableTracks == NULL)
		throw cannotAccessFindableTracksMemoryError();

	for (int i = 0; i < (*eventData)->getNumberOfTracks(); i++) {

		actualMCTrack = (*eventData)->getTrackByIndex(i);
		if (actualMCTrack == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (isfindableTrack(actualMCTrack, (*ratings)->getGradingPTableMinimumClassification(), NULL)) {

			if (computeCoordinates(*actualMCTrack, &estimatedPosition)) {

				actualFindableTrack.setParameter(actualMCTrack, estimatedPosition);
				findableTracks->push_back(actualFindableTrack);

			}

		}

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analysis::analysis() {

	eventData                      = NULL;
	tracks                         = NULL;
	ratings                        = NULL;
	space                          = NULL;
	luts                           = NULL;
	qualityAnalyser                = NULL;
	momentumAnalyser               = NULL;
	projectionAnalyser             = NULL;
	magnetfieldAnalyser            = NULL;
	magnetfieldFactorAnalyser      = NULL;
	histogramAnalyser              = NULL;
	showAnalyser                   = NULL;
	totalAnalyser                  = NULL;
	hardwareAnalyser               = NULL;

	minimumP                       = 0;

	configuration                  = false;
	detector                       = false;
	memory                         = false;
	event                          = false;
	classPriority                  = false;

	percentageOfHitsInSignature    = 0;
	percentageOfTracksForSignature = 0;
	analysisResultWarnings         = 0;
	analysisResultDisplays         = 0;

	pictures                       = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

analysis::analysis(initialParameter parameters,
				   bool initHistogramAnalysisToRoot) {

	this->eventData                = parameters.eventData;
	this->tracks                   = parameters.tracks;
	this->ratings                  = parameters.ratings;
	this->space                    = parameters.space;
	this->luts                     = parameters.luts;
	qualityAnalyser                = NULL;
	momentumAnalyser               = NULL;
	projectionAnalyser             = NULL;
	magnetfieldAnalyser            = NULL;
	magnetfieldFactorAnalyser      = NULL;
	histogramAnalyser              = NULL;
	showAnalyser                   = NULL;
	totalAnalyser                  = NULL;
	hardwareAnalyser               = NULL;

	minimumP                       = parameters.minP;

	configuration                  = parameters.initConfiguration;
	detector                       = parameters.initDetector;
	memory                         = parameters.initMemory;
	event                          = parameters.initEvent;
	classPriority                  = parameters.initClassPriority;

	percentageOfHitsInSignature    = parameters.percentageOfHitsInSignature;
	percentageOfTracksForSignature = parameters.percentageOfTracksForSignature;
	analysisResultWarnings         = parameters.analysisResultWarnings;
	analysisResultDisplays         = parameters.analysisResultDisplays;

	pictures                       = NULL;

	initQualityEFGCEventAbsoluteAnalysis(parameters.initQualityEFGCEventAbsolute);
	initQualityEFGCEventRelativeAnalysis(parameters.initQualityEFGCEventRelative);
	initQualityEFGCTotalAbsoluteAnalysis(parameters.initQualityEFGCTotalAbsolute);
	initQualityEFGCTotalRelativeAnalysis(parameters.initQualityEFGCTotalRelative);
	initMomentumEFGCEventPzEFGCAnalysis(parameters.initMomentumEFGCEventPzEFGC);
	initMomentumEFGCEventPtEFGCAnalysis(parameters.initMomentumEFGCEventPtEFGC);
	initMomentumEFGCTotalPzEFGCAnalysis(parameters.initMomentumEFGCTotalPzEFGC);
	initMomentumEFGCTotalPtEFGCAnalysis(parameters.initMomentumEFGCTotalPtEFGC);
	initProjectionEFGCNEvent12EFGCNAnalysis(parameters.initProjectionEFGCNEvent12EFGCN);
	initProjectionEFGCNEvent13EFGCNAnalysis(parameters.initProjectionEFGCNEvent13EFGCN);
	initProjectionEFGCNEvent32EFGCNAnalysis(parameters.initProjectionEFGCNEvent32EFGCN);
	initProjectionEFGCNTotal12EFGCNAnalysis(parameters.initProjectionEFGCNTotal12EFGCN);
	initProjectionEFGCNTotal13EFGCNAnalysis(parameters.initProjectionEFGCNTotal13EFGCN);
	initProjectionEFGCNTotal32EFGCNAnalysis(parameters.initProjectionEFGCNTotal32EFGCN);

	initMomentumEventAnalysis(parameters.initMomentumEvent);
	initMomentumTotalAnalysis(parameters.initMomentumTotal);
	initMomentumAnalysisDisplay(parameters.initMomentumDisplay);

	initProjectionEventAnalysis(parameters.initProjectionEvent);
	initProjectionTotalAnalysis(parameters.initProjectionTotal);
	initProjectionAnalysisDisplay(parameters.initProjectionDisplay);

	initMagnetfieldConstantAnalysis(parameters.initMagnetfieldFactors);
	initMagnetfieldConstantAnalysisDisplay(parameters.initMagnetfieldFactorDisplay);
	initMagnetfieldXAnalysis(parameters.initMagnetfieldX);
	initMagnetfieldYAnalysis(parameters.initMagnetfieldY);
	initMagnetfieldZAnalysis(parameters.initMagnetfieldZ);

	initMagnetfieldAnalysisDisplay(parameters.initMagnetfieldDisplay);

	initHistogramAnalysis(initHistogramAnalysisToRoot, parameters.histogram);

	initHardwareTracksPerColumnAnalysis(parameters.initTracksPerColumn);
	initHardwareTracksPerLayerAnalysis(parameters.initTracksPerLayer);

}
analysis::analysis(initialParameter parameters,
				   initialFileParameter fileParameters,
				   bool initCreatedHistogramToShow, bool initEncodedHistogramToShow,
				   bool initFilteredHistogramToShow, unsigned short initHistogramLayer) {

	this->eventData                = parameters.eventData;
	this->tracks                   = parameters.tracks;
	this->ratings                  = parameters.ratings;
	this->space                    = parameters.space;
	this->luts                     = parameters.luts;
	qualityAnalyser                = NULL;
	momentumAnalyser               = NULL;
	projectionAnalyser             = NULL;
	magnetfieldAnalyser            = NULL;
	magnetfieldFactorAnalyser      = NULL;
	histogramAnalyser              = NULL;
	showAnalyser                   = NULL;
	totalAnalyser                  = NULL;
	hardwareAnalyser               = NULL;

	minimumP                       = parameters.minP;

	configuration                  = parameters.initConfiguration;
	detector                       = parameters.initDetector;
	memory                         = parameters.initMemory;
	event                          = parameters.initEvent;
	classPriority                  = parameters.initClassPriority;

	percentageOfHitsInSignature    = parameters.percentageOfHitsInSignature;
	percentageOfTracksForSignature = parameters.percentageOfTracksForSignature;
	analysisResultWarnings         = parameters.analysisResultWarnings;
	analysisResultDisplays         = parameters.analysisResultDisplays;

	pictures                       = NULL;

	initQualityEFGCEventAbsoluteAnalysis(parameters.initQualityEFGCEventAbsolute);
	initQualityEFGCEventRelativeAnalysis(parameters.initQualityEFGCEventRelative);
	initQualityEFGCTotalAbsoluteAnalysis(parameters.initQualityEFGCTotalAbsolute);
	initQualityEFGCTotalRelativeAnalysis(parameters.initQualityEFGCTotalRelative);
	initMomentumEFGCEventPzEFGCAnalysis(parameters.initMomentumEFGCEventPzEFGC);
	initMomentumEFGCEventPtEFGCAnalysis(parameters.initMomentumEFGCEventPtEFGC);
	initMomentumEFGCTotalPzEFGCAnalysis(parameters.initMomentumEFGCTotalPzEFGC);
	initMomentumEFGCTotalPtEFGCAnalysis(parameters.initMomentumEFGCTotalPtEFGC);
	initProjectionEFGCNEvent12EFGCNAnalysis(parameters.initProjectionEFGCNEvent12EFGCN);
	initProjectionEFGCNEvent13EFGCNAnalysis(parameters.initProjectionEFGCNEvent13EFGCN);
	initProjectionEFGCNEvent32EFGCNAnalysis(parameters.initProjectionEFGCNEvent32EFGCN);
	initProjectionEFGCNTotal12EFGCNAnalysis(parameters.initProjectionEFGCNTotal12EFGCN);
	initProjectionEFGCNTotal13EFGCNAnalysis(parameters.initProjectionEFGCNTotal13EFGCN);
	initProjectionEFGCNTotal32EFGCNAnalysis(parameters.initProjectionEFGCNTotal32EFGCN);

	initMomentumEventAnalysis(parameters.initMomentumEvent);
	initMomentumTotalAnalysis(parameters.initMomentumTotal);
	initMomentumAnalysisDisplay(parameters.initMomentumDisplay);
	initProjectionEventAnalysis(parameters.initProjectionEvent);
	initProjectionTotalAnalysis(parameters.initProjectionTotal);
	initProjectionAnalysisDisplay(parameters.initProjectionDisplay);

	initMomentumAnalysisToRoot(fileParameters.initMomentumToRoot, fileParameters.name);
	initProjectionAnalysisToRoot(fileParameters.initProjectionToRoot, fileParameters.name);

	initMagnetfieldConstantAnalysis(parameters.initMagnetfieldFactors);
	initMagnetfieldConstantAnalysisDisplay(parameters.initMagnetfieldFactorDisplay);
	initMagnetfieldXAnalysis(parameters.initMagnetfieldX);
	initMagnetfieldYAnalysis(parameters.initMagnetfieldY);
	initMagnetfieldZAnalysis(parameters.initMagnetfieldZ);

	initMagnetfieldAnalysisDisplay(parameters.initMagnetfieldDisplay);

	initMagnetfieldAnalysisToRoot(fileParameters.initMagnetfieldToRoot, fileParameters.name);
	initHistogramAnalysis(fileParameters.initCreatedHistogramToRoot | fileParameters.initEncodedHistogramToRoot | fileParameters.initFilteredHistogramToRoot, parameters.histogram);
	initHistogramAnalysis(fileParameters.initCreatedHistogramToRoot, fileParameters.initEncodedHistogramToRoot, fileParameters.initFilteredHistogramToRoot, fileParameters.initJustOneCreatedHistogramToRoot, fileParameters.initJustOneEncodedHistogramToRoot, fileParameters.initJustOneFilteredHistogramToRoot, initCreatedHistogramToShow, initEncodedHistogramToShow, initFilteredHistogramToShow, initHistogramLayer, fileParameters.name);

	initHardwareTracksPerColumnAnalysis(parameters.initTracksPerColumn);
	initHardwareTracksPerLayerAnalysis(parameters.initTracksPerLayer);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

analysis::~analysis() {

	if (qualityAnalyser != NULL) {
		delete qualityAnalyser;
		qualityAnalyser = NULL;
	}

	if (momentumAnalyser != NULL) {
		delete momentumAnalyser;
		momentumAnalyser = NULL;
	}

	if (projectionAnalyser != NULL) {
		delete projectionAnalyser;
		projectionAnalyser = NULL;
	}

	if (magnetfieldAnalyser != NULL) {
		delete magnetfieldAnalyser;
		magnetfieldAnalyser = NULL;
	}

	if (magnetfieldFactorAnalyser != NULL) {
		delete magnetfieldFactorAnalyser;
		magnetfieldFactorAnalyser = NULL;
	}
	if (showAnalyser != NULL) {
		delete showAnalyser;
		showAnalyser = NULL;
	}
	if (totalAnalyser != NULL) {
		delete totalAnalyser;
		totalAnalyser = NULL;
	}
	if (hardwareAnalyser != NULL) {
		delete hardwareAnalyser;
		hardwareAnalyser = NULL;
	}
	if (pictures != NULL) {
		delete pictures;
		pictures = NULL;
	}

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void analysis::init(initialParameter parameters,
					bool initHistogramAnalysisToRoot) {

	this->eventData                = parameters.eventData;
	this->tracks                   = parameters.tracks;
	this->ratings                  = parameters.ratings;
	this->space                    = parameters.space;
	this->luts                     = parameters.luts;

	minimumP                       = parameters.minP;

	configuration                  = parameters.initConfiguration;
	detector                       = parameters.initDetector;
	memory                         = parameters.initMemory;
	event                          = parameters.initEvent;
	classPriority                  = parameters.initClassPriority;

	percentageOfHitsInSignature    = parameters.percentageOfHitsInSignature;
	percentageOfTracksForSignature = parameters.percentageOfTracksForSignature;
	analysisResultWarnings         = parameters.analysisResultWarnings;
	analysisResultDisplays         = parameters.analysisResultDisplays;

	initQualityEFGCEventAbsoluteAnalysis(parameters.initQualityEFGCEventAbsolute);
	initQualityEFGCEventRelativeAnalysis(parameters.initQualityEFGCEventRelative);
	initQualityEFGCTotalAbsoluteAnalysis(parameters.initQualityEFGCTotalAbsolute);
	initQualityEFGCTotalRelativeAnalysis(parameters.initQualityEFGCTotalRelative);
	initMomentumEFGCEventPzEFGCAnalysis(parameters.initMomentumEFGCEventPzEFGC);
	initMomentumEFGCEventPtEFGCAnalysis(parameters.initMomentumEFGCEventPtEFGC);
	initMomentumEFGCTotalPzEFGCAnalysis(parameters.initMomentumEFGCTotalPzEFGC);
	initMomentumEFGCTotalPtEFGCAnalysis(parameters.initMomentumEFGCTotalPtEFGC);
	initProjectionEFGCNEvent12EFGCNAnalysis(parameters.initProjectionEFGCNEvent12EFGCN);
	initProjectionEFGCNEvent13EFGCNAnalysis(parameters.initProjectionEFGCNEvent13EFGCN);
	initProjectionEFGCNEvent32EFGCNAnalysis(parameters.initProjectionEFGCNEvent32EFGCN);
	initProjectionEFGCNTotal12EFGCNAnalysis(parameters.initProjectionEFGCNTotal12EFGCN);
	initProjectionEFGCNTotal13EFGCNAnalysis(parameters.initProjectionEFGCNTotal13EFGCN);
	initProjectionEFGCNTotal32EFGCNAnalysis(parameters.initProjectionEFGCNTotal32EFGCN);

	initMomentumEventAnalysis(parameters.initMomentumEvent);
	initMomentumTotalAnalysis(parameters.initMomentumTotal);
	initMomentumAnalysisDisplay(parameters.initMomentumDisplay);
	initProjectionEventAnalysis(parameters.initProjectionEvent);
	initProjectionTotalAnalysis(parameters.initProjectionTotal);
	initProjectionAnalysisDisplay(parameters.initProjectionDisplay);

	initMagnetfieldConstantAnalysis(parameters.initMagnetfieldFactors);
	initMagnetfieldConstantAnalysisDisplay(parameters.initMagnetfieldFactorDisplay);
	initMagnetfieldXAnalysis(parameters.initMagnetfieldX);
	initMagnetfieldYAnalysis(parameters.initMagnetfieldY);
	initMagnetfieldZAnalysis(parameters.initMagnetfieldZ);

	initMagnetfieldAnalysisDisplay(parameters.initMagnetfieldDisplay);

	initHistogramAnalysis(initHistogramAnalysisToRoot, parameters.histogram);

	initHardwareTracksPerColumnAnalysis(parameters.initTracksPerColumn);
	initHardwareTracksPerLayerAnalysis(parameters.initTracksPerLayer);

}
void analysis::init(initialParameter parameters,
					initialFileParameter fileParameters,
					bool initCreatedHistogramToShow, bool initEncodedHistogramToShow,
					bool initFilteredHistogramToShow, unsigned short initHistogramLayer) {

	this->eventData                = parameters.eventData;
	this->tracks                   = parameters.tracks;
	this->ratings                  = parameters.ratings;
	this->space                    = parameters.space;
	this->luts                     = parameters.luts;

	minimumP                       = parameters.minP;

	configuration                  = parameters.initConfiguration;
	detector                       = parameters.initDetector;
	memory                         = parameters.initMemory;
	event                          = parameters.initEvent;
	classPriority                  = parameters.initClassPriority;

	percentageOfHitsInSignature    = parameters.percentageOfHitsInSignature;
	percentageOfTracksForSignature = parameters.percentageOfTracksForSignature;
	analysisResultWarnings         = parameters.analysisResultWarnings;
	analysisResultDisplays         = parameters.analysisResultDisplays;

	initQualityEFGCEventAbsoluteAnalysis(parameters.initQualityEFGCEventAbsolute);
	initQualityEFGCEventRelativeAnalysis(parameters.initQualityEFGCEventRelative);
	initQualityEFGCTotalAbsoluteAnalysis(parameters.initQualityEFGCTotalAbsolute);
	initQualityEFGCTotalRelativeAnalysis(parameters.initQualityEFGCTotalRelative);
	initMomentumEFGCEventPzEFGCAnalysis(parameters.initMomentumEFGCEventPzEFGC);
	initMomentumEFGCEventPtEFGCAnalysis(parameters.initMomentumEFGCEventPtEFGC);
	initMomentumEFGCTotalPzEFGCAnalysis(parameters.initMomentumEFGCTotalPzEFGC);
	initMomentumEFGCTotalPtEFGCAnalysis(parameters.initMomentumEFGCTotalPtEFGC);
	initProjectionEFGCNEvent12EFGCNAnalysis(parameters.initProjectionEFGCNEvent12EFGCN);
	initProjectionEFGCNEvent13EFGCNAnalysis(parameters.initProjectionEFGCNEvent13EFGCN);
	initProjectionEFGCNEvent32EFGCNAnalysis(parameters.initProjectionEFGCNEvent32EFGCN);
	initProjectionEFGCNTotal12EFGCNAnalysis(parameters.initProjectionEFGCNTotal12EFGCN);
	initProjectionEFGCNTotal13EFGCNAnalysis(parameters.initProjectionEFGCNTotal13EFGCN);
	initProjectionEFGCNTotal32EFGCNAnalysis(parameters.initProjectionEFGCNTotal32EFGCN);

	initMomentumEventAnalysis(parameters.initMomentumEvent);
	initMomentumTotalAnalysis(parameters.initMomentumTotal);
	initMomentumAnalysisDisplay(parameters.initMomentumDisplay);
	initProjectionEventAnalysis(parameters.initProjectionEvent);
	initProjectionTotalAnalysis(parameters.initProjectionTotal);
	initProjectionAnalysisDisplay(parameters.initProjectionDisplay);

	initMomentumAnalysisToRoot(fileParameters.initMomentumToRoot, fileParameters.name);
	initProjectionAnalysisToRoot(fileParameters.initProjectionToRoot, fileParameters.name);

	initMagnetfieldConstantAnalysis(parameters.initMagnetfieldFactors);
	initMagnetfieldConstantAnalysisDisplay(parameters.initMagnetfieldFactorDisplay);
	initMagnetfieldXAnalysis(parameters.initMagnetfieldX);
	initMagnetfieldYAnalysis(parameters.initMagnetfieldY);
	initMagnetfieldZAnalysis(parameters.initMagnetfieldZ);

	initMagnetfieldAnalysisDisplay(parameters.initMagnetfieldDisplay);

	initMagnetfieldAnalysisToRoot(fileParameters.initMagnetfieldToRoot, fileParameters.name);
	initHistogramAnalysis(fileParameters.initCreatedHistogramToRoot | fileParameters.initEncodedHistogramToRoot | fileParameters.initFilteredHistogramToRoot, parameters.histogram);
	initHistogramAnalysis(fileParameters.initCreatedHistogramToRoot, fileParameters.initEncodedHistogramToRoot, fileParameters.initFilteredHistogramToRoot, fileParameters.initJustOneCreatedHistogramToRoot, fileParameters.initJustOneEncodedHistogramToRoot, fileParameters.initJustOneFilteredHistogramToRoot, initCreatedHistogramToShow, initEncodedHistogramToShow, initFilteredHistogramToShow, initHistogramLayer, fileParameters.name);

	initHardwareTracksPerColumnAnalysis(parameters.initTracksPerColumn);
	initHardwareTracksPerLayerAnalysis(parameters.initTracksPerLayer);

}

/****************************************************************
 * method returns true if the track has a the specific track	*
 * pattern and has a specific P.								*
 ****************************************************************/

bool analysis::isfindableTrack(trackfinderInputTrack* track, bitArray minClassPriority, bitArray* globalTrackPattern) {

	bitArray               localTrackPattern;
	trackfinderInputHit*   hit;
	unsigned short         detectorIndex;
	bitArray               classPriority;
	bool                   returnValue;

	localTrackPattern = bitArray(0);

	if (track == NULL)
		throw cannotAccessHitsOrTracksError(ANALYSISLIB);

	track->resetHitPointer();

	for (unsigned short j = 0; j < track->getNumberOfHits(); j++) {

		hit = track->getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (hit->getStation() == NULL)
			throw cannotAccessStationError(ANALYSISLIB);

		detectorIndex = hit->getStation()->getIndex();
		localTrackPattern[detectorIndex]  = true;
		if (globalTrackPattern != NULL)
			(*globalTrackPattern)[detectorIndex] = true;

	}

	classPriority = evaluateTrackPatternGradingP(localTrackPattern);
	if((classPriority >= minClassPriority) && (track->getMomZ() > minimumP))
		returnValue = true;
	else
		returnValue = false;
	
	return returnValue;

}

/****************************************************************
 * method returns the number of tracks with a the specific		*
 * track patterns and with a specific P.						*
 ****************************************************************/

int analysis::getNumberOfTracksWithP() {

	return getNumberOfTracksWithP((*ratings)->getGradingPTableMinimumClassification());

}
int analysis::getNumberOfTracksWithP(bitArray minimumClassPriority) {

	bitArray               globalTrackPattern;
	trackfinderInputTrack* track;
	int                    returnValue;
	unsigned short         k;
	unsigned short         numberOfInactiveStations;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((*eventData)->getDetector().getNumberOfActiveStations() > (unsigned short)globalTrackPattern.length())
		throw tooManyActiveStationsError((*eventData)->getDetector().getNumberOfActiveStations(), (unsigned short)globalTrackPattern.length());

	globalTrackPattern = bitArray(0);
	returnValue        = 0;

	for (int i = 0; i < (*eventData)->getNumberOfTracks(); i++) {

		track = (*eventData)->getTrackByIndex(i);
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (isfindableTrack(track, minimumClassPriority, &globalTrackPattern))
			returnValue++;

	}

	numberOfInactiveStations = 0;
	for (k = 0; k < (*eventData)->getDetector().getNumberOfActiveStations(); k++) {
		
		if (!globalTrackPattern[k])
			numberOfInactiveStations++;

	}

	if (numberOfInactiveStations < (*eventData)->getDetector().getNumberOfActiveStations()) {

		for (k = 0; k < (*eventData)->getDetector().getNumberOfActiveStations(); k++) {

			if (!globalTrackPattern[k]) {

				noHitInDetectorStationWarningMsg* noHitInDetectorStation = new noHitInDetectorStationWarningMsg(k);
				noHitInDetectorStation->warningMsg();
				if(noHitInDetectorStation != NULL) {
					delete noHitInDetectorStation;
					noHitInDetectorStation = NULL;
				}

			}

		}

	}

	return returnValue;

}

/****************************************************************
 * method evaluates the found tracks.							*
 ****************************************************************/

void analysis::evaluateAlgorithm(std::streambuf* terminal) {

	unsigned short         i;
	unsigned short         trackFrequency;
	trackDigitalInformation       trackInfo;
	trackParameter         trackPara;
	trackfinderInputTrack* realTrack;
	bool                   actualTrackIsRealTrack;
	bool                   foundClone;
	bool                   foundWrong;
	bool                   foundFake;
	terminalSequence       statusSequenceForFoundTracks;
	terminalSequence       statusSequenceForRealTracks;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);
	if (*tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);

	initializeAlgorithmEvaluation();

	(*tracks)->resetActiveObject();

	createTerminalStatusSequence(&statusSequenceForFoundTracks, terminal, "Analyse found tracks:\t\t", (*tracks)->getNumberOfTracks());
	terminalInitialize(statusSequenceForFoundTracks);

	for (i = 0; i < (*tracks)->getNumberOfTracks(); i++) {

		(*tracks)->getNextTrackDigitalInfo(&trackInfo);
		(*tracks)->getTrackParameterFromTrackDigitalInfo(trackInfo, &trackPara);

		switch (evaluatePeakContribution(trackInfo, &foundClone, &foundWrong, &foundFake)) {
			case 0:
				if (foundFake)
					peakToFakeTrack(trackInfo.position, trackPara);
				else
					peakToNoTrack(trackInfo.position, trackPara);
				break;
			case 1:
				peakToOneTrack(trackInfo.position, trackPara, foundClone, foundWrong);
				break;
			default:
				peakToMoreTracks(trackInfo.position, trackPara, foundClone, foundWrong);
				break;
		}

		terminalOverwrite(statusSequenceForFoundTracks, i + 1);

	}

	terminalFinalize(statusSequenceForFoundTracks);

	createTerminalStatusSequence(&statusSequenceForRealTracks, terminal, "Analyse real tracks:\t\t", (*eventData)->getNumberOfTracks());
	terminalInitialize(statusSequenceForRealTracks);

	for (i = 0; i < (*eventData)->getNumberOfTracks(); i++) {

		realTrack = (*eventData)->getTrackByIndex(i);
		if (realTrack == NULL)
			throw cannotAccessTrackDataError(ANALYSISLIB);

		trackFrequency = realTrack->getFrequency();

		if (realTrack->getMomZ() > minimumP) {

			actualTrackIsRealTrack = evaluateFoundTrackIsRealTrack(realTrack);

			if (trackFrequency > 0) {
				
				if (trackFrequency == 1) {

					if (actualTrackIsRealTrack) {

						trackToOnePeak(*realTrack);
						if ((double)realTrack->getMaximumNumberOfHits() >= WELLFOUNDHITFACTOR * (double)evaluateNumberOfRealTrackHits(realTrack))
							wellFoundTracks(*realTrack);
					
					}
					else
						wrongFoundTracks(*realTrack);
				
				}
				else {

					if (actualTrackIsRealTrack) {

						trackToMorePeaks(*realTrack);
						if ((double)realTrack->getMaximumNumberOfHits() >= WELLFOUNDHITFACTOR * (double)evaluateNumberOfRealTrackHits(realTrack))
							wellFoundTracks(*realTrack);
					
					}
					else
						wrongFoundTracks(*realTrack);
				
				}
			}
			else {

				if(actualTrackIsRealTrack) {
			
					trackToNoPeak(*realTrack);

#ifdef WHICHTRACKIDISNOTFOUND

					terminalFinalize(statusSequenceForRealTracks);

					trackNotFoundWarningMsg* trackNotFound = new trackNotFoundWarningMsg(realTrack);
					trackNotFound->warningMsg();
					if(trackNotFound != NULL) {
						delete trackNotFound;
						trackNotFound = NULL;
					}

					terminalInitialize(statusSequenceForRealTracks);

#endif

				}

			}

		}
		else {

			if (trackFrequency > 0)
				wrongFoundTracks(*realTrack);

		}

		terminalOverwrite(statusSequenceForRealTracks, i + 1);

	}

	terminalFinalize(statusSequenceForRealTracks);

	finalizeAlgorithmEvaluation();

}

/****************************************************************
 * method evaluates the magnetfield factors.					*
 ****************************************************************/

void analysis::evaluateMagnetfieldFactors(bool updateFactors, double dim3StartEntry, double dim3StopEntry, std::streambuf* terminal) {

	trackfinderInputMagneticField* magneticField;
	unsigned short                 j;
	unsigned short                 trackIndex;
	terminalSequence               statusSequenceForFactors;
	std::list<findableTrack>       findableTracks;
	int                            wrongThirdDimensionBorder;
	double                         actualFactor;
	stationMinimum*                stationInfo;
	double                         actualHoughDistance;
	double                         minimalHoughDistance;
	trackfinderInputTrack*         track;
	trackfinderInputHit*           hit;
	unsigned short                 stationIndex;
	lutBorder                      border;
	prelutHoughBorder*             firstBorder;
	lutHoughBorder*                secondBorder;
	houghBorderPosition            actualHoughCoord;
	double                         magnetfieldFactor;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	magneticField = (*eventData)->getMagneticField();
	if (magneticField == NULL)
		throw cannotAccessMagneticFieldObjectError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutError(ANALYSISLIB);

	if (magnetfieldFactorAnalyser == NULL)
		throw cannotAccessMagnetfieldFactorAnalyserError();

	magnetfieldFactorAnalyser->set((*eventData)->getNumberOfActiveStations());

	firstBorder  = border.getPrelutHoughBorderPointer();
	secondBorder = border.getLutHoughBorderPointer();

	/* allocate the memory to store the actual freqency and
	 * the minimum distance value for each station for one hit. */
	stationInfo = new stationMinimum[(*eventData)->getNumberOfActiveStations()];
	if (stationInfo == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	/* this function creates a std::list which includes pointers to all
	 * findable tracks and their corresponding position in the Hough
	 * space */
	setupFindableTracks(&findableTracks);

	/* the counter for the number of hits which have no entry in the
	 * correct third dimension */
	wrongThirdDimensionBorder = 0;

	/* reset the counter for the corrections in the first and second
	 * dimension. Corrected are these values which have a difference
	 * of more than one in the first dimension according to the previous
	 * value */
	(*luts)->resetCorrectionCounter();

	createTerminalStatusSequence(&statusSequenceForFactors, terminal, "\nAnalyse magnetfield factors:\t", magnetfieldFactorAnalyser->getNumberOfFactors() * findableTracks.size());
	terminalInitialize(statusSequenceForFactors);

	/* loop to evaluate the minimal distance for each magnetfield factor */
	for (unsigned short i = 0; i < magnetfieldFactorAnalyser->getNumberOfFactors(); i++) {

		actualFactor = magnetfieldFactorAnalyser->getFactor(i);

		/* set the magnetfield factor for the lut computation */
		(*luts)->setMagneticFieldFactor(actualFactor);

		/* reset the actual distances for this magnetfield factor */
		for (j = 0; j < (*eventData)->getNumberOfActiveStations(); j++)
			magnetfieldFactorAnalyser->setDistance(j, i, 0);

		trackIndex   = 0;
		/* loop to evaluate the magnetfield factor for all findable tracks */
		for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++, trackIndex++) {

			track = actualTrack->getTrack();
			if (track == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			/* reset the frequency of hits occuring in the same station and
			 * the value of the minimal distance for the station */
			for (j = 0; j < (*eventData)->getNumberOfActiveStations(); j++) {
				stationInfo[j].frequency = 0;
				stationInfo[j].value     = 0;
			}
			track->resetHitPointer();

			/* loop to evaluate the magnetfield factor for all hits of this track */
			for (unsigned short k = 0; k < track->getNumberOfHits(); k++) {

				hit = track->getHit();
				if (hit == NULL)
					throw cannotAccessHitsOrTracksError(ANALYSISLIB);

				if (hit->getStation() == NULL)
					throw cannotAccessStationError(ANALYSISLIB);
				else
					stationIndex = hit->getStation()->getIndex();

				/* compute the borders for this hit */
				(*luts)->evaluate(hit, &border);

				if (secondBorder->houghCoord.getNumberOfEntries() == 0) {

					/* separate case because there is no houghCoord in the range.
					 * Take always the biggest value to fall out of range. */
					minimalHoughDistance = sqrt((double)sqr(0 - actualTrack->getPosition().get(DIM1)) + (double)sqr(0 - actualTrack->getPosition().get(DIM2)));
					actualHoughDistance  = sqrt((double)sqr((*space)->getStep(DIM1) - actualTrack->getPosition().get(DIM1)) + (double)sqr((*space)->getStep(DIM2) - actualTrack->getPosition().get(DIM2)));

					if (actualHoughDistance > minimalHoughDistance)
						minimalHoughDistance = actualHoughDistance;

				}
				else {

					secondBorder->houghCoord.resetActiveObject();

					/* loop to evaluate the minimal distance for this hit */
					for (unsigned int l = 0; l < secondBorder->houghCoord.getNumberOfEntries(); l++) {

						actualHoughCoord     = secondBorder->houghCoord.readActiveObjectAndMakeNextOneActive();
						actualHoughDistance  = sqrt((double)sqr(actualHoughCoord.pos1 - actualTrack->getPosition().get(DIM1)) + (double)sqr(actualHoughCoord.pos2 - actualTrack->getPosition().get(DIM2)));

						if (l == 0) {

							minimalHoughDistance = actualHoughDistance;

						}
						else {

							if (actualHoughDistance < minimalHoughDistance)
								minimalHoughDistance = actualHoughDistance;

						}

					}

				}

				/* Check if the third dimension border is in the range */
				if ((firstBorder->start <= actualTrack->getPosition().get(DIM3)) && (firstBorder->stop >= actualTrack->getPosition().get(DIM3))) {

					/* Add the minimal distance of the hit to the minimal
					 * distance of the correct station. Here it can happen
					 * that more than one hit can belong to the same station.
					 * This is counted by the frequency. This variable holds
					 * the information for all hits of one track. */
					stationInfo[stationIndex].frequency++;
					stationInfo[stationIndex].value += minimalHoughDistance;

				}
				else
					wrongThirdDimensionBorder++;

			}

			/* Loop to add the minimal distance for all stations of this track.
			 * If the frequency is zero, no hit would be in this station.
			 * If the frequency is one, one hit would be in this station.
			 * If the frequency is bigger than one, more than one hit would
			 * be in this station. In this case, the middle is taken as
			 * minimal distance for this station. */
			for (j = 0; j < (*eventData)->getNumberOfActiveStations(); j++) {
	
				if (stationInfo[j].frequency > 0)
					magnetfieldFactorAnalyser->addDistance(j, i, stationInfo[j].value / stationInfo[j].frequency);

			}

			terminalOverwrite(statusSequenceForFactors, i * findableTracks.size() + trackIndex + 1);

		}

	}

	terminalFinalize(statusSequenceForFactors);

	if (stationInfo != NULL) {
		delete [] stationInfo;
		stationInfo = NULL;
	}

#ifdef ENABLEALLWARNINGS

	if (wrongThirdDimensionBorder > 0) {

		numberOfWrongThirdDimensionBordersFoundWarningMsg* numberOfWrongThirdDimensionBordersFound = new numberOfWrongThirdDimensionBordersFoundWarningMsg(wrongThirdDimensionBorder / magnetfieldFactorAnalyser->getNumberOfFactors());
		numberOfWrongThirdDimensionBordersFound->warningMsg();
		if(numberOfWrongThirdDimensionBordersFound != NULL) {
			delete numberOfWrongThirdDimensionBordersFound;
			numberOfWrongThirdDimensionBordersFound = NULL;
		}

	}

#endif

	/* this sets the magnetfield factors for each station for the computing of the hough transformation */
	for (j = 0; j < (*eventData)->getNumberOfActiveStations(); j++) {
	
		magnetfieldFactor = magnetfieldFactorAnalyser->getMagnetfieldFactor(j);

#ifdef PRINTFACTORS

		setMagnetfieldFactorWarningMsg* setMagnetfieldFactor = new setMagnetfieldFactorWarningMsg(j, magnetfieldFactor);
		setMagnetfieldFactor->warningMsg();
		if(setMagnetfieldFactor != NULL) {
			delete setMagnetfieldFactor;
			setMagnetfieldFactor = NULL;
		}

#endif

		magneticField->setMagnetfieldFactor(j, magnetfieldFactor);

	}

}

/****************************************************************
 * method evaluates the goodness of the prelut.					*
 ****************************************************************/

void analysis::evaluatePrelutGoodness(std::streambuf* terminal) {

	unsigned int                    i;
	unsigned short                  j;
	unsigned short                  trackIndex;
	terminalSequence                statusSequenceForPrelut;
	std::list<findableTrack>        findableTracks;
	trackfinderInputTrack*          track;
	trackfinderInputHit*            hit;
	prelutHoughBorder               firstBorder;
	unsigned short                  stationIndex;
	double                          baseSlope;
	double                          actualSlope;
	double                          allowedSlope;
	unsigned short                  numberOfHitsMeetingCorrectLayer;
	unsigned int*                   numberOfTracksWithPercentCorrectHitsInLayer;
	unsigned short                  numberOfHitsMeetingCorrectSlope;
	unsigned int*                   numberOfTracksWithPercentCorrectSlope;
	hitsInHistogramBorderFrequency* histogramBorderFrequency;
	unsigned short                  numberOfHistogramBorderEntries;
	unsigned short                  borderIndex;
	hitsInHistogramBorderFrequency  maximumHistogramBorderFrequency;
	unsigned int*                   numberOfTracksWithPercentHitsInLayer;
	double                          distanceOfMaximumLayerAndCorrectLayer;
	unsigned long                   numberOfTracksWithGoodPrelutSignature;
	bitArray                        trackSignature;
	bitArray                        correctLayerSignature;
	bitArray                        bestLayerSignature;
	unsigned short                  numberOfCorrectSignaturePositions;
	double*                         yPositions;
	double*                         zPositions;
	unsigned short                  numberOfPositions;
	unsigned int*                   numberOfTracksWithPercentCorrectHitsInCell;
	unsigned int*                   numberOfTracksWithPercentHitsInCell;
	unsigned short                  actualNumberOfTracksForSignatureAcceptance;
	unsigned short                  minimalNumberOfTracksForSignatureAcceptance;
	bitArray                        actualSignatureForSignatureAcceptance;
	bitArray                        searchSignatureForSignatureAcceptance;

#ifndef HIGHOCCURENCEISGOODSIGNATURE

	unsigned short                  actualClassForSignatureAcceptance;
	bool                            searchFoundForSignatureAcceptance;

#endif

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutError();

	numberOfTracksWithPercentCorrectHitsInLayer  = new unsigned int[NUMBEROFDIFFERENTCORRECTHITS];
	if (numberOfTracksWithPercentCorrectHitsInLayer == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentCorrectSlope = new unsigned int[NUMBEROFDIFFERENTCORRECTHITS];
	if (numberOfTracksWithPercentCorrectSlope == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentHitsInLayer  = new unsigned int[NUMBEROFDIFFERENTCORRECTHITS];
	if (numberOfTracksWithPercentHitsInLayer == NULL)
		throw memoryAllocationError(ANALYSISLIB);


	numberOfTracksWithPercentCorrectHitsInCell  = new unsigned int[1 << (unsigned int)trackSignature.length()];
	if (numberOfTracksWithPercentCorrectHitsInCell == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentHitsInCell  = new unsigned int[1 << (unsigned int)trackSignature.length()];
	if (numberOfTracksWithPercentHitsInCell == NULL)
		throw memoryAllocationError(ANALYSISLIB);


	/* this function creates a std::list which includes pointers to all
	 * findable tracks and their corresponding position in the Hough
	 * space */
	setupFindableTracks(&findableTracks);

	createTerminalStatusSequence(&statusSequenceForPrelut, terminal, "\nAnalyse prelut goodness:\t", findableTracks.size());
	terminalInitialize(statusSequenceForPrelut);

	for (i = 0; i < NUMBEROFDIFFERENTCORRECTHITS; i++) {

		numberOfTracksWithPercentCorrectHitsInLayer[i]  = 0;
		numberOfTracksWithPercentCorrectSlope[i]        = 0;
		numberOfTracksWithPercentHitsInLayer[i]         = 0;

	}

	for (i = 0; i < (unsigned int)(1 << (unsigned int)trackSignature.length()); i++) {

		numberOfTracksWithPercentCorrectHitsInCell[i] = 0;
		numberOfTracksWithPercentHitsInCell[i]        = 0;

	}

	trackIndex                            = 0;
	histogramBorderFrequency              = NULL;
	numberOfHistogramBorderEntries        = 0;
	distanceOfMaximumLayerAndCorrectLayer = 0;
	numberOfTracksWithGoodPrelutSignature = 0;

	yPositions                            = NULL;
	zPositions                            = NULL;
	numberOfPositions                     = 0;

	/* loop to evaluate the prelut goodness for all findable tracks */
	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++, trackIndex++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		track->resetHitPointer();

		numberOfHitsMeetingCorrectLayer = 0;
		numberOfHitsMeetingCorrectSlope = 0;
		baseSlope                       = track->getMomY() / track->getMomZ();
		/* loop to evaluate the magnetfield factor for all hits of this track */
		for (j = 0; j < track->getNumberOfHits(); j++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() == NULL)
				throw cannotAccessStationError(ANALYSISLIB);
			else
				stationIndex = hit->getStation()->getIndex();

			/* compute the borders for this hit */
			(*luts)->evaluatePrelut(hit, &firstBorder);

			/* Check if the third dimension border is in the range */
			if ((firstBorder.start <= actualTrack->getPosition().get(DIM3)) && (firstBorder.stop >= actualTrack->getPosition().get(DIM3)))
				numberOfHitsMeetingCorrectLayer++;

			/* Check if the slope is in the range */
			actualSlope  = hit->getHit()->GetY() / hit->getHit()->GetZ();
			allowedSlope = STARTSLOPE + stationIndex * INCRSLOPE;

			if ((actualSlope >= baseSlope - allowedSlope) && (actualSlope <= baseSlope + allowedSlope))
				numberOfHitsMeetingCorrectSlope++;

			/* Accumulate the frequency of the third dimension border */
			for (unsigned short k = firstBorder.start; k <= firstBorder.stop; k++) {

				if (numberOfHistogramBorderEntries == 0) {

					numberOfHistogramBorderEntries        = 1;
					histogramBorderFrequency              = (hitsInHistogramBorderFrequency*)calloc(numberOfHistogramBorderEntries, sizeof(hitsInHistogramBorderFrequency));
					histogramBorderFrequency[0].border    = k;
					histogramBorderFrequency[0].frequency = 1;

				}
				else {
					
					/* check if this border has occured until now */
					borderIndex = numberOfHistogramBorderEntries;
					for (unsigned short l = 0; l < numberOfHistogramBorderEntries; l++) {

						if (histogramBorderFrequency[l].border == k) {

							borderIndex = l;
							break;

						}

					}
					
					/* if the border has not occured */
					if (borderIndex == numberOfHistogramBorderEntries) {

						numberOfHistogramBorderEntries++;
						histogramBorderFrequency                        = (hitsInHistogramBorderFrequency*)realloc(histogramBorderFrequency, numberOfHistogramBorderEntries * sizeof(hitsInHistogramBorderFrequency));
						histogramBorderFrequency[borderIndex].border    = k;
						histogramBorderFrequency[borderIndex].frequency = 1;

					}
					/* if the border has occured */
					else
						histogramBorderFrequency[borderIndex].frequency++;

				}

			}

			if (track->getTrackIndex() == 1343) {

				numberOfPositions = track->getNumberOfHits();
				if (yPositions == NULL) {
					yPositions = new double[numberOfPositions];
					if (yPositions == NULL)
						throw memoryAllocationError(ANALYSISLIB);
				}
				if (zPositions == NULL) {
					zPositions = new double[numberOfPositions];
					if (zPositions == NULL)
						throw memoryAllocationError(ANALYSISLIB);
				}

				yPositions[j] = hit->getHit()->GetY();
				zPositions[j] = hit->getHit()->GetZ();

			}

		}
/**/
		/* By enabling this you can write the trackIndex of the tracks which have a defined percentage of correct hits to the standard output */
//		if ((unsigned short)((((double)numberOfHitsMeetingCorrectSlope * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits())) == 6)
//			std::cout << numberOfTracksWithPercentCorrectSlope[(unsigned short)((((double)numberOfHitsMeetingCorrectSlope * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits()))] << ": " << track->getTrackIndex() << std::endl;

		numberOfTracksWithPercentCorrectHitsInLayer[(unsigned short)((((double)numberOfHitsMeetingCorrectLayer * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits()))]++;
		numberOfTracksWithPercentCorrectSlope[(unsigned short)((((double)numberOfHitsMeetingCorrectSlope * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits()))]++;

		maximumHistogramBorderFrequency.frequency = 0;
		maximumHistogramBorderFrequency.distance  = 0;
		for (j = 0; j < numberOfHistogramBorderEntries; j++) {

			histogramBorderFrequency[j].distance = sqrt((double)sqr(histogramBorderFrequency[j].border - actualTrack->getPosition().get(DIM3)));

			if (histogramBorderFrequency[j].frequency > maximumHistogramBorderFrequency.frequency)
				maximumHistogramBorderFrequency = histogramBorderFrequency[j];
			/* This is added because if there is a equal found clone peak which is nearer to the
			 * correct cell, this peak should be used because of a better quality approximation */
			else if ((histogramBorderFrequency[j].frequency == maximumHistogramBorderFrequency.frequency) && (histogramBorderFrequency[j].distance < maximumHistogramBorderFrequency.distance))
				maximumHistogramBorderFrequency = histogramBorderFrequency[j];

		}

		numberOfTracksWithPercentHitsInLayer[(unsigned short)((((double)maximumHistogramBorderFrequency.frequency * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits()))]++;

		distanceOfMaximumLayerAndCorrectLayer += (maximumHistogramBorderFrequency.distance / (double)findableTracks.size());

		if (histogramBorderFrequency != NULL) {
			free(histogramBorderFrequency);
			histogramBorderFrequency       = NULL;
			numberOfHistogramBorderEntries = 0;
		}

		trackSignature        = bitArray(0);
		correctLayerSignature = bitArray(0);
		bestLayerSignature    = bitArray(0);
		/* loop to evaluate the total prelut anaylsis*/
		for (j = 0; j < track->getNumberOfHits(); j++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() == NULL)
				throw cannotAccessStationError(ANALYSISLIB);
			else
				stationIndex = hit->getStation()->getIndex();

			/* compute the borders for this hit */
			(*luts)->evaluatePrelut(hit, &firstBorder);

			/* Check if the third dimension border is in the range */
			trackSignature[stationIndex] = true;
			if ((firstBorder.start <= actualTrack->getPosition().get(DIM3)) && (firstBorder.stop >= actualTrack->getPosition().get(DIM3)))
				correctLayerSignature[stationIndex] = true;
			if ((firstBorder.start <= maximumHistogramBorderFrequency.border) && (firstBorder.stop >= maximumHistogramBorderFrequency.border))
				bestLayerSignature[stationIndex] = true;

		}

		numberOfTracksWithPercentCorrectHitsInCell[correctLayerSignature.toULong()]++;

		numberOfTracksWithPercentHitsInCell[bestLayerSignature.toULong()]++;

		numberOfCorrectSignaturePositions = 0;
		for (unsigned short m = 0; m < trackSignature.length(); m++) {

			if (trackSignature[m] && (trackSignature[m] == bestLayerSignature[m]))
				numberOfCorrectSignaturePositions++;

		}

		if (numberOfCorrectSignaturePositions >= (unsigned short)ceil(((double)percentageOfHitsInSignature / (double)100) * (double)trackSignature.count()))
			numberOfTracksWithGoodPrelutSignature++;

		terminalOverwrite(statusSequenceForPrelut, trackIndex + 1);

	}

	addNumberOfTracksWithGoodPrelutSignature(numberOfTracksWithGoodPrelutSignature);

	terminalFinalize(statusSequenceForPrelut);

	if (analysisResultWarnings & TRACKWITHHITINCORRECTCELLDISTRIBUTION) {

		trackWithHitsInCorrectCellDistributionWarningMsg* trackWithHitsInCorrectCellDistribution = new trackWithHitsInCorrectCellDistributionWarningMsg(numberOfTracksWithPercentCorrectHitsInCell, (1 << (unsigned int)trackSignature.length()));
		trackWithHitsInCorrectCellDistribution->warningMsg();
		if(trackWithHitsInCorrectCellDistribution != NULL) {
			delete trackWithHitsInCorrectCellDistribution;
			trackWithHitsInCorrectCellDistribution = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITINCELLDISTRIBUTION) {

		trackWithHitsInCellDistributionWarningMsg* trackWithHitsInCellDistribution = new trackWithHitsInCellDistributionWarningMsg(numberOfTracksWithPercentHitsInCell, (1 << (unsigned int)trackSignature.length()));
		trackWithHitsInCellDistribution->warningMsg();
		if(trackWithHitsInCellDistribution != NULL) {
			delete trackWithHitsInCellDistribution;
			trackWithHitsInCellDistribution = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITINCORRECTBORDERDISTRIBUTION) {

		trackWithHitsInCorrectLayerDistributionWarningMsg* trackWithHitsInCorrectLayerDistribution = new trackWithHitsInCorrectLayerDistributionWarningMsg(numberOfTracksWithPercentCorrectHitsInLayer, NUMBEROFDIFFERENTCORRECTHITS);
		trackWithHitsInCorrectLayerDistribution->warningMsg();
		if(trackWithHitsInCorrectLayerDistribution != NULL) {
			delete trackWithHitsInCorrectLayerDistribution;
			trackWithHitsInCorrectLayerDistribution = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITINBORDERDISTRIBUTION) {

		trackWithHitsInLayerDistributionWarningMsg* trackWithHitsInLayerDistribution = new trackWithHitsInLayerDistributionWarningMsg(numberOfTracksWithPercentHitsInLayer, NUMBEROFDIFFERENTCORRECTHITS);
		trackWithHitsInLayerDistribution->warningMsg();
		if(trackWithHitsInLayerDistribution != NULL) {
			delete trackWithHitsInLayerDistribution;
			trackWithHitsInLayerDistribution = NULL;
		}

	}

	if (analysisResultWarnings & MIDDLEDISTANCEOFMAXIMUMANDCORRECTLAYER) {

		middleDistanceOfMaximumAndCorrectLayerWarningMsg* middleDistanceOfMaximumAndCorrectLayer = new middleDistanceOfMaximumAndCorrectLayerWarningMsg(distanceOfMaximumLayerAndCorrectLayer);
		middleDistanceOfMaximumAndCorrectLayer->warningMsg();
		if(middleDistanceOfMaximumAndCorrectLayer != NULL) {
			delete middleDistanceOfMaximumAndCorrectLayer;
			middleDistanceOfMaximumAndCorrectLayer = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITWITHCORRECTSLOPEDISTRIBUTION) {

		trackWithHitsWithCorrectSlopeDistributionWarningMsg* trackWithHitsWithCorrectSlopeDistribution = new trackWithHitsWithCorrectSlopeDistributionWarningMsg(numberOfTracksWithPercentCorrectSlope, NUMBEROFDIFFERENTCORRECTHITS);
		trackWithHitsWithCorrectSlopeDistribution->warningMsg();
		if(trackWithHitsWithCorrectSlopeDistribution != NULL) {
			delete trackWithHitsWithCorrectSlopeDistribution;
			trackWithHitsWithCorrectSlopeDistribution = NULL;
		}

	}

	if (analysisResultDisplays & TRACKWITHHITINCORRECTCELLDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addCorrectPrelutCellDistribution(numberOfTracksWithPercentCorrectHitsInCell, (1 << (unsigned int)trackSignature.length()));

	}

	if (analysisResultDisplays & TRACKWITHHITINCELLDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addPrelutCellDistribution(numberOfTracksWithPercentHitsInCell, (1 << (unsigned int)trackSignature.length()));

	}

	if (analysisResultDisplays & TRACKWITHHITINCORRECTBORDERDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addCorrectLayerDistribution(numberOfTracksWithPercentCorrectHitsInLayer, NUMBEROFDIFFERENTCORRECTHITS);

	}

	if (analysisResultDisplays & TRACKWITHHITINBORDERDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addLayerDistribution(numberOfTracksWithPercentHitsInLayer, NUMBEROFDIFFERENTCORRECTHITS);

	}

	if (analysisResultDisplays & TRACKWITHHITWITHCORRECTSLOPEDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addCorrectSlopeDistribution(numberOfTracksWithPercentCorrectSlope, NUMBEROFDIFFERENTCORRECTHITS);

	}

	if (analysisResultDisplays & TRACKWITHYZPROJECTION) {

		initializeShowAnalysis();

		showAnalyser->addYZProjection(yPositions, zPositions, numberOfPositions);

	}

	actualNumberOfTracksForSignatureAcceptance  = 0;
	minimalNumberOfTracksForSignatureAcceptance = 0;
	for (i = 0; i < (unsigned int)(1 << (unsigned int)trackSignature.length()); i++)
		minimalNumberOfTracksForSignatureAcceptance += numberOfTracksWithPercentHitsInCell[i];
	minimalNumberOfTracksForSignatureAcceptance = (unsigned short)(((double)percentageOfTracksForSignature / (double)100) * (double)minimalNumberOfTracksForSignatureAcceptance);

#ifndef HIGHOCCURENCEISGOODSIGNATURE

	actualClassForSignatureAcceptance = (unsigned short)trackSignature.length();

#endif

	while (minimalNumberOfTracksForSignatureAcceptance > actualNumberOfTracksForSignatureAcceptance) {

		searchSignatureForSignatureAcceptance = bitArray(0);

#ifdef HIGHOCCURENCEISGOODSIGNATURE

		for (i = 0; i < (unsigned int)(1 << (unsigned int)trackSignature.length()); i++) {

			actualSignatureForSignatureAcceptance = bitArray(i);

			if (numberOfTracksWithPercentHitsInCell[i] > numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()])
				searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
			else if (numberOfTracksWithPercentHitsInCell[i] == numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()])
				if (actualSignatureForSignatureAcceptance.count() > searchSignatureForSignatureAcceptance.count())
					searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;

		}

		addGoodPrelutSignature(searchSignatureForSignatureAcceptance, bitArray(1));
		actualNumberOfTracksForSignatureAcceptance                                           += numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()];
		numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()]  = 0;

#else

		searchFoundForSignatureAcceptance = false;

		for (i = 0; i < (unsigned int)(1 << (unsigned int)trackSignature.length()); i++) {

			actualSignatureForSignatureAcceptance = bitArray(i);
		
			if (actualSignatureForSignatureAcceptance.count() == actualClassForSignatureAcceptance) {

				if (numberOfTracksWithPercentHitsInCell[i] > numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()]) {

					searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
					searchFoundForSignatureAcceptance     = true;

				}

			}

		}

		if (searchFoundForSignatureAcceptance) {

			addGoodPrelutSignature(searchSignatureForSignatureAcceptance, bitArray(1));
			actualNumberOfTracksForSignatureAcceptance                                           += numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()];
			numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()]  = 0;

		}
		else
			actualClassForSignatureAcceptance--;

#endif

	}

	if (yPositions != NULL) {
		delete yPositions;
		yPositions = NULL;
	}
	if (zPositions != NULL) {
		delete zPositions;
		zPositions = NULL;
	}

	if (numberOfTracksWithPercentCorrectHitsInLayer != NULL) {
		delete[] numberOfTracksWithPercentCorrectHitsInLayer;
		numberOfTracksWithPercentCorrectHitsInLayer = NULL;
	}
	if (numberOfTracksWithPercentCorrectSlope != NULL) {
		delete[] numberOfTracksWithPercentCorrectSlope;
		numberOfTracksWithPercentCorrectSlope = NULL;
	}
	if (numberOfTracksWithPercentHitsInLayer != NULL) {
		delete[] numberOfTracksWithPercentHitsInLayer;
		numberOfTracksWithPercentHitsInLayer = NULL;
	}
	if (numberOfTracksWithPercentCorrectHitsInCell != NULL) {
		delete[] numberOfTracksWithPercentCorrectHitsInCell;
		numberOfTracksWithPercentCorrectHitsInCell = NULL;
	}
	if (numberOfTracksWithPercentHitsInCell != NULL) {
		delete[] numberOfTracksWithPercentHitsInCell;
		numberOfTracksWithPercentHitsInCell = NULL;
	}

}

/****************************************************************
 * method evaluates the lut goodness.							*
 ****************************************************************/

void analysis::evaluateLutGoodness(std::streambuf* terminal) {

	unsigned int   i;
	bitArray       maxSignature;
	unsigned int*  numberOfTracksWithPercentCorrectHits;
	unsigned int*  numberOfTracksWithSignature;
	unsigned int*  numberOfTracksWithPercentHits;
	double         distanceOfMaximumCellAndCorrectCell;
	std::string    terminalString;
	unsigned long  numberOfTracksWithGoodLutSignature;
	unsigned short actualNumberOfTracksForSignatureAcceptance;
	unsigned short minimalNumberOfTracksForSignatureAcceptance;
	bitArray       actualSignatureForSignatureAcceptance;
	bitArray       searchSignatureForSignatureAcceptance;

#ifndef HIGHOCCURENCEISGOODSIGNATURE

	unsigned short actualClassForSignatureAcceptance;
	bool           searchFoundForSignatureAcceptance;

#endif

	numberOfTracksWithSignature  = new unsigned int[1 << (unsigned int)maxSignature.length()];
	if (numberOfTracksWithSignature == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentCorrectHits  = new unsigned int[1 << (unsigned int)maxSignature.length()];
	if (numberOfTracksWithPercentCorrectHits == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentHits  = new unsigned int[1 << (unsigned int)maxSignature.length()];
	if (numberOfTracksWithPercentHits == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (i = 0; i < (unsigned int)(1 << (unsigned int)maxSignature.length()); i++) {

		numberOfTracksWithSignature[i]          = 0;
		numberOfTracksWithPercentCorrectHits[i] = 0;
		numberOfTracksWithPercentHits[i]        = 0;

	}

	distanceOfMaximumCellAndCorrectCell = 0;
	numberOfTracksWithGoodLutSignature  = 0;
	terminalString                      = "\nAnalyse lut goodness:\t\t";

	evaluateLutDistribution(numberOfTracksWithSignature, numberOfTracksWithPercentCorrectHits, numberOfTracksWithPercentHits, &distanceOfMaximumCellAndCorrectCell, &numberOfTracksWithGoodLutSignature, terminalString, terminal);

	addNumberOfTracksWithGoodLutSignature(numberOfTracksWithGoodLutSignature);

	if (analysisResultWarnings & TRACKWITHHITINCORRECTCELLDISTRIBUTION) {

		trackWithHitsInCorrectCellDistributionWarningMsg* trackWithHitsInCorrectCellDistribution = new trackWithHitsInCorrectCellDistributionWarningMsg(numberOfTracksWithPercentCorrectHits, (1 << (unsigned int)maxSignature.length()));
		trackWithHitsInCorrectCellDistribution->warningMsg();
		if(trackWithHitsInCorrectCellDistribution != NULL) {
			delete trackWithHitsInCorrectCellDistribution;
			trackWithHitsInCorrectCellDistribution = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITINCELLDISTRIBUTION) {

		trackWithHitsInCellDistributionWarningMsg* trackWithHitsInCellDistribution = new trackWithHitsInCellDistributionWarningMsg(numberOfTracksWithPercentHits, (1 << (unsigned int)maxSignature.length()));
		trackWithHitsInCellDistribution->warningMsg();
		if(trackWithHitsInCellDistribution != NULL) {
			delete trackWithHitsInCellDistribution;
			trackWithHitsInCellDistribution = NULL;
		}

	}

	if (analysisResultWarnings & MIDDLEDISTANCEOFMAXIMUMANDCORRECTCELL) {

		middleDistanceOfMaximumAndCorrectCellWarningMsg* middleDistanceOfMaximumAndCorrectCell = new middleDistanceOfMaximumAndCorrectCellWarningMsg(distanceOfMaximumCellAndCorrectCell);
		middleDistanceOfMaximumAndCorrectCell->warningMsg();
		if(middleDistanceOfMaximumAndCorrectCell != NULL) {
			delete middleDistanceOfMaximumAndCorrectCell;
			middleDistanceOfMaximumAndCorrectCell = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHSIGNATUREDISTRIBUTION) {

		trackWithSignatureDistributionWarningMsg* trackWithSignatureDistribution = new trackWithSignatureDistributionWarningMsg(numberOfTracksWithSignature, (1 << (unsigned int)maxSignature.length()));
		trackWithSignatureDistribution->warningMsg();
		if(trackWithSignatureDistribution != NULL) {
			delete trackWithSignatureDistribution;
			trackWithSignatureDistribution = NULL;
		}

	}

	if (analysisResultDisplays & TRACKWITHHITINCORRECTCELLDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addCorrectLutCellDistribution(numberOfTracksWithPercentCorrectHits, (1 << (unsigned int)maxSignature.length()));

	}

	if (analysisResultDisplays & TRACKWITHHITINCELLDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addLutCellDistribution(numberOfTracksWithPercentHits, (1 << (unsigned int)maxSignature.length()));

	}

	if (analysisResultDisplays & TRACKWITHSIGNATUREDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addSignatureDistribution(numberOfTracksWithSignature, (1 << (unsigned int)maxSignature.length()));

	}

	actualNumberOfTracksForSignatureAcceptance  = 0;
	minimalNumberOfTracksForSignatureAcceptance = 0;
	for (i = 0; i < (unsigned int)(1 << (unsigned int)maxSignature.length()); i++)
		minimalNumberOfTracksForSignatureAcceptance += numberOfTracksWithPercentHits[i];
	minimalNumberOfTracksForSignatureAcceptance = (unsigned short)(((double)percentageOfTracksForSignature / (double)100) * (double)minimalNumberOfTracksForSignatureAcceptance);

#ifndef HIGHOCCURENCEISGOODSIGNATURE

	actualClassForSignatureAcceptance = (unsigned short)maxSignature.length();

#endif

	while (minimalNumberOfTracksForSignatureAcceptance > actualNumberOfTracksForSignatureAcceptance) {

		searchSignatureForSignatureAcceptance = bitArray(0);

#ifdef HIGHOCCURENCEISGOODSIGNATURE

		for (i = 0; i < (unsigned int)(1 << (unsigned int)maxSignature.length()); i++) {

			actualSignatureForSignatureAcceptance = bitArray(i);

			if (numberOfTracksWithPercentHits[i] > numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()])
				searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
			else if (numberOfTracksWithPercentHits[i] == numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()])
				if (actualSignatureForSignatureAcceptance.count() > searchSignatureForSignatureAcceptance.count())
					searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;

		}

		addGoodLutSignature(searchSignatureForSignatureAcceptance, bitArray(1));
		actualNumberOfTracksForSignatureAcceptance                                           += numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()];
		numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()]  = 0;

#else

		searchFoundForSignatureAcceptance = false;

		for (i = 0; i < (1 << (unsigned int)maxSignature.length()); i++) {

			actualSignatureForSignatureAcceptance = bitArray(i);
		
			if (actualSignatureForSignatureAcceptance.count() == actualClassForSignatureAcceptance) {

				if (numberOfTracksWithPercentHits[i] > numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()]) {

					searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
					searchFoundForSignatureAcceptance     = true;

				}

			}

		}

		if (searchFoundForSignatureAcceptance) {

			addGoodLutSignature(searchSignatureForSignatureAcceptance, bitArray(1));
			actualNumberOfTracksForSignatureAcceptance                                           += numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()];
			numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()]  = 0;

		}
		else
			actualClassForSignatureAcceptance--;

#endif

	}

	if (numberOfTracksWithSignature != NULL) {
		delete[] numberOfTracksWithSignature;
		numberOfTracksWithSignature = NULL;
	}
	if (numberOfTracksWithPercentCorrectHits != NULL) {
		delete[] numberOfTracksWithPercentCorrectHits;
		numberOfTracksWithPercentCorrectHits = NULL;
	}
	if (numberOfTracksWithPercentHits != NULL) {
		delete[] numberOfTracksWithPercentHits;
		numberOfTracksWithPercentHits = NULL;
	}

}

/****************************************************************
 * method evaluates the lut distribution.						*
 ****************************************************************/

void analysis::evaluateLutDistribution(unsigned int* numberOfTracksWithSignature, unsigned int* numberOfTracksWithPercentCorrectHits, unsigned int* numberOfTracksWithPercentHits, double* distanceOfMaximumCellAndCorrectCell, unsigned long* numberOfTracksWithGoodLutSignature, std::string terminalString, std::streambuf* terminal) {

	unsigned short           trackIndex;
	terminalSequence         statusSequenceForLut;
	std::list<findableTrack> findableTracks;
	trackfinderInputTrack*   track;
	trackfinderInputHit*     hit;
	lutHoughBorder           secondBorder;
	unsigned short           stationIndex;
	houghBorderPosition      actualPosition;
	bitArray                 trackSignature;
	bitArray                 correctCellSignature;
	histogramCellSignatures  cellSignatures;
	histogramCellSignature   bestCellSignature;
	unsigned short           numberOfCorrectSignaturePositions;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutError(ANALYSISLIB);

	/* this function creates a std::list which includes pointers to all
	 * findable tracks and their corresponding position in the Hough
	 * space */
	setupFindableTracks(&findableTracks);

	createTerminalStatusSequence(&statusSequenceForLut, terminal, terminalString, findableTracks.size());
	terminalInitialize(statusSequenceForLut);

	trackIndex = 0;
	cellSignatures.reset();

	/* loop to evaluate the prelut goodness for all findable tracks */
	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++, trackIndex++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		track->resetHitPointer();

		correctCellSignature = bitArray(0);
		trackSignature       = bitArray(0);
		/* loop to evaluate the magnetfield factor for all hits of this track */
		for (unsigned short i = 0; i < track->getNumberOfHits(); i++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() == NULL)
				throw cannotAccessStationError(ANALYSISLIB);
			else
				stationIndex = hit->getStation()->getIndex();

			trackSignature[stationIndex] = true;

			/* compute the borders for this hit */
			(*luts)->evaluateLut(hit, &secondBorder);

			secondBorder.houghCoord.resetActiveObject();
			for (unsigned short j = 0; j < secondBorder.houghCoord.getNumberOfEntries(); j++) {

				actualPosition = secondBorder.houghCoord.readActiveObjectAndMakeNextOneActive();

				/* Check if the first and the second dimension border is in the range */
				if ((actualPosition.pos1 == actualTrack->getPosition().get(DIM1)) && (actualPosition.pos2 == actualTrack->getPosition().get(DIM2)))
					correctCellSignature[stationIndex] = true;

				/* Accumulate the frequency of the border */
				cellSignatures.add(actualPosition, stationIndex);

			}

		}

		if (numberOfTracksWithSignature != NULL)
			numberOfTracksWithSignature[trackSignature.toULong()]++;

		if (numberOfTracksWithPercentCorrectHits != NULL)
			numberOfTracksWithPercentCorrectHits[correctCellSignature.toULong()]++;

		bestCellSignature = cellSignatures.getBest(actualTrack->getPosition());

		if (numberOfTracksWithPercentHits != NULL)
			numberOfTracksWithPercentHits[bestCellSignature.value.toULong()]++;

		if (distanceOfMaximumCellAndCorrectCell != NULL)
			(*distanceOfMaximumCellAndCorrectCell) += (bestCellSignature.distance / (double)findableTracks.size());

		numberOfCorrectSignaturePositions = 0;
		for (unsigned short k = 0; k < trackSignature.length(); k++) {

			if (trackSignature[k] && (trackSignature[k] == bestCellSignature.value[k]))
				numberOfCorrectSignaturePositions++;

		}

		if (numberOfCorrectSignaturePositions >= (unsigned short)ceil(((double)percentageOfHitsInSignature / (double)100) * (double)trackSignature.count()))
			if (numberOfTracksWithGoodLutSignature != NULL)
				(*numberOfTracksWithGoodLutSignature)++;

		cellSignatures.reset();
		terminalOverwrite(statusSequenceForLut, trackIndex + 1);

	}

	terminalFinalize(statusSequenceForLut);

}

/****************************************************************
 * method evaluates the goodness of both luts.					*
 ****************************************************************/

void analysis::evaluateBothLutsDistribution(unsigned int* numberOfTracksWithHits, std::string terminalString, std::streambuf* terminal) {

	unsigned short                  i;
	unsigned short                  trackIndex;
	terminalSequence                statusSequenceForBothLuts;
	std::list<findableTrack>        findableTracks;
	trackfinderInputTrack*          track;
	trackfinderInputHit*            hit;
	unsigned short                  stationIndex;
	prelutHoughBorder               evaluateFirstBorder;
	lutBorder                       border;
	prelutHoughBorder*              firstBorder;
	lutHoughBorder*                 secondBorder;
	hitsInHistogramBorderFrequency* histogramBorderFrequency;
	unsigned short                  numberOfHistogramBorderEntries;
	unsigned short                  borderIndex;
	hitsInHistogramBorderFrequency  maximumHistogramBorderFrequency;
	houghBorderPosition             actualPosition;
	histogramCellSignatures         cellSignatures;
	histogramCellSignature          bestCellSignature;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutError();

	if (numberOfTracksWithHits == NULL)
		throw cannotAccessDistributionMemoryError(ANALYSISLIB);

	firstBorder  = border.getPrelutHoughBorderPointer();
	secondBorder = border.getLutHoughBorderPointer();

	/* this function creates a std::list which includes pointers to all
	 * findable tracks and their corresponding position in the Hough
	 * space */
	setupFindableTracks(&findableTracks);

	createTerminalStatusSequence(&statusSequenceForBothLuts, terminal, terminalString, findableTracks.size());
	terminalInitialize(statusSequenceForBothLuts);

	trackIndex                     = 0;
	histogramBorderFrequency       = NULL;
	numberOfHistogramBorderEntries = 0;
	cellSignatures.reset();

	/* loop to evaluate the prelut goodness for all findable tracks */
	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++, trackIndex++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		track->resetHitPointer();

		/* loop to evaluate the magnetfield factor for all hits of this track */
		for (i = 0; i < track->getNumberOfHits(); i++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			/* compute the borders for this hit */
			(*luts)->evaluatePrelut(hit, &evaluateFirstBorder);

			/* Accumulate the frequency of the third dimension border */
			for (unsigned short j = evaluateFirstBorder.start; j <= evaluateFirstBorder.stop; j++) {

				if (numberOfHistogramBorderEntries == 0) {

					numberOfHistogramBorderEntries        = 1;
					histogramBorderFrequency              = (hitsInHistogramBorderFrequency*)calloc(numberOfHistogramBorderEntries, sizeof(hitsInHistogramBorderFrequency));
					histogramBorderFrequency[0].border    = j;
					histogramBorderFrequency[0].frequency = 1;

				}
				else {
					
					/* check if this border has occured until now */
					borderIndex = numberOfHistogramBorderEntries;
					for (unsigned short k = 0; k < numberOfHistogramBorderEntries; k++) {

						if (histogramBorderFrequency[k].border == j) {

							borderIndex = k;
							break;

						}

					}
					
					/* if the border has not occured */
					if (borderIndex == numberOfHistogramBorderEntries) {

						numberOfHistogramBorderEntries++;
						histogramBorderFrequency                        = (hitsInHistogramBorderFrequency*)realloc(histogramBorderFrequency, numberOfHistogramBorderEntries * sizeof(hitsInHistogramBorderFrequency));
						histogramBorderFrequency[borderIndex].border    = j;
						histogramBorderFrequency[borderIndex].frequency = 1;

					}
					/* if the border has occured */
					else
						histogramBorderFrequency[borderIndex].frequency++;

				}

			}

		}

		maximumHistogramBorderFrequency.frequency = 0;
		maximumHistogramBorderFrequency.distance  = 0;
		for (i = 0; i < numberOfHistogramBorderEntries; i++) {

			histogramBorderFrequency[i].distance = sqrt((double)sqr(histogramBorderFrequency[i].border - actualTrack->getPosition().get(DIM3)));

			if (histogramBorderFrequency[i].frequency > maximumHistogramBorderFrequency.frequency)
				maximumHistogramBorderFrequency = histogramBorderFrequency[i];
			/* This is added because if there is a equal found clone peak which is nearer to the
			 * correct cell, this peak should be used because of a better quality approximation */
			else if ((histogramBorderFrequency[i].frequency == maximumHistogramBorderFrequency.frequency) && (histogramBorderFrequency[i].distance < maximumHistogramBorderFrequency.distance))
				maximumHistogramBorderFrequency = histogramBorderFrequency[i];

		}

		if (histogramBorderFrequency != NULL) {
			free(histogramBorderFrequency);
			histogramBorderFrequency       = NULL;
			numberOfHistogramBorderEntries = 0;
		}

		/* loop to evaluate the total prelut anaylsis*/
		for (i = 0; i < track->getNumberOfHits(); i++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() == NULL)
				throw cannotAccessStationError(ANALYSISLIB);
			else
				stationIndex = hit->getStation()->getIndex();

			/* compute the borders for this hit */
			(*luts)->evaluate(hit, &border);

			/* Check if the third dimension border is in the range */
			if ((firstBorder->start <= maximumHistogramBorderFrequency.border) && (firstBorder->stop >= maximumHistogramBorderFrequency.border)) {

				secondBorder->houghCoord.resetActiveObject();
				for (unsigned short l = 0; l < secondBorder->houghCoord.getNumberOfEntries(); l++) {

					actualPosition = secondBorder->houghCoord.readActiveObjectAndMakeNextOneActive();

					/* Accumulate the frequency of the border */
					cellSignatures.add(actualPosition, stationIndex);

				}

			}

		}

		bestCellSignature = cellSignatures.getBest(actualTrack->getPosition());

		numberOfTracksWithHits[bestCellSignature.value.toULong()]++;

		cellSignatures.reset();

		terminalOverwrite(statusSequenceForBothLuts, trackIndex + 1);

	}

	terminalFinalize(statusSequenceForBothLuts);

}

/****************************************************************
 * method evaluates the magnetfield.							*
 ****************************************************************/

void analysis::evaluateMagnetfield(std::streambuf* terminal) {

	trackfinderInputMagneticField* magneticField;
	terminalSequence               statusSequenceForMagnetfield;
	unsigned int                   totalNumberOfBins;
	unsigned int                   actualProgress;
	unsigned int                   i;
	magnetfieldDisplayInfo*        actualDisplayInfo;
	double                         actualPositionIncrement;
	magneticFieldValue             actualFieldValue;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (magnetfieldAnalyser == NULL)
		throw cannotAccessMagnetfieldAnalyser();

	magneticField = (*eventData)->getMagneticField();
	if (magneticField == NULL)
		throw magneticFieldNotFoundError();

	initializeMagnetfieldEvaluation();

	totalNumberOfBins = 0;
	if (magnetfieldAnalyser->isMagnetfieldXBxAnalysisEnabled())
		if (magnetfieldAnalyser->getXBxDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getXBxDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldXByAnalysisEnabled())
		if (magnetfieldAnalyser->getXByDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getXByDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldXBzAnalysisEnabled())
		if (magnetfieldAnalyser->getXBzDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getXBzDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldYBxAnalysisEnabled())
		if (magnetfieldAnalyser->getYBxDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getYBxDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldYByAnalysisEnabled())
		if (magnetfieldAnalyser->getYByDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getYByDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldYBzAnalysisEnabled())
		if (magnetfieldAnalyser->getYBzDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getYBzDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldZBxAnalysisEnabled())
		if (magnetfieldAnalyser->getZBxDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getZBxDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldZByAnalysisEnabled())
		if (magnetfieldAnalyser->getZByDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getZByDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldZBzAnalysisEnabled())
		if (magnetfieldAnalyser->getZBzDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getZBzDisplayEntries();

	createTerminalStatusSequence(&statusSequenceForMagnetfield, terminal, "Analyse magnetfield:\t\t", totalNumberOfBins);
	terminalInitialize(statusSequenceForMagnetfield);

	actualProgress = 0;
	if (magnetfieldAnalyser->isMagnetfieldXBxAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getXBxDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getXBxDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getXBxDisplayEntries(); i++, actualProgress++) {

			magneticField->getFieldValues(i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setXBxPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldX());

			terminalOverwrite(statusSequenceForMagnetfield, actualProgress + 1);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldXByAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getXByDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getXByDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getXByDisplayEntries(); i++, actualProgress++) {

			magneticField->getFieldValues(i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setXByPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldY());

			terminalOverwrite(statusSequenceForMagnetfield, actualProgress + 1);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldXBzAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getXBzDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getXBzDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getXBzDisplayEntries(); i++, actualProgress++) {

			magneticField->getFieldValues(i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setXBzPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldZ());

			terminalOverwrite(statusSequenceForMagnetfield, actualProgress + 1);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldYBxAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getYBxDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getYBxDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getYBxDisplayEntries(); i++, actualProgress++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, i * actualPositionIncrement + actualDisplayInfo->min,  actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setYBxPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldX());

			terminalOverwrite(statusSequenceForMagnetfield, actualProgress + 1);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldYByAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getYByDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getYByDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getYByDisplayEntries(); i++, actualProgress++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setYByPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldY());

			terminalOverwrite(statusSequenceForMagnetfield, actualProgress + 1);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldYBzAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getYBzDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getYBzDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getYBzDisplayEntries(); i++, actualProgress++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setYBzPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldZ());

			terminalOverwrite(statusSequenceForMagnetfield, actualProgress + 1);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldZBxAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getZBxDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getZBxDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getZBxDisplayEntries(); i++, actualProgress++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, i * actualPositionIncrement + actualDisplayInfo->min, &actualFieldValue);
			magnetfieldAnalyser->setZBxPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldX());

			terminalOverwrite(statusSequenceForMagnetfield, actualProgress + 1);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldZByAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getZByDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getZByDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getZByDisplayEntries(); i++, actualProgress++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, i * actualPositionIncrement + actualDisplayInfo->min, &actualFieldValue);
			magnetfieldAnalyser->setZByPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldY());

			terminalOverwrite(statusSequenceForMagnetfield, actualProgress + 1);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldZBzAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getZBzDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getZBzDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getZBzDisplayEntries(); i++, actualProgress++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, i * actualPositionIncrement + actualDisplayInfo->min, &actualFieldValue);
			magnetfieldAnalyser->setZBzPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldZ());

			terminalOverwrite(statusSequenceForMagnetfield, actualProgress + 1);

		}

	}


	terminalFinalize(statusSequenceForMagnetfield);

	finalizeMagnetfieldEvaluation();

}

/****************************************************************
 * method compares the magnetfield with the factors.			*
 ****************************************************************/

void analysis::evaluateMagnetfieldVSFactors(double constXPos, double constYPos, bool median) {

	trackfinderInputMagneticField* magneticField;
	double                         actualZPos;
	double*                        positions;
	double*                        factors;
	magneticFieldValue*            fieldValues;
	magneticFieldValue*            integratedFieldValues;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	magneticField = (*eventData)->getMagneticField();
	if (magneticField == NULL)
		throw magneticFieldNotFoundForComparissonToFactorsError();

	positions = new double[(*eventData)->getNumberOfActiveStations()];
	if (positions == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	factors = new double[(*eventData)->getNumberOfActiveStations()];
	if (factors == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	fieldValues = new magneticFieldValue[(*eventData)->getNumberOfActiveStations()];
	if (fieldValues == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	integratedFieldValues = new magneticFieldValue[(*eventData)->getNumberOfActiveStations()];
	if (integratedFieldValues == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (unsigned short i = 0; i < (*eventData)->getNumberOfActiveStations(); i++) {

		actualZPos   = (*eventData)->getDetector().getStationByIndex(i).getDistance();
		positions[i] = actualZPos;
		factors[i]   = magneticField->evaluateMagnetfieldFactor(i);
		magneticField->getFieldValues(constXPos, constYPos, actualZPos, &fieldValues[i]);
		magneticField->getIntegratedFieldValues(constXPos, constYPos, actualZPos, i, &integratedFieldValues[i], median);

	}


	initializeShowAnalysis();

	showAnalyser->addMagnetfieldFactorsZ(positions, factors, (*eventData)->getNumberOfActiveStations());
	showAnalyser->addMagnetfieldValuesZ(positions, fieldValues, (*eventData)->getNumberOfActiveStations());
	showAnalyser->addIntegratedMagnetfieldValuesZ(positions, integratedFieldValues, (*eventData)->getNumberOfActiveStations());

	if (positions != NULL) {
		delete [] positions;
		positions = NULL;
	}

	if (factors != NULL) {
		delete [] factors;
		factors = NULL;
	}

	if (fieldValues != NULL) {
		delete [] fieldValues;
		fieldValues = NULL;
	}

	if (integratedFieldValues != NULL) {
		delete [] integratedFieldValues;
		integratedFieldValues = NULL;
	}

}

/****************************************************************
 * method evaluates the goodness of the used analytic formula.	*
 ****************************************************************/

void analysis::evaluateHoughTransformGoodness(std::streambuf* terminal) {

	unsigned int*            numberOfTracksWithMomentaError;
	unsigned short           i;
	unsigned short           j;
	std::list<findableTrack> findableTracks;
	terminalSequence         statusSequenceForFormula;
	unsigned short           trackIndex;
	trackfinderInputTrack*   track;
	trackfinderInputHit*     hit;
	lutBorder                border;
	prelutHoughBorder*       firstBorder;
	lutHoughBorder*          secondBorder;
	unsigned short           stationIndex;
	houghBorderPosition      actualPosition;
	histogramCellSignatures  cellSignatures;
	bitArray                 minimumClass;
	histogramCellSignature   bestCellSignature;
	trackMomentum            momentum;
	analyticFormula          formula;
	double                   absoluteTrackMomentum;
	double                   absoluteHitMomentum;
	double                   relativeErrorMomentum;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutError(ANALYSISLIB);

	numberOfTracksWithMomentaError  = new unsigned int[NUMBEROFDIFFERENTMOMENTAS];
	if (numberOfTracksWithMomentaError == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	firstBorder  = border.getPrelutHoughBorderPointer();
	secondBorder = border.getLutHoughBorderPointer();

	setupFindableTracks(&findableTracks);

	createTerminalStatusSequence(&statusSequenceForFormula, terminal, "\nAnalyse formula goodness:\t", findableTracks.size());
	terminalInitialize(statusSequenceForFormula);

	trackIndex   = 0;
	cellSignatures.reset();
	minimumClass = bitArray(0);
	for (i = 0; i < NUMBEROFDIFFERENTMOMENTAS; i++)
		numberOfTracksWithMomentaError[i]  = 0;

	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++, trackIndex++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		momentum.set(track->getMomX(), PX);
		momentum.set(track->getMomY(), PY);
		momentum.set(track->getMomZ(), PZ);

		absoluteTrackMomentum = formula.evaluateAbsP(momentum);

		track->resetHitPointer();

		/* loop to evaluate the magnetfield factor for all hits of this track */
		for (i = 0; i < track->getNumberOfHits(); i++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() == NULL)
				throw cannotAccessStationError(ANALYSISLIB);
			else
				stationIndex = hit->getStation()->getIndex();

			/* compute the borders for this hit */
			(*luts)->evaluate(hit, &border);

			secondBorder->houghCoord.resetActiveObject();
			for (j = 0; j < secondBorder->houghCoord.getNumberOfEntries(); j++) {

				actualPosition = secondBorder->houghCoord.readActiveObjectAndMakeNextOneActive();

				for(unsigned short k = firstBorder->start; k <= firstBorder->stop; k++) {

					/* Accumulate the frequency of the border */
					cellSignatures.add(actualPosition, k, stationIndex);

				}

			}

		}

		bestCellSignature = cellSignatures.getBest(actualTrack->getPosition());

		if (evaluateTrackPatternCoding(bestCellSignature.value) > minimumClass) {

			formula.evaluateP(bestCellSignature.pos, *(*space), &momentum);

			absoluteHitMomentum = formula.evaluateAbsP(momentum);

			if (absoluteHitMomentum < absoluteTrackMomentum)
				relativeErrorMomentum = (1 - (absoluteHitMomentum / absoluteTrackMomentum));
			else if (absoluteHitMomentum < 2 * absoluteTrackMomentum)
				relativeErrorMomentum = ((absoluteHitMomentum / absoluteTrackMomentum) - 1);
			else
				relativeErrorMomentum = 1;

			numberOfTracksWithMomentaError[(unsigned short)(relativeErrorMomentum * (NUMBEROFDIFFERENTMOMENTAS - 1) + 0.5)]++;

		}

		cellSignatures.reset();
		terminalOverwrite(statusSequenceForFormula, trackIndex + 1);

	}

	terminalFinalize(statusSequenceForFormula);

	if (analysisResultWarnings & TRACKWITHMOMENTAERRORDISTRIBUTION) {

		trackWithMomentaErrorDistributionWarningMsg* trackWithMomentaErrorDistribution = new trackWithMomentaErrorDistributionWarningMsg(numberOfTracksWithMomentaError, NUMBEROFDIFFERENTMOMENTAS);
		trackWithMomentaErrorDistribution->warningMsg();
		if(trackWithMomentaErrorDistribution != NULL) {
			delete trackWithMomentaErrorDistribution;
			trackWithMomentaErrorDistribution = NULL;
		}

	}

	if (analysisResultDisplays & TRACKWITHMOMENTAERRORDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addMomentaErrorDistribution(numberOfTracksWithMomentaError, NUMBEROFDIFFERENTMOMENTAS);

	}

	if (numberOfTracksWithMomentaError != NULL) {
		delete[] numberOfTracksWithMomentaError;
		numberOfTracksWithMomentaError = NULL;
	}

}

/****************************************************************
 * method evaluates the goodness of the used quatization.		*
 ****************************************************************/

void analysis::evaluateQuantizationGoodness(std::streambuf* terminal) {

	unsigned int*            numberOfTracksWithMomentumX;
	unsigned int*            numberOfTracksWithMomentumY;
	unsigned int*            numberOfTracksWithMomentumZ;
	unsigned int*            numberOfTracksWithDim1Coordinate;
	unsigned int*            numberOfTracksWithDim2Coordinate;
	unsigned int*            numberOfTracksWithDim3Coordinate;
	unsigned int*            numberOfTracksWithMomentum;
	unsigned int*            numberOfTracksWithCoordinate;
	double                   momentumXMin;
	double                   momentumXMax;
	double                   momentumXIncr;
	double                   momentumYMin;
	double                   momentumYMax;
	double                   momentumYIncr;
	double                   momentumZMin;
	double                   momentumZMax;
	double                   momentumZIncr;
	unsigned short           i;
	unsigned int             j;
	std::list<findableTrack> findableTracks;
	terminalSequence         statusSequenceForQuantization;
	unsigned short           trackIndex;
	trackfinderInputTrack*   track;
	double                   actualMomentum;
	unsigned short           actualPosition1;
	unsigned short           actualPosition2;
	unsigned short           actualPosition3;
	unsigned long            numberOfNotFindableTracks;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	numberOfTracksWithMomentumX      = new unsigned int[(*space)->getStep(HTHETA)];
	if (numberOfTracksWithMomentumX == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithMomentumY      = new unsigned int[(*space)->getStep(HGAMMA)];
	if (numberOfTracksWithMomentumY == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithMomentumZ      = new unsigned int[(*space)->getStep(HRADIUS)];
	if (numberOfTracksWithMomentumZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithDim1Coordinate = new unsigned int[(*space)->getStep(DIM1)];
	if (numberOfTracksWithDim1Coordinate == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithDim2Coordinate = new unsigned int[(*space)->getStep(DIM2)];
	if (numberOfTracksWithDim2Coordinate == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithDim3Coordinate = new unsigned int[(*space)->getStep(DIM3)];
	if (numberOfTracksWithDim3Coordinate == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithMomentum       = new unsigned int[(*space)->getStep(HTHETA) * (*space)->getStep(HGAMMA) * (*space)->getStep(HRADIUS)];
	if (numberOfTracksWithMomentum == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithCoordinate     = new unsigned int[(*space)->getStep(DIM1) * (*space)->getStep(DIM2) * (*space)->getStep(DIM3)];
	if (numberOfTracksWithCoordinate == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	momentumZMin  = 1 / (*space)->getMax(HRADIUS);
	momentumZMax  = 1 / (*space)->getIncr(HRADIUS);
	momentumZIncr = (momentumZMax - momentumZMin) / ((*space)->getStep(HRADIUS) - 1);
	momentumYMin  = tan((*space)->getMin(HGAMMA)) * momentumZMax;
	momentumYMax  = tan((*space)->getMax(HGAMMA)) * momentumZMax;
	momentumYIncr = (momentumYMax - momentumYMin) / ((*space)->getStep(HGAMMA) - 1);
	momentumXMin  = tan((*space)->getMin(HTHETA)) * momentumZMax;
	momentumXMax  = tan((*space)->getMax(HTHETA)) * momentumZMax;
	momentumXIncr = (momentumXMax - momentumXMin) / ((*space)->getStep(HTHETA) - 1);

	setupFindableTracks(&findableTracks);

	createTerminalStatusSequence(&statusSequenceForQuantization, terminal, "\nAnalyse quantization goodness:\t", findableTracks.size());
	terminalInitialize(statusSequenceForQuantization);

	for (i = 0; i < (*space)->getStep(HTHETA); i++)
		numberOfTracksWithMomentumX[i]       = 0;
	for (i = 0; i < (*space)->getStep(HGAMMA); i++)
		numberOfTracksWithMomentumY[i]       = 0;
	for (i = 0; i < (*space)->getStep(HRADIUS); i++)
		numberOfTracksWithMomentumZ[i]       = 0;
	for (i = 0; i < (*space)->getStep(DIM1); i++)
		numberOfTracksWithDim1Coordinate[i]  = 0;
	for (i = 0; i < (*space)->getStep(DIM2); i++)
		numberOfTracksWithDim2Coordinate[i]  = 0;
	for (i = 0; i < (*space)->getStep(DIM3); i++)
		numberOfTracksWithDim3Coordinate[i]  = 0;

	for (j = 0; j < (unsigned int)(*space)->getStep(HTHETA) * (unsigned int)(*space)->getStep(HGAMMA) * (unsigned int)(*space)->getStep(HRADIUS); j++)
		numberOfTracksWithMomentum[j]        = 0;
	for (j = 0; j < (unsigned int)(*space)->getStep(DIM1) * (unsigned int)(*space)->getStep(DIM2) * (unsigned int)(*space)->getStep(DIM3); j++)
		numberOfTracksWithCoordinate[j]      = 0;

	trackIndex = 0;
	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++, trackIndex++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		actualMomentum  = track->getMomX();
		if (actualMomentum <  momentumXMin)
			actualMomentum = momentumXMin;
		if (actualMomentum >  momentumXMax)
			actualMomentum = momentumXMax;
		actualPosition1 = (unsigned short)(((actualMomentum - momentumXMin) / momentumXIncr) + 0.5);
		numberOfTracksWithMomentumX[actualPosition1]++;
		actualMomentum  = track->getMomY();
		if (actualMomentum <  momentumYMin)
			actualMomentum = momentumYMin;
		if (actualMomentum >  momentumYMax)
			actualMomentum = momentumYMax;
		actualPosition2 = (unsigned short)(((actualMomentum - momentumYMin) / momentumYIncr) + 0.5);
		numberOfTracksWithMomentumY[actualPosition2]++;
		actualMomentum  = track->getMomZ();
		if (actualMomentum <  momentumZMin)
			actualMomentum = momentumZMin;
		if (actualMomentum >  momentumZMax)
			actualMomentum = momentumZMax;
		actualPosition3 = (unsigned short)(((actualMomentum - momentumZMin) / momentumZIncr) + 0.5);
		numberOfTracksWithMomentumZ[actualPosition3]++;

		if ((actualPosition1 < (*space)->getStep(HTHETA)) && (actualPosition2 < (*space)->getStep(HGAMMA)) && (actualPosition3 < (*space)->getStep(HRADIUS)))
			numberOfTracksWithMomentum[(unsigned int)actualPosition1 * (unsigned int)(*space)->getStep(HGAMMA) * (unsigned int)(*space)->getStep(HRADIUS) + (unsigned int)actualPosition2 * (unsigned int)(*space)->getStep(HRADIUS) + (unsigned int)actualPosition3]++;

		actualPosition1 = actualTrack->getPosition().get(DIM1);
		if (actualPosition1 < (*space)->getStep(DIM1))
			numberOfTracksWithDim1Coordinate[actualPosition1]++;
		actualPosition2 = actualTrack->getPosition().get(DIM2);
		if (actualPosition2 < (*space)->getStep(DIM2))
			numberOfTracksWithDim2Coordinate[actualPosition2]++;
		actualPosition3 = actualTrack->getPosition().get(DIM3);
		if (actualPosition3 < (*space)->getStep(DIM3))
			numberOfTracksWithDim3Coordinate[actualPosition3]++;

		if ((actualPosition1 < (*space)->getStep(DIM1)) && (actualPosition2 < (*space)->getStep(DIM2)) && (actualPosition3 < (*space)->getStep(DIM3)))
			numberOfTracksWithCoordinate[(unsigned int)actualPosition1 * (unsigned int)(*space)->getStep(DIM2) * (unsigned int)(*space)->getStep(DIM3) + (unsigned int)actualPosition2 * (unsigned int)(*space)->getStep(DIM3) + (unsigned int)actualPosition3]++;

		terminalOverwrite(statusSequenceForQuantization, trackIndex + 1);

	}

	numberOfNotFindableTracks = 0;
	for (j = 0; j < (unsigned int)(*space)->getStep(DIM1) * (unsigned int)(*space)->getStep(DIM2) * (unsigned int)(*space)->getStep(DIM3); j++)
		if (numberOfTracksWithCoordinate[j] > 1)
			numberOfNotFindableTracks += (numberOfTracksWithCoordinate[j] - 1);

	terminalFinalize(statusSequenceForQuantization);

	if (analysisResultWarnings & TRACKWITHMOMENTADISTRIBUTION) {

		trackWithMomentaXDistributionWarningMsg* trackWithMomentaXDistribution = new trackWithMomentaXDistributionWarningMsg(numberOfTracksWithMomentumX, (*space)->getStep(HTHETA), momentumXMin, momentumXMax);
		trackWithMomentaXDistribution->warningMsg();
		if(trackWithMomentaXDistribution != NULL) {
			delete trackWithMomentaXDistribution;
			trackWithMomentaXDistribution = NULL;
		}
		trackWithMomentaYDistributionWarningMsg* trackWithMomentaYDistribution = new trackWithMomentaYDistributionWarningMsg(numberOfTracksWithMomentumY, (*space)->getStep(HGAMMA), momentumYMin, momentumYMax);
		trackWithMomentaYDistribution->warningMsg();
		if(trackWithMomentaYDistribution != NULL) {
			delete trackWithMomentaYDistribution;
			trackWithMomentaYDistribution = NULL;
		}
		trackWithMomentaZDistributionWarningMsg* trackWithMomentaZDistribution = new trackWithMomentaZDistributionWarningMsg(numberOfTracksWithMomentumZ, (*space)->getStep(HRADIUS), momentumZMin, momentumZMax);
		trackWithMomentaZDistribution->warningMsg();
		if(trackWithMomentaZDistribution != NULL) {
			delete trackWithMomentaZDistribution;
			trackWithMomentaZDistribution = NULL;
		}
		trackWithCoordinateXDistributionWarningMsg* trackWithCoordinateXDistribution = new trackWithCoordinateXDistributionWarningMsg(numberOfTracksWithDim1Coordinate, (*space)->getStep(DIM1));
		trackWithCoordinateXDistribution->warningMsg();
		if(trackWithCoordinateXDistribution != NULL) {
			delete trackWithCoordinateXDistribution;
			trackWithCoordinateXDistribution = NULL;
		}
		trackWithCoordinateYDistributionWarningMsg* trackWithCoordinateYDistribution = new trackWithCoordinateYDistributionWarningMsg(numberOfTracksWithDim2Coordinate, (*space)->getStep(DIM2));
		trackWithCoordinateYDistribution->warningMsg();
		if(trackWithCoordinateYDistribution != NULL) {
			delete trackWithCoordinateYDistribution;
			trackWithCoordinateYDistribution = NULL;
		}
		trackWithCoordinateZDistributionWarningMsg* trackWithCoordinateZDistribution = new trackWithCoordinateZDistributionWarningMsg(numberOfTracksWithDim3Coordinate, (*space)->getStep(DIM3));
		trackWithCoordinateZDistribution->warningMsg();
		if(trackWithCoordinateZDistribution != NULL) {
			delete trackWithCoordinateZDistribution;
			trackWithCoordinateZDistribution = NULL;
		}
		trackWithMomentaDistributionWarningMsg* trackWithMomentaDistribution = new trackWithMomentaDistributionWarningMsg(numberOfTracksWithMomentum, (*space)->getStep(HTHETA) * (*space)->getStep(HGAMMA) * (*space)->getStep(HRADIUS));
		trackWithMomentaDistribution->warningMsg();
		if(trackWithMomentaDistribution != NULL) {
			delete trackWithMomentaDistribution;
			trackWithMomentaDistribution = NULL;
		}
		trackWithCoordinateDistributionWarningMsg* trackWithCoordinateDistribution = new trackWithCoordinateDistributionWarningMsg(numberOfTracksWithCoordinate, (*space)->getStep(DIM1) * (*space)->getStep(DIM2) * (*space)->getStep(DIM3));
		trackWithCoordinateDistribution->warningMsg();
		if(trackWithCoordinateDistribution != NULL) {
			delete trackWithCoordinateDistribution;
			trackWithCoordinateDistribution = NULL;
		}

	}

	if (analysisResultDisplays & TRACKWITHMOMENTADISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addMomentaXDistribution(numberOfTracksWithMomentumX, (*space)->getStep(HTHETA), momentumXMin, momentumXMax);
		showAnalyser->addMomentaYDistribution(numberOfTracksWithMomentumY, (*space)->getStep(HGAMMA), momentumYMin, momentumYMax);
		showAnalyser->addMomentaZDistribution(numberOfTracksWithMomentumZ, (*space)->getStep(HRADIUS), momentumZMin, momentumZMax);
		showAnalyser->addCoordinateXDistribution(numberOfTracksWithDim1Coordinate, (*space)->getStep(DIM1));
		showAnalyser->addCoordinateYDistribution(numberOfTracksWithDim2Coordinate, (*space)->getStep(DIM2));
		showAnalyser->addCoordinateZDistribution(numberOfTracksWithDim3Coordinate, (*space)->getStep(DIM3));

	}

	if (analysisResultDisplays & COMPLETEMOMENTADISTRIBUTION) {

		if (analysisResultDisplays & COMPLETECOORDINATEDISTRIBUTION) {

			youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg* youCannotEnableTrackWithSameMomentaAndCoordinateDistribution = new youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg();
			youCannotEnableTrackWithSameMomentaAndCoordinateDistribution->warningMsg();
			if(youCannotEnableTrackWithSameMomentaAndCoordinateDistribution != NULL) {
				delete youCannotEnableTrackWithSameMomentaAndCoordinateDistribution;
				youCannotEnableTrackWithSameMomentaAndCoordinateDistribution = NULL;
			}

		}
		else {

			initializeShowAnalysis();

			showAnalyser->addMomentaDistribution(numberOfTracksWithMomentum, (unsigned int)(*space)->getStep(HTHETA) * (unsigned int)(*space)->getStep(HGAMMA) * (unsigned int)(*space)->getStep(HRADIUS));

		}

	}
	else {

		if (analysisResultDisplays & COMPLETECOORDINATEDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addCoordinateDistribution(numberOfTracksWithCoordinate, (unsigned int)(*space)->getStep(DIM1) * (unsigned int)(*space)->getStep(DIM2) * (unsigned int)(*space)->getStep(DIM3));

		}

	}

	if (analysisResultWarnings & TRACKWITHSAMECOORDINATEDISTRIBUTION) {

		trackWithSameCoordinateDistributionWarningMsg* trackWithSameCoordinateDistribution = new trackWithSameCoordinateDistributionWarningMsg(numberOfTracksWithCoordinate, (*space)->getStep(DIM1) * (*space)->getStep(DIM2) * (*space)->getStep(DIM3));
		trackWithSameCoordinateDistribution->warningMsg();
		if(trackWithSameCoordinateDistribution != NULL) {
			delete trackWithSameCoordinateDistribution;
			trackWithSameCoordinateDistribution = NULL;
		}

	}

	if (analysisResultWarnings & NUMBEROFTRACKSWHICHCANNOTBEFOUND) {

		numberOfTracksWhichCannotBeFoundWarningMsg* numberOfTracksWhichCannotBeFound = new numberOfTracksWhichCannotBeFoundWarningMsg(numberOfNotFindableTracks);
		numberOfTracksWhichCannotBeFound->warningMsg();
		if(numberOfTracksWhichCannotBeFound != NULL) {
			delete numberOfTracksWhichCannotBeFound;
			numberOfTracksWhichCannotBeFound = NULL;
		}

	}

	addNumberOfTracksWhichCannotBeFound(numberOfNotFindableTracks);

	if (numberOfTracksWithMomentumX != NULL) {
		delete[] numberOfTracksWithMomentumX;
		numberOfTracksWithMomentumX = NULL;
	}
	if (numberOfTracksWithMomentumY != NULL) {
		delete[] numberOfTracksWithMomentumY;
		numberOfTracksWithMomentumY = NULL;
	}
	if (numberOfTracksWithMomentumZ != NULL) {
		delete[] numberOfTracksWithMomentumZ;
		numberOfTracksWithMomentumZ = NULL;
	}
	if (numberOfTracksWithDim1Coordinate != NULL) {
		delete[] numberOfTracksWithDim1Coordinate;
		numberOfTracksWithDim1Coordinate = NULL;
	}
	if (numberOfTracksWithDim2Coordinate != NULL) {
		delete[] numberOfTracksWithDim2Coordinate;
		numberOfTracksWithDim2Coordinate = NULL;
	}
	if (numberOfTracksWithDim3Coordinate != NULL) {
		delete[] numberOfTracksWithDim3Coordinate;
		numberOfTracksWithDim3Coordinate = NULL;
	}

	if (numberOfTracksWithMomentum != NULL) {
		delete[] numberOfTracksWithMomentum;
		numberOfTracksWithMomentum = NULL;
	}
	if (numberOfTracksWithCoordinate != NULL) {
		delete[] numberOfTracksWithCoordinate;
		numberOfTracksWithCoordinate = NULL;
	}

}

/****************************************************************
 * method initializes the configuration-Analysis.				*
 ****************************************************************/

void analysis::initConfigurationAnalysis(bool enable) {

	configuration = enable;

}

/****************************************************************
 * method initializes the configuration-Analysis.				*
 ****************************************************************/

void analysis::initDetectorAnalysis(bool enable) {

	detector = enable;

}

/****************************************************************
 * method initializes the memory-Analysis.						*
 ****************************************************************/

void analysis::initMemoryAnalysis(bool enable) {

	memory = enable;

}

/****************************************************************
 * method initializes the event-Analysis.						*
 ****************************************************************/

void analysis::initEventAnalysis(bool enable) {

	event = enable;

}

/****************************************************************
 * method initializes the classPriority-Analysis.				*
 ****************************************************************/

void analysis::initClassPriorityAnalysis(bool enable) {

	classPriority = enable;

}

/****************************************************************
 * method initializes the qualityEFGCEventAbsolute-Analysis.	*
 ****************************************************************/

void analysis::initQualityEFGCEventAbsoluteAnalysis(bool enable) {

	if (enable) {

		if (qualityAnalyser == NULL)
			qualityAnalyser = new qualityAnalysis();

		qualityAnalyser->initQualityEFGCEventAbsoluteAnalysis(enable);

	}
	else {

		if (qualityAnalyser != NULL) {

			qualityAnalyser->initQualityEFGCEventAbsoluteAnalysis(enable);

			if ((!qualityAnalyser->isQualityEFGCEventAnalysisEnabled()) && (!qualityAnalyser->isQualityEFGCTotalAnalysisEnabled())) {
				delete qualityAnalyser;
				qualityAnalyser = NULL;
			}
		
		}

	}

}

/****************************************************************
 * method initializes the qualityEFGCEventRelative-Analysis.	*
 ****************************************************************/

void analysis::initQualityEFGCEventRelativeAnalysis(bool enable) {

	if (enable) {

		if (qualityAnalyser == NULL)
			qualityAnalyser = new qualityAnalysis();

		qualityAnalyser->initQualityEFGCEventRelativeAnalysis(enable);

	}
	else {

		if (qualityAnalyser != NULL) {

			qualityAnalyser->initQualityEFGCEventRelativeAnalysis(enable);

			if ((!qualityAnalyser->isQualityEFGCEventAnalysisEnabled()) && (!qualityAnalyser->isQualityEFGCTotalAnalysisEnabled())) {
				delete qualityAnalyser;
				qualityAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the qualityEFGCTotalAbsolute-Analysis.	*
 ****************************************************************/

void analysis::initQualityEFGCTotalAbsoluteAnalysis(bool enable) {

	if (enable) {

		if (qualityAnalyser == NULL)
			qualityAnalyser = new qualityAnalysis();

		qualityAnalyser->initQualityEFGCTotalAbsoluteAnalysis(enable);

	}
	else {

		if (qualityAnalyser != NULL) {

			qualityAnalyser->initQualityEFGCTotalAbsoluteAnalysis(enable);

			if ((!qualityAnalyser->isQualityEFGCEventAnalysisEnabled()) && (!qualityAnalyser->isQualityEFGCTotalAnalysisEnabled())) {
				delete qualityAnalyser;
				qualityAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the qualityEFGCTotalRelative-Analysis.	*
 ****************************************************************/

void analysis::initQualityEFGCTotalRelativeAnalysis(bool enable) {

	if (enable) {

		if (qualityAnalyser == NULL)
			qualityAnalyser = new qualityAnalysis();

		qualityAnalyser->initQualityEFGCTotalRelativeAnalysis(enable);

	}
	else {

		if (qualityAnalyser != NULL) {

			qualityAnalyser->initQualityEFGCTotalRelativeAnalysis(enable);

			if ((!qualityAnalyser->isQualityEFGCEventAnalysisEnabled()) && (!qualityAnalyser->isQualityEFGCTotalAnalysisEnabled())) {
				delete qualityAnalyser;
				qualityAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzE-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzEAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzEAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzF-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzFAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzFAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzG-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzGAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzGAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzC-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzEFGC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzEFGCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzEFGCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtE-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtEAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtEAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtF-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtFAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtFAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtG-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtGAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtGAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtC-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtEFGC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtEFGCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtEFGCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzE-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzEAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzEAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzF-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzFAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzFAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzG-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzGAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzGAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzEFGC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzEFGCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzEFGCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtE-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtEAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtEAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtF-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtFAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtFAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtG-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtGAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtGAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtEFGC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtEFGCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtEFGCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initMomentumAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisMomentumOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisMomentumOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (momentumAnalyser != NULL) {
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			momentumAnalyser->initMomentumAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initProjectionAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisProjectionOutputFileNameSpecifiedWarningMsg* noAnalysisProjectionOutputFileNameSpecified = new noAnalysisProjectionOutputFileNameSpecifiedWarningMsg();
		noAnalysisProjectionOutputFileNameSpecified->warningMsg();
		if(noAnalysisProjectionOutputFileNameSpecified != NULL) {
			delete noAnalysisProjectionOutputFileNameSpecified;
			noAnalysisProjectionOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (projectionAnalyser != NULL) {
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			projectionAnalyser->initProjectionAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the graphic's display.				*
 ****************************************************************/

void analysis::initMomentumAnalysisDisplay(bool enable) {

	if (enable) {

		if (projectionAnalyser != NULL) {

			if (projectionAnalyser->isProjectionDisplayEnabled()) {

				projectionAnalyser->initProjectionAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser != NULL) {

			if (magnetfieldAnalyser->isMagnetfieldDisplayEnabled()) {

				magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser != NULL) {

			if (magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled()) {

				magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumAnalysisDisplay(enable);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumAnalysisDisplay(enable);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the graphic's display.				*
 ****************************************************************/

void analysis::initProjectionAnalysisDisplay(bool enable) {

	if (enable) {

		if (momentumAnalyser != NULL) {

			if (momentumAnalyser->isMomentumDisplayEnabled()) {

				momentumAnalyser->initMomentumAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser != NULL) {

			if (magnetfieldAnalyser->isMagnetfieldDisplayEnabled()) {

				magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser != NULL) {

			if (magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled()) {

				magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionAnalysisDisplay(enable);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionAnalysisDisplay(enable);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXBxAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldXBxAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldXBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldXBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXByAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldXByAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldXByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldXByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXBzAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldXBzAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldXBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldXBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the										*
 * initMagnetfieldFactorAnalysis-Analysis.						*
 ****************************************************************/

void analysis::initMagnetfieldConstantAnalysis(bool enable, int nBins, double min, double max) {

	if (enable) {

		if (magnetfieldFactorAnalyser == NULL)
			magnetfieldFactorAnalyser = new magnetfieldFactorAnalysis();
		
		magnetfieldFactorAnalyser->init(nBins, min, max);

	}
	else {

		if (magnetfieldFactorAnalyser != NULL) {

			delete magnetfieldFactorAnalyser;
			magnetfieldFactorAnalyser = NULL;

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldXAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldXAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldXAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYBxAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldYBxAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldYBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldYBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYByAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldYByAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldYByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldYByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYBzAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldYBzAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldYBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldYBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldYAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldYAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldYAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZBxAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldZBxAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldZBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldZBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZByAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldZByAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldZByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldZByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZBzAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldZBzAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldZBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldZBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldZAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldZAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldZAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldAnalysis(bool enable, int nBins, double xMin, double yMin, double zMin, double xMax, double yMax, double zMax, double constValDim1, double constValDim2, double constValueDim3) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldAnalysis(enable, nBins, xMin, yMin, zMin, xMax, yMax, zMax, constValDim1, constValDim2, constValueDim3);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldAnalysis(enable, nBins, xMin, yMin, zMin, xMax, yMax, zMax, constValDim1, constValDim2, constValueDim3);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initMagnetfieldAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisMomentumOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisMomentumOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (magnetfieldAnalyser != NULL) {
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			magnetfieldAnalyser->initMagnetfieldAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the magnetfield's display.			*
 ****************************************************************/

void analysis::initMagnetfieldAnalysisDisplay(bool enable) {

	if (enable) {

		if (momentumAnalyser != NULL) {

			if (momentumAnalyser->isMomentumDisplayEnabled()) {

				momentumAnalyser->initMomentumAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (projectionAnalyser != NULL) {

			if (projectionAnalyser->isProjectionDisplayEnabled()) {

				projectionAnalyser->initProjectionAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser != NULL) {

			if (magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled()) {

				magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(enable);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(enable);

			if ((!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled())) {
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initMagnetfieldConstantAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisMomentumOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisMomentumOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (magnetfieldFactorAnalyser != NULL) {
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the magnetfield's display.			*
 ****************************************************************/

void analysis::initMagnetfieldConstantAnalysisDisplay(bool enable) {

	if (enable) {

		if (momentumAnalyser != NULL) {

			if (momentumAnalyser->isMomentumDisplayEnabled()) {

				momentumAnalyser->initMomentumAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (projectionAnalyser != NULL) {

			if (projectionAnalyser->isProjectionDisplayEnabled()) {

				projectionAnalyser->initProjectionAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser != NULL) {

			if (magnetfieldAnalyser->isMagnetfieldDisplayEnabled()) {

				magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser == NULL)
			magnetfieldFactorAnalyser = new magnetfieldFactorAnalysis();

		magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(enable);

	}
	else {

		if (magnetfieldFactorAnalyser != NULL) {

			magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(enable);

			if ((!magnetfieldFactorAnalyser->isMagnetfieldFactorAnalysisEnabled())) {
				delete magnetfieldFactorAnalyser;
				magnetfieldFactorAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the histogram analysis.				*
 ****************************************************************/

void analysis::initHistogramAnalysis(bool enable, histogramData** histogram) {

	if (enable) {

		if (histogramAnalyser == NULL)
			histogramAnalyser = new histogramAnalysis(histogram);

	}
	else {

		if (histogramAnalyser != NULL) {

			delete histogramAnalyser;
			histogramAnalyser = NULL;

		}

	}

}

/****************************************************************
 * This method initializes the root directory for the analysis.	*
 ****************************************************************/

void analysis::initHistogramAnalysis(bool enableCreated, bool enableEncoded, bool enableFiltered,
									 bool justOneCreated, bool justOneEncoded, bool justOneFiltered,
									 bool showCreated, bool showEncoded, bool showFiltered,
									 unsigned short layer, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisMomentumOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisMomentumOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (histogramAnalyser != NULL) {
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			histogramAnalyser->initHistogramAnalysisToRoot(enableCreated, enableEncoded, enableFiltered, justOneCreated, justOneEncoded, justOneFiltered, name, justUpdate);
			histogramAnalyser->initHistogramAnalysisToShow(showCreated, showEncoded, showFiltered);
			histogramAnalyser->initHistogramLayer(layer);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * method returns true if the configuration-Analysis is enabled.*
 ****************************************************************/

bool analysis::isConfigurationAnalysisEnabled() {

	return configuration;

}

/****************************************************************
 * method returns true if the detector-Analysis is enabled.		*
 ****************************************************************/

bool analysis::isDetectorAnalysisEnabled() {

	return detector;

}

/****************************************************************
 * method returns true if the memory-Analysis is enabled.		*
 ****************************************************************/

bool analysis::isMemoryAnalysisEnabled() {

	return memory;

}

/****************************************************************
 * method returns true if the event-Analysis is enabled.		*
 ****************************************************************/

bool analysis::isEventAnalysisEnabled() {

	return event;

}

/****************************************************************
 * method returns true if the classPriority-Analysis is enabled.*
 ****************************************************************/

bool analysis::isClassPriorityAnalysisEnabled() {

	return classPriority;

}

/****************************************************************
 * method returns true if the quality-Analysis for each event	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isQualityEventAnalysisEnabled() {

	bool returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->isQualityEFGCEventAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the quality-Analysis over all events	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isQualityTotalAnalysisEnabled() {

	bool returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->isQualityEFGCTotalAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the momentumEFGC-Analysis for each	*
 * event is enabled.											*
 ****************************************************************/

bool analysis::isMomentumEFGCEventAnalysisEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = (momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled() || momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled());
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the projectionEFGCN-Analysis for each	*
 * event is enabled.											*
 ****************************************************************/

bool analysis::isProjectionEFGCNEventAnalysisEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = (projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled() || projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled() || projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled());
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the momentumEFGC-Analysis over all	*
 * events is enabled.											*
 ****************************************************************/

bool analysis::isMomentumEFGCTotalAnalysisEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = (momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled() || momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled());
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the projectionEFGCN-Analysis over all	*
 * events is enabled.											*
 ****************************************************************/

bool analysis::isProjectionEFGCNTotalAnalysisEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = (projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled() || projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled() || projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled());
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the									*
 * graphic-to-root-analysis is enabled.							*
 ****************************************************************/

bool analysis::isMomentumToRootEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->isMomentumToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the									*
 * graphic-to-root-analysis is enabled.							*
 ****************************************************************/

bool analysis::isProjectionToRootEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = projectionAnalyser->isProjectionToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the									*
 * magnetfield-to-root-analysis is enabled.						*
 ****************************************************************/

bool analysis::isMagnetfieldToRootEnabled() {

	bool returnValue;

	if (magnetfieldAnalyser != NULL)
		returnValue = magnetfieldAnalyser->isMagnetfieldToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the									*
 * magnetfield-constant-to-root-analysis is enabled.			*
 ****************************************************************/

bool analysis::isMagnetfieldFactorToRootEnabled() {

	bool returnValue;

	if (magnetfieldFactorAnalyser != NULL)
		returnValue = magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * qualityEFGCEvent-Analysis.									*
 ****************************************************************/

std::string analysis::getQualityEFGCEventAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getQualityEFGCEventAnalysis();
	else
		returnValue = "";

	return returnValue;

}
std::string analysis::getAbsoluteQualityEFGCEventAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getAbsoluteQualityEFGCEventAnalysis();
	else
		returnValue = "";

	return returnValue;

}
std::string analysis::getRelativeQualityEFGCEventAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getRelativeQualityEFGCEventAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * qualityEFGCTotal-Analysis.									*
 ****************************************************************/

std::string analysis::getQualityEFGCTotalAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getQualityEFGCTotalAnalysis();
	else
		returnValue = "";

	return returnValue;

}
std::string analysis::getAbsoluteQualityEFGCTotalAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getAbsoluteQualityEFGCTotalAnalysis();
	else
		returnValue = "";

	return returnValue;

}
std::string analysis::getRelativeQualityEFGCTotalAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getRelativeQualityEFGCTotalAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * method returns true if the momentumAnalysis for each event	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isMomentumEventAnalysisEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->isMomentumEventAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the momentumAnalysis over all events	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isMomentumTotalAnalysisEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->isMomentumTotalAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true if the graphic's display is enabled.*
 ****************************************************************/

bool analysis::isMomentumDisplayEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->isMomentumDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the graphic-Analysis display.					*
 ****************************************************************/

void analysis::momentumAnalysisUpdate() {

	if (momentumAnalyser != NULL)
		momentumAnalyser->momentumAnalysisUpdate();
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the graphic-Analysis display.					*
 ****************************************************************/

void analysis::momentumAnalysisDraw() {

	if (momentumAnalyser != NULL)
		momentumAnalyser->momentumAnalysisDraw();
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method returns true if the momentumAnalysis for each event	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isProjectionEventAnalysisEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = projectionAnalyser->isProjectionEventAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the momentumAnalysis over all events	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isProjectionTotalAnalysisEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = projectionAnalyser->isProjectionTotalAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true if the graphic's display is enabled.*
 ****************************************************************/

bool analysis::isProjectionDisplayEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = projectionAnalyser->isProjectionDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the graphic-Analysis display.					*
 ****************************************************************/

void analysis::projectionAnalysisUpdate() {

	if (projectionAnalyser != NULL) {

		projectionAnalyser->controlProjectionEFGCNEvent12EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNEvent13EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM3));
		projectionAnalyser->controlProjectionEFGCNEvent32EFGCNRange((*space)->getStep(DIM3), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNTotal12EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNTotal13EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM3));
		projectionAnalyser->controlProjectionEFGCNTotal32EFGCNRange((*space)->getStep(DIM3), (*space)->getStep(DIM2));

		projectionAnalyser->projectionAnalysisUpdate();

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the graphic-Analysis display.					*
 ****************************************************************/

void analysis::projectionAnalysisDraw() {

	if (projectionAnalyser != NULL) {

		projectionAnalyser->controlProjectionEFGCNEvent12EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNEvent13EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM3));
		projectionAnalyser->controlProjectionEFGCNEvent32EFGCNRange((*space)->getStep(DIM3), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNTotal12EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNTotal13EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM3));
		projectionAnalyser->controlProjectionEFGCNTotal32EFGCNRange((*space)->getStep(DIM3), (*space)->getStep(DIM2));
		
		projectionAnalyser->projectionAnalysisDraw();
	
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
/****************************************************************
 * method returns true if the magnetfieldAnalysis				*
 * is enabled.													*
 ****************************************************************/

bool analysis::isMagnetfieldAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldAnalyser != NULL)
		returnValue = magnetfieldAnalyser->isMagnetfieldAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true if the magnetfield's display is		*
 * enabled.														*
 ****************************************************************/

bool analysis::isMagnetfieldDisplayEnabled() {

	bool returnValue;

	if (magnetfieldAnalyser != NULL)
		returnValue = magnetfieldAnalyser->isMagnetfieldDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the magnetfield-Analysis display.				*
 ****************************************************************/

void analysis::magnetfieldAnalysisUpdate() {

	if (magnetfieldAnalyser != NULL)
		magnetfieldAnalyser->magnetfieldAnalysisUpdate();
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the magnetfield-Analysis display.				*
 ****************************************************************/

void analysis::magnetfieldAnalysisDraw() {

	if (magnetfieldAnalyser != NULL)
		magnetfieldAnalyser->magnetfieldAnalysisDraw();
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method returns true if the magnetfield factor's display	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isMagnetfieldFactorDisplayEnabled() {

	bool returnValue;

	if (magnetfieldFactorAnalyser != NULL)
		returnValue = magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the magnetfield factor-Analysis display.		*
 ****************************************************************/

void analysis::magnetfieldFactorAnalysisUpdate() {

	if (magnetfieldFactorAnalyser != NULL) {

		magnetfieldFactorAnalyser->set((*eventData)->getNumberOfActiveStations());
		magnetfieldFactorAnalyser->magnetfieldFactorAnalysisUpdate();
	
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the magnetfield factor-Analysis display.		*
 ****************************************************************/

void analysis::magnetfieldFactorAnalysisDraw(bitArray preventDraw) {

	if (magnetfieldFactorAnalyser != NULL) {

		magnetfieldFactorAnalyser->set((*eventData)->getNumberOfActiveStations());
		magnetfieldFactorAnalyser->magnetfieldFactorAnalysisDraw(preventDraw);

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the momentumEvent-Analysis for each event into	*
 * a root file.													*
 ****************************************************************/

void analysis::momentumEventAnalysisWrite(int eventNumber) {

	if (momentumAnalyser != NULL)
		if (momentumAnalyser->isMomentumToRootEnabled())
			momentumAnalyser->momentumEventAnalysisWrite(eventNumber);
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the momentumTotal-Analysis over all events into*
 * a root file.													*
 ****************************************************************/

void analysis::momentumTotalAnalysisWrite(int eventNumber) {

	if (momentumAnalyser != NULL)
		if (momentumAnalyser->isMomentumToRootEnabled())
			momentumAnalyser->momentumTotalAnalysisWrite(eventNumber);
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the projectionEvent-Analysis for each event	*
 * into a root file.											*
 ****************************************************************/

void analysis::projectionEventAnalysisWrite(int eventNumber) {

	if (projectionAnalyser != NULL)
		if (projectionAnalyser->isProjectionToRootEnabled())
			projectionAnalyser->projectionEventAnalysisWrite(eventNumber);
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the projectionTotal-Analysis over all events	*
 * into a root file.											*
 ****************************************************************/

void analysis::projectionTotalAnalysisWrite(int eventNumber) {

	if (projectionAnalyser != NULL)
		if (projectionAnalyser->isProjectionToRootEnabled())
			projectionAnalyser->projectionTotalAnalysisWrite(eventNumber);
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the magnetfield-Analysis into					*
 * a root file.													*
 ****************************************************************/

void analysis::magnetfieldAnalysisWrite() {

	if (magnetfieldAnalyser != NULL)
		if (magnetfieldAnalyser->isMagnetfieldToRootEnabled())
			magnetfieldAnalyser->magnetfieldAnalysisWrite();
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the magnetfieldFactor-Analysis into			*
 * a root file.													*
 ****************************************************************/

void analysis::magnetfieldFactorAnalysisWrite(int eventNumber) {

	if (magnetfieldFactorAnalyser != NULL) {

		if (magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled()) {
			magnetfieldFactorAnalyser->set((*eventData)->getNumberOfActiveStations());
			magnetfieldFactorAnalyser->magnetfieldFactorAnalysisWrite(eventNumber);
		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method initializes the momentumEFGCEventAnalysis.			*
 ****************************************************************/

void analysis::initMomentumEventAnalysis(bool enable) {

	if (momentumAnalyser != NULL)
		momentumAnalyser->initMomentumEventAnalysis(enable);

}

/****************************************************************
 * method initializes the momentumEFGCTotalAnalysis.			*
 ****************************************************************/

void analysis::initMomentumTotalAnalysis(bool enable) {

	if (momentumAnalyser != NULL)
		momentumAnalyser->initMomentumTotalAnalysis(enable);

}

/****************************************************************
 * method initializes the projectionEFGCNEventAnalysis.			*
 ****************************************************************/

void analysis::initProjectionEventAnalysis(bool enable) {

	if (projectionAnalyser != NULL)
		projectionAnalyser->initProjectionEventAnalysis(enable);

}

/****************************************************************
 * method initializes the projectionEFGCNTotalAnalysis.			*
 ****************************************************************/

void analysis::initProjectionTotalAnalysis(bool enable) {

	if (projectionAnalyser != NULL)
		projectionAnalyser->initProjectionTotalAnalysis(enable);

}

/****************************************************************
 * method returns a string representing the result of the		*
 * momentumEvent-Analysis.										*
 ****************************************************************/

std::string analysis::getMomentumEventAnalysis() {

	std::string returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->getMomentumEFGCEventAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * momentumTotal-Analysis.										*
 ****************************************************************/

std::string analysis::getMomentumTotalAnalysis() {

	std::string returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->getMomentumEFGCTotalAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * magnetfield-Analysis.										*
 ****************************************************************/

std::string analysis::getMagnetfieldAnalysis() {

	std::string returnValue;

	if (magnetfieldAnalyser != NULL)
		returnValue = magnetfieldAnalyser->getMagnetfieldAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * method returns if the created histogram layer can			*
 * be written into the file.									*
 ****************************************************************/

bool analysis::isWritingCreatedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isCreatedHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the encoded histogram layer can			*
 * be written into the file.									*
 ****************************************************************/

bool analysis::isWritingEncodedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isEncodedHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the filtered histogram layer can			*
 * be written into the file.									*
 ****************************************************************/

bool analysis::isWritingFilteredHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isFilteredHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if just one created histogram layer can		*
 * be accessed.													*
 ****************************************************************/

bool analysis::isWritingJustOneCreatedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isJustOneCreatedHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if just one encoded histogram layer can		*
 * be accessed.													*
 ****************************************************************/

bool analysis::isWritingJustOneEncodedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isJustOneEncodedHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if just one filtered histogram layer can		*
 * be accessed.													*
 ****************************************************************/

bool analysis::isWritingJustOneFilteredHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isJustOneFilteredHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the created histogram layer can			*
 * be shown.													*
 ****************************************************************/

bool analysis::isShowingCreatedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isCreatedHistogramShowingEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the encoded histogram layer can			*
 * be shown.													*
 ****************************************************************/

bool analysis::isShowingEncodedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isEncodedHistogramShowingEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the filtered histogram layer can			*
 * be shown.													*
 ****************************************************************/

bool analysis::isShowingFilteredHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isFilteredHistogramShowingEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns the index of the histogram layer			*
 * which should be accessed.									*
 ****************************************************************/

unsigned short analysis::getWritingHistogramLayerIndex() {

	unsigned short returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->getHistogramLayerIndexToRoot();
	else
		returnValue = 0;

	return returnValue;

}

/****************************************************************
 * method writes the actual created histogram layer as the		*
 * layer 'layer' into the file.									*
 ****************************************************************/

void analysis::createdHistogramLayerWrite(unsigned short layer) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isCreatedHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->createdHistogramLayerWrite((*eventData)->getEventNumber(), layer);
	
		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the actual encoded histogram layer as the		*
 * layer 'layer' into the file.									*
 ****************************************************************/

void analysis::encodedHistogramLayerWrite(unsigned short layer) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isEncodedHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->encodedHistogramLayerWrite((*eventData)->getEventNumber(), layer);
	
		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the actual filtered histogram layer as the		*
 * layer 'layer' into the file.									*
 ****************************************************************/

void analysis::filteredHistogramLayerWrite(unsigned short layer) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isFilteredHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->filteredHistogramLayerWrite((*eventData)->getEventNumber(), layer);
	
		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method shows the histogram after the filtering			*
 ****************************************************************/

void analysis::createdHistogramLayerShow(unsigned short layer, std::string drawOption) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isCreatedHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->createdHistogramLayerShow((*eventData)->getEventNumber(), layer, drawOption);
	
		}
		else {

			cannotShowHistogramWithoutWritingToRootWarningMsg* cannotShowHistogramWithoutWritingToRoot = new cannotShowHistogramWithoutWritingToRootWarningMsg();
			cannotShowHistogramWithoutWritingToRoot->warningMsg();
			if(cannotShowHistogramWithoutWritingToRoot != NULL) {
				delete cannotShowHistogramWithoutWritingToRoot;
				cannotShowHistogramWithoutWritingToRoot = NULL;
			}

		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
void analysis::createdHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption) {

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (eventNumber == (*eventData)->getEventNumber())
		createdHistogramLayerShow(layer, drawOption);

}

/****************************************************************
 * This method shows the histogram after the creation			*
 ****************************************************************/

void analysis::encodedHistogramLayerShow(unsigned short layer, std::string drawOption) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isEncodedHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->encodedHistogramLayerShow((*eventData)->getEventNumber(), layer, drawOption);
	
		}
		else {

			cannotShowHistogramWithoutWritingToRootWarningMsg* cannotShowHistogramWithoutWritingToRoot = new cannotShowHistogramWithoutWritingToRootWarningMsg();
			cannotShowHistogramWithoutWritingToRoot->warningMsg();
			if(cannotShowHistogramWithoutWritingToRoot != NULL) {
				delete cannotShowHistogramWithoutWritingToRoot;
				cannotShowHistogramWithoutWritingToRoot = NULL;
			}

		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
void analysis::encodedHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption) {

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (eventNumber == (*eventData)->getEventNumber())
		encodedHistogramLayerShow(layer, drawOption);

}

/****************************************************************
 * This method shows the histogram after the encoding			*
 ****************************************************************/

void analysis::filteredHistogramLayerShow(unsigned short layer, std::string drawOption) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isFilteredHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->filteredHistogramLayerShow((*eventData)->getEventNumber(), layer, drawOption);
	
		}
		else {

			cannotShowHistogramWithoutWritingToRootWarningMsg* cannotShowHistogramWithoutWritingToRoot = new cannotShowHistogramWithoutWritingToRootWarningMsg();
			cannotShowHistogramWithoutWritingToRoot->warningMsg();
			if(cannotShowHistogramWithoutWritingToRoot != NULL) {
				delete cannotShowHistogramWithoutWritingToRoot;
				cannotShowHistogramWithoutWritingToRoot = NULL;
			}

		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
void analysis::filteredHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption) {

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (eventNumber == (*eventData)->getEventNumber())
		filteredHistogramLayerShow(layer, drawOption);

}

/****************************************************************
 * method returns the number of active detector stations		*
 ****************************************************************/

unsigned int analysis::getNumberOfActiveStations() {

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	return (*eventData)->getDetector().getNumberOfActiveStations();

}

/****************************************************************
 * method initializes the hardwareTracksPerColumn-Analysis.		*
 ****************************************************************/

void analysis::initHardwareTracksPerColumnAnalysis(bool enable) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (enable) {

		if (hardwareAnalyser == NULL)
			hardwareAnalyser = new hardwareAnalysis();

		hardwareAnalyser->initHardwareTracksPerColumnAnalysis((*space)->getStep(DIM1), enable);

	}
	else {

		if (hardwareAnalyser != NULL) {

			hardwareAnalyser->initHardwareTracksPerColumnAnalysis((*space)->getStep(DIM1), enable);

			if (!hardwareAnalyser->isNumberOfTracksInAllColumnsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllLayersAnalysisEnabled()) {
				delete hardwareAnalyser;
				hardwareAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the hardwareTracksPerLayer-Analysis.		*
 ****************************************************************/

void analysis::initHardwareTracksPerLayerAnalysis(bool enable) {

	if ((tracks == NULL) || (*tracks == NULL))
		throw cannotAccessTrackDataError(ANALYSISLIB);

	if (enable) {

		if (hardwareAnalyser == NULL)
			hardwareAnalyser = new hardwareAnalysis();

		hardwareAnalyser->initHardwareTracksPerLayerAnalysis((*tracks)->getNumberOfLayers(), enable);

	}
	else {

		if (hardwareAnalyser != NULL) {

			hardwareAnalyser->initHardwareTracksPerLayerAnalysis((*tracks)->getNumberOfLayers(), enable);

			if (!hardwareAnalyser->isNumberOfTracksInAllColumnsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllLayersAnalysisEnabled()) {
				delete hardwareAnalyser;
				hardwareAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all columns of the histogram layers is				*
 * enabled or not												*
 ****************************************************************/

bool analysis::isNumberOfTracksInAllColumnsAnalysisEnabled() {

	bool returnValue;

	if (hardwareAnalyser != NULL)
		returnValue = hardwareAnalyser->isNumberOfTracksInAllColumnsAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all histogram layers is enabled or not				*
 ****************************************************************/

bool analysis::isNumberOfTracksInAllLayersAnalysisEnabled() {

	bool returnValue;

	if (hardwareAnalyser != NULL)
		returnValue = hardwareAnalyser->isNumberOfTracksInAllLayersAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method evaluates the minimal, the maximal and			*
 * the average number of tracks in all columns of the histogram.*
 ****************************************************************/

void analysis::evaluateNumberOfTracksInAllColumns() {

	if (hardwareAnalyser != NULL)
		hardwareAnalyser->evaluateNumberOfTracksInAllColumns(*tracks);
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method evaluates the distribution of the number of		*
 * tracks in all layers of the histogram.						*
 ****************************************************************/

void analysis::evaluateNumberOfTracksInAllLayers() {

	if (hardwareAnalyser != NULL)
		hardwareAnalyser->evaluateNumberOfTracksInAllLayers(*tracks);
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method shows the minimal, the maximal and				*
 * the average number of tracks in all columns of the histogram.*
 ****************************************************************/

void analysis::showNumberOfTracksInAllColumns() {

	if (hardwareAnalyser != NULL) {

		if (analysisResultWarnings & TRACKSPERCOLUMNDISTRIBUTION) {

			numberOfMinimalTracksInAllColumnsWarningMsg* numberOfMinimalTracksInAllColumns = new numberOfMinimalTracksInAllColumnsWarningMsg(hardwareAnalyser->getMinimalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());
			numberOfMinimalTracksInAllColumns->warningMsg();
			if(numberOfMinimalTracksInAllColumns != NULL) {
				delete numberOfMinimalTracksInAllColumns;
				numberOfMinimalTracksInAllColumns = NULL;
			}

			numberOfAverageTracksInAllColumnsWarningMsg* numberOfAverageTracksInAllColumns = new numberOfAverageTracksInAllColumnsWarningMsg(hardwareAnalyser->getAverageNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries(), hardwareAnalyser->getNumberOfColumnAnaylsis());
			numberOfAverageTracksInAllColumns->warningMsg();
			if(numberOfAverageTracksInAllColumns != NULL) {
				delete numberOfAverageTracksInAllColumns;
				numberOfAverageTracksInAllColumns = NULL;
			}

			numberOfMaximalTracksInAllColumnsWarningMsg* numberOfMaximalTracksInAllColumns = new numberOfMaximalTracksInAllColumnsWarningMsg(hardwareAnalyser->getMaximalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());
			numberOfMaximalTracksInAllColumns->warningMsg();
			if(numberOfMaximalTracksInAllColumns != NULL) {
				delete numberOfMaximalTracksInAllColumns;
				numberOfMaximalTracksInAllColumns = NULL;
			}

			sizeOfSeparatorFifosWarningMsg* sizeOfSeparatorFifos = new sizeOfSeparatorFifosWarningMsg(hardwareAnalyser->getMaximalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());
			sizeOfSeparatorFifos->warningMsg();
			if(sizeOfSeparatorFifos != NULL) {
				delete sizeOfSeparatorFifos;
				sizeOfSeparatorFifos = NULL;
			}

		}

		if (analysisResultDisplays & TRACKSPERCOLUMNDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addMinimalTrackColumnDistribution(hardwareAnalyser->getMinimalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());
			showAnalyser->addAverageTrackColumnDistribution(hardwareAnalyser->getAverageNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries(), hardwareAnalyser->getNumberOfColumnAnaylsis());
			showAnalyser->addMaximalTrackColumnDistribution(hardwareAnalyser->getMaximalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());
			showAnalyser->addFifoForColumnDistribution(hardwareAnalyser->getMaximalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());

		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method shows the distribution of the number of			*
 * tracks in all layers of the histogram.						*
 ****************************************************************/

void analysis::showNumberOfTracksInAllLayers() {

	if (hardwareAnalyser != NULL) {

		if (analysisResultWarnings & TRACKSPERLAYERDISTRIBUTION) {

			numberOfTracksInAllLayersWarningMsg* numberOfTracksInAllLayers = new numberOfTracksInAllLayersWarningMsg(hardwareAnalyser->getNumberOfTracksPerLayerDistribution(), hardwareAnalyser->getNumberOfTracksInAllLayersEntries(), hardwareAnalyser->getNumberOfLayerAnaylsis());
			numberOfTracksInAllLayers->warningMsg();
			if(numberOfTracksInAllLayers != NULL) {
				delete numberOfTracksInAllLayers;
				numberOfTracksInAllLayers = NULL;
			}

			numberOfTrackDensitiesInAllLayersWarningMsg* numberOfTrackDensitiesInAllLayers = new numberOfTrackDensitiesInAllLayersWarningMsg(hardwareAnalyser->getNumberOfTrackDensitiesPerLayerDistribution(), hardwareAnalyser->getNumberOfTracksInAllLayersEntries(), hardwareAnalyser->getNumberOfLayerAnaylsis());
			numberOfTrackDensitiesInAllLayers->warningMsg();
			if(numberOfTrackDensitiesInAllLayers != NULL) {
				delete numberOfTrackDensitiesInAllLayers;
				numberOfTrackDensitiesInAllLayers = NULL;
			}

		}

		if (analysisResultDisplays & TRACKSPERLAYERDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addTrackLayerDistribution(hardwareAnalyser->getNumberOfTracksPerLayerDistribution(), hardwareAnalyser->getNumberOfTracksInAllLayersEntries(), hardwareAnalyser->getNumberOfLayerAnaylsis());
			showAnalyser->addTrackDensityLayerDistribution(hardwareAnalyser->getNumberOfTrackDensitiesPerLayerDistribution(), hardwareAnalyser->getNumberOfTracksInAllLayersEntries(), hardwareAnalyser->getNumberOfLayerAnaylsis());

		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method resets the totalAnalysis object with				*
 * its actual results.											*
 ****************************************************************/

void analysis::resetTotalAnalysis() {

	initializeTotalAnalysis();

	totalAnalyser->reset();

}

/****************************************************************
 * This method adds the actual number of Hough transform		*
 * corrections for the total multi event summary to the			*
 * totalAnalysis object.										*
 ****************************************************************/

void analysis::addNumberOfHoughTransformCorrections(unsigned long numberOfCorrections, unsigned long numberOfCoordCorrections) {

	initializeTotalAnalysis();

	totalAnalyser->addNumberOfCorrections(numberOfCorrections);
	totalAnalyser->addNumberOfCoordCorrections(numberOfCoordCorrections);

}

/****************************************************************
 * This method adds the actual number of tracks which cannot be	*
 * found.														*
 ****************************************************************/

void analysis::addNumberOfTracksWhichCannotBeFound(unsigned long numberOfTracksWhichCannotBeFound) {

	initializeTotalAnalysis();

	totalAnalyser->addNumberOfTracksWhichCannotBeFound(numberOfTracksWhichCannotBeFound);

}

/****************************************************************
 * This method adds the actual number of tracks which have an	*
 * acceptable signature belonging to prelut.					*
 ****************************************************************/

void analysis::addNumberOfTracksWithGoodPrelutSignature(unsigned long numberOfTracksWithGoodPrelutSignature) {

	initializeTotalAnalysis();

	totalAnalyser->addNumberOfTracksWithGoodPrelutSignature(numberOfTracksWithGoodPrelutSignature);

}

/****************************************************************
 * This method adds the actual number of tracks which have an	*
 * acceptable signature belonging to lut.						*
 ****************************************************************/

void analysis::addNumberOfTracksWithGoodLutSignature(unsigned long numberOfTracksWithGoodLutSignature) {

	initializeTotalAnalysis();

	totalAnalyser->addNumberOfTracksWithGoodLutSignature(numberOfTracksWithGoodLutSignature);

}

/****************************************************************
 * This method adds the actual signature to the					*
 * acceptable signatures belonging to prelut.					*
 ****************************************************************/

void analysis::addGoodPrelutSignature(bitArray goodPrelutSignature, bitArray classification) {

	initializeTotalAnalysis();

	totalAnalyser->addGoodPrelutSignature(goodPrelutSignature, classification);

}

/****************************************************************
 * This method adds the actual signature to the					*
 * acceptable signatures belonging to lut.						*
 ****************************************************************/

void analysis::addGoodLutSignature(bitArray goodLutSignature, bitArray classification) {

	initializeTotalAnalysis();

	totalAnalyser->addGoodLutSignature(goodLutSignature, classification);

}

/****************************************************************
 * This method prints the actual results of the totalAnalysis.	*
 ****************************************************************/

void analysis::printTotalAnalysis() {

	if (totalAnalyser != NULL) {

		if (analysisResultWarnings & LUTCORRECTIONCOUNTER) {

			totalAnalyser->printCorrectionCounter();

		}

		if (analysisResultWarnings & NUMBEROFTRACKSWHICHCANNOTBEFOUND) {

			totalAnalyser->printNumberOfTracksWhichCannotBeFound();

		}

		if (analysisResultWarnings & GOODSIGNATURES) {

			totalAnalyser->printNumberOfTracksWithGoodPrelutSignature();
			totalAnalyser->printNumberOfTracksWithGoodLutSignature();
			totalAnalyser->printNumberOfGoodPrelutSignatures();
			totalAnalyser->printNumberOfGoodLutSignatures();

		}

		if (analysisResultWarnings & GOODSIGNATURETABLES) {

			totalAnalyser->printGoodPrelutSignatures();
			totalAnalyser->printGoodLutSignatures();

		}

		if (analysisResultWarnings & USEDSIGNATURETABLES) {

			usedTablesWarningMsg* usedTables = new usedTablesWarningMsg((*ratings)->getInfo());
			usedTables->warningMsg();
			if(usedTables != NULL) {
				delete usedTables;
				usedTables = NULL;
			}

		}

	}

}

/****************************************************************
 * This method displays all arrays of the totalAnalysis object.	*
 ****************************************************************/

void analysis::displayTotalAnalysis() {

	initializeTotalAnalysis();
	initializeShowAnalysis();

	if (analysisResultDisplays & LUTCORRECTIONCOUNTER) {

		showAnalyser->addDistribution((double*)correctionCounterSteps, (double*)correctionCounterValues, numberOfCorrectionCounters, CORRECTIONCOUNTERNAME, CORRECTIONCOUNTERTITLE, CORRECTIONCOUNTERXAXITITLE, CORRECTIONCOUNTERYAXITITLE);

	}

	if (analysisResultDisplays & NUMBEROFTRACKSWHICHCANNOTBEFOUND) {

		showAnalyser->addDistribution((double*)tracksWhichAreNotFindableSteps, (double*)tracksWhichAreNotFindableValues, numberOfTracksWhichArenotFindable, NOTFINDABLETRACKSNAME, NOTFINDABLETRACKSTITLE, NOTFINDABLETRACKSXAXITITLE, NOTFINDABLETRACKSYAXITITLE);
	
	}

	if (analysisResultDisplays & GOODSIGNATURES) {

		showAnalyser->addDistribution((double*)tracksWithAGoodPrelutSignatureSteps, (double*)tracksWithAGoodPrelutSignatureValues, numberOfTracksWithAGoodPrelutSignature, TRACKSWITHGOODPRELUTSIGNATURENAME, TRACKSWITHGOODPRELUTSIGNATURETITLE, TRACKSWITHGOODPRELUTSIGNATUREXAXITITLE, TRACKSWITHGOODPRELUTSIGNATUREYAXITITLE);
		showAnalyser->addDistribution((double*)tracksWithAGoodLutSignatureSteps, (double*)tracksWithAGoodLutSignatureValues, numberOfTracksWithAGoodLutSignature, TRACKSWITHGOODLUTSIGNATURENAME, TRACKSWITHGOODLUTSIGNATURETITLE, TRACKSWITHGOODLUTSIGNATUREXAXITITLE, TRACKSWITHGOODLUTSIGNATUREYAXITITLE);
		showAnalyser->addDistribution((double*)goodPrelutSignaturesSteps, (double*)goodPrelutSignaturesValues, numberOfGoodPrelutSignatures, GOODPRELUTSIGNATURESNAME, GOODPRELUTSIGNATURESTITLE, GOODPRELUTSIGNATURESXAXITITLE, GOODPRELUTSIGNATURESYAXITITLE);
		showAnalyser->addDistribution((double*)goodLutSignaturesSteps, (double*)goodLutSignaturesValues, numberOfGoodLutSignatures, GOODLUTSIGNATURESNAME, GOODLUTSIGNATURESTITLE, GOODLUTSIGNATURESXAXITITLE, GOODLUTSIGNATURESYAXITITLE);

	}

	if (analysisResultDisplays & GOODSIGNATURETABLES) {

		initializeShowAnalysis();

		showAnalyser->addTable(totalAnalyser->getGoodPrelutSignatureTable(), GOODPRELUTSIGNATURETABLENAME, GOODPRELUTSIGNATURETABLETITLE, GOODPRELUTSIGNATURETABLEXAXITITLE, GOODPRELUTSIGNATURETABLEYAXITITLE);
		showAnalyser->addTable(totalAnalyser->getGoodLutSignatureTable(), GOODLUTSIGNATURETABLENAME, GOODLUTSIGNATURETABLETITLE, GOODLUTSIGNATURETABLEXAXITITLE, GOODLUTSIGNATURETABLEYAXITITLE);

	}

	if (analysisResultDisplays & USEDSIGNATURETABLES) {

		initializeShowAnalysis();

		showAnalyser->addTable((*ratings)->getCodingTable(), CODINGTABLENAME, CODINGTABLETITLE, CODINGTABLEXAXITITLE, CODINGTABLEYAXITITLE);
		showAnalyser->addTable((*ratings)->getGradingPTable(), GRADINGPTABLENAME, GRADINGPTABLETITLE, GRADINGPTABLEXAXITITLE, GRADINGPTABLEYAXITITLE);
		showAnalyser->addTable((*ratings)->getGradingRTable(), GRADINGRTABLENAME, GRADINGRTABLETITLE, GRADINGRTABLEXAXITITLE, GRADINGRTABLEYAXITITLE);

	}

	if (analysisResultDisplays & HOUGHPICTURES) {

		if (pictures == NULL) {

			pictures = new houghPictures();

			pictures->draw();

		}

	}

}

/****************************************************************
 * This method returns the value for the percentage of			*
 * hits which must be in a signature which is accepted			*
 ****************************************************************/

unsigned short analysis::getPercentageOfHitsInSignature() {

	return percentageOfHitsInSignature;

}

/****************************************************************
 * This method returns the value for the percentage of real		*
 * tracks which must be found with the accepted signatures		*
 ****************************************************************/

unsigned short analysis::getPercentageOfTracksForSignature() {

	return percentageOfTracksForSignature;

}
