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


#ifndef _FILTERDIMX_H
#define _FILTERDIMX_H


#include "../../DataRootObjectLIB/include/histogramData.h"
#include "../../MiscLIB/include/bitArray.h"
#include "filterBasicStyle.h"
#include <iostream>


/**
 * Defines the filter strategy which should be used:
 *
 * FIRSTFILTERHANDLINGTYPE |      FILTEROBJECT      | FILTERSTRATEGY   
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            0            | filterBasicNeutral     | no filtering
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            1            | filterBasicSimple      | delete array[element] if array[i] >= array[element]
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            2            | filterBasicSimpleMod   | delete array[element] if array[i] > array[element]
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            3            | filterBasicComplex     | delete array[element] if not global maximum or is not the local middle element
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            4            | filterBasicComplexMod  | delete array[element] if not global maximum or is not the absolute local maximum
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            5            | filterBasicSpecial     | if array[element] >= maximumClass use filterBasicSimpleMod, else use filterBasicSimple
 *--------------------------------------------------------------------------------------------------------------------------------------------
 */

#define FIRSTFILTERHANDLINGTYPE  1


/* **************************************************************
 * CLASS filterDimX			 									*
 * **************************************************************/

class filterDimX {

protected:

	histogramData**   histogram;			/**< Object for accessing the histogram. */
	bitArray*         filterMem;			/**< Array to store the input values for filtering. */
	filterBasicStyle* baseFilter;			/**< Object to build the filtering strategy. */
	unsigned short    filterSize;			/**< Variable to store the complete size of the area of the filter. */
	unsigned short    filterLocalSize;		/**< Variable to store the complete size of the area of the local filter. */

public:

/**
 * Default constructor
 */

	filterDimX();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	filterDimX( histogramData** histogram,
				unsigned short  size,
				unsigned short  localSize);

/**
 * Default destructor
 */

	virtual ~filterDimX();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	void init( histogramData**  histogram,
			   unsigned short   size,
			   unsigned short   localSize);

/**
 * This method implements the filter.
 */

	virtual void filter() = 0;

};

#endif
