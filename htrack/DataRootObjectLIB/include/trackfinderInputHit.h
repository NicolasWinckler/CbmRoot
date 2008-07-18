//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�ner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl�
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
///     - consists of the information for the hits
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-04-16 10:37:51 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERINPUTHIT_H
#define _TRACKFINDERINPUTHIT_H


#include "../../MiscLIB/include/defs.h"
#ifdef CBMROOTFRAMEWORK
	#include "CbmHit.h"
	#include "CbmMCPoint.h"
#else
	#include "../../RootFrameworkLIB/include/CbmHit.h"
	#include "../../RootFrameworkLIB/include/CbmMCPoint.h"
#endif
#include "trackfinderInputTrack.h"
#include "trackfinderInputStation.h"
#include "TObject.h"


class trackfinderInputTrack;


/* **************************************************************
 * CLASS trackfinderInputHit					 				*
 * **************************************************************/

class trackfinderInputHit : public TObject {

protected:

	CbmHit*                  hit;			/**< Pointer to the original hit object coming from the framework. */
	CbmMCPoint*              point;			/**< Pointer to the original point object belonging to this hit coming from the framework. */
	trackfinderInputTrack*   track;			/**< Pointer to the original track for this hit object coming from the framework. */
	trackfinderInputStation* station;		/**< Pointer to the station of the detector which the hit hits. */
	bool                     isPointer;		/**< Variable to store if this object is just a pointer or it has allocated memory which has to be freed. */
	int                      hitIndex;		/**< Variable to store the index of this hit. */

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderInputHit, 1);

/**
 * Default constructor
 */

	trackfinderInputHit();

/**
 * Constructor
 * @see CbmHit()
 */

	trackfinderInputHit(const trackfinderInputHit& value);
	trackfinderInputHit(CbmHit* hitPointer, int index);
	trackfinderInputHit(int detectorId, double posX, double posY, double posZ, double xError, double yError, int index, int pointIndex, bool maps, bool strip, bool hybrid);
	trackfinderInputHit(int detectorId, double posX, double posY, double posZ, double xError, double yError, int index, int pointIndex);

/**
 * Destructor
 */

	virtual ~trackfinderInputHit();

/**
 * operator = ()
 */

	const trackfinderInputHit& operator = (const trackfinderInputHit& value);

/**
 * method returns the detectorId
 */

	int getStationId();

/**
 * method returns the x position
 */

	double getPosX();

/**
 * method returns the y position
 */

	double getPosY();

/**
 * method returns the z position
 */

	double getPosZ();

/**
 * method gets the original hit-object
 */

	CbmHit* getHit();

/**
 * method gets the original point-object from hit
 */

	CbmMCPoint* getPoint();

/**
 * method gets the track-object from hit
 */

	trackfinderInputTrack* getTrack();

/**
 * method gets the station-object from hit
 */

	trackfinderInputStation* getStation();

/**
 * method returns true if the original hit-object is from a
 * maps detector.
 */

	bool isMapsHit();

/**
 * method returns true if the original hit-object is from a
 * strip detector.
 */

	bool isStripHit();

/**
 * method returns true if the original hit-object is from a
 * hybrid detector.
 */

	bool isHybridHit();

/**
 * method gets the index of the original hit-object
 */

	int getHitIndex();

/**
 * method sets the original point-object from hit
 */

	void setPoint(CbmMCPoint* actualPoint);

/**
 * method sets the track-object from hit
 */

	void setTrack(trackfinderInputTrack* actualTrack);

/**
 * method sets the station-object from hit
 */

	void setStation(trackfinderInputStation* actualStation);

};

#endif
