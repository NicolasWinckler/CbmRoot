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
//     - abstract class for all filter methods for
//		 maxMorphSearch
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:35:20 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/filterDimZ.h"
#include <stdio.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimZ::filterDimZ() {

	tracks          = NULL;
	filterMem       = NULL;	/* allocation in derived class */
	baseFilter      = NULL;	/* allocation in derived class */
	filterSize      = 0;
	filterLocalSize = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimZ::filterDimZ( trackData**    tracks,
						unsigned short size,
						unsigned short localSize) {

	init(tracks, size, localSize);
	filterMem       = NULL;
	baseFilter      = NULL;

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimZ::~filterDimZ() {

	if(filterMem != NULL) {
	
		delete[] filterMem;
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

void filterDimZ::init( trackData**    tracks,
					   unsigned short size,
					   unsigned short localSize) {

	this->tracks    = tracks;
	filterSize      = size;
	filterLocalSize = localSize;

}
