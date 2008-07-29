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
// $Date: 2008-02-29 11:42:58 $
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
#include "../include/firstFilterFinalMod.h"
#include <malloc.h>
#include <stdio.h>


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 


/****************************************************************
 * Default constructor											*
 ****************************************************************/

firstFilterFinalMod::firstFilterFinalMod() : filterDimXDimX() {

	filterResultSize = 0;
	filterResultMem  = NULL;

}

/****************************************************************
 * Constructor													*
 * Errors:														*
 * - memoryAllocationError										*
 ****************************************************************/

firstFilterFinalMod::firstFilterFinalMod( histogramData** histogram,
										  unsigned short  size1,
										  unsigned short  size2,
										  unsigned short  localSize1,
										  unsigned short  localSize2,
										  bitArray maximumClass) : filterDimXDimX() {

	unsigned short size;
	unsigned short localSize;

	size      = (size1 + 1) * (size2 / 2) + 1;
	localSize = 2 * ( min( localSize1, localSize2) * min( localSize1, localSize2) + min( localSize1, localSize2)) + 1;
	if (localSize > size)
		localSize = size;

	filterDimXDimX::init(histogram, size1, size2, size, localSize);
	filterResultSize = (*histogram)->getValueDim1() * ((size2 + 1) / 2);

#if (FIRSTFILTERHANDLINGTYPE == 0)
	baseFilter      = new filterBasicNeutral();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 1)
	baseFilter      = new filterBasicSimple();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 2)
	baseFilter      = new filterBasicSimpleMod();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 3)
	baseFilter      = new filterBasicComplex();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 4)
	baseFilter      = new filterBasicComplexMod();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 5)
	baseFilter      = new filterBasicSpecial(maximumClass);
#endif

	filterMem       = new bitArray[filterSize];
	filterResultMem = new bitArray[filterResultSize];

	if (filterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);
	if (filterResultMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

firstFilterFinalMod::~firstFilterFinalMod() {

	if(filterResultMem != NULL) {
	
		delete[] filterResultMem;
		filterResultMem = NULL;
	}

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void firstFilterFinalMod::init( histogramData** histogram,
							    unsigned short  size1,
							    unsigned short  size2,
							    unsigned short  localSize1,
							    unsigned short  localSize2,
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
	if (filterResultMem != NULL) {
		delete[] filterResultMem;
		filterResultMem = NULL;
	}

	/* set new parameter */
	size      = (size1 + 1) * (size2 / 2) + 1;
	localSize = 2 * ( min( localSize1, localSize2) * min( localSize1, localSize2) + min( localSize1, localSize2)) + 1;
	if (localSize > size)
		localSize = size;

	filterDimXDimX::init(histogram, size1, size2, size, localSize);
	filterResultSize = (*histogram)->getValueDim1() * ((size2 + 1) / 2);

	/* allocate new space */
#if (FIRSTFILTERHANDLINGTYPE == 0)
	baseFilter      = new filterBasicNeutral();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 1)
	baseFilter      = new filterBasicSimple();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 2)
	baseFilter      = new filterBasicSimpleMod();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 3)
	baseFilter      = new filterBasicComplex();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 4)
	baseFilter      = new filterBasicComplexMod();
#endif
#if (FIRSTFILTERHANDLINGTYPE == 5)
	baseFilter      = new filterBasicSpecial(maximumClass);
#endif

	filterMem       = new bitArray[filterSize];
	filterResultMem = new bitArray[filterResultSize];

	if (filterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);
	if (filterResultMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

}

/****************************************************************
 * This method implements the filter in all axe-dimensions.		*
 ****************************************************************/

void firstFilterFinalMod::filter() {

	unsigned short valueDim1;
	unsigned short valueDim2;
	unsigned short i;
	unsigned short j;
	int            k;
	int            l;
	unsigned short filterCounter;
	unsigned short filterMiddleIndex;
	unsigned short resultEntryIndex;
	bool           startCopyingBack;

	if (filterResultMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (filterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();

	if (baseFilter == NULL)
		throw cannotAccessFilterError();

	valueDim1 = (*histogram)->getValueDim1();
	valueDim2 = (*histogram)->getValueDim2();
	resultEntryIndex = 0;
	startCopyingBack = false;

	for (i = 0; i < valueDim2; i++) {
		for (j = 0; j < valueDim1; j++) {
			/* copy result back */
			if (startCopyingBack) {
				(*histogram)->getCell(j, i - filterSize2 / 2 - 1)->value = filterResultMem[resultEntryIndex];
			}
			/* initialize filterMemory */
			filterCounter = 0;
			for (k = filterSize2 / 2; k > 0; k--) {
				for (l = filterSize1 / 2; l >= 0; l--) {
					if (j - l >= 0) {
						if (i - k >= 0)
							filterMem[filterCounter] =  (*histogram)->getCell(j - l, i - k)->value;
						else
							filterMem[filterCounter] = bitArray(0);
						filterCounter++;
					}
				}
			}
			filterMem[filterCounter] = (*histogram)->getCell(j, i)->value;
			filterMiddleIndex        = filterCounter;
			filterCounter++;
			for (k = 1; k <= filterSize2 / 2; k++) {
				for (l = 0; l <= filterSize1 / 2; l++) {
					if (j + l < valueDim1) {
						if (i + k < valueDim2)
							filterMem[filterCounter] =  (*histogram)->getCell(j + l, i + k)->value;
						else
							filterMem[filterCounter] = bitArray(0);
						filterCounter++;
					}
				}
			}
			/* do filtering */
			filterResultMem[resultEntryIndex] = baseFilter->filter(filterMem, filterCounter, filterLocalSize, filterMiddleIndex);
			if (resultEntryIndex < filterResultSize - 1)
				resultEntryIndex++;
			else
				resultEntryIndex = 0;
			if (resultEntryIndex == 0)
				startCopyingBack = true;
		}
	}
	for (i = valueDim2 - filterSize2 / 2 - 1; i < valueDim2; i++) {
		for (j = 0; j < valueDim1; j++) {
			(*histogram)->getCell(j, i)->value = filterResultMem[resultEntryIndex];
			if (resultEntryIndex < filterResultSize - 1)
				resultEntryIndex++;
			else
				resultEntryIndex = 0;
		}
	}

}
