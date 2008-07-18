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
//     - consists of the hits and a special addon for the special memory
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:27:43 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../include/inputHitStlList.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputHitStlList::inputHitStlList() {

	clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputHitStlList::inputHitStlList(const inputHitStlList& value) {

	this->stlList             = value.stlList;
	this->activeObjectPointer = this->stlList.begin();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputHitStlList::~inputHitStlList() {

	clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const inputHitStlList& inputHitStlList::operator = (const inputHitStlList& value) {

	this->stlList.clear();
	this->stlList             = value.stlList;
	this->activeObjectPointer = this->stlList.begin();

	return *this;

}

/****************************************************************
 * sets the activeObject to the initial value					*
 ****************************************************************/

void inputHitStlList::resetActiveObject() {

	activeObjectPointer = stlList.begin();

}

/****************************************************************
 * method clears the memory										*
 ****************************************************************/

void inputHitStlList::clear() {

	stlList.clear();
	resetActiveObject();

}

/****************************************************************
 * method returns the number of entries							*
 ****************************************************************/

unsigned short inputHitStlList::getNumberOfEntries() {

	return stlList.size();

}

/****************************************************************
 * method pushes the element at the end of the memory.			*
 ****************************************************************/

bool inputHitStlList::push(trackfinderInputHit* hit) {

	bool         returnValue;
	bool         alreadyInMem;

	returnValue = true;

	alreadyInMem = false;
	for (std::list<trackfinderInputHit*>::iterator i = stlList.begin(); i != stlList.end(); i++) {
		if (*i == hit) {
			alreadyInMem = true;
			break;
		}
	}

	if (!alreadyInMem) {

		stlList.push_back(hit);
		activeObjectPointer = stlList.end();
		activeObjectPointer--;

	}

	return returnValue;

}

/****************************************************************
 * returns a reference of the activeObject and set the next one	*
 * to the active object.										*
 ****************************************************************/

trackfinderInputHit* inputHitStlList::readActiveObjectAndMakeNextOneActive() {

	trackfinderInputHit* returnValue;

	if (getNumberOfEntries() > 0)
		returnValue = *activeObjectPointer;
	else
		throw cannotReadEmptyMemoryError(DATAROOTOBJECTLIB);

	activeObjectPointer++;
	if (activeObjectPointer == stlList.end())
		resetActiveObject();

	return returnValue;

}
