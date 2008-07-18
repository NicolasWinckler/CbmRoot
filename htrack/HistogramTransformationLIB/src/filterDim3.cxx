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
//     - class for evaluating the basic filter method for
//		 maxMorphSearch
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/11/17 15:12:39 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/terminal.h"
#include "../include/histogramTransformationError.h"
#include "../include/filterBasicNeutral.h"
#include "../include/filterBasicSimple.h"
#include "../include/filterBasicSimpleMod.h"
#include "../include/filterBasicComplex.h"
#include "../include/filterBasicComplexMod.h"
#include "../include/filterBasicSpecial.h"
#include "../include/filterDim3.h"
#include <malloc.h>
#include <stdio.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDim3::filterDim3() : filterDimZ() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDim3::filterDim3( trackData**    tracks,
						unsigned short size,
						unsigned short localSize,
						bitArray maximumClass) : filterDimZ(
						tracks, size, localSize) {

#if (SECONDFILTERHANDLINGTYPE == 0)
	baseFilter      = new filterBasicNeutral();
#endif
#if (SECONDFILTERHANDLINGTYPE == 1)
	baseFilter      = new filterBasicSimple();
#endif
#if (SECONDFILTERHANDLINGTYPE == 2)
	baseFilter      = new filterBasicSimpleMod();
#endif
#if (SECONDFILTERHANDLINGTYPE == 3)
	baseFilter      = new filterBasicComplex();
#endif
#if (SECONDFILTERHANDLINGTYPE == 4)
	baseFilter      = new filterBasicComplexMod();
#endif
#if (SECONDFILTERHANDLINGTYPE == 5)
	baseFilter      = new filterBasicSpecial(maximumClass);
#endif

	filterMem       = new bitArray[filterSize];

	if (filterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterDim3::~filterDim3() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDim3::init( trackData**    tracks,
					   unsigned short size,
					   unsigned short localSize,
					   bitArray maximumClass) {
					  
	/* free the old allocated space */
	if (baseFilter != NULL) {
		delete baseFilter;
		baseFilter = NULL;
	}
	if (filterMem != NULL) {
		delete[] filterMem;
		filterMem = NULL;
	}

	/* set new parameter */
	filterDimZ::init(tracks, size, localSize);

	/* allocate new space */
#if (SECONDFILTERHANDLINGTYPE == 0)
	baseFilter      = new filterBasicNeutral();
#endif
#if (SECONDFILTERHANDLINGTYPE == 1)
	baseFilter      = new filterBasicSimple();
#endif
#if (SECONDFILTERHANDLINGTYPE == 2)
	baseFilter      = new filterBasicSimpleMod();
#endif
#if (SECONDFILTERHANDLINGTYPE == 3)
	baseFilter      = new filterBasicComplex();
#endif
#if (SECONDFILTERHANDLINGTYPE == 4)
	baseFilter      = new filterBasicComplexMod();
#endif
#if (SECONDFILTERHANDLINGTYPE == 5)
	baseFilter      = new filterBasicSpecial(maximumClass);
#endif

	filterMem       = new bitArray[filterSize];

	if (filterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

}

/****************************************************************
 * This method implements the filter in the Dim3-axe.			*
 ****************************************************************/

void filterDim3::filter(std::streambuf* terminal) {

	unsigned short                 i;
	unsigned short                 j;
	terminalSequence               statusSequence;
	trackLayerAccess*              trackAccess;
	int                            elementCoordinate;
	int                            filterCoordinate;
	bool                           dim1Reset;
	bool                           dim2Reset;
	bitArray                       filterResult;
	bool                           firstValueIsRemoved;

	if (filterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (baseFilter == NULL)
		throw cannotAccessFilterError();

	trackAccess = new trackLayerAccess[filterSize];

	if (trackAccess == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Process histogram:\t\t", (*tracks)->getNumberOfLayers());
	terminalInitialize(statusSequence);

	for (i = 0; i < (*tracks)->getNumberOfLayers(); i++) {

		for (j = 0; j < filterSize; j++) {

			if (((int)i - (int)(filterSize/2) + (int)j >= (int)0) && ((int)i - (int)(filterSize/2) + (int)j < (int)(*tracks)->getNumberOfLayers())) {
				trackAccess[j].begin   = (*tracks)->getTrackIteratorBeginOfHistogramLayer(i - (filterSize/2) + j);
				trackAccess[j].end     = (*tracks)->getTrackIteratorEndOfHistogramLayer(i - (filterSize/2) + j);
				trackAccess[j].isValid = true;
			}
			else
				trackAccess[j].isValid = false;

		}

		for (trackLayer::iterator element = trackAccess[filterSize/2].begin; element != trackAccess[filterSize/2].end; ) {

			for (j = 0; j < filterSize; j++) {

				filterMem[j] = bitArray(0);

				if (!trackAccess[j].isValid)
					continue;

				if (j == filterSize/2) {
					filterMem[j] = element->value;
					continue;
				}

				for (trackLayer::iterator filter = trackAccess[j].begin; filter != trackAccess[j].end; filter++) {

					elementCoordinate    = element->position.get(DIM1);
					filterCoordinate     = filter->position.get(DIM1);
					if (filterCoordinate == elementCoordinate)
						dim1Reset = true;
					else
						dim1Reset = false;
				
					if (!dim1Reset)
						continue;

					elementCoordinate    = element->position.get(DIM2);
					filterCoordinate     = filter->position.get(DIM2);
					if (filterCoordinate == elementCoordinate)
						dim2Reset = true;
					else
						dim2Reset = false;

					if (dim1Reset && dim2Reset) {
						filterMem[j] = filter->value;
						break;
					}

				}

			}

			filterResult = baseFilter->filter(filterMem, filterSize, filterLocalSize, filterSize/2);
			
			if (filterResult != element->value) {

/* This removing of tracks was in the space of the next computations.	*
 * But this filter should delete the values after all computations.	So	*
 * the position must be saved and the removing has to be done later.	*/

				if (element == trackAccess[filterSize/2].begin)
					firstValueIsRemoved = true;
				else
					firstValueIsRemoved = false;
				element = (*tracks)->removeTrack(element, i);
				if (firstValueIsRemoved)
					trackAccess[filterSize/2].begin = element;

			}
			else
				element++;

		}

		terminalOverwrite(statusSequence, i + 1);

	}

	terminalFinalize(statusSequence);

	if (trackAccess != NULL) {
		delete[] trackAccess;
		trackAccess = NULL;
	}

}
