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
//     - writes the evaluated tracks into a root-file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:36:01 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/outputResultWarningMsg.h"
#include "../include/outputSTrack.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

outputSTrack::outputSTrack() : outputTrack() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

outputSTrack::outputSTrack(trackData** tracks) : outputTrack(tracks) {

}
outputSTrack::outputSTrack(const char* name, trackData** tracks) : outputTrack(name, tracks) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

outputSTrack::~outputSTrack() {

}

/****************************************************************
 * Method inits the variables.									*
 ****************************************************************/

void outputSTrack::init() {

	initialize();

}

/****************************************************************
 * writes the evaluated tracks into the root file				*
 ****************************************************************/

void outputSTrack::write(std::streambuf* terminal, TClonesArray* trackArray) {

	if (trackArray != NULL) {

		trackPointerIsNotSupportedWarningMsg* trackPointerIsNotSupported = new trackPointerIsNotSupportedWarningMsg();
		trackPointerIsNotSupported->warningMsg();
		if(trackPointerIsNotSupported != NULL) {
			delete trackPointerIsNotSupported;
			trackPointerIsNotSupported = NULL;
		}

	}

	writeTracks(terminal);

}
