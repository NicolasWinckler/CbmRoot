/******************************************************************************
*  $Id: CbmRichRingMatch.h,v 1.2 2006/02/22 09:16:15 hoehne Exp $
*
*  Class      : CbmRichMatchRings
*  Description: Data structure describing the matching of a reconstructed CbmRichRing
*               with a Monte Carlo CbmMCTrack.
*  Author     : Supriya Das (following CbmStsTrackMatch)
*  E-mail     : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingMatch.h,v $
*  Revision 1.2  2006/02/22 09:16:15  hoehne
*  store also number of MC hits per ring (Simeon Lebedev)
*
*  Revision 1.1  2006/01/23 11:42:05  hoehne
*  initial version: data class describing the marching of reconstructed to MC rings
*
*
*******************************************************************************/

#ifndef CBM_RICH_RING_MATCH_H
#define CBM_RICH_RING_MATCH_H 1


#include "TObject.h"



class CbmRichRingMatch : public TObject
{

 public:

  /** Default constructor **/
  CbmRichRingMatch();


  /** Standard constructor
  *@param mcTrackID   Index of matched MCTrack
  *@param nTrue       Good hits from matched MCTrack
  *@param nWrong      Good Hits from other MCTracks
  *@param nFake       Fake Hits
  *@nMCHits           Number of Hits in MC ring
  *@param nRings      Number of Rings with common MC points
  **/
  CbmRichRingMatch(Int_t mcTrackID, Int_t nTrue, Int_t nWrong,
		   Int_t nFake, Int_t nMCHits, Int_t nRings);


  /** Destructor **/
  virtual ~CbmRichRingMatch();


  /** Accessors **/
  Int_t GetMCTrackID()    { return fMCTrackID;    };
  Int_t GetNofTrueHits()  { return fNofTrueHits;  };
  Int_t GetNofWrongHits() { return fNofWrongHits; };
  Int_t GetNofFakeHits()  { return fNofFakeHits;  };
  Int_t GetNofRings()     { return fNofRings;  };
  Int_t GetNofMCHits()     { return fNofMCHits;  };


 private:

  /** Best matching CbmMCTrack  **/
  Int_t fMCTrackID;

  /** Number of good hits belonging to the matched MCTrack **/
  Int_t fNofTrueHits;

  /** Number of good hits belonging to other MCTracks **/
  Int_t fNofWrongHits;

  /** Number of fake hits **/
  Int_t fNofFakeHits;

  /**Number of hits in MC ring**/
  Int_t fNofMCHits;

  /** Number of Rings with common MC points **/
  Int_t fNofRings;


  ClassDef(CbmRichRingMatch,1);

};

#endif

