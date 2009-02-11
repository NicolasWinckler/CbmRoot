#ifndef CBMLITRECQA_H_
#define CBMLITRECQA_H_

#include "FairTask.h"
#include "CbmDetectorList.h"

#include "TH2D.h"

#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <string>

class TClonesArray;
class TH1;
class TH1F;
class TList;
class CbmMuchTrack;
class CbmTrdTrack;

class CbmLitRecQa : public FairTask
{
public:

  CbmLitRecQa(
		  Int_t minPoints, 
		  Double_t quota, 
		  DetectorId detId,
		  Int_t iVerbose);
  virtual ~CbmLitRecQa();

  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();
  virtual void Exec(Option_t* opt);

  Int_t GetNormType() {return fNormType;}
  Int_t GetMinPoints() {return fMinPoints;}

  void SetNormType(Int_t normType) { fNormType = normType;}
  void SetMinPoints(Int_t minPoints) { fMinPoints = minPoints;}

private:

  virtual void Finish();

  void ProcessStsTracks();
  void ProcessMuchTracks();
  void ProcessTrdTracks();
  void ProcessMcTracks();
  Bool_t IsMismatch(
		  Int_t mcId);
  void CountClones();
  
  void CreateHistos();

  void DivideHistos(
		  TH1* histo1, 
		  TH1* histo2, 
		  TH1* histo3);
  
  void ZeroGlobalCounters();
  void ZeroEventCounters();
  void IncreaseCounters();
  void CalcEventEff();
  void CalcEffAndRates();
  void PrintEventStatistic();
  void PrintStatistic();
  void WriteToFile();
  
  void FillTrackParams(
		CbmMuchTrack* track, 
		const std::string& type);
  void FillTrackParams(
  		CbmTrdTrack* track,
  		const std::string& type);
  
  DetectorId fDetId;
  
  std::ofstream fNN;
  
  std::multimap<Int_t, Int_t> fMcRecMap;
  std::multimap<Int_t, Int_t> fMcStsMap;

// Pointers to data arrays
  TClonesArray* fMCTracks;   
  TClonesArray* fMCPoints; 
  TClonesArray* fRecTracks;
  TClonesArray* fRecHits; 
  TClonesArray* fRecMatches;
  TClonesArray* fStsTracks; 
  TClonesArray* fStsMatches;

//  Normalisation type.
//   1 - by number of reconstructable in Much MC tracks.
//   2 - by number of STS tracks, which match to reco MC tracks
  Int_t fNormType;

  Int_t fMinPoints; // Minimal number of MCPoints for considered MCTrack
  Double_t fQuota;  // True/all hits for track to be considered reconstructed

  
  // Histograms
  TH1F* fhMomAccAll, *fhMomRecAll, *fhMomEffAll; 
  TH1F* fhMomAccRef, *fhMomRecRef, *fhMomEffRef; 
  TH1F* fhMomAccPrim, *fhMomRecPrim, *fhMomEffPrim; 
  TH1F* fhMomAccSec, *fhMomRecSec, *fhMomEffSec; 
  TH1F* fhMomAccMuons, *fhMomRecMuons, *fhMomEffMuons;
  TH1F* fhMomAccElectrons, *fhMomRecElectrons, *fhMomEffElectrons;
  TH1F* fhNpAccAll, *fhNpRecAll,   *fhNpEffAll;
  TH1F* fhNpAccRef, *fhNpRecRef, *fhNpEffRef; 
  TH1F* fhNpAccPrim, *fhNpRecPrim, *fhNpEffPrim;  
  TH1F* fhNpAccSec, *fhNpRecSec,   *fhNpEffSec;  
  TH1F* fhNpAccMuons, *fhNpRecMuons, *fhNpEffMuons;
  TH1F* fhNpAccElectrons, *fhNpRecElectrons, *fhNpEffElectrons;
  TH1F* fhNhClones, *fhNhGhosts;
  TH1F* fhMomMismatches, *fhNpMismatches, *fhMomEffMismatches, *fhNpEffMismatches;
  TH2D* fhMomNhAccAll, *fhMomNhRecAll, *fhMomNhEffAll;
  
