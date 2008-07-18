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
//     - abstract class for all filter methods for
//		 maxMorphSearch
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:35:19 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/filterDimX.h"
#include <stdio.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimX::filterDimX() {

	histogram       = NULL;
	filterMem       = NULL;	/* allocation in derived class */
	baseFilter      = NULL;	/* allocation in derived class */
	filterSize      = 0;
	filterLocalSize = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimX::filterDimX( histogramData** histogram,
						unsigned short  size,
						unsigned short  localSize) {

	init(histogram, size, localSize);
	filterMem       = NULL;
	baseFilter      = NULL;

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimX::~filterDimX() {

	if(filterMem != NULL) {
	
		delete [] filterMem;
		filterMem = NULL;
	}
	if(baseFilter != NULL) {
	
		delete baseFilter;
		baseFilter = NULL;
	}

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDimX::init( histogramData** histogram,
					   unsigned short  size,
					   unsigned short  localSize) {

	this->histogram = histogram;
	filterSize      = size;
	filterLocalSize = localSize;

}
