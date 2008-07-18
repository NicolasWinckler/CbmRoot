/* *******************************************************************
// (C)opyright 2004
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
//     - base class for holding the information from the look-up-tables
//       for one hit
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-05-14 15:43:59 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/lutBorder.h"


/****************************************************************
 * CLASS lutBorder								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutBorder::lutBorder() {

	firstBorder.start = 0;
	firstBorder.stop  = 0;
	secondBorder.houghCoord.clear();
	hit               = NULL;

}

/****************************************************************
 * Copy constructor												*
 ****************************************************************/

lutBorder::lutBorder(const lutBorder& value) {

	this->firstBorder  = value.firstBorder;
	this->secondBorder = value.secondBorder;
	this->hit          = value.hit;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutBorder::~lutBorder() {

	secondBorder.houghCoord.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const lutBorder& lutBorder::operator = (const lutBorder& value) {

	this->firstBorder  = value.firstBorder;
	this->secondBorder = value.secondBorder;
	this->hit          = value.hit;

	return *this;

}

/****************************************************************
 * method returns the prelut border								*
 ****************************************************************/

prelutHoughBorder& lutBorder::getPrelutHoughBorder() {

	return firstBorder;

}

/****************************************************************
 * method returns the lut border								*
 ****************************************************************/

lutHoughBorder& lutBorder::getLutHoughBorder() {

	return secondBorder;

}

/****************************************************************
 * method returns the hit corresponding to the borders			*
 ****************************************************************/

trackfinderInputHit* lutBorder::getHit() {

	return hit;

}

/****************************************************************
 * method sets the prelut border								*
 ****************************************************************/

prelutHoughBorder* lutBorder::getPrelutHoughBorderPointer() {

	return &firstBorder;

}

/****************************************************************
 * method sets the lut border									*
 ****************************************************************/

lutHoughBorder* lutBorder::getLutHoughBorderPointer() {

	return &secondBorder;

}

/****************************************************************
 * method sets the prelut border								*
 ****************************************************************/

void lutBorder::setPrelutHoughBorder(prelutHoughBorder& border) {

	firstBorder = border;

}

/****************************************************************
 * method sets the lut border									*
 ****************************************************************/

void lutBorder::setLutHoughBorder(lutHoughBorder& border) {

	secondBorder = border;

}

/****************************************************************
 * method sets the hit corresponding to the borders				*
 ****************************************************************/

void lutBorder::setHit(trackfinderInputHit* hit) {

	this->hit = hit;

}
