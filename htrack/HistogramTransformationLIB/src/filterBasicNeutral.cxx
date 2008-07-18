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
// $Date: 2006/07/17 11:35:15 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/histogramTransformationError.h"
#include "../include/filterBasicNeutral.h"


/****************************************************************
 * Constructor													*
 ****************************************************************/

filterBasicNeutral::filterBasicNeutral() : filterBasicStyle() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterBasicNeutral::~filterBasicNeutral() {

}

/****************************************************************
 * This method implements the filter without respect to the		*
 * axis.														*
 ****************************************************************/

bitArray filterBasicNeutral::filter( bitArray* array, unsigned int length,
								  	      unsigned int small, unsigned int element) {

	if (array == NULL)
		throw cannotAccessElementsToFilterError();

	if (length <= element)
		throw middleElementIsOutOfRangeError();

	return array[element];

}
