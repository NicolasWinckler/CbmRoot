/** CbmAnaTimingMuchDigitizer.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-01-06
 **
 **/

#ifndef CbmAnaTimingMuchDigitizer_H_
#define CbmAnaTimingMuchDigitizer_H_ 1

#include "FairTask.h"
#include "TString.h"
class TH3D;
class TH1D;
class CbmMuchGeoScheme;
class TClonesArray;
class CbmMCEpoch;

class CbmAnaTimingMuchDigitizer : public FairTask{
public:
  CbmAnaTimingMuchDigitizer();
  CbmAnaTimingMuchDigitizer(const char* name, TString digiFile, TString histoName);
  virtual ~CbmAnaTimingMuchDigitizer();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  virtual void SetParContainers();

private:
  Int_t         fEvent;             //!
  CbmMuchGeoScheme* fGeoScheme;     //!
  TString       fHistoName;
  TString       fDigiFile;         
  TClonesArray* fMuchPoints;        //!
  TClonesArray* fMuchDigis;         //!
  TClonesArray* fMuchDigiMatches;   //!
  CbmMCEpoch*   fMcEpoch;           //!
  TH3D* fhPointXYT;        //!
  TH3D* fhSectorPointXYT;  //!
  TH3D* fhDigiXYT;         //!
  TH3D* fhSectorDigiXYT;   //!
  TH1D* fhChannelT;        //!
  TH1D* fhChannelHits;     //!
  TH1D* fhChannelHitDist;  //!
  TH1D* fhPointT;          //!
  TH1D* fhModuleT;         //!
  ClassDef(CbmAnaTimingMuchDigitizer,1);
};

#endif
