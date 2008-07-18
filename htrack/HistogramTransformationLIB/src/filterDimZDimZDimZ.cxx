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
// $Date: 2006/07/17 11:35:21 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/filterDimZDimZDimZ.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimZDimZDimZ::filterDimZDimZDimZ() : filterDimZDimZ() {

	filterSize3 = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimZDimZDimZ::filterDimZDimZDimZ( trackData**    tracks,
									    unsigned short size1,
									    unsigned short size2,
									    unsigned short size3,
									    unsigned short size,
										unsigned short localSize) :
										filterDimZDimZ(
										tracks, size1, size2,
										size, localSize) {

	filterSize3 = size3;

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimZDimZDimZ::~filterDimZDimZDimZ() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDimZDimZDimZ::init( trackData**    tracks,
							   unsigned short size1,
							   unsigned short size2,
							   unsigned short size3,
							   unsigned short size,
							   unsigned short localSize) {

	filterDimZDimZ::init(tracks, size1, size2, size, localSize);
	filterSize3 = size3;

}
