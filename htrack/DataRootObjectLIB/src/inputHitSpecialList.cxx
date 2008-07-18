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
//     - consists of the hits and a special addon for the special memory
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:27:42 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../include/inputHitSpecialList.h"


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputHitSpecialList::inputHitSpecialList() {

	specialAddArray.clear();
	numberOfEntries = 0;

	resetActiveObject();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputHitSpecialList::inputHitSpecialList(const inputHitSpecialList& value) {

	specialAddArray.clear();
	this->numberOfEntries           = value.numberOfEntries;

	for (unsigned short i = 0; i < min(minimalArraySize, value.numberOfEntries); i++)
		this->specialArray[i]       = value.specialArray[i];

	this->specialAddArray = value.specialAddArray;
	
	this->resetActiveObject();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputHitSpecialList::~inputHitSpecialList() {

	specialAddArray.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const inputHitSpecialList& inputHitSpecialList::operator = (const inputHitSpecialList& value) {

	clear();

	this->numberOfEntries            = value.numberOfEntries;

	for (unsigned short i = 0; i < min(minimalArraySize, value.numberOfEntries); i++)
		this->specialArray[i]       = value.specialArray[i];

	this->specialAddArray = value.specialAddArray;
	
	this->resetActiveObject();

	return *this;

}

/****************************************************************
 * sets the activeObject to the initial value					*
 ****************************************************************/

void inputHitSpecialList::resetActiveObject() {

	activeObjectPointer       = 0;
	internalListObjectPointer = 0;

}

/****************************************************************
 * method clears the memory										*
 ****************************************************************/

void inputHitSpecialList::clear() {

	specialAddArray.clear();
	numberOfEntries           = 0;
	resetActiveObject();

}

/****************************************************************
 * method returns the number of entries							*
 ****************************************************************/

unsigned short inputHitSpecialList::getNumberOfEntries() {

	return numberOfEntries;

}

/****************************************************************
 * method pushes the element at the end of the memory.			*
 ****************************************************************/

bool inputHitSpecialList::push(trackfinderInputHit* hit) {

	bool returnValue;
	bool alreadyInMem;

	if (getNumberOfEntries() == (unsigned short)-1)
		returnValue = false;
	else {

		returnValue = true;

		alreadyInMem = false;
		for (unsigned short i = 0; i < min(minimalArraySize, getNumberOfEntries()); i++) {
			if (specialArray[i] == hit) {
				alreadyInMem = true;
				break;
			}
		}
		for (std::list<trackfinderInputHit*>::iterator j = specialAddArray.begin(); j != specialAddArray.end(); j++) {
			if (*j == hit) {
				alreadyInMem = true;
				break;
			}
		}

		if (!alreadyInMem) {

			activeObjectPointer = getNumberOfEntries();
			numberOfEntries++;

			if (activeObjectPointer < minimalArraySize)
				specialArray[activeObjectPointer] = hit;
			else
				specialAddArray.push_back(hit);

		}

	}

	return returnValue;

}

/****************************************************************
 * returns a reference of the activeObject and set the next one	*
 * to the active object.										*
 ****************************************************************/

trackfinderInputHit* inputHitSpecialList::readActiveObjectAndMakeNextOneActive() {

	trackfinderInputHit* returnValue;

	if (activeObjectPointer < minimalArraySize)
		if (getNumberOfEntries() > 0)
			returnValue = specialArray[activeObjectPointer];
		else
			throw cannotReadEmptyMemoryError(DATAROOTOBJECTLIB);
	else {
		
		if ((activeObjectPointer - minimalArraySize == internalListObjectPointer + 1) && (activeObjectPointer - minimalArraySize != 0)) {

			internalListObjectPointer++;
			specialAddArrayIterator++;	

		}
		else {

			internalListObjectPointer =  activeObjectPointer - minimalArraySize;
			specialAddArrayIterator   =  specialAddArray.begin();
			for (unsigned short i = 0; i < internalListObjectPointer; i++)
				specialAddArrayIterator++;

		}
		returnValue = *specialAddArrayIterator;

	}

	activeObjectPointer++;
	if (activeObjectPointer == getNumberOfEntries())
		resetActiveObject();

	return returnValue;

}
