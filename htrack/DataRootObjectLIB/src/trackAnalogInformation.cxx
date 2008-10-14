/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
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
// $Date: 2008-08-14 12:33:08 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/trackAnalogInformation.h"


/****************************************************************
 * CLASS trackAnalogInformation					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackAnalogInformation::trackAnalogInformation() {

	position = trackParameter(0, 0, 0);

#ifndef NOANALYSIS

	hits     = hitArray();

#endif

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackAnalogInformation::trackAnalogInformation(const trackAnalogInformation& value) {

	this->position = value.position;

#ifndef NOANALYSIS

	this->hits     = value.hits;

#endif

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackAnalogInformation::~trackAnalogInformation() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackAnalogInformation& trackAnalogInformation::operator = (const trackAnalogInformation& value) {

	this->position = value.position;

#ifndef NOANALYSIS

	this->hits     = value.hits;

#endif

	return *this;

}
