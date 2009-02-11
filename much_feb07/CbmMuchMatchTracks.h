// -------------------------------------------------------------------------
// -----                  CbmMuchMatchTracks header file                -----
// -----                  Created 10/10/07  by A. Lebedev              -----
// -------------------------------------------------------------------------

/** CbmMuchMatchTracks.h
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 **
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
  TClonesArray* fTracks; 
  TClonesArray* fPoints; 
  TClonesArray* fHits;
  TClonesArray* fMatches;
  TClonesArray* fDigiMatch;

  Int_t fNofHits;
  Int_t fNofTrueHits;
  Int_t fNofWrongHits;
  Int_t fNofFakeHits;
  
  Int_t    fNEvents;       

  ClassDef(CbmMuchMatchTracks,1);
};

#endif
