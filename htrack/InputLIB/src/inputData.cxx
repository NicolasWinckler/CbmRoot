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
//     - reads the event information from a special file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:35:32 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../../DataRootObjectLIB/include/trackfinderInputHit.h"
#include "../../DataRootObjectLIB/include/trackfinderInputTrack.h"
#include "../../RootFrameworkLIB/include/hitProducer.h"
#include "../include/inputError.h"
#include "../include/inputData.h"
#include <malloc.h>
#include <stdlib.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputData::inputData() {

	data.removeDetector();
	data.removeAllHits();
	data.removeAllTracks();

	detectorMask      = 0;
	typeOfHitProducer = NONEHITPRODUCER;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputData::inputData(bitArray detMask) {

	data.removeDetector();
	data.removeAllHits();
	data.removeAllTracks();

	detectorMask      = detMask;
	typeOfHitProducer = NONEHITPRODUCER;

}
inputData::inputData(int hitProducer) {

	data.removeDetector();
	data.removeAllHits();
	data.removeAllTracks();

	detectorMask      = 0;
	typeOfHitProducer = hitProducer;

}
inputData::inputData(bitArray detMask, int hitProducer) {

	data.removeDetector();
	data.removeAllHits();
	data.removeAllTracks();

	detectorMask      = detMask;
	typeOfHitProducer = hitProducer;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputData::~inputData() {

}

/****************************************************************
 * method initializes the object.								*
 ****************************************************************/

void inputData::init(bitArray detMask) {

	detectorMask = detMask;

}
void inputData::init(int hitProducer) {

	typeOfHitProducer = hitProducer;

}

/****************************************************************
 * method returns a reference to the data struct				*
 ****************************************************************/

trackfinderInputData& inputData::getInputData() {

	return data;

}

/****************************************************************
 * method returns a reference to the data struct				*
 ****************************************************************/

trackfinderInputData* inputData::getInputDataPointer() {

	return &data;

}

ClassImp(inputData)
