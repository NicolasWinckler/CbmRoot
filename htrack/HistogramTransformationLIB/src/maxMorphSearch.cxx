/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gläß
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - class for evaluating the histogram
//     - class computes the tracks
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/19 11:33:49 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/histogramTransformationError.h"
#include "../include/maxMorphSearch.h"
#include "../include/firstFilterFinal.h"
#include "../include/firstFilterFinalMod.h"
#include "../include/filterDim1.h"
#include "../include/filterDim1Dim2.h"
#include "../include/filterDim1Dim2Mod.h"
#include "../include/filterDim12Dim1.h"
#include "../include/filterDim12.h"
#include "../include/filterDim2.h"
#include "../include/filterDim3.h"
#include "../include/filterDim3Mod.h"
#include "../include/secondFilterFinal.h"
#include "../include/secondFilterFinalMod.h"


/****************************************************************
 * This method filters the peakclasses in a speciallized manner.*
 ****************************************************************/

void maxMorphSearch::filteringHistogramLayer() {

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();

	if (firstFilter != NULL)
		firstFilter->filter();
	if (secondFilter != NULL)
		secondFilter->filter();
	if (combiFilter != NULL)
		combiFilter->filter();

}

/****************************************************************
 * This method filters the tracks in a speciallized manner		*
 * again.														*
 ****************************************************************/

