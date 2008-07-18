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


#include "../include/dataRootObjectError.h"
#include "../include/inputHitStlMem.h"


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputHitStlMem::inputHitStlMem() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputHitStlMem::inputHitStlMem(const inputHitStlMem& value) {

	for (unsigned short i = 0; i < numberOfMemories; i++)
		this->stlMem[i] = value.stlMem[i];

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputHitStlMem::~inputHitStlMem() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const inputHitStlMem& inputHitStlMem::operator = (const inputHitStlMem& value) {

	for (unsigned short i = 0; i < numberOfMemories; i++)
		this->stlMem[i] = value.stlMem[i];

	return *this;

}

/****************************************************************
 * method resets the memory										*
 ****************************************************************/

void inputHitStlMem::reset() {

	for (unsigned short i = 0; i < numberOfMemories; i++)
		stlMem[i].clear();

}

/****************************************************************
 * method returns the number of memories						*
 ****************************************************************/

unsigned short inputHitStlMem::getNumberOfMemories() {

	return numberOfMemories;

}

/****************************************************************
 * operator []													*
 ****************************************************************/

inputHitStlList& inputHitStlMem::operator [] (size_t index) {

	inputHitStlList* returnValue;

	if (index < numberOfMemories)
		returnValue = &stlMem[index];
	else
		throw tooBigHitMemoryIndexError();

	return *returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * hits.														*
 ****************************************************************/

size_t inputHitStlMem::getUsedSizeOfMemory() {

	size_t returnValue;

	returnValue = 0;
	for (unsigned short i = 0; i < numberOfMemories; i++) {
		returnValue += stlMem[i].getNumberOfEntries() * sizeof(trackfinderInputHit*);
	}

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * hits.														*
 ****************************************************************/

size_t inputHitStlMem::getAllocatedSizeOfMemory() {

	size_t returnValue;

	returnValue = 0;
	for (unsigned short i = 0; i < numberOfMemories; i++) {
		returnValue += stlMem[i].getNumberOfEntries() * sizeof(trackfinderInputHit*);
	}

	return returnValue;

}
