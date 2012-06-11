#ifndef CBMTRDQA_H
#define CBMTRDQA_H

#include "FairTask.h"

#include <map>
class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;
class TClonesArray;
class CbmTrdPoint;
class CbmTrdDigi;
class CbmTrdCluster;
class CbmTrdHit;
class TH1I;
class TH2I;
class TCanvas;

class CbmTrdQa : public FairTask {

 public:
  CbmTrdQa();
  CbmTrdQa(const char *name, const char *title="CBM Task", const char *geo="", Double_t triggerThreshold = 1.0e-6);
  virtual ~CbmTrdQa();
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  virtual void Exec(Option_t * option);
  void SaveHistos();
  void CreateLayerView();
  virtual void FinishEvent();
  virtual void FinishTask();
  void Register();

 private:
  CbmTrdQa& operator=(const CbmTrdQa&);
  CbmTrdQa(const CbmTrdQa&);
  TClonesArray *fMCTracks;
  TClonesArray *fPoints; //!
  TClonesArray *fDigis; //!
  TClonesArray *fClusters;//!
  TClonesArray *fHits; //!
  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdGeoHandler *fGeoHandler; //!
  std::map<Int_t, TCanvas*> fLayerMap; //!
  std::map<Int_t, TCanvas*>::iterator fLayerMapIt; //!  

  std::map<Int_t, std::vector<Int_t> > fModulePointMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModulePointMapIt; //!  
  std::map<Int_t, std::vector<Int_t> > fModuleDigiMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModuleDigiMapIt; //!  
  std::map<Int_t, std::vector<Int_t> > fModuleClusterMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModuleClusterMapIt; //!  
  std::map<Int_t, std::vector<Int_t> > fModuleHitMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModuleHitMapIt; //!  

  std::map<Int_t, TH1I*> fModuleGhostMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleGhostMapIt; //!  
  std::map<Int_t, TH1I*> fModuleLostMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleLostMapIt; //!  

  Double_t fTriggerThreshold;
  TString fGeo;

  //histos

  TH2I *fLayerDummy;

  TH1I *fDistanceMcToHit;
  TH1I *fdEdxPionMc;
  TH1I *fdEdxPionHit;
  TH1I *fdEdxElectronMc;
  TH1I *fdEdxElectronHit;
  TH1I *fGhostHitVsR;
  TH1I *fLostPointVsR;

  //LayerView

  ClassDef(CbmTrdQa,1);
};
#endif // 
