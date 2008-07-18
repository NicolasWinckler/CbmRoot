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
///     - base class for handling the created borders of one layer
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/12/12 13:04:37 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HISTOGRAMCELL_H
#define _HISTOGRAMCELL_H


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/bitArray.h"
#include "../../MiscLIB/include/hitArray.h"


/* **************************************************************
 * CLASS histogramCell							 				*
 * **************************************************************/

class histogramCell {

public:

	bitArray value;		/**< Object to store the value of the peak computed for this cell. */ 

#ifndef NOANALYSIS

	hitArray hits;		/**< Object to store all hits computed to belong to this cell. */

#endif

/**
 * Default constructor
 */

	histogramCell();

/**
 * Constructor
 */

	histogramCell(const histogramCell& value);

/**
 * Destructor
 */

	virtual ~histogramCell();

/**
 * operator = ()
 */

	const histogramCell& operator = (const histogramCell& value);

/**
 * Method resets the object.
 */

	void reset();

/**
 * Method returns the size of the used memory for the hits.
 */

	size_t getUsedSizeForHitMemory();

/**
 * Method returns the size of the allocated memory for the hits.
 */

	size_t getAllocatedSizeForHitMemory();

};

#endif
