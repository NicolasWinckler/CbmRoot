//////////////////////////////////////////////////////////////////////
/// (C)opyright 2005
/// 
/// Institute of Computer Science V
/// Prof. M�ner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
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
///     - base class for holding the digital hit information
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:39:20 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DIGITALHIT_H
#define _DIGITALHIT_H


#include "../../MiscLIB/include/defs.h"

#if (ARCHITECTURE != PS3)

#include "trackfinderInputHit.h"

#endif

#include <string>


/* **************************************************************
 * CLASS digitalHit								 				*
 * **************************************************************/

class digitalHit {

private:

	unsigned long data;			/**< Variable to store the digital hit data. */

public:

/**
 * Default constructor
 */

	digitalHit();

/**
 * Constructor
 */

	digitalHit(const digitalHit& value);

/**
 * Destructor
 */

	virtual ~digitalHit();

/**
 * operator = ()
 */

	const digitalHit& operator = (const digitalHit& value);

/**
 * method returns the maximuum number of digital hits
 * which can occur
 */

#if (ARCHITECTURE != PS3)

	unsigned long getMaxNumberOfDigitalHitData();

#endif

/**
 * method returns the digital hit data
 */

	unsigned long getData();

/**
 * Method returns an analog hit based on the digital data.
 * CAUTION: The return object contains just coordinates.
 * Other parts of such an object are not set.
 */

#if (ARCHITECTURE != PS3)

	trackfinderInputHit getHit();

#endif

/**
 * method sets the digital hit data
 */

	void setData(unsigned long value);

/**
 * method sets the digital data based on an analog hit
 */

#if (ARCHITECTURE != PS3)

	void setHit(trackfinderInputHit* hit);

#endif

/**
 * method sets the digital data based on a string representation
 */

	void setNotIdentifiedString(std::string& value);

/**
 * This method converts the object into a string representation
 * without adding an identifier.
 */
	
	std::string toNotIdentifiedString();

/**
 * This method converts the object into a string representation
 * and adds an identifier.
 */

	std::string toIdentifiedString();

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
