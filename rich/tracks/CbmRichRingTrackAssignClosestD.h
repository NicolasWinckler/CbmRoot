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

class TClonesArray;

class CbmRichRingTrackAssignClosestD : public CbmRichRingTrackAssign
{

public:

  /**
   * \brief Default constructor.
   */
	CbmRichRingTrackAssignClosestD();

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmRichRingTrackAssignClosestD();

	/**
	 * \brief Initialisation.
	 */
	void Init();

	/** Method DoAssign.
	 ** Task: Read the extrapolated Tracks, read the found rings, assign according to closest distance criterium
	 ** pointers to both Rings are given as argument
	 **
	 *@param pTrack    pointer to extrapolated track
	 *@param pRing     pointer to found ring
	 **/
	void DoAssign(
	      TClonesArray* pRing,
	      TClonesArray* pTrack);

private:
	TClonesArray* fGlobalTracks;
	TClonesArray* fTrdTracks;

	Double_t fMaxDistance; // max. distance between ring center and track extrapolation
	Int_t fMinNofHitsInRing; // min number of hits per ring

   /*
    * \brief Check if global track was identified as electron in the TRD detector.
    * \param iTrack Index of global track.
    */
   Bool_t IsTrdElectron(
         Int_t iTrack);

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
