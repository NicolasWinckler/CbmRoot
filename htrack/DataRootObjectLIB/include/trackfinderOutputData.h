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
///     - class for structuring the dataObjects for output
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/12/12 13:04:39 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDEROUTPUTDATA_H
#define _TRACKFINDEROUTPUTDATA_H


#include "../../MiscLIB/include/defs.h"
#ifdef CBMROOTFRAMEWORK
	#include "CbmStsTrack.h"
#else
	#include "../../RootFrameworkLIB/include/CbmStsTrack.h"
#endif
#include "../include/trackfinderInputHit.h"
#include "../../DataObjectLIB/include/trackMomentum.h"
#include "../../DataObjectLIB/include/trackFrameworkMomentum.h"
#include "TClonesArray.h"
#include "TObject.h"


/* **************************************************************
 * CLASS trackfinderOutputData					 				*
 * **************************************************************/

class trackfinderOutputData : public TObject {

protected:

	TClonesArray* tracks;			/**< Object to store all tracks. */
	CbmStsTrack*  actualTrack;		/**< Pointer to the actual track for accessing. */ 
	bool          tracksAreLocal;	/**< Variable to store if the TClonesArray tracks is locally allocated or a pointer set from outside. */ 

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderOutputData, 1);

/**
 * Default constructor
 */

	trackfinderOutputData();

/**
 * Constructor
 */

	trackfinderOutputData(const trackfinderOutputData& value);

/**
 * Destructor
 */

	virtual ~trackfinderOutputData();

/**
 * operator = ()
 */

	const trackfinderOutputData& operator = (const trackfinderOutputData& value);

/**
 * initializes the object
 */

	void init();
	void init(TClonesArray* tracks);

/**
 * method returns the number of tracks
 */

	int getNumberOfTracks();

/**
 * method returns a reference to the track data
 */

	TClonesArray* getTrackData();

/**
 * method returns a reference to a track with a specified index
 */

	CbmStsTrack* getTrackByIndex(unsigned int index);

/**
 * This method returns the size of the used memory for the
 * source data.
 */

	double getUsedSizeOfData(unsigned short dimension);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 */

	double getAllocatedSizeOfData(unsigned short dimension);

/**
 * method adds a track to the end of the Track-TClonesArray
 */

	void addTrack(CbmStsTrack& track);
	void addTrack(trackMomentum& momentum);

/**
 * method adds a track to the end of the Track-TClonesArray
 */

	void addTrack(trackFrameworkMomentum& momentum);

/**
 * method adds a hit to the end of the Hit-TClonesArray
 */

	void addHitToActualTrack(trackfinderInputHit* hit);

/**
 * method removes all tracks from the Track-TClonesArray
 */

	void removeAllTracks();

};

#endif
