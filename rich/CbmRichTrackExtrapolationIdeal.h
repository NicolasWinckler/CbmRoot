/******************************************************************************
* $Id: CbmRichTrackExtrapolationIdeal.h,v 1.2 2006/01/30 11:00:55 hoehne Exp $
*
*  Class  : CbmRichTrackExtrapolationIdeal
*  Description: "TrackExtrapolation" from MC points.
*               It reads the PointArray with ImPlanePoints from MC and selects those to
*               be projected to the Rich Photodetector
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichTrackExtrapolationIdeal.h,v $
*  Revision 1.2  2006/01/30 11:00:55  hoehne
*  bug fix: Point Array at z-plane was not filled correctly (counters not correct)
*
*  Revision 1.1  2006/01/26 09:53:22  hoehne
*  initial version for track extrapolation (base class + concrete implementations + task) to z-plane in RICH
*
*
*
*******************************************************************************/

#ifndef CBM_RICH_TARCK_EXTRAPOLATION_Ideal
#define CBM_RICH_TRACK_EXTRAPOLATION_Ideal 1

#include "CbmRichTrackExtrapolation.h"

class TClonesArray;
class CbmGlobalTrack;

class CbmRichTrackExtrapolationIdeal : public CbmRichTrackExtrapolation
{

 public:

  /** Default constructor **/
  CbmRichTrackExtrapolationIdeal();


  /** Standard constructor **/
  CbmRichTrackExtrapolationIdeal(Int_t MinNsts, Int_t verbose);


  /** Destructor **/
  virtual ~CbmRichTrackExtrapolationIdeal();


  /** Initialisation **/
  virtual void Init();

  /** Finish **/
  virtual void Finish();

/** Method DoExtrapolate.
   ** Task: Read the track array and fill the point array at given z-Plane in RICH detector
   ** pointers to which are given as argument
   **
   *@param rTrackArray  Array of tracks
   *@param rTrackParamArray  Array of FairTrackParam
   *@value Number of tracks extrapolated
   **/
  virtual Int_t DoExtrapolate(TClonesArray* gTrackArray, Double_t fZ, TClonesArray *fTrackParamArray);




 private:

  /** Arrays of MC information **/
  TClonesArray* fTrackParamArray;
  TClonesArray* fMCPointArray;
  TClonesArray* fMCTrackArray;
  TClonesArray* fSTSArray;
  TClonesArray* fTrackMatchArray;

  /** Verbosity level **/
  Int_t fVerbose;

  Int_t    fMinNsts;       /** number of STS hits required for extrapolated track */
  Int_t    iEx;            /** number of extrapolated tracks */

  ClassDef(CbmRichTrackExtrapolationIdeal,1);

};

#endif
