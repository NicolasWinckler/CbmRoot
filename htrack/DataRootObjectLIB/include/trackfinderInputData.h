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
///     - class for structuring the dataObjects for input
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-05-04 11:19:37 $
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERINPUTDATA_H
#define _TRACKFINDERINPUTDATA_H


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/bitArray.h"
#include "../include/trackfinderInputHit.h"
#include "../include/trackfinderInputTrack.h"
#include "../include/trackfinderInputDetector.h"
#ifdef CBMROOTFRAMEWORK
#include "../include/trackfinderInputFMagneticField.h"
#else
#include "../include/trackfinderInputSMagneticField.h"
#endif
#include "TClonesArray.h"
#include "TObject.h"


//#define SUPEREXACTNOTEXISTINGTRACKIDWARNING		/**< This definition enables exact information about such tracks from NOTEXISTINGTRACKIDWARNING. */
#undef SUPEREXACTNOTEXISTINGTRACKIDWARNING

//#define EXACTNOTEXISTINGTRACKIDWARNING			/**< This definition enables a warning message, if a hit which has an index for a track is detected, and this track does not exist. */
#undef EXACTNOTEXISTINGTRACKIDWARNING

#define NOTEXISTINGTRACKIDWARNING					/**< This definition enables a summary about such tracks from NOTEXISTINGTRACKIDWARNING. */
//#undef NOTEXISTINGTRACKIDWARNING

//#define EXACTTRACKWITHNOPOINTSWARNING				/**< This definition enables information about such tracks which have zero StsPoints. */
#undef EXACTTRACKWITHNOPOINTSWARNING

//#define EXACTTRACKWITHNOHITSWARNING				/**< This definition enables information about such tracks which have zero StsPoints. */
#undef EXACTTRACKWITHNOHITSWARNING

//#define EXACTTRACKWITHHITSTOPOINTSWARNING			/**< This definition enables exact information about such tracks which have less or more real hits than StsPoints. */
#undef EXACTTRACKWITHHITSTOPOINTSWARNING

#define TRACKWITHNOPOINTSWARNING					/**< This definition enables information about such tracks which have zero StsPoints. */
//#undef TRACKWITHNOPOINTSWARNING

#define TRACKWITHNOHITSWARNING						/**< This definition enables information about such tracks which have zero StsPoints. */
//#undef TRACKWITHNOHITSWARNING

//#define TRACKWITHHITSTOPOINTSWARNING				/**< This definition enables a summary about such tracks which have less or more real hits than StsPoints. */
#undef TRACKWITHHITSTOPOINTSWARNING

//#define EXACTHITWITHNOPOINTSWARNING				/**< This definition enables information about such hits which have zero StsPoints. */
#undef EXACTHITWITHNOPOINTSWARNING

//#define EXACTHITWITHNOTRACKWARNING				/**< This definition enables information about such hits which have zero Tracks. */
#undef EXACTHITWITHNOTRACKWARNING

#define HITWITHNOPOINTWARNING						/**< This definition enables a summary about such hits which have zero StsPoints. */
//#undef HITWITHNOPOINTWARNING

#define HITWITHNOTRACKWARNING						/**< This definition enables a summary about such hits which have zero Tracks. */
//#undef HITWITHNOTRACKWARNING


/**
 * Struct consisting of pointers to the tracks at the position of track->getTrackIndex().
 * It is used to setup the pointers from tracks to hits and from hits to tracks.
 */
typedef struct {

	int                       maxEntry;		/**< Variable to store the number of the actual maximum entries. */
	trackfinderInputTrack**   list;			/**< Memory to store the pointers to the tracks. */

} trackListFromId;


/* **************************************************************
 * CLASS trackfinderInputData					 				*
 * **************************************************************/

class trackfinderInputData : public TObject {

protected:

	unsigned int                   eventNumber;						/**< Variable to store the number of the actual event. */
	TClonesArray*                  hits;							/**< Object to store the hits for this event. */
	TClonesArray*                  tracks;							/**< Object to store the tracks for this event. */
	trackfinderInputDetector*      detector;						/**< Object to store the detector for this event. */

#ifdef CBMROOTFRAMEWORK

	trackfinderInputFMagneticField* magneticField;					/**< Object to store the field map for this event. */

#else

	trackfinderInputSMagneticField* magneticField;					/**< Object to store the field map for this event. */

#endif

