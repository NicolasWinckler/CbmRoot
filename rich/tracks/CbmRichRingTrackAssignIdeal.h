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

  /** Default constructor **/
  CbmRichRingTrackAssignIdeal();

  /** Standard constructor
   *@param distance         max. distance between ring center and projected track [cm]
   *@param npoints         min number of points on ring
   *@param verbose          verbosity level
   */
  CbmRichRingTrackAssignIdeal(Double_t distance, Int_t npoints, Int_t verbose);

  /** Standard Constructor with name and title **/
  CbmRichRingTrackAssignIdeal(const char *name, const char *title,
   				    Double_t distance, Int_t npoints, Int_t verbose);

  /** Destructor **/
  virtual ~CbmRichRingTrackAssignIdeal();

  /** Initialisation **/
  void Init();


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
  TClonesArray* fMCTrackArray;
  TClonesArray* gTrackArray;
  TClonesArray* fRingMatchArray;
  TClonesArray* fTrackMatchArray;


  /** Verbosity level **/
  Int_t fVerbose;

  Double_t fDistance;  /** max. distance between ring center and track extrapolation */
  Int_t    fNpoints;   /** min number of points per ring */

  ClassDef(CbmRichRingTrackAssignIdeal,1);

};

#endif
