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
///     - base class for holding the track coordinates of the transformation
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/11/17 15:12:33 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKCOORDINATES_H
#define _TRACKCOORDINATES_H


#include "../../MiscLIB/include/coordinateSystem.h"


/* **************************************************************
 * CLASS trackCoordinates						 				*
 * **************************************************************/

class trackCoordinates {

protected:

	unsigned short dim[3];		/**< Variable to store the coordinatevector. */

public:

/**
 * Default constructor
 */

	trackCoordinates();

/**
 * Constructor
 * @param dim1 is the value in the first dimension
 * @param dim2 is the value in the second dimension
 * @param dim3 is the value in the third dimension
 */

	trackCoordinates(const trackCoordinates& value);
	trackCoordinates(unsigned short dim1, unsigned short dim2, unsigned short dim3);

/**
 * Destructor
 */

	virtual ~trackCoordinates();

/**
 * operator = ()
 */

	const trackCoordinates& operator = (const trackCoordinates& value);

/**
 * returns the value in the dimension
 */

	unsigned short get(unsigned short dimension);

/**
 * sets the value in the dimension
 */

	void set(unsigned short set, unsigned short dimension);

};

#endif
