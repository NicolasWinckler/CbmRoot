//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
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
/// $Date: 2006/07/17 11:35:10 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDIMXDIMX_H
#define _FILTERDIMXDIMX_H


#include "filterDimX.h"


/* **************************************************************
 * CLASS filterDimXDimX		 									*
 * **************************************************************/

class filterDimXDimX : public filterDimX {

protected:

	unsigned short filterSize1;		/**< Variable to store the size of the filter in the first dimension. */
	unsigned short filterSize2;		/**< Variable to store the size of the filter in the second dimension. */

public:

/**
 * Default constructor
 */

	filterDimXDimX();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	filterDimXDimX( histogramData** histogram,
					unsigned short  size1,
					unsigned short  size2,
					unsigned short  size,
					unsigned short  localSize);

/**
 * Default destructor
 */

	virtual ~filterDimXDimX();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	void init( histogramData** histogram,
			   unsigned short  size1,
			   unsigned short  size2,
			   unsigned short  size,
			   unsigned short  localSize);

};

#endif
