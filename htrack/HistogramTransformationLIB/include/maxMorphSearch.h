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
///     - class for evaluating the histogram
///     - class computes the tracks
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/19 11:33:47 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MAXMORPHSEARCH_H
#define _MAXMORPHSEARCH_H


#include "histogramTransformation.h"
#include "filterDimX.h"
#include "filterDimZ.h"
#include "filterDimXDimX.h"


/**
 * Defines the filter's geometrie which should be used:
 *
 * FIRSTFILTERGEOMETRIETYPE |            FILTEROBJECTS            | FILTERSTRUCTURE | FILTERSIZE
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |                 |
 *                          | firstFilter  = NULL                 |                 |
 *             0            | secondFilter = NULL                 |   no structure  | 0
 *                          | combiFilter  = NULL                 |                 |
 *                          |                                     |                 |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |  X              |
 *                          | firstFilter  = filterDim2()         |  X              | f2
 *             1            | secondFilter = filterDim1()         |  X + X X X X X  | f1
 *                          | combiFilter  = NULL                 |  X              |
 *                          |                                     |  X              |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |      X    X     |
 *                          | firstFilter  = filterDim12()        |     X     X     | f1
 *             2            | secondFilter = filterDim2()         |    X   +  X     | f2
 *                          | combiFilter  = NULL                 |   X       X     |
 *                          |                                     |  X        X     |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |     XXXX        |
 *                          | firstFilter  = NULL                 |     XXXX        |
 *             3            | secondFilter = NULL                 |     X           |
 *                          | combiFilter  = filterDim1Dim2()     |  XXXX           | 2 * { S[i=0;f2/2-1] (f1+1)/2 } + 1 = (f2/2)*(f1+1)+1
 *                          |                                     |  XXXX           |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |     XXXX        |
 *                          | firstFilter  = NULL                 |     XXXX        |
 *             4            | secondFilter = NULL                 |  XXXXXXX        |
 *                          | combiFilter  = filterDim1Dim2Mod()  |  XXXX           | 2 * { S[i=0;f2/2-1] (f1+1)/2 } + f1 = (f2/2)*(f1+1)+f1
 *                          |                                     |  XXXX           |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |      XXXX       |
 *                          | firstFilter  = NULL                 |      XXX        |
 *             5            | secondFilter = NULL                 |     XXX         |
 *                          | combiFilter  = filterDim12Dim1()    |    XXX          | 2 * { S[i=0;f1/2] min(i, f2) } + f2 = 2 * { S[i=0;f1/2] i } + f2 - 2 * { S[i=0;f1/2-f2] i } = {(f1/2)*(f1/2+1) + f2} - {max(f1/2-f2,0)*(f1/2-f2+1)}
 *                          |                                     |   XXXX          |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |     XXXX        |
 *                          | firstFilter  = NULL                 |     XXXX        |
 *             6            | secondFilter = NULL                 |     X inPlace   |
 *                          | combiFilter  = firstFilterFinal()   |  XXXX           | 2 * { S[i=0;f2/2-1] (f1+1)/2 } + 1 = (f2/2)*(f1+1)+1 different localFiltersize
 *                          |                                     |  XXXX           |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |     XXXX        |
 *                          | firstFilter  = NULL                 |     XXXX        |
 *             7            | secondFilter = NULL                 |     X copy      |
 *                          | combiFilter  = firstFilterFinalMod()|  XXXX           | 2 * { S[i=0;f2/2-1] (f1+1)/2 } + 1 = (f2/2)*(f1+1)+1 different localFiltersize
 *                          |                                     |  XXXX           |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * S[i=0;x] is an abbreviation for Sum from i = 0 to x
 * f1 is an abbreviation for filtersize1 (This value must be odd)
 * f2 is an abbreviation for filtersize2 (This value must be odd)
 *
 *
 * SECONDFILTERGEOMETRIETYPE |            FILTEROBJECTS            | FILTERSTRUCTURE  | FILTERSIZE                                                                                                | FILTERSTRUCTURE (rotate dim3 90∞)
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |                  |                                                                                                           |
 *                           |                                     |                  |                                                                                                           |
 *             0             | thirdFilter  = NULL                 |   no structure   | 0                                                                                                         |  no structure
 *                           |                                     |                  |                                                                                                           |
 *                           |                                     |                  |                                                                                                           |
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |                  |                                                                                                           |
 *                           |                                     |                  |                                                                                                           |
 *             1             | thirdFilter  = filterDim3Mod        |        X copy    | f3                                                                                                        |  X X X X X
 *                           |                                     |                  |                                                                                                           |      copy
 *                           |                                     |                  |                                                                                                           |
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |        XXXX      |                                                                                                           |     XXX
 *                           |                                     |        XXXX      |                                                                                                           |     XXX
 *             2             | thirdFilter  = secondFilterFinal    |        X inplace | (f3-1) * ( 2 * { S[i=0;f1/2-1] (f2+1)/2 } + 1) + 1 = (f3-1)*((f1/2)*(f2+1)+1)+1 different localFiltersize |     XXX
 *                           |                                     |     XXXX         |                                                                                                           |     XXX inplace
 *                           |                                     |     XXXX         |                                                                                                           |     XXX
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |        XXXX      |                                                                                                           |     XXX
 *                           |                                     |        XXXX      |                                                                                                           |     XXX
 *             3             | thirdFilter  = secondFilterFinalMod |        X copy    | (f3-1) * ( 2 * { S[i=0;f1/2-1] (f2+1)/2 } + 1) + 1 = (f3-1)*((f1/2)*(f2+1)+1)+1 different localFiltersize |     XXX
 *                           |                                     |     XXXX         |                                                                                                           |     XXX copy
 *                           |                                     |     XXXX         |                                                                                                           |     XXX
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |                  |                                                                                                           |
 *                           |                                     |                  |                                                                                                           |
 *             4             | thirdFilter  = filterDim3           |        X inplace | f3                                                                                                        |  X X X X X
 *                           |                                     |                  |                                                                                                           |      inplace
 *                           |                                     |                  |                                                                                                           |
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * f3 is an abbreviation for filtersize3 (This value must be odd)
 *
 */


