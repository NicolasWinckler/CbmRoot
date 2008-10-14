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
/// $Date: 2008-10-10 13:47:04 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DIGITALHIT_H
#define _DIGITALHIT_H


#include <string>


/* **************************************************************
 * CLASS digitalHit								 				*
 * **************************************************************/

class digitalHit {

public:

	unsigned long identifier;		/**< Variable to store the digital hit identifier. */

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
 * method returns the start value
 */

	unsigned long getIdentifier();

/**
 * method sets the identifier value
 */

	void setNotIdentifiedIdentifier(std::string& value);

/**
 * method sets the identifier value
 */

	void setIdentifier(unsigned long value);

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
