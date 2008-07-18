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
///     - abstract class for all filter methods for
///		 maxMorphSearch
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:35:11 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDIMZDIMZDIMZ_H
#define _FILTERDIMZDIMZDIMZ_H


#include "filterDimZDimZ.h"


/* **************************************************************
 * CLASS filterDimZDimZDimZ	 									*
 * **************************************************************/

class filterDimZDimZDimZ : public filterDimZDimZ {

protected:

	unsigned short filterSize3;

public:

/**
 * Default constructor
 */

	filterDimZDimZDimZ();

/**
 * Constructor
 * @param tracks is an object for accessing the tracks
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param size3 is the filtersize in the third dimension
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	filterDimZDimZDimZ( trackData**    tracks,
					    unsigned short size1,
					    unsigned short size2,
					    unsigned short size3,
					    unsigned short size,
					    unsigned short localSize);

/**
 * Default destructor
 */

	virtual ~filterDimZDimZDimZ();

/**
 * This method initializes the object.
 * @param tracks is an object for accessing the tracks
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param size3 is the filtersize in the third dimension
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	void init( trackData**    tracks,
			   unsigned short size1,
			   unsigned short size2,
			   unsigned short size3,
			   unsigned short size,
			   unsigned short localSize);

};

#endif
