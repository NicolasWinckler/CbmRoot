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
// $Date: 2006/07/17 11:35:20 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/filterDimXDimX.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimXDimX::filterDimXDimX() : filterDimX() {

	filterSize1 = 0;
	filterSize2 = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimXDimX::filterDimXDimX( histogramData** histogram,
							    unsigned short  size1,
							    unsigned short  size2,
							    unsigned short  size,
								unsigned short  localSize) :
								filterDimX(
								histogram, size, localSize) {

	filterSize1 = size1;
	filterSize2 = size2;

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimXDimX::~filterDimXDimX() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDimXDimX::init( histogramData** histogram,
						   unsigned short  size1,
						   unsigned short  size2,
						   unsigned short  size,
						   unsigned short  localSize) {

	filterDimX::init(histogram, size, localSize);
	filterSize1 = size1;
	filterSize2 = size2;

}
