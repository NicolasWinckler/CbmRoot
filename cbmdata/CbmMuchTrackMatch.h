// -------------------------------------------------------------------------
// -----                   CbmMuchTrackMatch header file                -----
// -----                  Created 10/10/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmMuchTrackMatch.h
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 **
 **/

#ifndef CBMMUCHTRACKMATCH_H_
#define CBMMUCHTRACKMATCH_H_ 1

#include "TObject.h"

class CbmMuchTrackMatch : public TObject
{
public:
  CbmMuchTrackMatch();

  /** Standard constructor 
  *@param mcTrackID   Index of matched MCTrack
  *@param nTrue       Good hits from matched MCTrack
  *@param nWrong      Good Hits from other MCTracks
  *@param nFake       Fake Hits
  *@param nTracks     Number of MCTracks with common points
  **/
  CbmMuchTrackMatch(
		  Int_t mcTrackID, 
		  Int_t nTrue, 
		  Int_t nWrong, 
          Int_t nFake, 
          Int_t nTracks);
  virtual ~CbmMuchTrackMatch();

  Int_t GetMCTrackId() const { return fMCTrackId;};
  Int_t GetNofTrueHits() const { return fNofTrueHits;};
  Int_t GetNofWrongHits() const { return fNofWrongHits;};
  Int_t GetNofFakeHits() const { return fNofFakeHits;};
  Int_t GetNofMCTracks() const { return fNofMCTracks;};
  
  void SetMCTrackId(Int_t mctrackId) {fMCTrackId = mctrackId;}
  void SetNofTrueHits(Int_t nofTrueHits) {fNofTrueHits = nofTrueHits;}
  void SetNofWrongHits(Int_t nofWrongHits) {fNofWrongHits = nofWrongHits;}
  void SetNofFakeHits(Int_t nofFakeHits) {fNofFakeHits = nofFakeHits;}
  void SetNofMCTracks(Int_t nofMCTracks) {fNofMCTracks = nofMCTracks;}
  
private:

  Int_t fMCTrackId;
  Int_t fNofTrueHits;
  Int_t fNofWrongHits;
  Int_t fNofFakeHits;
  Int_t fNofMCTracks;

  ClassDef(CbmMuchTrackMatch,1);
};
#endif



