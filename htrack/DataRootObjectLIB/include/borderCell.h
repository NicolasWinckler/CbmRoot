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
/// $Date: 2006/07/17 11:27:32 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _BORDERCELL_H
#define _BORDERCELL_H


#include "lutBorder.h"
#include <vector>


/* **************************************************************
 * CLASS borderCell								 				*
 * **************************************************************/

class borderCell {

public:

	std::vector<lutBorder> layer;		/**< Memory to store all computed borders with border.prelut.start is equal to this layer's number. */

/**
 * Default constructor
 */

	borderCell();

/**
 * Constructor
 */

	borderCell(const borderCell& value);

/**
 * Destructor
 */

	virtual ~borderCell();

/**
 * operator = ()
 */

	const borderCell& operator = (const borderCell& value);

};

#endif
