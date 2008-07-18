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
//     - class for handling the hough-transformation
//     - class computes the histogram
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-06-06 14:21:02 $
// $Revision: 1.9 $
//
// *******************************************************************/


//#define PRINTBORDERSTOFILE						/**< This definition enables the printing of all borders into a file */
#undef PRINTBORDERSTOFILE

//#define PRINTCREATEDHISTOGRAMLAYERSTOFILE			/**< This definition enables the printing of all histogram layers into a file */
#undef PRINTCREATEDHISTOGRAMLAYERSTOFILE

/**
 * If this define is set, a warning would occur if it is not
 * enough space to add the hit to the hit-list of the station
 * in the histogram. If this define is unset, no warning would
 * be shown.
 */

#define CHECKNUMBEROFHITENTRIESINHISTCELL


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/terminal.h"
#include "../include/houghTransformationError.h"
#include "../include/houghTransformationWarningMsg.h"
#include "../include/houghTransformation.h"

#ifdef DEBUGJUSTONEGOODTRACK

#include "../../DataObjectLIB/include/histogramSpace.h"
#include "../../DataObjectLIB/include/trackMomentum.h"
#include "../../DataObjectLIB/include/analyticFormula.h"
#include <iostream>

#endif

#ifdef PRINTCREATEDHISTOGRAMLAYERSTOFILE

#include "../../MiscLIB/include/conversionRoutines.h"

#endif


/****************************************************************
 * This method does the entries for one layer of the histogram	*
 ****************************************************************/

