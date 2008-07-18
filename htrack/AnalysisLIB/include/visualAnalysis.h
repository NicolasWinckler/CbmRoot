/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
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
//     - implements methods to visualize tracks
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-04-26 12:50:07 $
// $Revision: 1.3 $
//
// *******************************************************************/


#ifndef _VISUALANALYSIS_H
#define _VISUALANALYSIS_H


#include "TGeoManager.h"
#include "TVirtualGeoTrack.h"
#include <string>


/* **************************************************************
 * CLASS visualAnalysis					 						*
 * **************************************************************/

class visualAnalysis {

public:

/**
 * Default constructor
 */

	visualAnalysis();

/**
 * Destructor
 */

	virtual ~visualAnalysis();

/**
 * method sets the visibility for the TGeoManager volumes to display
 * @param name is the name identifier of the stations which have to be displayed. All others will be discarded.
 */

	void setupDisplay(std::string name1 = "sts", std::string name2 = "cave", std::string name3 = "pipevac", std::string name4 = "");

/**
 * method removes all tracks from the TGeoManager
 */

	void removeTracks();

/**
 * method adds the track to the TGeoManager
 * @param pdgCode is the particle identification code
 * @param trackIndex is the index for the track to be added
 */

	void addCurrentTrack(int pdgCode = 2212, int trackIndex = -1);

/**
 * method adds a point to the current track in the TGeoManager
 * @param x is the coordinate in the x-dimension
 * @param y is the coordinate in the y-dimension
 * @param z is the coordinate in the z-dimension
 * @param time is the arrival time at the given coordinate (must be in rising order)
 */

	void addPointToCurrentTrack(double x, double y, double z, double time = -1);

/**
 * method draws all tracks from the TGeoManager
 */

	void drawTracks();

};

#endif
