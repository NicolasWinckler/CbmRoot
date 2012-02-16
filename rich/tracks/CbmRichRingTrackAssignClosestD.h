/******************************************************************************
* $Id: CbmRichRingTrackAssignClosestD.h,v 1.1 2006/01/26 09:54:27 hoehne Exp $
*
*  Class  : CbmRichRingTrackAssignClosestD
*  Description: Ring-Track Assignement according to the closest distance criterion
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingTrackAssignClosestD.h,v $
*  Revision 1.1  2006/01/26 09:54:27  hoehne
*  initial version: assignement of Rich rings and extrapolated tracks (base class, concrete implementation, Task)
*
*
*
*******************************************************************************/



#ifndef CBMRichRingTrackAssignClosestD_H
#define CBMRichRingTrackAssignClosestD_H

#include "CbmRichRingTrackAssign.h"
#include "CbmRichRing.h"

#include "CbmStack.h"

#include "TClonesArray.h"


class CbmRichRingTrackAssignClosestD : public CbmRichRingTrackAssign {

public:

  /** Default constructor **/
	CbmRichRingTrackAssignClosestD();

	/** Standard constructor
	 *@param distance         max. distance between ring center and projected track [cm]
	 *@param npoints         min number of points on ring
	 *@param verbose          verbosity level
	 */
	CbmRichRingTrackAssignClosestD(Double_t distance, Int_t npoints,
			Int_t verbose);

	/** Standard Constructor with name and title **/
	CbmRichRingTrackAssignClosestD(const char *name, const char *title,
			Double_t distance, Int_t npoints, Int_t verbose);

	/** Destructor **/
	virtual ~CbmRichRingTrackAssignClosestD();

	/** Initialisation **/
	void Init();

	Bool_t IsTrdElectron(Int_t iTrack);

	/** Method DoAssign.
	 ** Task: Read the extrapolated Tracks, read the found rings, assign according to closest distance criterium
	 ** pointers to both Rings are given as argument
	 **
	 *@param pTrack    pointer to extrapolated track
	 *@param pRing     pointer to found ring
	 **/
	void DoAssign(TClonesArray* pRing, TClonesArray* pTrack);

private:

	/** Arrays of track information **/
	TClonesArray* gTrackArray;
	TClonesArray* fTrdTracks;

	/** Verbosity level **/
	Int_t fVerbose;

	Double_t fDistance; /** max. distance between ring center and track extrapolation */
	Int_t fNpoints; /** min number of points per ring */

  /**
   * \brief Copy constructor.
   */
	CbmRichRingTrackAssignClosestD(const CbmRichRingTrackAssignClosestD&);

  /**
   * \brief Assignment operator.
   */
	CbmRichRingTrackAssignClosestD& operator=(const CbmRichRingTrackAssignClosestD&);


	ClassDef(CbmRichRingTrackAssignClosestD, 1);

};

#endif
