/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�nner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl��
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


#include "../include/filterDimXDimXDimX.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimXDimXDimX::filterDimXDimXDimX() : filterDimXDimX() {

	filterSize3 = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimXDimXDimX::filterDimXDimXDimX( histogramData** histogram,
									    unsigned short  size1,
									    unsigned short  size2,
									    unsigned short  size3,
									    unsigned short  size,
										unsigned short  localSize) :
										filterDimXDimX(
										histogram, size1, size2,
										size, localSize) {

	filterSize3 = size3;

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimXDimXDimX::~filterDimXDimXDimX() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDimXDimXDimX::init( histogramData** histogram,
							   unsigned short  size1,
							   unsigned short  size2,
							   unsigned short  size3,
							   unsigned short  size,
							   unsigned short  localSize) {

	filterDimXDimX::init(histogram, size1, size2, size, localSize);
	filterSize3 = size3;

}
