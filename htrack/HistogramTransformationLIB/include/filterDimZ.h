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


#ifndef _FILTERDIMZ_H
#define _FILTERDIMZ_H


#include "../../DataRootObjectLIB/include/trackData.h"
#include "../../MiscLIB/include/bitArray.h"
#include "filterBasicStyle.h"
#include <iostream>


/**
 * Defines the filter strategy which should be used:
 *
 * SECONDFILTERHANDLINGTYPE |      FILTEROBJECT      | FILTERSTRATEGY   
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            0             | filterBasicNeutral     | no filtering
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            1             | filterBasicSimple      | delete array[element] if array[i] >= array[element]
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            2             | filterBasicSimpleMod   | delete array[element] if array[i] > array[element]
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            3             | filterBasicComplex     | delete array[element] if not global maximum or is not the local middle element
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            4             | filterBasicComplexMod  | delete array[element] if not global maximum or is not the absolute local maximum
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            5             | filterBasicSpecial     | if array[element] >= maximumClass use filterBasicSimpleMod, else use filterBasicSimple
 *--------------------------------------------------------------------------------------------------------------------------------------------
 */

#define SECONDFILTERHANDLINGTYPE  1


/**
 * Struct to store the information of a filter-deleted object
 */

typedef struct {

	trackLayer::iterator element;
	unsigned short       layer;

} removingInformation;


/* **************************************************************
 * CLASS filterDimZ			 									*
 * **************************************************************/

class filterDimZ {

protected:

	trackData**       tracks;				/**< Object for accessing the tracks. */
	bitArray*         filterMem;			/**< Array to store the input values for filtering. */
	filterBasicStyle* baseFilter;			/**< Object to build the filtering strategy. */
	unsigned short    filterSize;			/**< Variable to store the complete size of the area of the filter. */
	unsigned short    filterLocalSize;		/**< Variable to store the complete size of the area of the local filter. */

public:

/**
 * Default constructor
 */

	filterDimZ();

/**
 * Constructor
 * @param tracks is an object for accessing the tracks
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	filterDimZ( trackData**    tracks,
				unsigned short size,
				unsigned short localSize);

/**
 * Default destructor
 */

	virtual ~filterDimZ();

/**
 * This method initializes the object.
 * @param tracks is an object for accessing the tracks
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	void init( trackData**    tracks,
			   unsigned short size,
			   unsigned short localSize);

/**
 * This method implements the filter.
 */

	virtual void filter(std::streambuf* terminal = NULL) = 0;

};

#endif
