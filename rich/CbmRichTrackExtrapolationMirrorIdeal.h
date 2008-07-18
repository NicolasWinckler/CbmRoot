/******************************************************************************
* $Id: CbmRichTrackExtrapolationMirrorIdeal.h,v 1.1 2006/06/21 13:53:38 hoehne Exp $
*
*  Class  : CbmRichTrackExtrapolationMirrorIdeal
*  Description: "TrackExtrapolation" from MC points - acts on points stored in Rich Mirror!
*               It reads the PointArray with MirrorPoint from MC and selects those to
*               be projected to the Rich Photodetector
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichTrackExtrapolationMirrorIdeal.h,v $
*  Revision 1.1  2006/06/21 13:53:38  hoehne
*  for extrapolation from MirrorPoint:
*  store mirror point with STS track selection and track ID as chosen in RichExtrapolate in mirror
*
*
*
*******************************************************************************/

#ifndef CBM_RICH_TARCK_EXTRAPOLATION_Ideal
#define CBM_RICH_TRACK_EXTRAPOLATION_Ideal 1

#include "CbmRichTrackExtrapolation.h"

class TClonesArray;
class CbmGlobalTrack;

class CbmRichTrackExtrapolationMirrorIdeal : public CbmRichTrackExtrapolation
{

 public:

  /** Default constructor **/
  CbmRichTrackExtrapolationMirrorIdeal();


  /** Standard constructor **/
  CbmRichTrackExtrapolationMirrorIdeal(Int_t MinNsts, Int_t verbose);


  /** Destructor **/
  virtual ~CbmRichTrackExtrapolationMirrorIdeal();


  /** Initialisation **/
  virtual void Init();

  /** Finish **/
  virtual void Finish();

/** Method DoExtrapolate.
   ** Task: Read the track array and fill the point array in RICH mirror
   ** pointers to which are given as argument
   **
   *@param rTrackArray  Array of tracks
   *@param rTrackParamArray  Array of CbmTrackParam
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

  ClassDef(CbmRichTrackExtrapolationMirrorIdeal,1);

};

#endif
