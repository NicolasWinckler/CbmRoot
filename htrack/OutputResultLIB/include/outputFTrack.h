//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M‰nner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl‰ﬂ
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - writes the evaluated tracks into a root-file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-04-13 09:18:16 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _OUTPUTFTRACK_H
#define _OUTPUTFTRACK_H


#include "outputTrack.h"


/* **************************************************************
 * CLASS outputFTrack							 				*
 * **************************************************************/

class outputFTrack : public outputTrack {

protected:

	bool writeTracksToFile;

public:

/**
 * Default constructor
 */

	outputFTrack();

/**
 * Constructor
 * @param name is the name of the output file to place the computed tracks
 * @param tracks is the object to access the computed tracks
 */

	outputFTrack(trackData** tracks);
	outputFTrack(trackData** tracks, bool writeTracksToFile);
	outputFTrack(const char* name, trackData** tracks);
	outputFTrack(const char* name, trackData** tracks, bool writeTracksToFile);

/**
 * Destructor
 */

	virtual ~outputFTrack();

/**
 * Method inits the variables.
 * @param name is the name of the output file to place the computed tracks
 * @param tracks is the object to access the computed tracks
 */

	void init();
	void init(bool writeTracksToFile);
	void init(trackData** tracks, bool writeTracksToFile);
	void init(const char* name, trackData** tracks, bool writeTracksToFile);

/**
 * writes the evaluated tracks into the root file
 * @param terminal is a buffer to place the process information
 * @param trackArray is the array to write the tracks to
 */

	void write(std::streambuf* terminal = NULL, TClonesArray* trackArray = NULL);

};

#endif
