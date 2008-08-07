// -------------------------------------------------------------------------
// -----                  CbmLitTrdRecQa header file               -----
// -----                  Created 15/10/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitTrdReckQa.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** Quality check task for CbmTrdFindTracks
 **/


#ifndef CBMLITTRDRECQA_H_
#define CBMLITTRDRECQA_H_

#include "CbmTask.h"

#include "TH2D.h"

#include <set>
#include <map>

class TClonesArray;
class TH1;
class TH1F;
class TList;
class CbmTrdTrack;

class CbmLitTrdRecQa : public CbmTask
{
public:
  CbmLitTrdRecQa();
  CbmLitTrdRecQa(Int_t minPoints, Double_t quota, Int_t iVerbose = 1);
  virtual ~CbmLitTrdRecQa();


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
  void ProcessTrdTracks();
  void ProcessMcTracks();
  Bool_t IsMismatch(Int_t mcId);
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
  
  std::multimap<Int_t, Int_t> fMcTrdMap;
  std::multimap<Int_t, Int_t> fMcStsMap;

// Pointers to data arrays
  TClonesArray* fMCTracks;   
  TClonesArray* fMCPoints; 
  TClonesArray* fTrdTracks;
  TClonesArray* fTrdHits; 
  TClonesArray* fStsTracks; 
  TClonesArray* fTrdMatches;
  TClonesArray* fStsMatches;
//  TClonesArray* fTrdDigiMatches;

//  Normalisation type.
//   1 - by number of reconstructable in Trd MC tracks.
//   2 - by number of STS tracks, which match to reco MC tracks
  Int_t fNormType;

  Int_t fMinPoints; 
  Double_t fQuota;

  
  // Histograms
  TH1F* fhMomAccAll, *fhMomRecAll, *fhMomEffAll; 
  TH1F* fhMomAccRef, *fhMomRecRef, *fhMomEffRef; 
  TH1F* fhMomAccPrim, *fhMomRecPrim, *fhMomEffPrim; 
  TH1F* fhMomAccSec, *fhMomRecSec, *fhMomEffSec; 
  TH1F* fhMomAccMuons, *fhMomRecMuons, *fhMomEffMuons;
  TH1F* fhNpAccAll, *fhNpRecAll,   *fhNpEffAll;
  TH1F* fhNpAccRef, *fhNpRecRef, *fhNpEffRef; 
  TH1F* fhNpAccPrim, *fhNpRecPrim, *fhNpEffPrim;  
  TH1F* fhNpAccSec, *fhNpRecSec,   *fhNpEffSec;  
  TH1F* fhNpAccMuons, *fhNpRecMuons, *fhNpEffMuons;
  TH1F* fhNhClones, *fhNhGhosts;
  TH1F* fhMomMismatches, *fhNpMismatches, *fhMomEffMismatches, *fhNpEffMismatches;
  TH2D* fhMomNhAccAll, *fhMomNhRecAll, *fhMomNhEffAll;


// List of histograms
  TList* fHistoList;

// Counters
  Int_t fNofMcTracks;
  Int_t fNofTrdTracks; 
  Int_t fNofStsTracks; 
  Int_t fNofAccAll;
  Int_t fNofAccRef;
  Int_t fNofAccPrim;
  Int_t fNofAccSec;
  Int_t fNofAccMuons;
  Int_t fNofRecAll;
  Int_t fNofRecRef;
  Int_t fNofRecPrim;
  Int_t fNofRecSec;
  Int_t fNofRecMuons;
  Int_t fNofGhosts;
  Int_t fNofClones;
  Int_t fNofMismatches;
  
  // rates
  Double_t fRateMcTracks;
  Double_t fRateStsTracks;
  Double_t fRateTrdTracks;
  Double_t fRateRecAll;
  Double_t fRateRecRef;
  Double_t fRateRecPrim;
  Double_t fRateRecSec;
  Double_t fRateRecMuons;
  Double_t fRateAccAll;
  Double_t fRateAccRef;
  Double_t fRateAccPrim;
  Double_t fRateAccSec;
  Double_t fRateAccMuons;
  Double_t fRateGhosts;
  Double_t fRateClones;
  Double_t fRateMismatches;
  
  //eff
  Double_t fEffAll;
  Double_t fEffRef;
  Double_t fEffPrim;
  Double_t fEffSec;
  Double_t fEffMuons;
  Double_t fEffGhosts;
  Double_t fEffClones;
  Double_t fEffMismatches;
  
  
// Counters per event
  Int_t fEvNofMcTracks;
  Int_t fEvNofTrdTracks; 
  Int_t fEvNofStsTracks; 
  Int_t fEvNofAccAll;
  Int_t fEvNofAccRef;
  Int_t fEvNofAccPrim;
  Int_t fEvNofAccSec;
  Int_t fEvNofAccMuons;
  Int_t fEvNofRecAll;
  Int_t fEvNofRecRef;
  Int_t fEvNofRecPrim;
  Int_t fEvNofRecSec;
  Int_t fEvNofRecMuons;
  Int_t fEvNofGhosts;
  Int_t fEvNofClones;
  Int_t fEvNofMismatches;
// efficiency per event
  Double_t fEvEffAll;
  Double_t fEvEffRef;
  Double_t fEvEffPrim;
  Double_t fEvEffSec;
  Double_t fEvEffMuons;
  Double_t fEvEffGhosts;
  Double_t fEvEffClones;
  Double_t fEvEffMismatches;  
  
  Int_t fNEvents; 

  ClassDef(CbmLitTrdRecQa,1);
};

#endif