void houghTransformation::doHistogramLayerEntry(std::vector<lutBorder>::iterator border) {

	doHistogramLayerEntry(border->getPrelutHoughBorder(), border->getLutHoughBorder(), border->getHit());

}
void houghTransformation::doHistogramLayerEntry(lutBorder* border) {
	
	doHistogramLayerEntry(border->getPrelutHoughBorder(), border->getLutHoughBorder(), border->getHit());

}
void houghTransformation::doHistogramLayerEntry(prelutHoughBorder& firstBorder, lutHoughBorder& secondBorder, trackfinderInputHit* hit) {

	unsigned short                         stationIndex;
	houghBorderPosition                    actualHoughCoord;
	tooMuchHitEntriesInHistCellWarningMsg* tooMuchHitEntriesInHistCell;

	tooMuchHitEntriesInHistCell = NULL;

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	if (hit != NULL)
		if (hit->getStation() != NULL)
			stationIndex = hit->getStation()->getIndex();
		else
			throw cannotAccessHitsOrTracksError(HOUGHTRANSFORMATIONLIB);
	else
		throw cannotAccessHitsOrTracksError(HOUGHTRANSFORMATIONLIB);

	if (!hit->getStation()->isMasked()) {

		secondBorder.houghCoord.resetActiveObject();
		for (unsigned int k = 0; k < secondBorder.houghCoord.getNumberOfEntries(); k++) {

			actualHoughCoord = secondBorder.houghCoord.readActiveObjectAndMakeNextOneActive();

			/* Zuordnung von Detektorhit (1 im Bin‰rbild) zur quantisierten Funktion im Houghraum (h: p=x*cos+y*sin) */
			(*histogram)->setStationHit(actualHoughCoord.pos1, actualHoughCoord.pos2, stationIndex);

#ifndef NOANALYSIS

#ifdef CHECKNUMBEROFHITENTRIESINHISTCELL

			if (!(*histogram)->addStationHit(actualHoughCoord.pos1, actualHoughCoord.pos2, stationIndex, hit)) {
					
				tooMuchHitEntriesInHistCell = new tooMuchHitEntriesInHistCellWarningMsg(actualHoughCoord.pos1, actualHoughCoord.pos2, actualLayer, stationIndex);
				tooMuchHitEntriesInHistCell->warningMsg();
				if(tooMuchHitEntriesInHistCell != NULL) {
					delete tooMuchHitEntriesInHistCell;
					tooMuchHitEntriesInHistCell = NULL;
				}

			}

#else

			(*histogram)->addStationHit(actualHoughCoord.pos1, actualHoughCoord.pos2, stationIndex, hit);

#endif

#endif

		}

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

houghTransformation::houghTransformation() {

	eventData   = NULL;
	histogram   = NULL;
	lut         = NULL;
	dynamicLayerMemory.clear();
	actualLayer = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

houghTransformation::houghTransformation(trackfinderInputData** eventData, histogramData** histogram, lutImplementation** lut) {

	this->eventData = eventData;
	this->histogram = histogram;
	this->lut       = lut;

	dynamicLayerMemory.clear();
	actualLayer = 0;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

houghTransformation::~houghTransformation() {

	dynamicLayerMemory.clear();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void houghTransformation::init(trackfinderInputData** eventData, histogramData** histogram, lutImplementation** lut) {

	/* allocate new space */
	this->eventData = eventData;
	this->histogram = histogram;
	this->lut       = lut;

	dynamicLayerMemory.clear();
	actualLayer = 0;

}

/****************************************************************
 * This method creates the borders for each hit					*
 ****************************************************************/

void houghTransformation::createBorders(std::streambuf* terminal) {

	trackfinderInputHit* hit;
	lutBorder            border;
	terminalSequence     statusSequence;

#if (DEBUGJUSTONEGOODTRACK > 0)

	histogramSpace       space;
	trackMomentum        actualMomentum;
	analyticFormula      formula;
	trackCoordinates     actualPosition;
	int                  goodTrack[DEBUGJUSTONEGOODTRACK];
	int                  numberOfGoodTracks;
	bool                 goodTrackIsFound;

#endif

	if (eventData == NULL)
		throw cannotAccessEventDataError(HOUGHTRANSFORMATIONLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(HOUGHTRANSFORMATIONLIB);
	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (lut == NULL)
		throw cannotAccessLutError(HOUGHTRANSFORMATIONLIB);
	if (*lut == NULL)
		throw cannotAccessLutError(HOUGHTRANSFORMATIONLIB);

	(*lut)->resetCorrectionCounter();

	hit = NULL;
	createTerminalStatusSequence(&statusSequence, terminal, "\nCreate borders:\t\t\t", (*eventData)->getNumberOfHits());
	terminalInitialize(statusSequence);

#if (DEBUGJUSTONEGOODTRACK > 0)

	space.setMin((*lut)->getLutDefinition().dim1Min, DIM1);
	space.setMin((*lut)->getLutDefinition().dim2Min, DIM2);
	space.setMin((*lut)->getPrelutDefinition().dim3Min, DIM3);
	space.setMax((*lut)->getLutDefinition().dim1Max, DIM1);
	space.setMax((*lut)->getLutDefinition().dim2Max, DIM2);
	space.setMax((*lut)->getPrelutDefinition().dim3Max, DIM3);
	space.setStep((*lut)->getLutDefinition().dim1Step, DIM1);
	space.setStep((*lut)->getLutDefinition().dim2Step, DIM2);
	space.setStep((*lut)->getPrelutDefinition().dim3Step, DIM3);
	numberOfGoodTracks = 0;

#endif

	for(int i = 0; i < (*eventData)->getNumberOfHits(); i++) {

		hit = (*eventData)->getHitByIndex(i);
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(HOUGHTRANSFORMATIONLIB);

#if (DEBUGJUSTONEGOODTRACK > 0)

		if (hit->getTrack() == NULL)
			continue;

		if (numberOfGoodTracks < DEBUGJUSTONEGOODTRACK) {

			if (hit->getTrack()->getNumberOfHits() >= (*eventData)->getNumberOfActiveStations()) {
			
				goodTrackIsFound = false;
				for (int search = 0; search < numberOfGoodTracks; search++) {
					if (goodTrack[search] == hit->getTrack()->getTrackIndex())
						goodTrackIsFound = true;
				}

				if (!goodTrackIsFound) {
					goodTrack[numberOfGoodTracks] = hit->getTrack()->getTrackIndex();
					numberOfGoodTracks++;
				}

				if (numberOfGoodTracks == DEBUGJUSTONEGOODTRACK) {
				
					actualMomentum.set(hit->getTrack()->getMomX(), PX);
					actualMomentum.set(hit->getTrack()->getMomY(), PY);
					actualMomentum.set(hit->getTrack()->getMomZ(), PZ);

					formula.evaluateHough(actualMomentum, hit->getTrack()->getCharge(), space, &actualPosition);
				
				}

			}

		}

		if ((hit->getTrack()->getTrackIndex() != goodTrack[DEBUGJUSTONEGOODTRACK - 1]) || (numberOfGoodTracks != DEBUGJUSTONEGOODTRACK)) {
	
			terminalOverwrite(statusSequence, i + 1);
			continue;
		
		}

#endif

		(*lut)->evaluate(hit, &border);
		border.setHit(hit);

		(*histogram)->addBorder(border);

		terminalOverwrite(statusSequence, i + 1);

	}

	(*lut)->clear();
	terminalFinalize(statusSequence);

#ifdef DEBUGJUSTONEGOODTRACK

	if (numberOfGoodTracks != DEBUGJUSTONEGOODTRACK)
		throw noGoodTrackFoundError();
	else {
		std::cout << "Momentum of the track/peak (px, py , pz): (" << actualMomentum.get(PX) << ", " << actualMomentum.get(PY) << ", " << actualMomentum.get(PZ) << ")" << std::endl;
		std::cout << "Estimated position to find the peak (dim1, dim2 , dim3): (" << actualPosition.get(DIM1) << ", " << actualPosition.get(DIM2) << ", " << actualPosition.get(DIM3) << ")" << std::endl;
	}

#endif

#if (LUTVERSION == 2)

	if ((*lut)->getNumberOfCoordCorrections() > 0) {
		houghLutCorrectionWarningMsg* houghLutCorrection = new houghLutCorrectionWarningMsg("HOUGHTRANSFORMATIONLIB", (*lut)->getNumberOfCorrections(), (*lut)->getNumberOfCoordCorrections());
		houghLutCorrection->warningMsg();
		if(houghLutCorrection != NULL) {
			delete houghLutCorrection;
			houghLutCorrection = NULL;
		}
	}

#else

#if (LUTVERSION == 3)

	houghLutCorrectionWarningMsg* houghLutCorrection = new houghLutCorrectionWarningMsg("HOUGHTRANSFORMATIONLIB", (*lut)->getNumberOfCorrections(), (*lut)->getNumberOfCoordCorrections());
	houghLutCorrection->warningMsg();
	if(houghLutCorrection != NULL) {
		delete houghLutCorrection;
		houghLutCorrection = NULL;
	}

#endif

#endif

#ifdef PRINTBORDERSTOFILE

	(*histogram)->printBorder(0, (*histogram)->getValueDim3(), "borders.txt");

#endif

}

/****************************************************************
 * This method creates one layer of the histogram				*
 ****************************************************************/

void houghTransformation::createHistogramLayer(unsigned short layer) {

	lutBorder*  transferBorder;

#ifdef PRINTCREATEDHISTOGRAMLAYERSTOFILE

	std::string file;
	std::string id;

#endif

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (layer >= (*histogram)->getValueDim3())
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	/* erase the content of the dynamic memory because the layer is the first one */
	/* erase the content of the dynamic memory and fill it with the correct content */
	if ((layer == 0) || (layer != actualLayer + 1)) {

		dynamicLayerMemory.clear();

		for (unsigned short i = 0; i < layer; i++) {

			for (std::vector<lutBorder>::iterator j = (*histogram)->getBorderBeginIterator(i); j != (*histogram)->getBorderEndIterator(i); j++) {

				if (j->getPrelutHoughBorder().stop >= layer) {
					transferBorder = &(*j);
					dynamicLayerMemory.push_back(transferBorder);
				}

			}

		}

	}

	/* set the layer */
	actualLayer = layer;

	/* do the dynamic memory entries */
	for (std::list<lutBorder*>::iterator k = dynamicLayerMemory.begin(); k != dynamicLayerMemory.end(); ) {

		transferBorder = *k;
		doHistogramLayerEntry(transferBorder);

		if (transferBorder->getPrelutHoughBorder().stop <= actualLayer)
			k = dynamicLayerMemory.erase(k);
		else
			k++;

	}

	/* do the static memory entries */
	for (std::vector<lutBorder>::iterator l = (*histogram)->getBorderBeginIterator(actualLayer); l != (*histogram)->getBorderEndIterator(actualLayer); l++) {

		doHistogramLayerEntry(l);

		/* do the entry into the dynamic memory if the border is also valid for the next layer */
		if (l->getPrelutHoughBorder().stop > actualLayer) {
		
			transferBorder = &(*l);
			dynamicLayerMemory.push_back(transferBorder);
		
		}

	}

#ifdef PRINTCREATEDHISTOGRAMLAYERSTOFILE

	ustos(layer, &id, 10, 0);
	file = "createdHistogramLayer" + id + ".txt";
	(*histogram)->printLayer(0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)file.c_str());

#endif

}

/****************************************************************
 * This method resets the layer of the histogram object.		*
 ****************************************************************/

void houghTransformation::resetHistogramLayer() {

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	(*histogram)->resetLayer();

}

/****************************************************************
 * This method resets the histogram object.						*
 ****************************************************************/

void houghTransformation::resetHistogram() {

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	(*histogram)->reset();

}

/****************************************************************
 * This method returns the number of the histogram layers for	*
 * full processing.												*
 ****************************************************************/

unsigned short houghTransformation::getNumberOfHistogramLayers() {

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	return (*histogram)->getValueDim3();

}
