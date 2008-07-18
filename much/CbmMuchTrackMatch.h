// -------------------------------------------------------------------------
// -----                   CbmMuchTrackMatch header file                -----
// -----                  Created 10/10/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmMuchTrackMatch.h
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 **
 **/


#ifndef CbmMuchTrackMatch_H
#define CbmMuchTrackMatch_H 1


#include "TObject.h"



class CbmMuchTrackMatch : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchTrackMatch();


  /** Standard constructor 
  *@param mcTrackID   Index of matched MCTrack
  *@param nTrue       Good hits from matched MCTrack
  *@param nWrong      Good Hits from other MCTracks
  *@param nFake       Fake Hits
  *@param nTracks     Number of MCTracks with common points
  **/
  CbmMuchTrackMatch(Int_t mcTrackID, Int_t nTrue, Int_t nWrong, 
                    Int_t nFake, Int_t nTracks);


  /** Destructor **/
  virtual ~CbmMuchTrackMatch();


  /** Accessors **/
  Int_t GetMCTrackId()    { return fMCTrackId;    };
  Int_t GetNofTrueHits()  { return fNofTrueHits;  };
  Int_t GetNofWrongHits() { return fNofWrongHits; };
  Int_t GetNofFakeHits()  { return fNofFakeHits;  };
  Int_t GetNofMCTracks()  { return fNofMCTracks;  };


 private:

  /** Best matching CbmMCTrack  **/
  Int_t fMCTrackId;

  /** Number of good hits belonging to the matched MCTrack **/
  Int_t fNofTrueHits;

  /** Number of good hits belonging to other MCTracks **/
  Int_t fNofWrongHits;

  /** Number of fake hits **/
  Int_t fNofFakeHits;

  /** Number of MCTrackx with common points **/
  Int_t fNofMCTracks;


  ClassDef(CbmMuchTrackMatch,1);

};


#endif



