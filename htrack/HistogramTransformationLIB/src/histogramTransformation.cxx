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
//     - class for evaluating the histogram
//     - class computes the tracks
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/12/14 12:17:31 $
// $Revision: 1.4 $
//
// *******************************************************************/



#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/bitArray.h"
#include "../include/histogramTransformationError.h"
#include "../include/histogramTransformation.h"
#include <stdio.h>
#ifdef DEBUGJUSTONEGOODTRACK
#include <iostream>
#endif


/****************************************************************
 * Default constructor											*
 ****************************************************************/

histogramTransformation::histogramTransformation() {

	init(NULL, NULL, NULL, 0, 0, 0);

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

histogramTransformation::histogramTransformation(histogramData** histogram, trackData** tracks, tables** ratings, unsigned short dim1ClearRadius, unsigned short dim2ClearRadius, unsigned short dim3ClearRadius) {

	init(histogram, tracks, ratings, dim1ClearRadius, dim2ClearRadius, dim3ClearRadius);

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

histogramTransformation::~histogramTransformation() {

}

/****************************************************************
 * This method initializes the object.							*
 * Errors:														*
 * - memoryAllocationError										*
 ****************************************************************/

void histogramTransformation::init(histogramData** histogram, trackData** tracks, tables** ratings, unsigned short dim1ClearRadius, unsigned short dim2ClearRadius, unsigned short dim3ClearRadius) {

	this->histogram = histogram;
	this->tracks    = tracks;
	this->ratings   = ratings;

	firstFilterNeighborhoodDim1ClearRadius  = dim1ClearRadius;
	firstFilterNeighborhoodDim2ClearRadius  = dim2ClearRadius;
	secondFilterNeighborhoodDim3ClearRadius = dim3ClearRadius;

}

/****************************************************************
 * This method resets the tracks object.						*
 ****************************************************************/

void histogramTransformation::resetTracks() {

	if (tracks == NULL)
		throw cannotAccessTracksError();
	if (*tracks == NULL)
		throw cannotAccessTracksError();

	(*tracks)->reset();

}

/****************************************************************
 * This method encodes the histogram value to a special class	*
 * value.														*
 ****************************************************************/

void histogramTransformation::encodeHistogramLayer(unsigned short layer) {

	histogramCell* actualCell;
	bitArray       zeroComparator;

#ifdef PRINTORIGINALHISTOGRAMLAYERSTOFILE

	std::string    origialFile;
	std::string    originalId;

#endif

#ifdef PRINTENCODEDHISTOGRAMLAYERSTOFILE

	std::string    encodedFile;
	std::string    encodedId;

#endif

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();
	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

#ifdef PRINTORIGINALHISTOGRAMLAYERSTOFILE

	ustos(layer, &originalId, 10, 0);
	origialFile = "originalHistogramLayer" + originalId + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)origialFile.c_str());

#endif

	zeroComparator = bitArray(0);

	for(unsigned short i = 0; i < (*histogram)->getValueDim2(); i++) {

		for(unsigned short j = 0; j < (*histogram)->getValueDim1(); j++) {

			actualCell = (*histogram)->getCell(j, i);

			if (actualCell->value != zeroComparator)
				actualCell->value = (*ratings)->getCodingTableClassification(actualCell->value);

#ifdef SKIPFIRSTHISTOGRAMCOLUMN

			if (j == 0)
				actualCell->value = bitArray(0);

#endif

		}

	}

#ifdef PRINTENCODEDHISTOGRAMLAYERSTOFILE

	ustos(layer, &encodedId, 10, 0);
	encodedFile = "encodedHistogramLayer" + encodedId + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)encodedFile.c_str());

#endif

}

/****************************************************************
 * This method handles the diagonles in the histogramPeaks		*
 * array to	perform a better quality with doubled peaks.		*
 ****************************************************************/

