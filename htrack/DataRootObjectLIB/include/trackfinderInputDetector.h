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
///     - consists of the information for the detector
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:27:37 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERINPUTDETECTOR_H
#define _TRACKFINDERINPUTDETECTOR_H


#include "trackfinderInputStation.h"
#include "TObject.h"


/* **************************************************************
 * CLASS trackfinderInputDetector				 				*
 * **************************************************************/

class trackfinderInputDetector : public TObject {

private:

/**
 * method searches the detector for a specific station with id.
 */

	trackfinderInputStation* searchStation(int stationId);

protected:

	unsigned short           numberOfStations;			/**< Variable to store the number of stations of the detector. */
	unsigned short           numberOfActiveStations;	/**< Variable to store the number of stations which are not masked. That means which should be taken into the computation. */
	trackfinderInputStation* detector;					/**< Memory to store all stations of the detector. */

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderInputDetector, 1);

/**
 * Default constructor
 */

	trackfinderInputDetector();

/**
 * Constructor
 */

	trackfinderInputDetector(const trackfinderInputDetector& value);

/**
 * Destructor
 */

	virtual ~trackfinderInputDetector();

/**
 * operator = ()
 */

	const trackfinderInputDetector& operator = (const trackfinderInputDetector& value);

/**
 * method inits the default detector.
 */

	void initDefaultDetector();

/**
 * method returns the number of detector stations
 */

	unsigned short getNumberOfStations();

/**
 * method returns the number of active detector stations
 */

	unsigned short getNumberOfActiveStations();

/**
 * This method returns the actual layers id.
 */

	unsigned short getStationIndex(int stationId);

/**
 * method gets the detector station with a specific stationId
 */

	trackfinderInputStation& getStationById(int stationId);

/**
 * method gets the detector station with stationIndex
 * @see trackfinderInputStation::index
 */

	trackfinderInputStation& getStationByIndex(unsigned int stationIndex);

/**
 * method gets the detector station with a specific stationId
 */

	trackfinderInputStation* getStationPointer(int stationId);

/**
 * method adds the detector station
 */

	void addStation(trackfinderInputStation station);

/**
 * method removes the detector station with a specific stationId
 */

	void removeStation(int stationId);

/**
 * method removes all detector stations
 */

	void removeDetector();

/**
 * method sorts the detector stations
 */

	void sortDetector();

/**
 * method sets the indexes for all detector stations
 */

	void reIndex();

};

#endif