  TH1F* fhChi2True, *fhChi2Ghost;
  TH1F* fhMomTrue, *fhMomGhost;
  TH1F* fhNofHitsTrue, *fhNofHitsGhost;
  TH1F* fhRadialPosTrue, *fhRadialPosGhost;
  TH1F* fhLastPlaneIdTrue, *fhLastPlaneIdGhost;
  TH2D* fhMomChi2True, *fhMomChi2Ghost;
  
  TH1F* fhNofGoodHits, *fhNofBadHits, *fhNofHits;

// List of histograms
  TList* fHistoList;

// Counters
  Int_t fNofMcTracks;
  Int_t fNofRecTracks; 
  Int_t fNofStsTracks;
  Int_t fNofRecTracksNoSts;
  Int_t fNofAccAll;
  Int_t fNofAccRef;
  Int_t fNofAccPrim;
  Int_t fNofAccSec;
  Int_t fNofAccMuons;
  Int_t fNofAccMuonPairs;  
  Int_t fNofAccElectrons;  
  Int_t fNofRecAll;
  Int_t fNofRecRef;
  Int_t fNofRecPrim;
  Int_t fNofRecSec;
  Int_t fNofRecMuons;
  Int_t fNofRecMuonPairs;
  Int_t fNofRecElectrons;  
  Int_t fNofGhosts;
  Int_t fNofClones;
  Int_t fNofMismatches;
  
  // rates
  Double_t fRateMcTracks;
  Double_t fRateStsTracks;
  Double_t fRateRecTracks;
  Double_t fRateRecTracksNoSts;
  Double_t fRateRecAll;
  Double_t fRateRecRef;
  Double_t fRateRecPrim;
  Double_t fRateRecSec;
  Double_t fRateRecMuons;
  Double_t fRateRecElectrons;  
  Double_t fRateAccAll;
  Double_t fRateAccRef;
  Double_t fRateAccPrim;
  Double_t fRateAccSec;
  Double_t fRateAccMuons;
  Double_t fRateAccElectrons;  
  Double_t fRateGhosts;
  Double_t fRateClones;
  Double_t fRateMismatches;
  
  //eff
  Double_t fEffAll;
  Double_t fEffRef;
  Double_t fEffPrim;
  Double_t fEffSec;
  Double_t fEffMuons;
  Double_t fEffElectrons;
  Double_t fEffGhosts;
  Double_t fEffClones;
  Double_t fEffMismatches;
  
  
// Counters per event
  Int_t fEvNofMcTracks;
  Int_t fEvNofRecTracks; 
  Int_t fEvNofStsTracks; 
  Int_t fEvNofRecTracksNoSts; 
  Int_t fEvNofAccAll;
  Int_t fEvNofAccRef;
  Int_t fEvNofAccPrim;
  Int_t fEvNofAccSec;
  Int_t fEvNofAccMuons;
  Int_t fEvNofAccMuonPairs;
  Int_t fEvNofAccElectrons;
  Int_t fEvNofRecAll;
  Int_t fEvNofRecRef;
  Int_t fEvNofRecPrim;
  Int_t fEvNofRecSec;
  Int_t fEvNofRecMuons;
  Int_t fEvNofRecMuonPairs;
  Int_t fEvNofRecElectrons;
  Int_t fEvNofGhosts;
  Int_t fEvNofClones;
  Int_t fEvNofMismatches;
// efficiency per event
  Double_t fEvEffAll;
  Double_t fEvEffRef;
  Double_t fEvEffPrim;
  Double_t fEvEffSec;
  Double_t fEvEffMuons;
  Double_t fEvEffElectrons;
  Double_t fEvEffGhosts;
  Double_t fEvEffClones;
  Double_t fEvEffMismatches;  
  
  Int_t fNEvents; 
  
  //Int_t fNofTrue[20];

  ClassDef(CbmLitRecQa,1);
};

#endif
