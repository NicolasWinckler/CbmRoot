// -------------------------------------------------------------------------
// -----                  CbmMuchMatchTracks header file                -----
// -----                  Created 10/10/07  by A. Lebedev              -----
// -------------------------------------------------------------------------

/** CbmMuchMatchTracks.h
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 *
 * 2009-03-16 E. Kryshen
 * Matching of hits to clusters with several MC points
 **/

#ifndef CBMMUCHMATCHTRACKS_H_
#define CBMMUCHMATCHTRACKS_H_ 1

#include "FairTask.h"

#include <map>

class TClonesArray;

class CbmMuchMatchTracks : public FairTask
{
public:
  CbmMuchMatchTracks();
  virtual ~CbmMuchMatchTracks();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();

private:
  void DigiToTrackMatch(Int_t digiIndex, std::map<Int_t, Int_t> &matchMap);
  TClonesArray* fTracks;
  TClonesArray* fPoints;
  TClonesArray* fHits;
  TClonesArray* fMatches;
  TClonesArray* fDigiMatch;
  TClonesArray* fClusters;

  Int_t fNofHits;
  Int_t fNofTrueHits;
  Int_t fNofWrongHits;
  Int_t fNofFakeHits;

  Int_t    fNEvents;

  ClassDef(CbmMuchMatchTracks,1);
};

#endif
