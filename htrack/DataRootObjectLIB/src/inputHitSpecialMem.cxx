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
// $Date: 2006/07/17 11:27:42 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/dataRootObjectError.h"
#include "../include/inputHitSpecialMem.h"


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputHitSpecialMem::inputHitSpecialMem() {

	for (unsigned short i = 0; i < numberOfMemories; i++)
		specialMem[i] = typeOfMemories();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputHitSpecialMem::inputHitSpecialMem(const inputHitSpecialMem& value) {

	for (unsigned short i = 0; i < numberOfMemories; i++)
		this->specialMem[i] = typeOfMemories(value.specialMem[i]);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputHitSpecialMem::~inputHitSpecialMem() {

	reset();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const inputHitSpecialMem& inputHitSpecialMem::operator = (const inputHitSpecialMem& value) {

	this->reset();
	for (unsigned short i = 0; i < numberOfMemories; i++)
		this->specialMem[i] = value.specialMem[i];

	return *this;

}

/****************************************************************
 * method resets the memory										*
 ****************************************************************/

void inputHitSpecialMem::reset() {

	for (unsigned short i = 0; i < numberOfMemories; i++)
		specialMem[i].clear();

}

/****************************************************************
 * method returns the number of memories						*
 ****************************************************************/

unsigned short inputHitSpecialMem::getNumberOfMemories() {

	return numberOfMemories;

}

/****************************************************************
 * operator []													*
 ****************************************************************/

typeOfMemories& inputHitSpecialMem::operator [] (size_t index) {

	typeOfMemories* returnValue;

	if (index < numberOfMemories)
		returnValue = &specialMem[index];
	else
		throw tooBigHitMemoryIndexError();

	return *returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * hits.														*
 ****************************************************************/

size_t inputHitSpecialMem::getUsedSizeOfMemory() {

	size_t returnValue;

	returnValue = 0;
	for (unsigned short i = 0; i < numberOfMemories; i++) {
		returnValue += sizeof(trackfinderInputHit**);
		returnValue += specialMem[i].getNumberOfEntries() * sizeof(trackfinderInputHit*);
	}

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * hits.														*
 ****************************************************************/

size_t inputHitSpecialMem::getAllocatedSizeOfMemory() {

	size_t returnValue;

	returnValue = 0;
	for (unsigned short i = 0; i < numberOfMemories; i++) {
		returnValue += sizeof(trackfinderInputHit**);
		returnValue += max(specialMem[i].getNumberOfEntries(), minimalArraySize) * sizeof(trackfinderInputHit*);
	}

	return returnValue;

}
