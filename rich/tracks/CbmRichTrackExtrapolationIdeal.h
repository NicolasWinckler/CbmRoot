/**
* \file CbmRichTrackExtrapolationIdeal.h
*
* \brief This is the implementation of the TrackExtrapolation from MC points.
* It reads the STS track array, gets the corresponding MC RefPlanePoint
* and selects those to be projected to the Rich Photodetector.
*
* \author Claudia Hoehne
* \date 2006
**/

#ifndef CBM_RICH_TARCK_EXTRAPOLATION_IDEAL
#define CBM_RICH_TRACK_EXTRAPOLATION_IDEAL

#include "CbmRichTrackExtrapolation.h"

class TClonesArray;
class CbmGlobalTrack;

/**
* \class CbmRichTrackExtrapolationIdeal
*
* \brief "TrackExtrapolation" from MC points. It reads the PointArray with ImPlanePoints
* from MC and selects those to be projected to the Rich Photodetector.
*
* \author Claudia Hoehne
* \date 2006
**/
class CbmRichTrackExtrapolationIdeal : public CbmRichTrackExtrapolation
{
public:

   /**
   * \brief Default constructor.
   */
   CbmRichTrackExtrapolationIdeal();

   /**
   * \brief Destructor.
   */
   virtual ~CbmRichTrackExtrapolationIdeal();

   /**
   * \brief Initialisation.
   */
   virtual void Init();

   /**
   * \brief Finish.
   */
   virtual void Finish();

   /** Method DoExtrapolate.
   ** Task: Read the track array and fill the point array at given z-Plane in RICH detector
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
   TClonesArray* fRefPlanePoints;
   TClonesArray* fMcTracks;
   TClonesArray* fStsTracks;
   TClonesArray* fStsTrackMatches;

   Int_t fMinNofStsHits; // number of STS hits required for extrapolated track

   /**
   * \brief Copy constructor.
   */
   CbmRichTrackExtrapolationIdeal(const CbmRichTrackExtrapolationIdeal&);

   /**
   * \brief Assignment operator.
   */
   CbmRichTrackExtrapolationIdeal& operator=(const CbmRichTrackExtrapolationIdeal&);

   ClassDef(CbmRichTrackExtrapolationIdeal,1);
};

#endif
