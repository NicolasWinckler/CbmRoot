/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gläß
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
//     - base class for holding the track information of the transformation
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/12/12 13:04:40 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/trackDigitalInformation.h"


/****************************************************************
 * CLASS trackDigitalInformation				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackDigitalInformation::trackDigitalInformation() {

	position = trackCoordinates(0, 0, 0);
	value    = bitArray(0);

#ifndef NOANALYSIS

	hits     = hitArray();

#endif

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackDigitalInformation::trackDigitalInformation(const trackDigitalInformation& value) {

	this->position = value.position;
	this->value    = value.value;

#ifndef NOANALYSIS

	this->hits     = value.hits;

#endif

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackDigitalInformation::~trackDigitalInformation() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackDigitalInformation& trackDigitalInformation::operator = (const trackDigitalInformation& value) {

	this->position = value.position;
	this->value    = value.value;

#ifndef NOANALYSIS

	this->hits     = value.hits;

#endif

	return *this;

}
