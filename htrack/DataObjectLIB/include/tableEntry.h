//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gläß
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
///     - The class handles one classification set for the peaks in the histogram
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/11/06 11:12:13 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TABLEENTRY_H
#define _TABLEENTRY_H


#include "../../MiscLIB/include/bitArray.h"


/* **************************************************************
 * CLASS tableEntry								 				*
 * **************************************************************/

class tableEntry {

protected:

	bitArray signature;
	bitArray classification;

public:

/**
 * Default constructor
 */

	tableEntry();

/**
 * Constructor
 */

	tableEntry(bitArray& signature, bitArray& classification);

/**
 * Copy constructor
 */

	tableEntry(const tableEntry& value);

/**
 * Destructor
 */

	virtual ~tableEntry();

/**
 * operator = ()
 */

	const tableEntry& operator = (const tableEntry& value);

/**
 * This method converts the object into a string representation.
 */

	operator std::string();

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
 * method returns a pointer to the object
 */

	tableEntry* getPointer();

/**
 * method compare two objects of this class
 * @return value > 0, if element is smaller
 * @return value = 0, if element is equal
 * @return value < 0, if element is bigger
 */

	int compare(tableEntry element);

/**
 * method returns the signature
 */

	bitArray& getSignature();

/**
 * method returns the classification
 */

	bitArray& getClassification();

/**
 * method sets the signature
 */

	void setSignature(bitArray& value);

/**
 * method sets the classification
 */

	void setClassification(bitArray& value);

};

#endif
