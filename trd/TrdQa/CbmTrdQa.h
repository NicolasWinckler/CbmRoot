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
class TProfile;
class TProfile2D;
class TH1F;
class TH1I;
class TH2I;
class TH2F;
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
  void NormalizeHistos();
  void CreateLayerView();
  void GetPadInfos(Int_t moduleId, Double_t x, Double_t y, Int_t &iCol, Int_t &iRow, Double_t &padSizeX, Double_t &padSizeY);
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
  std::map<Int_t, TH2I*> fLayerPointMap; 
  std::map<Int_t, TH2I*>::iterator fLayerPointMapIt; //! 
  std::map<Int_t, TH2I*> fLayerHitMap; 
  std::map<Int_t, TH2I*>::iterator fLayerHitMapIt; //! 

  std::map<Int_t, std::vector<Int_t> > fModulePointMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModulePointMapIt; //!  
  std::map<Int_t, std::vector<Int_t> > fModuleDigiMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModuleDigiMapIt; //!  
  std::map<Int_t, std::vector<Int_t> > fModuleClusterMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModuleClusterMapIt; //!  
  std::map<Int_t, std::vector<Int_t> > fModuleHitMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModuleHitMapIt; //!  

  std::map<Int_t, TH1F*> fModuleGhostMap; //!
  std::map<Int_t, TH1F*>::iterator fModuleGhostMapIt; //!  
  std::map<Int_t, TH1F*> fModuleLostMap; //!
  std::map<Int_t, TH1F*>::iterator fModuleLostMapIt; //!
  std::map<Int_t, TH1F*> fModuleEfficiencyMap; //!
  std::map<Int_t, TH1F*>::iterator fModuleEfficiencyMapIt; //!

  Double_t fTriggerThreshold;
  TString fGeo;

  //histos

  Bool_t fD;
  Bool_t fT;
  Bool_t fP;
  Bool_t fC;
  Bool_t fH;

  TH2I *fLayerDummy;

  TH1I *fDistanceMcToHit;
  TH1I *fDistanceMcToHitAll;

  TH1I *fPositionResolutionShort;
  TH1I *fPositionResolutionLong;

  TH1F *fdEdxPoint;
  TH1F *fdEdxDigi;
  TH1F *fdEdxCluster;
  TH1F *fdEdxHit;

  TH1F *fdEdxPionMc;
  TH1F *fdEdxPionHit;
  TH1F *fdEdxPionGhost;
  TH1F *fdEdxElectronMc;
  TH1F *fdEdxElectronHit;
  TH1F *fdEdxElectronGhost;

  TH1I *fClusterSize;
  TH1I *fPointsPerDigi;
  TH1I *fDigiPerCluster;
  TH1I *fClusterPerHit;

  TProfile *fMultiPointssVsR;
  TProfile *fGhostPointsVsR;
  TProfile *fMultiHitsVsR;
  TProfile *fGhostHitVsR;
  TProfile *fLostPointVsR;
  TProfile *fHitToPointEfficiencyVsR;
  TProfile *fMultiHitsVsAlpha;
  TProfile *fGhostHitVsAlpha;
  TProfile *fLostPointVsAlpha;
  TProfile *fHitToPointEfficiencyVsAlpha;
  TProfile *fPRF_1D;
  TH2I *fPRF_2D;
  //LayerView

  ClassDef(CbmTrdQa,1);
};
#endif // 
