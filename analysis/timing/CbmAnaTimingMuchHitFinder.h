/** CbmAnaTimingMuchHitFinder.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-01-10
 **
 **/

#ifndef CbmAnaTimingMuchHitFinder_H_
#define CbmAnaTimingMuchHitFinder_H_ 1

#include "FairTask.h"
#include "TString.h"
class TH3D;
class TH1D;
class CbmMuchGeoScheme;
class TClonesArray;
class CbmMCEpoch;

class CbmAnaTimingMuchHitFinder : public FairTask{
public:
  CbmAnaTimingMuchHitFinder();
  CbmAnaTimingMuchHitFinder(const char* name, TString digiFile, TString histoName);
  virtual ~CbmAnaTimingMuchHitFinder();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  virtual void SetParContainers();

private:
  Int_t         fEvent;             //!
  CbmMuchGeoScheme* fGeoScheme;     //!
  TString       fHistoName;
  TString       fDigiFile;         
  TClonesArray* fMuchClusters;      //!
  TClonesArray* fMuchPoints;        //!
  TClonesArray* fMuchDigis;         //!
  TClonesArray* fMuchDigiMatches;   //!
  CbmMCEpoch*   fMcEpoch;           //!
  ClassDef(CbmAnaTimingMuchHitFinder,1);
};

#endif
