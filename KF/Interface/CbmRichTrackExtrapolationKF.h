/******************************************************************************
* $Id: CbmRichTrackExtrapolationKF.h,v 1.1 2006/02/17 23:13:37 sgorboun Exp $
*
*  Class  : CbmRichTrackExtrapolationKF
*  Description: "TrackExtrapolation" from STS tracks (Kalman Fitter)
*               It reads the track array form STS and extrapolates those to
*               be projected to the Rich Photodetector to some z-Plane in RICH
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichTrackExtrapolationKF.h,v $
*  Revision 1.1  2006/02/17 23:13:37  sgorboun
*  Update of KF&L1 classes
*
*  Revision 1.2  2006/01/30 11:00:55  hoehne
*  bug fix: Point Array at z-plane was not filled correctly (counters not correct)
*
*  Revision 1.1  2006/01/26 09:53:22  hoehne
*  initial version for track extrapolation (base class + concrete implementations + task) to z-plane in RICH
*
*
*
*******************************************************************************/

#ifndef CBM_RICH_TARCK_EXTRAPOLATION_KF
#define CBM_RICH_TRACK_EXTRAPOLATION_KF 1

#include "CbmRichTrackExtrapolation.h"

class TClonesArray;
class CbmGlobalTrack;

class CbmRichTrackExtrapolationKF : public CbmRichTrackExtrapolation
{

 public:

  /** Default constructor **/
  CbmRichTrackExtrapolationKF();


  /** Standard constructor **/
  CbmRichTrackExtrapolationKF(Int_t MinNsts, Int_t verbose);


  /** Destructor **/
  virtual ~CbmRichTrackExtrapolationKF();


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
  virtual Int_t DoExtrapolate(TClonesArray* gTrackArray, Double_t fZ, TClonesArray* fTrackParamArray);



 private:

  /** Arrays **/
  TClonesArray* fTrackParamArray;
  TClonesArray* fSTSArray;

  /** Verbosity level **/
  Int_t fVerbose;

  Int_t    fMinNsts;       /** number of STS hits required for extrapolated track */
  Int_t    iEx;            /** number of extrapolated tracks */

  ClassDef(CbmRichTrackExtrapolationKF,1);

};

#endif
