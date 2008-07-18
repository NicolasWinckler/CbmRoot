/* *******************************************************************
// (C)opyright 2005
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
// $Date: 2006/11/17 15:12:40 $
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
#include "../include/secondFilterFinal.h"
#include <malloc.h>
#include <stdio.h>

using std::min;

/****************************************************************
 * Default constructor											*
 ****************************************************************/

secondFilterFinal::secondFilterFinal() : filterDimZDimZDimZ() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

secondFilterFinal::secondFilterFinal( trackData**    tracks,
									  unsigned short size1,
									  unsigned short size2,
									  unsigned short size3,
									  unsigned short localSize1,
									  unsigned short localSize2,
									  unsigned short localSize3,
									  bitArray maximumClass) : filterDimZDimZDimZ() {

	unsigned short size;
	unsigned short localSize;

	size      = (size3 - 1) * ((size1 / 2) * (size2 + 1) + 1) + 1;
	localSize = (2 * ( min( localSize1, localSize2) * min( localSize1, localSize2) + min( localSize1, localSize2)) + 1) * (localSize3 - 1);
	if (localSize > size)
		localSize = size;

	filterDimZDimZDimZ::init(tracks, size1, size2, size3, size, localSize);

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

secondFilterFinal::~secondFilterFinal() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void secondFilterFinal::init( trackData**    tracks,
							  unsigned short   size1,
							  unsigned short   size2,
							  unsigned short   size3,
							  unsigned short   localSize1,
							  unsigned short   localSize2,
							  unsigned short   localSize3,
							  bitArray maximumClass) {
					  
	unsigned short size;
	unsigned short localSize;

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
	size      = (size3 - 1) * ((size1 / 2) * (size2 + 1) + 1) + 1;
	localSize = (2 * ( min( localSize1, localSize2) * min( localSize1, localSize2) + min( localSize1, localSize2)) + 1) * (localSize3 - 1);
	if (localSize > size)
		localSize = size;

	filterDimZDimZDimZ::init(tracks, size1, size2, size3, size, localSize);

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

void secondFilterFinal::filter(std::streambuf* terminal) {

	unsigned short                 i;
	unsigned short                 j;
	unsigned short                 filterCounter;
	unsigned short                 filterMiddleIndex;
	terminalSequence               statusSequence;
	trackLayerAccess*              trackAccess;
	int                            elementCoordinate1;
	int                            elementCoordinate2;
	int                            filterCoordinate1;
	int                            filterCoordinate2;
	bool                           dim1Reset;
	bool                           dim2Reset;
	bool                           dim3Reset;
	bitArray                       filterResult;
	bool                           firstValueIsRemoved;

	if (filterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (baseFilter == NULL)
		throw cannotAccessFilterError();

	trackAccess = new trackLayerAccess[filterSize3];

	if (trackAccess == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Process histogram:\t\t", (*tracks)->getNumberOfLayers());
	terminalInitialize(statusSequence);

	for (i = 0; i < (*tracks)->getNumberOfLayers(); i++) {

		for (j = 0; j < filterSize3; j++) {

			if (((int)i - (int)(filterSize3/2) + (int)j >= (int)0) && ((int)i - (int)(filterSize3/2) + (int)j < (int)(*tracks)->getNumberOfLayers())) {
				trackAccess[j].begin   = (*tracks)->getTrackIteratorBeginOfHistogramLayer(i - (filterSize3/2) + j);
				trackAccess[j].end     = (*tracks)->getTrackIteratorEndOfHistogramLayer(i - (filterSize3/2) + j);
				trackAccess[j].isValid = true;
			}
			else
				trackAccess[j].isValid = false;

		}

		for (trackLayer::iterator element = trackAccess[filterSize3/2].begin; element != trackAccess[filterSize3/2].end; ) {
			
			filterCounter     = 0;
			filterMiddleIndex = 0;

			for (j = 0; j < filterSize3; j++) {

				if (!trackAccess[j].isValid)
					continue;

				if (j == filterSize3/2) {
					filterMem[filterCounter] = element->value;
					filterMiddleIndex        = filterCounter;
					filterCounter++;
					continue;
				}

				elementCoordinate1   = element->position.get(DIM1);
				elementCoordinate2   = element->position.get(DIM2);

				for (trackLayer::iterator filter = trackAccess[j].begin; filter != trackAccess[j].end; filter++) {

					filterCoordinate1    = filter->position.get(DIM1);
					filterCoordinate2    = filter->position.get(DIM2);
					
					if ((filterCoordinate1 < elementCoordinate1) && (filterCoordinate1 >= elementCoordinate1 - filterSize1/2) && (filterCoordinate2 <= elementCoordinate2) && (filterCoordinate2 >= elementCoordinate2 - filterSize2/2))
						dim1Reset = true;
					else
						dim1Reset = false;

					if ((filterCoordinate1 == elementCoordinate1) && (filterCoordinate2 == elementCoordinate2))
						dim2Reset = true;
					else
						dim2Reset = false;

					if ((filterCoordinate1 > elementCoordinate1) && (filterCoordinate1 <= elementCoordinate1 + filterSize1/2) && (filterCoordinate2 >= elementCoordinate2) && (filterCoordinate2 <= elementCoordinate2 + filterSize2/2))
						dim3Reset = true;
					else
						dim3Reset = false;
				
					if (dim1Reset || dim2Reset || dim3Reset) {
						filterMem[filterCounter] = filter->value;
						filterCounter++;
					}

				}

			}

			filterResult = baseFilter->filter(filterMem, filterCounter, filterLocalSize, filterMiddleIndex);
			
			if (filterResult != element->value) {

/* This removing of tracks was in the space of the next computations.	*
 * But this filter should delete the values after all computations.	So	*
 * the position must be saved and the removing has to be done later.	*/

				if (element == trackAccess[filterSize3/2].begin)
					firstValueIsRemoved = true;
				else
					firstValueIsRemoved = false;
				element = (*tracks)->removeTrack(element, i);
				if (firstValueIsRemoved)
					trackAccess[filterSize3/2].begin = element;

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
