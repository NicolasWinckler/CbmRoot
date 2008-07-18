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
///     - class for evaluating the basic filter method for
///		 maxMorphSearch
///
/// | FILTERSTRUCTURE | FILTERSIZE
/// ---------------------------------------------------------------------------------------------------
/// |     XXXX        |
/// |     XXX         |
/// |    XXX          |
/// |   XXX           | 2 * { S[i=0;f1/2] (f2+1)/2 - i } - 1 = (f1/2+1)*(f2+1-f1/2) - 1
/// |  XXXX           |
/// ---------------------------------------------------------------------------------------------------
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/19 11:33:46 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDIM12DIM1_H
#define _FILTERDIM12DIM1_H


#include "filterDimXDimX.h"


/* **************************************************************
 * CLASS filterDim12Dim1	 									*
 * **************************************************************/

class filterDim12Dim1 : public filterDimXDimX {

protected:

	bitArray***  filterTempMem;			/**< Array for easier constructing the geometry of the filter. */
	bitArray**   memValues;				/**< Array to store the original values of the histogram for the next cell in the filter. */
	bool**       allocaterMarker;		/**< Array to store if the actual pointer of filterTempMem is a pointer or an allocated cell. */

public:

/**
 * Default constructor
 */

	filterDim12Dim1();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param localSize1 is the local filtersize in the first dimension
 * @param localSize2 is the local filtersize in the second dimension
 * @param maximumClass is the maximal class which can occur while filtering
 */

	filterDim12Dim1( histogramData** histogram,
					 unsigned short  size1,
					 unsigned short  size2,
					 unsigned short  localSize1,
					 unsigned short  localSize2,
					 bitArray maximumClass);

/**
 * Destructor
 */

	virtual ~filterDim12Dim1();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param localSize1 is the local filtersize in the first dimension
 * @param localSize2 is the local filtersize in the second dimension
 * @param maximumClass is the maximal class which can occur while filtering
 */

	void init( histogramData** histogram,
			   unsigned short  size1,
			   unsigned short  size2,
			   unsigned short  localSize1,
			   unsigned short  localSize2,
			   bitArray maximumClass);

/**
 * This method implements the filter.
 */

	void filter();

};

#endif
