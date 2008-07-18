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
///       the prelut look-up-table
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/11/06 11:12:13 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUTHOUGHBORDER_H
#define _PRELUTHOUGHBORDER_H


#include <iostream>


/* **************************************************************
 * CLASS prelutHoughBorder						 				*
 * **************************************************************/

class prelutHoughBorder {

public:

	unsigned short start;		/**< Variable to store the starting index of the border. */
	unsigned short stop;		/**< Variable to store the end index of the border. */

/**
 * Default constructor
 */

	prelutHoughBorder();

/**
 * Constructor
 */

	prelutHoughBorder(const prelutHoughBorder& value);

/**
 * Destructor
 */

	virtual ~prelutHoughBorder();

/**
 * operator = ()
 */

	const prelutHoughBorder& operator = (const prelutHoughBorder& value);

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

	std::string getStart();

/**
 * method returns the stop value
 */

	std::string getStop();

/**
 * method sets the start value
 */

	void setStart(std::string& value);

/**
 * method sets the stop value
 */

	void setStop(std::string& value);

};

#endif
