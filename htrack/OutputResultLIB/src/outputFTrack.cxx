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
// $Date: 2007-04-13 09:18:16 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/outputResultWarningMsg.h"
#include "../include/outputFTrack.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

outputFTrack::outputFTrack() : outputTrack() {

	writeTracksToFile = true;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

outputFTrack::outputFTrack(trackData** tracks) : outputTrack(tracks) {

	writeTracksToFile = true;

}
outputFTrack::outputFTrack(trackData** tracks, bool writeTracksToFile) : outputTrack(tracks) {

	this->writeTracksToFile = writeTracksToFile;

}
outputFTrack::outputFTrack(const char* name, trackData** tracks) : outputTrack(name, tracks) {

	writeTracksToFile = true;

}
outputFTrack::outputFTrack(const char* name, trackData** tracks, bool writeTracksToFile) : outputTrack(name, tracks) {

	this->writeTracksToFile = writeTracksToFile;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

outputFTrack::~outputFTrack() {

}

/****************************************************************
 * Method inits the variables.									*
 ****************************************************************/

void outputFTrack::init() {

	if (writeTracksToFile)
		initialize();

}

/****************************************************************
 * Method inits the variables.									*
 ****************************************************************/

void outputFTrack::init(bool writeTracksToFile) {

	this->writeTracksToFile = writeTracksToFile;

	init();

}
void outputFTrack::init(trackData** tracks, bool writeTracksToFile) {

	this->writeTracksToFile = writeTracksToFile;

	outputTrack::init(tracks);

}
void outputFTrack::init(const char* name, trackData** tracks, bool writeTracksToFile) {

	this->writeTracksToFile = writeTracksToFile;

	outputTrack::init(name, tracks);

}

/****************************************************************
 * writes the evaluated tracks into the root file				*
 ****************************************************************/

void outputFTrack::write(std::streambuf* terminal, TClonesArray* trackArray) {

	if (!writeTracksToFile)
		data.init(trackArray);
	else {
	
		if (trackArray != NULL) {

			trackPointerIsDisabledWarningMsg* trackPointerIsDisabled = new trackPointerIsDisabledWarningMsg();
			trackPointerIsDisabled->warningMsg();
			if(trackPointerIsDisabled != NULL) {
				delete trackPointerIsDisabled;
				trackPointerIsDisabled = NULL;
			}

		}

	}

	writeTracks(terminal);

}
