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
/// $Date: 2006/12/12 13:04:38 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKANALOGINFORMATION_H
#define _TRACKANALOGINFORMATION_H


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/hitArray.h"
#include "../../DataObjectLIB/include/trackParameter.h"


/* **************************************************************
 * CLASS trackAnalogInformation					 				*
 * **************************************************************/

class trackAnalogInformation {

public:

	trackParameter position;	/**< Object to store the parameters of the histogram for the peak for this track. */

#ifndef NOANALYSIS

	hitArray       hits;		/**< Object to store all hits computed to belong to this track. */

#endif

/**
 * Default constructor
 */

	trackAnalogInformation();

/**
 * Constructor
 */

	trackAnalogInformation(const trackAnalogInformation& value);

/**
 * Destructor
 */

	virtual ~trackAnalogInformation();

/**
 * operator = ()
 */

	const trackAnalogInformation& operator = (const trackAnalogInformation& value);

};

#endif
