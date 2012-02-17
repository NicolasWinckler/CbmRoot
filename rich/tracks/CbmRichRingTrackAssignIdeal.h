/******************************************************************************
* $Id: CbmRichRingTrackAssignIdeal.h,v 1.1 2006/02/23 14:56:00 hoehne Exp $
*
*  Class  : CbmRichRingTrackAssignIdeal
*  Description: Ideal Ring-Track Assignement
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingTrackAssignIdeal.h,v $
*  Revision 1.1  2006/02/23 14:56:00  hoehne
*  initial version: Ideal ring-track assignement
*
*  
*
*******************************************************************************/



#ifndef CBMRichRingTrackAssignIdeal_H
#define CBMRichRingTrackAssignIdeal_H

#include "CbmRichRing.h"
#include "CbmRichRingTrackAssign.h"

#include "CbmStack.h"

#include "TClonesArray.h"


class CbmRichRingTrackAssignIdeal : public CbmRichRingTrackAssign {

public:

   /**
   * \brief Default constructor.
   */
   CbmRichRingTrackAssignIdeal();

   /**
   * \brief Destructor.
   */
   virtual ~CbmRichRingTrackAssignIdeal();

   /**
    * \brief Initialisation.
    */
   void Init();


  /**
   * \brief Read the extrapolated Tracks, read the found rings, assign according
   * to closest distance criterium
   * pointers to both Rings are given as argument
   *
   * \param[in] pTrack ointer to extrapolated track
   * \param[in] pRing     pointer to found ring
   */
   virtual void DoAssign(
         TClonesArray* rings,
         TClonesArray* richProj);

private:
   TClonesArray* fMcTracks;
   TClonesArray* fGlobalTracks;
   TClonesArray* fRingMatches;
   TClonesArray* fStsTrackMatches;

   Double_t fMaxDistance;  // max. distance between ring center and track extrapolation */
   Int_t fMinNofHitsInRing; // min number of hits per ring */

   /**
    * \brief Copy constructor.
    */
   CbmRichRingTrackAssignIdeal(const CbmRichRingTrackAssignIdeal&);

   /**
    * \brief Assignment operator.
    */
   CbmRichRingTrackAssignIdeal& operator=(const CbmRichRingTrackAssignIdeal&);

   ClassDef(CbmRichRingTrackAssignIdeal,1);
};

#endif
