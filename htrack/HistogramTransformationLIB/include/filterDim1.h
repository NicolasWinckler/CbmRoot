//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�nner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl��
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
/// |  X              |
/// |  X              | f1
/// |  X              |
/// |  X              |
/// |  X              |
/// ---------------------------------------------------------------------------------------------------
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/19 11:33:45 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDIM1_H
#define _FILTERDIM1_H


#include "filterDimX.h"


/* **************************************************************
 * CLASS filterDim1			 									*
 * **************************************************************/

class filterDim1 : public filterDimX {

public:

/**
 * Default constructor
 */

	filterDim1();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 * @param maximumClass is the maximal class which can occur while filtering
 */

	filterDim1( histogramData** histogram,
				unsigned short  size,
				unsigned short  localSize,
				bitArray maximumClass);

/**
 * Destructor
 */

	virtual ~filterDim1();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 * @param maximumClass is the maximal class which can occur while filtering
 */

	void init( histogramData** histogram,
			   unsigned short  size,
			   unsigned short  localSize,
			   bitArray maximumClass);

/**
 * This method implements the filter.
 */

	void filter();

};

#endif