void maxMorphSearch::filteringHistogram(std::streambuf* terminal) {

	if (tracks == NULL)
		throw cannotAccessHistogramError();
	if (*tracks == NULL)
		throw cannotAccessHistogramError();

	if (thirdFilter != NULL)
		thirdFilter->filter(terminal);

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

maxMorphSearch::maxMorphSearch() : histogramTransformation() {

	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = NULL;
	thirdFilter  = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

maxMorphSearch::maxMorphSearch(histogramData** histogram, trackData** tracks, tables** ratings, unsigned short firstFilterDim1ClearRadius, unsigned short firstFilterDim2ClearRadius, unsigned short secondFilterDim1ClearRadius, unsigned short secondFilterDim2ClearRadius, unsigned short secondFilterDim3ClearRadius) : histogramTransformation(histogram, tracks, ratings, 2 * firstFilterDim1ClearRadius + 1, 2 * firstFilterDim2ClearRadius + 1, 2 * secondFilterDim3ClearRadius + 1) {

	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

#if (FIRSTFILTERGEOMETRIETYPE == 0)
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = NULL;
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 1)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = new filterDim2( histogram,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
	secondFilter = new filterDim1( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
	combiFilter  = NULL;
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 2)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = new filterDim12( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
	secondFilter = new filterDim2( histogram,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
	combiFilter  = NULL;
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 3)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new filterDim1Dim2( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 4)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new filterDim1Dim2Mod( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 5)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new filterDim12Dim1( histogram,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 6)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new firstFilterFinal( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 7)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new firstFilterFinalMod( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif

#if (SECONDFILTERGEOMETRIETYPE == 0)
	thirdFilter  = NULL;
#endif
#if (SECONDFILTERGEOMETRIETYPE == 1)
	unsigned short secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;
	thirdFilter  = new filterDim3Mod( tracks,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (SECONDFILTERGEOMETRIETYPE == 2)
	unsigned short secondFilterNeighborhoodDim1ClearRadius = 2 * secondFilterDim1ClearRadius + 1;
	unsigned short secondFilterNeighborhoodDim1LocalRadius = 2 * (secondFilterDim1ClearRadius / 2) + 1;
	unsigned short secondFilterNeighborhoodDim2ClearRadius = 2 * secondFilterDim2ClearRadius + 1;
	unsigned short secondFilterNeighborhoodDim2LocalRadius = 2 * (secondFilterDim2ClearRadius / 2) + 1;
	unsigned short secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;
	thirdFilter = new secondFilterFinal( tracks,
						   secondFilterNeighborhoodDim1ClearRadius,
						   secondFilterNeighborhoodDim2ClearRadius,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim1LocalRadius,
						   secondFilterNeighborhoodDim2LocalRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (SECONDFILTERGEOMETRIETYPE == 3)
	unsigned short secondFilterNeighborhoodDim1ClearRadius = 2 * secondFilterDim1ClearRadius + 1;
	unsigned short secondFilterNeighborhoodDim1LocalRadius = 2 * (secondFilterDim1ClearRadius / 2) + 1;
	unsigned short secondFilterNeighborhoodDim2ClearRadius = 2 * secondFilterDim2ClearRadius + 1;
	unsigned short secondFilterNeighborhoodDim2LocalRadius = 2 * (secondFilterDim2ClearRadius / 2) + 1;
	unsigned short secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;
	thirdFilter = new secondFilterFinalMod( tracks,
						   secondFilterNeighborhoodDim1ClearRadius,
						   secondFilterNeighborhoodDim2ClearRadius,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim1LocalRadius,
						   secondFilterNeighborhoodDim2LocalRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (SECONDFILTERGEOMETRIETYPE == 4)
	unsigned short secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;
	thirdFilter  = new filterDim3( tracks,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

maxMorphSearch::~maxMorphSearch() {

	if (firstFilter != NULL) {
		delete firstFilter;
		firstFilter = NULL;
	}
	if (secondFilter != NULL) {
		delete secondFilter;
		secondFilter = NULL;
	}
	if (combiFilter != NULL) {
		delete combiFilter;
		combiFilter = NULL;
	}
	if (thirdFilter != NULL) {
		delete thirdFilter;
		thirdFilter = NULL;
	}

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void maxMorphSearch::init( histogramData** histogram, trackData** tracks,
						   tables** ratings,
						   unsigned short firstFilterDim1ClearRadius,
						   unsigned short firstFilterDim2ClearRadius,
						   unsigned short secondFilterDim1ClearRadius,
						   unsigned short secondFilterDim2ClearRadius,
						   unsigned short secondFilterDim3ClearRadius) {


	histogramTransformation::init(histogram, tracks, ratings,
								  2 * firstFilterDim1ClearRadius + 1,
								  2 * firstFilterDim2ClearRadius + 1,
								  2 * secondFilterDim3ClearRadius + 1);

#if (FIRSTFILTERGEOMETRIETYPE == 0)
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = NULL;
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 1)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = new filterDim2( histogram,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
	secondFilter = new filterDim1( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
	combiFilter  = NULL;
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 2)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = new filterDim12( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
	secondFilter = new filterDim2( histogram,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
	combiFilter  = NULL;
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 3)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new filterDim1Dim2( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 4)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new filterDim1Dim2Mod( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 5)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new filterDim12Dim1( histogram,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 6)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new firstFilterFinal( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (FIRSTFILTERGEOMETRIETYPE == 7)
	unsigned short firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	unsigned short firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = new firstFilterFinalMod( histogram,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif

#if (SECONDFILTERGEOMETRIETYPE == 0)
	thirdFilter  = NULL;
#endif
#if (SECONDFILTERGEOMETRIETYPE == 1)
	unsigned short secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;
	thirdFilter  = new filterDim3Mod( tracks,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (SECONDFILTERGEOMETRIETYPE == 2)
	unsigned short secondFilterNeighborhoodDim1ClearRadius = 2 * secondFilterDim1ClearRadius + 1;
	unsigned short secondFilterNeighborhoodDim1LocalRadius = 2 * (secondFilterDim1ClearRadius / 2) + 1;
	unsigned short secondFilterNeighborhoodDim2ClearRadius = 2 * secondFilterDim2ClearRadius + 1;
	unsigned short secondFilterNeighborhoodDim2LocalRadius = 2 * (secondFilterDim2ClearRadius / 2) + 1;
	unsigned short secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;
	thirdFilter = new secondFilterFinal( tracks,
						   secondFilterNeighborhoodDim1ClearRadius,
						   secondFilterNeighborhoodDim2ClearRadius,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim1LocalRadius,
						   secondFilterNeighborhoodDim2LocalRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (SECONDFILTERGEOMETRIETYPE == 3)
	unsigned short secondFilterNeighborhoodDim1ClearRadius = 2 * secondFilterDim1ClearRadius + 1;
	unsigned short secondFilterNeighborhoodDim1LocalRadius = 2 * (secondFilterDim1ClearRadius / 2) + 1;
	unsigned short secondFilterNeighborhoodDim2ClearRadius = 2 * secondFilterDim2ClearRadius + 1;
	unsigned short secondFilterNeighborhoodDim2LocalRadius = 2 * (secondFilterDim2ClearRadius / 2) + 1;
	unsigned short secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;
	thirdFilter = new secondFilterFinalMod( tracks,
						   secondFilterNeighborhoodDim1ClearRadius,
						   secondFilterNeighborhoodDim2ClearRadius,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim1LocalRadius,
						   secondFilterNeighborhoodDim2LocalRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif
#if (SECONDFILTERGEOMETRIETYPE == 4)
	unsigned short secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;
	thirdFilter  = new filterDim3( tracks,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*ratings)->getCodingTableMaximumClassification());
#endif

}
