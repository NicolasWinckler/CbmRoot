/**
* \file CbmRichTrackExtrapolationMirrorIdeal.h
*
* \brief This is the implementation of the TrackExtrapolation from MC points -
* operating on points in the RICH mirror!
* It reads the STS track array, gets the corresponding MC MirrorPoint
* and selects those to be projected to the Rich Photodetector
* points will be stored on mirror surface again
*
* \author Claudia Hoehne
* \date 2006
**/

#ifndef CBM_RICH_TARCK_EXTRAPOLATION_MIRROR_IDEAL
#define CBM_RICH_TARCK_EXTRAPOLATION_MIRROR_IDEAL

#include "CbmRichTrackExtrapolation.h"

class TClonesArray;
class CbmGlobalTrack;

/**
* \class CbmRichTrackExtrapolationMirrorIdeal
*
* \brief This is the implementation of the TrackExtrapolation from MC points -
* operating on points in the RICH mirror!
* It reads the STS track array, gets the corresponding MC MirrorPoint
* and selects those to be projected to the Rich Photodetector
* points will be stored on mirror surface again
*
* \author Claudia Hoehne
* \date 2006
**/
class CbmRichTrackExtrapolationMirrorIdeal : public CbmRichTrackExtrapolation
{
public:

   /**
   * \brief Default constructor.
   */
   CbmRichTrackExtrapolationMirrorIdeal();

   /**
   * \brief Destructor.
   */
   virtual ~CbmRichTrackExtrapolationMirrorIdeal();

   /**
   * \brief Initialisation.
   */
   virtual void Init();

   /**
   * \brief Finish.
   */
   virtual void Finish();

   /** Method DoExtrapolate.
   ** Task: Read the track array and fill the point array in RICH mirror
   ** pointers to which are given as argument
   **
   *@param rTrackArray  Array of tracks
   *@param rTrackParamArray  Array of FairTrackParam
   *@value Number of tracks extrapolated
   **/
   virtual Int_t DoExtrapolate(
        TClonesArray* gTrackArray,
        Double_t fZ,
        TClonesArray *fTrackParamArray);

private:
  TClonesArray* fRichMirrorPoints;
  TClonesArray* fMcTracks;
  TClonesArray* fSTSArray;
  TClonesArray* fTrackMatchArray;

  Int_t fMinNofStsHits; // number of STS hits required for extrapolated track

  /**
   * \brief Copy constructor.
   */
  CbmRichTrackExtrapolationMirrorIdeal(const CbmRichTrackExtrapolationMirrorIdeal&);

  /**
   * \brief Assignment operator.
   */
  CbmRichTrackExtrapolationMirrorIdeal& operator=(const CbmRichTrackExtrapolationMirrorIdeal&);

  ClassDef(CbmRichTrackExtrapolationMirrorIdeal,1);

};

#endif
