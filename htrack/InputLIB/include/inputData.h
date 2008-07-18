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
///     - reads the event information from a special file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-04-16 10:38:13 $
/// $Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INPUTDATA_H
#define _INPUTDATA_H


#include "../../MiscLIB/include/bitArray.h"
#include "../../DataRootObjectLIB/include/trackfinderInputData.h"
#include "../../DataRootObjectLIB/include/trackfinderInputDetector.h"
#ifdef CBMROOTFRAMEWORK
	#include "CbmStsDigiScheme.h"
#else
	#include "../../RootFrameworkLIB/include/CbmStsDigiScheme.h"
#endif
#include "TObject.h"


//#define DETECTORINFO
#undef DETECTORINFO

//#define HITINFO
#undef HITINFO

//#define TRACKINFO
#undef TRACKINFO


/* **************************************************************
 * CLASS inputData								 				*
 * **************************************************************/

class inputData : public TObject {

protected:

	trackfinderInputData data;					/**< Object to store the input data. */
	bitArray             detectorMask;			/**< Object to store the mask for each detector station. */
	int                  typeOfHitProducer;		/**< Variable to store the way to get hits from points. */

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(inputData, 1);

/**
 * Default constructor
 */

	inputData();

/**
 * Constructor
 * @param detMask is an object representing the mask for each detector station
 * @param hitProducer defines the way to get hits from points
 */

	inputData(bitArray detMask);
	inputData(int hitProducer);
	inputData(bitArray detMask, int hitProducer);

/**
 * Destructor
 */

	virtual ~inputData();

/**
 * method initializes the object.
 * @param name is the name of the input file to read the data
 * @param detMask is an object representing the mask for each detector station
 * @param hitProducer defines the way to get hits from points
 */

	virtual void init() = 0;
	void init(bitArray detMask);
	void init(int hitProducer);
	virtual void init(const char* name, int hitProducer) = 0;

/**
 * method returns a reference to the data struct
 */

	trackfinderInputData& getInputData();

/**
 * method returns a reference to the data struct
 */

	trackfinderInputData* getInputDataPointer();

/**
 * Method inits the variables based on the detector stations.
 * @param scheme is an object consisting of the detector
 */

	virtual void readDetectorInfo(CbmStsDigiScheme* scheme = NULL) = 0;

/**
 * method reads the source data from a file based on the
 * CbmStsMapsHit-class, CbmStsHybridHit-class and
 * CbmStsStripHit-class
 * @param event identifies the number of the event to read
 * @param mHitArray is the memory for the mapsHits
 * @param hHitArray is the memory for the hybridHits
 * @param sHitArray is the memory for the stripHits
 */

	virtual void readDataSource(unsigned int event, TClonesArray* mHitArray, TClonesArray* hHitArray, TClonesArray* sHitArray) = 0;

/**
 * method reads the source data from a file based on the
 * CbmStsHit-class
 * @param event identifies the number of the event to read
 * @param mvdHitArray is the memory for the MVD hits
 * @param stsHitArray is the memory for the STS hits
 */

	virtual void readDataSource(unsigned int event, TClonesArray* mvdHitArray, TClonesArray* stsHitArray) = 0;

/**
 * This method returns the size of the used memory for the
 * source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	virtual double getUsedSizeOfData(unsigned short dimension) = 0;

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	virtual double getAllocatedSizeOfData(unsigned short dimension) = 0;

};

#endif
