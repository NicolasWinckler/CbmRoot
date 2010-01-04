/** CbmAnaDimuonHisto.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-01-02
 **/

#include "CbmAnaDimuonHisto.h"
#include "CbmAnaDimuonCandidate.h"
#include "CbmAnaMuonCandidate.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"

// -----   Default constructor   -------------------------------------------
CbmAnaDimuonHisto::CbmAnaDimuonHisto(){
  
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmAnaDimuonHisto::CbmAnaDimuonHisto(const char* name,TString histoFileName)
:FairTask(name){
  fMmin = 0.7;
  fMmax = 0.9;
  fMbins = 100;
  fHistoFileName=histoFileName;
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmAnaDimuonHisto::~CbmAnaDimuonHisto(){
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmAnaDimuonHisto::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();

  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaDimuonHisto::Init()
{
  // Get and check FairRootManager
  FairRootManager* fManager = FairRootManager::Instance();
  fMuCandidates     = (TClonesArray*) fManager->GetObject("MuCandidates");
  fDimuonCandidates = (TClonesArray*) fManager->GetObject("DimuonCandidates");
  fTree=fManager->GetInTree();
  fEvent=0;
  
  if (!(fMuCandidates && fDimuonCandidates)){
    printf(" %i",fMuCandidates);
    printf(" %i",fDimuonCandidates);
    printf("\n");
    Fatal("Init","One of TCloneArrays not available");
  }  

  fDimuonMmc = new TH1D("hDimuonMmc",";M_{#mu#mu} [GeV/c];Entries",fMbins,fMmin,fMmax);
  fDimuonMrc = new TH1D("hDimuonMrc",";M_{#mu#mu} [GeV/c];Entries",fMbins,fMmin,fMmax);
  fBgdM      = new TH1D("hBgdM"     ,";M_{#mu#mu} [GeV/c];Entries",fMbins,fMmin,fMmax);

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaDimuonHisto::Exec(Option_t* opt){
  Int_t nDimuons       = fDimuonCandidates->GetEntriesFast();
  Int_t nMuons         = fMuCandidates->GetEntriesFast();
  if (fVerbose>-1) printf(" Event: %4i",fEvent);
  if (fVerbose>0) printf(" Dimuons: %2i",nDimuons);
  if (fVerbose>0) printf(" Muons: %2i",nMuons);
  if (fVerbose>-1) printf("\n");

  for (Int_t iDimuon=0;iDimuon<nDimuons;iDimuon++){
    CbmAnaDimuonCandidate* dimuon = (CbmAnaDimuonCandidate*) fDimuonCandidates->At(iDimuon);
    for (Int_t sign=0;sign<2;sign++){
      CbmAnaMuonCandidate* mu = dimuon->GetMu(sign);
//      printf("mu(%2i,%i) planes: ",iDimuon,sign);
      Int_t* points = mu->GetMuchPoints();
      Int_t* hits   = mu->GetMuchHits();
//      for (Int_t iPlane=0;iPlane<NPLANES;iPlane++){
//        if (points[iPlane]>=0) printf("%2i",iPlane); else printf("  ");
//        if (hits[iPlane]>=0)  printf("!",iPlane); else printf(" ");
//      }
//      printf("\n");
    }//sign
//    printf("%f\n",dimuon->GetMomentumMC().M());
    fDimuonMmc->Fill(dimuon->GetMomentumMC().M());
    if (!dimuon->IsReconstructed()) continue;
    fDimuonMrc->Fill(dimuon->GetMomentumRC().M());
  }//dimuon


  Int_t fNoMixedEv=30;
  
  for (Int_t iMuP=0;iMuP<nMuons;iMuP++){
    CbmAnaMuonCandidate* muP = (CbmAnaMuonCandidate*) fMuCandidates->At(iMuP);
    if (muP->GetSign()<0) continue;
    if (!muP->IsReconstructed()) continue;
    TLorentzVector pP = TLorentzVector(*(muP->GetMomentumRC()));
//    printf("%f\n",pP.P());
    for (Int_t ev=fEvent+1; ev<fEvent+1+fNoMixedEv;ev++){
      if (ev<1000) fTree->GetEntry(ev);
      else         fTree->GetEntry(ev-1000);
//      printf("%f\n",pP.P());
      for (Int_t iMuN=0;iMuN<fMuCandidates->GetEntriesFast();iMuN++){
        CbmAnaMuonCandidate* muN = (CbmAnaMuonCandidate*) fMuCandidates->At(iMuN);
        if (muN->GetSign()>0) continue;
        if (!muN->IsReconstructed()) continue;
        TLorentzVector* pN = muN->GetMomentumRC();
        //printf("%f\n",pN->P());
        fBgdM->Fill(((*pN)+pP).M());
      } // negative muons
    } // events
    fTree->GetEntry(fEvent);
  } // positive muons
//  fTree->GetEntry(fEvent);
  fEvent++;
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaDimuonHisto::Finish(){
  TFile* f = new TFile(fHistoFileName,"recreate");
  fDimuonMmc->Write();
  fDimuonMrc->Write();
  fBgdM->Write();
  f->Close();
  
  TCanvas* c1 = new TCanvas("cM","Invariant mass",800,800);
  c1->Divide(2,2);
  c1->cd(1);
  fDimuonMmc->Draw("e");
  
  c1->cd(2);
  fDimuonMrc->Fit("gaus");
  fDimuonMrc->Draw("e");
  printf("%f\n",fDimuonMrc->Integral(30,50));
  
  c1->cd(3);
  fBgdM->SetMinimum(0);
//  fBgdM->Add(fDimuonMrc);
  fBgdM->Draw("e");
  printf("%f\n",fBgdM->Integral(30,50));
}
// -------------------------------------------------------------------------


ClassImp(CbmAnaDimuonHisto);