	trackListFromId                trackFromId;						/**< Struct to setup the track and hit pointers. */
	unsigned int                   numberOfHitsWithWrongTrackId;	/**< Variable to store the number of hits which has a trackId of a not-existing track. */

/**
 * method adds a trackIdAccess to the array
 */

	void addTrackToIdList(trackfinderInputTrack* track);

/**
 * method removes all trackIdAccess from the array
 */

	void removeTrackListFromId();

/**
 * method returns a reference to a track with a specified id
 */

	trackfinderInputTrack* getTrackById(int id);

/**
 * method initializes the tracks for the object
 */

	void initHits();

/**
 * method initializes the tracks for the object
 */

	void initTracks();

/**
 * method initializes the detector for the object
 */

	void initDetector();

/**
 * method initializes the magneticField for the object
 */

	void initMagneticField();

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderInputData, 1);

/**
 * Default constructor
 */

	trackfinderInputData();

/**
 * Constructor
 */

	trackfinderInputData(const trackfinderInputData& value);

/**
 * Destructor
 */

	virtual ~trackfinderInputData();

/**
 * operator = ()
 */

	const trackfinderInputData& operator = (const trackfinderInputData& value);

/**
 * method initializes the object
 */

	void init(char* magneticFieldFileName = NULL);

/**
 * Method inits the default detector.
 */

	void initDefaultDetector();

/**
 * method returns the number of detector stations
 */

	unsigned int getNumberOfStations();

/**
 * method returns the number of active detector stations
 */

	unsigned int getNumberOfActiveStations();

/**
 * This method returns the actual layers id.
 */

	unsigned short getStationIndex(int stationId);

/**
 * method returns the number of the actual event
 */

	unsigned int getEventNumber();

/**
 * method returns the number of hits
 */

	int getNumberOfHits();

/**
 * method returns the number of tracks
 */

	int getNumberOfTracks();

/**
 * method returns a reference to the track data
 */

	TClonesArray& getTrackData();

/**
 * method returns a reference to a track with a specified index
 */

	trackfinderInputTrack* getTrackByIndex(unsigned int index);

/**
 * method returns a reference to the point data
 */

	TClonesArray& getHitData();

/**
 * method returns a reference to a hit with a specified index
 */

	trackfinderInputHit* getHitByIndex(unsigned int index);

/**
 * method gets the whole detector with all stations
 */

	trackfinderInputDetector& getDetector();

/**
 * method gets the whole detector with all stations
 */

	trackfinderInputDetector* getDetectorPointer();

/**
 * This method returns the size of the used memory for the
 * source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfData(unsigned short dimension);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfData(unsigned short dimension);

/**
 * method sets the number of the actual event
 */

	void setEventNumber(unsigned int number);

/**
 * method adds a track to the end of the Track-TClonesArray
 * @see CbmMCTrack()
 */

	void addTrack(trackfinderInputTrack& track);
	void addTrack(CbmMCTrack* track, int trackId, double charge);
	void addTrack(int pdgId, double momX, double momY, double momZ, int trackId, double charge);

/**
 * method adds a hit to the end of the Hit-TClonesArray
 * @see CbmHit()
 */

	void addHit(trackfinderInputHit& hit);
	void addHit(int detectorId, CbmHit* hit, CbmMCPoint* point, int hitIndex);
	void addHit(int trackId, int detectorId, double hitPosX, double hitPosY, double pointPosX, double pointPosY, double posZ, int hitIndex, int pointIndex, bool maps, bool strip, bool hybrid);
	void addHit(int trackId, int detectorId, double hitPosX, double hitPosY, double pointPosX, double pointPosY, double posZ, int hitIndex, int pointIndex);

/**
 * method adds a detector station
 */

	void addStation(trackfinderInputStation& actualStation);

/**
 * method masks the detector stations
 */

	void maskDetector(bitArray mask);

/**
 * method removes all tracks from the Track-TClonesArray
 */

	void removeAllTracks();

/**
 * method removes all hits from the Hit-TClonesArray
 */

	void removeAllHits();

/**
 * method removes all detector stations
 */

	void removeDetector();

/**
 * method finalizes the hit/track assignment and destroyes the trackFromId-struct
 */

	void finalizeHitTrackAssignment(bool applyAssignmentCheck = true);

/**
 * method returns the magnetic field object
 */

#ifdef CBMROOTFRAMEWORK

	trackfinderInputFMagneticField* getMagneticField();

#else

	trackfinderInputSMagneticField* getMagneticField();

#endif

};

#endif
