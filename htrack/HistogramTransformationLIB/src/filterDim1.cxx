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
// $Date: 2008-02-29 11:42:57 $
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
#include "../include/filterDim1.h"
#include <malloc.h>
#include <stdio.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDim1::filterDim1() : filterDimX() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDim1::filterDim1( histogramData** histogram,
						unsigned short  size,
						unsigned short  localSize,
						bitArray maximumClass) :
						filterDimX(
						histogram, size, localSize) {

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

	if (filterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterDim1::~filterDim1() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDim1::init( histogramData** histogram,
					   unsigned short  size,
					   unsigned short  localSize,
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
	filterDimX::init(histogram, size, localSize);

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
 * This method implements the filter in the Dim1-axe.			*
 ****************************************************************/

void filterDim1::filter() {
	
	unsigned short valueDim1;
	unsigned short valueDim2;
	unsigned short k;

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

	for (unsigned short i = 0; i < valueDim2; i++) {
		for (k = 0; k < filterSize; k++) {
			filterMem[k] = bitArray(0);
		}
		for (unsigned short j = 0; j < valueDim1 + filterSize / 2; j++) {
			for (k = 1; k < filterSize; k++) {
				filterMem[k-1] = filterMem[k];
			}
			if (j < valueDim1) {
				filterMem[filterSize - 1] = (*histogram)->getCell(j, i)->value;
			}
			else {
				filterMem[filterSize - 1] = bitArray(0);
			}
			/* end mode */
			if (j >= valueDim1) {
				(*histogram)->getCell(j - filterSize / 2, i)->value = baseFilter->filter(filterMem, filterSize - (j - valueDim1) - 1, filterLocalSize, filterSize / 2);
			}
			/* normal mode */
			else if (j >= filterSize - 1) {
				(*histogram)->getCell(j - filterSize / 2, i)->value = baseFilter->filter(filterMem, filterSize, filterLocalSize, filterSize / 2);
			}
			/* start mode */
			else if (j >= filterSize / 2) {
				(*histogram)->getCell(j - filterSize / 2, i)->value = baseFilter->filter(&filterMem[filterSize - 1 - j], j + 1, filterLocalSize, j - filterSize / 2);
			}
		}
	}

}
