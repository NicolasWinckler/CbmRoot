//////////////////////////////////////////////////////////////////////
/// (C)opyright 2005
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
///     - base class for holding the track information of the transformation
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-10-19 14:32:59 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKDIGITALINFORMATION_H
#define _TRACKDIGITALINFORMATION_H


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/bitArray.h"
#include "../../MiscLIB/include/hitArray.h"
#include "../../DataObjectLIB/include/trackCoordinates.h"


/* **************************************************************
 * CLASS trackDigitalInformation				 				*
 * **************************************************************/

class trackDigitalInformation {

public:

	trackCoordinates position;	/**< Object to store the coordinates of the histogram for the peak for this track. */
	bitArray         value;		/**< Object to store the value of the peak computed for this track. */ 

#ifndef NOANALYSIS

	hitArray         hits;		/**< Object to store all hits computed to belong to this track. */

#endif

/**
 * Default constructor
 */

	trackDigitalInformation();

/**
 * Constructor
 */

	trackDigitalInformation(const trackDigitalInformation& value);

/**
 * Destructor
 */

	virtual ~trackDigitalInformation();

/**
 * operator = ()
 */

	const trackDigitalInformation& operator = (const trackDigitalInformation& value);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfData(unsigned short dimension = 0);

};

#endif