void histogramTransformation::diagonalizeHistogramLayer(unsigned short layer) {

	histogramCell* actualCell;
	bitArray       comparator;
	histogramCell* nextCell;

#ifdef PRINTDIAGONALIZEDHISTOGRAMLAYERSTOFILE

	std::string    file;
	std::string    id;

#endif

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();

	comparator = bitArray(0);

	for (unsigned short i = 0; i < (*histogram)->getValueDim2()-1; i++) {

		for (unsigned short j = 0; j < (*histogram)->getValueDim1()-1; j++) {

			actualCell = (*histogram)->getCell(j, i);
			nextCell   = (*histogram)->getCell(j + 1, i + 1);

			if ((actualCell->value > comparator) && (actualCell->value == nextCell->value))
				//actualCell->value += DIAGONALIZATIONSUMMAND;
				nextCell->value += DIAGONALIZATIONSUMMAND;

		}

	}

#ifdef PRINTDIAGONALIZEDHISTOGRAMLAYERSTOFILE

	ustos(layer, &id, 10, 0);
	file = "diagonalizedHistogramLayer" + id + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)file.c_str());

#endif

}

/****************************************************************
 * This method filters the peakclasses in a speciallized manner.*
 ****************************************************************/

void histogramTransformation::filterHistogramLayer(unsigned short layer) {

#ifdef PRINTFILTEREDHISTOGRAMLAYERSTOFILE

	std::string    file;
	std::string    id;

#endif

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();

	filteringHistogramLayer();

#ifdef PRINTFILTEREDHISTOGRAMLAYERSTOFILE

	ustos(layer, &id, 10, 0);
	file = "filteredHistogramLayer" + id + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)file.c_str());

#endif

}

/****************************************************************
 * This method converts the filtered histogramPeaks array to	*
 * the correspondance to be equal to the eraser-object result.	*
 ****************************************************************/

void histogramTransformation::finalizeHistogramLayer(unsigned short layer, std::streambuf* terminal, terminalSequence* statusSequence, unsigned short statusMax) {

	histogramCell* actualCell;
	bitArray       comparator;

#ifdef PRINTFINALIZEDHISTOGRAMLAYERSTOFILE

	std::string    file;
	std::string    id;

#endif

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();

	if (tracks == NULL)
		throw cannotAccessHistogramError();
	if (*tracks == NULL)
		throw cannotAccessHistogramError();

	comparator = (*ratings)->getCodingTableMinimumClassification();

	for (unsigned short i = 0; i < (*histogram)->getValueDim2(); i++) {

		for (unsigned short j = 0; j < (*histogram)->getValueDim1(); j++) {

			actualCell = (*histogram)->getCell(j, i);

			if (actualCell->value >= comparator) {

				(*tracks)->addTrack(j, i, layer, *actualCell);

#ifdef DEBUGJUSTONEGOODTRACK

				if ((terminal != NULL) && (statusSequence != NULL))
					terminalFinalize(*statusSequence);

				std::cout << "Position where the peak is found (dim1, dim2 , dim3): (" << j << ", " << i << ", " << layer << ")" << std::endl;

				if ((terminal != NULL) && (statusSequence != NULL)) {
					createTerminalStatusSequence(statusSequence, terminal, "Process layers:\t\t\t", statusMax);
					terminalInitialize(*statusSequence);
				}

#endif

			}

		}

	}

#ifdef PRINTFINALIZEDHISTOGRAMLAYERSTOFILE

	ustos(layer, &id, 10, 0);
	file = "finalizedHistogramLayer" + id + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)file.c_str());

#endif

}

/****************************************************************
 * This method filters the tracks in a speciallized manner		*
 * again.														*
 ****************************************************************/

void histogramTransformation::filterHistogram(std::streambuf* terminal) {

	if (tracks == NULL)
		throw cannotAccessHistogramError();
	if (*tracks == NULL)
		throw cannotAccessHistogramError();

#ifdef PRINTPREFILTEREDTRACKLAYERSTOFILE

	(*tracks)->printTracks(maxClass, 0, (*tracks)->getNumberOfLayers(), 0, (*histogram)->getValueDimSH(), false, "prefilteredTracks.txt");

#endif

	filteringHistogram(terminal);

#ifdef PRINTFILTEREDTRACKLAYERSTOFILE

	(*tracks)->printTracks(maxClass, 0, (*tracks)->getNumberOfLayers(), 0, (*histogram)->getValueDimSH(), false, "filteredTracks.txt");

#endif

}

