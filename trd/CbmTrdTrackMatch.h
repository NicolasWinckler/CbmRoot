// -------------------------------------------------------------------------
// -----                   CbmTrdTrackMatch header file                -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmTrdTrackMatch.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Data structure describing the matching of a reconstructed CbmTrdTrack
 ** with a Monte Carlo CbmMCTrack.
 **/


#ifndef CBMTRDTRACKMATCH_H
#define CBMTRDTRACKMATCH_H 1


#include "TObject.h"



class CbmTrdTrackMatch : public TObject
{

 public:

  /** Default constructor **/
  CbmTrdTrackMatch();


  /** Standard constructor 
  *@param mcTrackID   Index of matched MCTrack
  *@param nTrue       Good hits from matched MCTrack
  *@param nWrong      Good Hits from other MCTracks
  *@param nFake       Fake Hits
  *@param nTracks     Number of MCTracks with common points
  **/
  CbmTrdTrackMatch(Int_t mcTrackID, Int_t nTrue, Int_t nWrong, 
		   Int_t nFake, Int_t nTracks);


  /** Destructor **/
  virtual ~CbmTrdTrackMatch();


  /** Accessors **/
  Int_t GetMCTrackID()    { return fMCTrackID;    };
  Int_t GetNofTrueHits()  { return fNofTrueHits;  };
  Int_t GetNofWrongHits() { return fNofWrongHits; };
  Int_t GetNofFakeHits()  { return fNofFakeHits;  };
  Int_t GetNofMCTracks()  { return fNofMCTracks;  };


 private:

  /** Best matching CbmMCTrack  **/
  Int_t fMCTrackID;

  /** Number of good hits belonging to the matched MCTrack **/
  Int_t fNofTrueHits;

  /** Number of good hits belonging to other MCTracks **/
  Int_t fNofWrongHits;

  /** Number of fake hits **/
  Int_t fNofFakeHits;

  /** Number of MCTrackx with common points **/
  Int_t fNofMCTracks;


  ClassDef(CbmTrdTrackMatch,1);

};


#endif
				 