#define FIRSTFILTERGEOMETRIETYPE  6


#define SECONDFILTERGEOMETRIETYPE 2


/* **************************************************************
 * CLASS maxMorphSearch		 									*
 * **************************************************************/

class maxMorphSearch : public histogramTransformation {

protected:

	filterDimX*     firstFilter;	/**< Object to filter the histogram layer at first. */
	filterDimX*     secondFilter;	/**< Object to filter the histogram layer at second. */
	filterDimXDimX* combiFilter;	/**< Object to filter the histogram layer instead of firstFilter and secondFilter. */
	filterDimZ*     thirdFilter;	/**< Object to filter the tracks coming from the filtered histogram layer. */

/**
 * This method filters the peakclasses in a speciallized manner.
 */

	void filteringHistogramLayer();

/**
 * This method filters the tracks in a speciallized manner
 * again.
 * @param terminal is a buffer to place the process information
 */

	void filteringHistogram(std::streambuf* terminal = NULL);

public:

/**
 * Default constructor
 */

	maxMorphSearch();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 * @param firstFilterDim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param firstFilterDim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param secondFilterDim1ClearRadius is the region of interest for the second filter in the first dimension
 * @param secondFilterDim2ClearRadius is the region of interest for the second filter in the second dimension
 * @param secondFilterDim3ClearRadius is the region of interest for the second filter in the third dimension
 */

	maxMorphSearch( histogramData** histogram, trackData** tracks,
					tables** ratings,
					unsigned short firstFilterDim1ClearRadius,
					unsigned short firstFilterDim2ClearRadius,
					unsigned short secondFilterDim1ClearRadius,
					unsigned short secondFilterDim2ClearRadius,
					unsigned short secondFilterDim3ClearRadius);

/**
 * Destructor
 */

	virtual ~maxMorphSearch();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 * @param firstFilterDim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param firstFilterDim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param secondFilterDim1ClearRadius is the region of interest for the second filter in the first dimension
 * @param secondFilterDim2ClearRadius is the region of interest for the second filter in the second dimension
 * @param secondFilterDim3ClearRadius is the region of interest for the second filter in the third dimension
 */

void init( histogramData** histogram, trackData** tracks,
		   tables** ratings,
		   unsigned short firstFilterDim1ClearRadius,
		   unsigned short firstFilterDim2ClearRadius,
		   unsigned short secondFilterDim1ClearRadius,
		   unsigned short secondFilterDim2ClearRadius,
		   unsigned short secondFilterDim3ClearRadius);

};

#endif
