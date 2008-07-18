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
///     - base class for easy access the iterators of one track layer
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/11/17 15:12:37 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKLAYERACCESS_H
#define _TRACKLAYERACCESS_H


#include "trackDigitalInformation.h"
#include <list>


#define trackLayer std::list<trackDigitalInformation>		/**< Definition for easier reading code and easy exchaninging of the stl conatainer */


/* **************************************************************
 * CLASS trackLayerAccess						 				*
 * **************************************************************/

class trackLayerAccess {

public:

	trackLayer::iterator begin;		/**< Iterator to store the beginning of the stl container. */
	trackLayer::iterator end;		/**< Iterator to store the end of the stl container. */
	bool                 isValid;	/**< Variable to store if the iterators are valid. */

/**
 * Default constructor
 */

	trackLayerAccess();

/**
 * Constructor
 */

	trackLayerAccess(const trackLayerAccess& value);

/**
 * Destructor
 */

	virtual ~trackLayerAccess();

/**
 * operator = ()
 */

	const trackLayerAccess& operator = (const trackLayerAccess& value);

};

#endif
