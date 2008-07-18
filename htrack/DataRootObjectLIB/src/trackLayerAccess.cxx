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
//     - base class for easy access the iterators of one track layer
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:27:45 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/trackLayerAccess.h"


/****************************************************************
 * CLASS trackLayerAccess						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackLayerAccess::trackLayerAccess() {

	isValid  = false;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackLayerAccess::trackLayerAccess(const trackLayerAccess& value) {

	this->begin    = value.begin;
	this->end      = value.end;
	this->isValid  = value.isValid;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackLayerAccess::~trackLayerAccess() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackLayerAccess& trackLayerAccess::operator = (const trackLayerAccess& value) {

	this->begin    = value.begin;
	this->end      = value.end;
	this->isValid  = value.isValid;

	return *this;

}
