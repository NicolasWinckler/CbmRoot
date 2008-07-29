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
///     - base class for holding the houghTransformation border from
///       the lut look-up-table
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-10-19 14:43:57 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTHOUGHBORDER_H
#define _LUTHOUGHBORDER_H


#include "specialMemClass.h"
#include "houghBorderPosition.h"
#include "houghBorderCommand.h"
#include <iostream>


/* **************************************************************
 * CLASS lutHoughBorder							 				*
 * **************************************************************/

class lutHoughBorder {

public:

	specialMemClass<houghBorderPosition> houghCoord;		/**< Object to store all positions of the entry in the histogram in a list of coordinates. */

/**
 * Default constructor
 */

	lutHoughBorder();

/**
 * Constructor
 */

	lutHoughBorder(const lutHoughBorder& value);

/**
 * Destructor
 */

	virtual ~lutHoughBorder();

/**
 * operator = ()
 */

	const lutHoughBorder& operator = (const lutHoughBorder& value);

/**
 * This method converts the object into a string representation
 * and adds no identifiers.
 */
	
	std::string toNotIdentifiedString();

/**
 * This method converts the object into a string representation
 * and adds identifiers.
 */

	std::string toIdentifiedString();

/**
 * This method clears the object.
 */

	void clear();

/**
 * This method returns the houghBorder command.
 * @param maxDim is the stepsize of the second dimension to have a range check 
 */

	houghBorderCommand getHoughBorderCommand(unsigned short maxDim);

/**
 * This method sets the houghBorder command.
 * @param value is the source to setup the values for the specialMem<houghBorderPosition> houghCoord
 * @param maxFirstDim is the stepsize of the first dimension to have a range check 
 * @param maxSecondDim is the stepsize of the second dimension to have a range check 
 */

	void setHoughBorderCommand(houghBorderCommand& value, unsigned short maxFirstDim, unsigned short maxSecondDim);

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
