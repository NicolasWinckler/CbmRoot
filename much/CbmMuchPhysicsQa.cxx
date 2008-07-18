// -------------------------------------------------------------------------
// -----                  CbmMuchPhysicsQa source file                 -----
// -----                 Created **/01/08 by A. Kiseleva               -----
// -------------------------------------------------------------------------
#include <iostream>
#include "math.h"

#include "CbmMuchPhysicsQa.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmTrackParam.h"
#include "CbmMCTrack.h"
#include "CbmGeoVector.h"
#include "CbmGeoNode.h"

#include "CbmStsTrack.h"
#include "CbmStsTrackMatch.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmKF.h"

#include "CbmMuchPoint.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchHit.h"
#include "CbmMuchTrack.h"

#include "TCanvas.h"
#include "TH1D.h"
#include "TMath.h"
#include "TLatex.h"
#include "TVirtualPad.h"
#include "TLegend.h"
#include "TLorentzVector.h"

#include "much_histo_style.C"

using std::cout;
using std::endl;
using std::max;

ClassImp(CbmMuchPhysicsQa)

// -------------------------------------------------------------------------
CbmMuchPhysicsQa::CbmMuchPhysicsQa(const char *name, Int_t verbose)
 :CbmTask(name,verbose){
 
 much_histo_style();
  
 fVerbose = verbose;
 
 fQaType = 0;
 
 fMuchPIdType = 0;
 
 fNofEvents = 0;
 
 fNofPi = 0;
 fNofMu = 0;
 fNofK = 0;
 fNofP = 0; 
 
 fNofPiMuch = 0;
 fNofMuMuch = 0;
 fNofKMuch = 0;
 fNofPMuch = 0; 
 fNofEMuch = 0; 
 
 fNofDetectors = 0;
 fNofAbsorbers = 0;
 fZofLastAbsorber = 0.;
 fNofLastDetectors = 0;
 
 strcpy(fQaFileName,"QaHistograms.ps");
 
 MuonMCTrackID = new TH1D("MuonMCTrackID","MuonMCTrackID", 100000, 0, 100000);
 
//**************************** Histograms
	
 fRapidityBg = new TH1D("RapidityBg","Rapidity", 100, 0, 4);
	(fRapidityBg->GetXaxis())->SetTitle("Rapidity");
 (fRapidityBg->GetYaxis())->SetTitle("counts/event");
 
 fRapidityS = (TH1D*)fRapidityBg->Clone("RapidityS");
 fRapidityS->SetTitle("Rapidity");
 fRapidityS->SetLineColor(3);
	
//**************************** 

 fTransMomentumBg = new TH1D("TransMomentumBg", "Transverse momentum (GeV/c)", 100, 0, 5);
 (fTransMomentumBg->GetXaxis())->SetTitle("p_{t} (GeV/c)");
 (fTransMomentumBg->GetYaxis())->SetTitle("counts/event");
 
 fTransMomentumS = (TH1D*)fTransMomentumBg->Clone("TransMomentumS");
 fTransMomentumS->SetTitle("Transverse momentum (GeV/c)"); 
 fTransMomentumS->SetLineColor(3);
	
//**************************** 
 
 fMomentumBg = new TH1D("MomentumBg", "Momentum (GeV/c)", 100, 0, 15);
 (fMomentumBg->GetXaxis())->SetTitle("p (GeV/c)"); 
 (fMomentumBg->GetYaxis())->SetTitle("counts/event");
 
 fMomentumS = (TH1D*)fMomentumBg->Clone("MomentumS");
 fMomentumS->SetTitle("Momentum (GeV/c)"); 
 fMomentumS->SetLineColor(3);
 
//**************************** 

 fNofTracksBg = new TH1D("NofMuchTracksBg","N of tracks",10, -0.5, 9.5);
 (fNofTracksBg->GetXaxis())->SetTitle("N _{tracks}/event");
 (fNofTracksBg->GetYaxis())->SetTitle("events");

 fNofTracksS = (TH1D*)fNofTracksBg->Clone("NofMuchTracksS");
 fNofTracksS->SetTitle("N of tracks");
 fNofTracksS->SetLineColor(3);
 
//**************************** 
 
 fMomentumThetaBg = new TH2D("MomentumThetaBg", "Theta(rad) vs. Momentum(GeV/c)", 50, 1, 15, 50, 0, 1);
 (fMomentumThetaBg->GetXaxis())->SetTitle("p (GeV/c)"); 
 (fMomentumThetaBg->GetYaxis())->SetTitle("#Theta (rad)");
 
 fMomentumThetaS = (TH2D*)fMomentumThetaBg->Clone("MomentumThetaS");
 fMomentumThetaS->SetTitle("Theta(rad) vs. Momentum(GeV/c)"); 
}
// -------------------------------------------------------------------------
CbmMuchPhysicsQa::~CbmMuchPhysicsQa(){
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::SetQaType(Int_t type){
 fQaType = type;
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::SetQaFileName(const  char* QaFileName){
 strcpy(fQaFileName,QaFileName);
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::SetMuchPIdType(Int_t type){
 fMuchPIdType = type;
}
// -------------------------------------------------------------------------
InitStatus CbmMuchPhysicsQa::Init(){

  CbmRootManager* manager = CbmRootManager::Instance();
  strcpy(fInFileName,(manager->GetInFile())->GetName());

//--------------- MC transport file
  if(fQaType == 0)fMCTracks = (TClonesArray*) manager->GetObject("MCTrack");  
  else
  {
   fMCTracks = (TClonesArray*) manager->GetObject("MCTrack"); 
   fMuchPoints = (TClonesArray*) manager->GetObject("MuchPoint");
   
//--------------- MuCh files: hits and reconstructed tracks
   fMuchTracks = (TClonesArray*) manager->GetObject("MuchTrack");
   fMuchHits =(TClonesArray *) manager->GetObject("MuchHit");
   fMuchHitMatch =(TClonesArray *) manager->GetObject("MuchDigiMatch");
   
//--------------- STS file: reconstructed tracks and found primary vertex
   fStsTracks = (TClonesArray *)manager->GetObject("STSTrack");  
   fStsTrackMatch = (TClonesArray*) manager->GetObject("STSTrackMatch");
   fPrimVtx = (CbmVertex *)  manager->GetObject("PrimaryVertex");
  }
//--------------- Get number of absorbers and detectors  
  TObjArray* sens = fGeoPar->GetGeoSensitiveNodes();
  TObjArray* passive = fGeoPar->GetGeoPassiveNodes();
  fNofDetectors = sens->GetEntries();
  
  Int_t I = 0;
  Int_t MotherV = 0;
  CbmGeoNode *node = dynamic_cast<CbmGeoNode*> (passive->At(0)); 
  if(node->GetListOfDaughters()){MotherV = 1;I = 1;}
  
  for(Int_t i=I; i<passive->GetEntries(); i++)
  {
	  CbmGeoNode *node = dynamic_cast<CbmGeoNode*> (passive->At(i)); 
   fZofLastAbsorber = max(ReadGeo(node), fZofLastAbsorber);
  }
  
  for(Int_t i=0; i<fNofDetectors; i++)
  {
   CbmGeoNode *node = dynamic_cast<CbmGeoNode*> (sens->At(i));   
   if(ReadGeo(node) < fZofLastAbsorber)continue;
   fNofLastDetectors++;
  }
    
  fNofAbsorbers = passive->GetEntries()-MotherV; // N of passive volumes = MuCh mother volume + N absorbers

/*  
  cout << "***********************************************************" << endl;
  cout << "********* MuCh system:" << endl;
  cout << "********* N of detectors                     - " << fNofDetectors << endl;
  cout << "********* N of absorbers                     - " << fNofAbsorbers << endl;
  cout << "********* Z of last absorber                 - " << fZofLastAbsorber << endl;
  cout << "********* N of detectors after last absorber - " << fNofLastDetectors << endl;
  cout << "***********************************************************" << endl;
*/  

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::SetParContainers() {

//--------------- Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if (!run)Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if (!db)Fatal("SetParContainers", "No runtime database");

//--------------- Get MUCH geometry parameter container
  fGeoPar = (CbmGeoMuchPar*)db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::Exec(Option_t * option){
  
  if(fQaType == 0)ExecMC();
  else ExecReco();
  fNofEvents++;
  if(fNofEvents% 50  == 0)cout << "------- Event N " << fNofEvents << endl;
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::ExecMC(){
  
 Bool_t verbose = (fVerbose>1);  
 
 Int_t nS = 0;
 Int_t nBg = 0;
 
 for (Int_t i=0;i<fMCTracks->GetEntriesFast();i++)
 {
  CbmMCTrack* mct = (CbmMCTrack*) fMCTracks->At(i);
  if(mct->GetNPoints(kSTS) < 4)continue;
  if(mct->GetNPoints(kMUCH) < fNofDetectors)continue;
  
  FillHistogram(i);
  if(mct->GetMotherId() < 0 && TMath::Abs(mct->GetPdgCode()) == 13)nS++;
  else nBg++;
 }
 if(nBg != 0)fNofTracksBg->Fill(nBg);
 if(nS != 0)fNofTracksS->Fill(nS);
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::ExecReco(){
  
 Bool_t verbose = (fVerbose>1);  
  
 CbmStsKFTrackFitter StsFitter;
 
 Double_t cutChiPrimary = 2.0;
  
 Double_t muMass = 0.1057;
  
 Int_t nS = 0;
 Int_t nBg = 0;
 Int_t nTracks = fMuchTracks->GetEntries();
 for (Int_t i=0; i<nTracks; i++)
 {
  CbmMuchTrack* mt = (CbmMuchTrack*) fMuchTracks->At(i);
  
  if(mt->GetNMissedHits() > 0)continue;
  if(mt->GetNMissedStations() > 0)continue;
  
  if(mt->GetChi2()/mt->GetNDF() > 1.7)continue;
  if(mt->GetNHits() < fNofDetectors)continue;
  
  if(mt->GetStsTrackID() < 0)continue;
  CbmStsTrack* stst = (CbmStsTrack*) fStsTracks->At(mt->GetStsTrackID());
  if(stst->GetNStsHits() < 4)continue;
  CbmStsTrackMatch* ststm = (CbmStsTrackMatch*) fStsTrackMatch->At(mt->GetStsTrackID());
  if(!stst)continue;
  if(!ststm)continue;
//--------------- Calculation of the reconstructed in STS momentum   
  CbmTrackParam extr;
  StsFitter.Extrapolate(stst, fPrimVtx->GetZ(), &extr ); // extr. to vertex 
  if(StsFitter.GetChiToVertex(stst)> cutChiPrimary) continue;
  
  Double_t P = fabs( extr.GetQp() );
  if( P < 1.e-8 ) P = 1.e8;
  else P = 1./P;
  Double_t E = sqrt(P*P + muMass*muMass);
  Double_t Pz = TMath::Sqrt(P*P/( extr.GetTx()*extr.GetTx() + extr.GetTy()*extr.GetTy() + 1));
  Double_t Px = extr.GetTx() * Pz;
  Double_t Py =  extr.GetTy()* Pz;
  TLorentzVector mom;
  mom.SetPxPyPzE(Px,Py,Pz,E);  
//--------------- 
  if(ststm->GetMCTrackId() < 0)continue;
  CbmMCTrack* mct = (CbmMCTrack*) fMCTracks->At(ststm->GetMCTrackId());
  if(!mct)continue;

  FillHistogram(ststm->GetMCTrackId());
   
  if(mct->GetMotherId() < 0 && TMath::Abs(mct->GetPdgCode()) == 13)nS++;
  else nBg++;
   
//-------------- MuCh particle ID: 70% true MuCh hits
  Int_t count = 0;
  Int_t trackID = -10;
  for(Int_t j=0; j<mt->GetNHits(); j++)
  {
   Int_t ih = mt->GetHitIndex(j);
   CbmMuchHit *h = (CbmMuchHit*)fMuchHits->At(ih);
   if(!h)continue;   
   CbmMuchDigiMatch *Digi = (CbmMuchDigiMatch*)fMuchHitMatch->At(h->GetDigi());
   Bool_t digi = kFALSE;
   Int_t iDigi = 0;
   
   for(int ii=0; ii<3; ii++)
   {
    iDigi = Digi->GetRefIndex(ii);
    if(iDigi < 0)continue;
    digi = kTRUE;
    break;
   }   
   if(!digi)continue;
   if(iDigi < 0)continue;
   CbmMuchPoint *pt = (CbmMuchPoint*)fMuchPoints->At(iDigi);
   if(!pt)continue;
   TVector3 pos;
   pt->Position(pos);
   if(fMuchPIdType == 1)
   {
    if(pos.Z() < fZofLastAbsorber)continue;
    if(trackID == -10)trackID = pt->GetTrackID();
    if(trackID == pt->GetTrackID())count++;   
   }
   else MuonMCTrackID->Fill(pt->GetTrackID());
  }
  if(fMuchPIdType == 1){if(count == fNofLastDetectors)ParticleCounter(trackID, kMUCH);}
  else
  {
   for(Int_t it=0; it<100000; it++)
   {
    if(MuonMCTrackID->GetBinContent(it) < 0.65*fNofDetectors)continue;
    ParticleCounter(it-1, kMUCH);
    break;
   }   
   MuonMCTrackID->Reset();  
  }
//--------------- 
 }
 if(nBg != 0)fNofTracksBg->Fill(nBg);
 if(nS != 0)fNofTracksS->Fill(nS);
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::Finish(){

//-------------- all histograms - entries per event!

 Double_t scale = 1./((Double_t)fNofEvents);

 fRapidityS->Scale(scale);
 fTransMomentumS->Scale(scale);
 fMomentumS->Scale(scale);
 fMomentumThetaS->Scale(scale);
 
 fRapidityBg->Scale(scale);
 fTransMomentumBg->Scale(scale);
 fMomentumBg->Scale(scale);
 fMomentumThetaBg->Scale(scale);
 
 (fNofTracksBg->GetYaxis())->SetRangeUser(0.1*scale,fNofEvents);
 (fNofTracksS->GetYaxis())->SetRangeUser(0.1*scale,fNofEvents);
 
 (fRapidityBg->GetYaxis())->SetRangeUser(0.1*scale,1.);
 (fTransMomentumBg->GetYaxis())->SetRangeUser(0.1*scale,1.);
 (fMomentumBg->GetYaxis())->SetRangeUser(0.1*scale,1.);  
 
 (fRapidityS->GetYaxis())->SetRangeUser(0.1*scale,1.);
 (fTransMomentumS->GetYaxis())->SetRangeUser(0.1*scale,1.);
 (fMomentumS->GetYaxis())->SetRangeUser(0.1*scale,1.); 
 
  char tt[256]; 
  
//-------------- statistics: names of files, parameters of tracks, ID of particles etc.  
  
  TCanvas *MC_QA;
  if(fQaType == 0)MC_QA = new TCanvas("MC_QA", "accepted hard muon tracks",81,88,1192,929);
  else MC_QA = new TCanvas("MC_QA", "reconstructed hard muon tracks",81,88,1192,929);
  
  MC_QA->Range(0,0,1,1);
  
  TPad *MC_QA_1 = new TPad("MC_QA_1", "MC_QA_1",0.01,0.7,0.99,0.99);
  MC_QA_1->Draw();
  MC_QA_1->cd();
  MC_QA_1->Range(0,0,1,1);
  TLatex * tex1 = new TLatex();
  tex1->SetTextSize(0.08);  
  sprintf(tt,"Muon system: %d detector layers + %d absorbers", fNofDetectors, fNofAbsorbers);
  tex1->DrawLatex(0.03,0.9,tt);
  if(fQaType == 0)sprintf(tt,"Accepted hard tracks: #geq 4 STS + %d MuCh", fNofDetectors);
  else sprintf(tt,"Reconstructed hard tracks: #geq 4 STS + %d MuCh", fNofDetectors);
  tex1->DrawLatex(0.03,0.75,tt);
  sprintf(tt,"N events %d",fNofEvents);
  tex1->DrawLatex(0.03,0.6,tt);   
   
  TLegend *lg = new TLegend(0.64,0.,1.,0.3);
  lg->SetFillColor(10);   
  lg->SetBorderSize(0);
  lg->SetTextSize(0.075);	
  lg->SetHeader("Tracks:");
  lg->AddEntry(fRapidityBg, "background","l");
  lg->AddEntry(fRapidityS, "signal","l");
  lg->Draw();
  
  tex1->DrawLatex(0.65,0.9,"Background particles/event:");
  if(fQaType == 0)tex1->DrawLatex(0.65,0.8,"MC ID");
  else tex1->DrawLatex(0.65,0.8,"STS ID");
  sprintf(tt,"#mu #topfoo  %3.3f", fNofMu*scale);
  tex1->DrawLatex(0.65,0.7,tt);
  sprintf(tt,"#pi #topfoo  %3.3f", fNofPi*scale);
  tex1->DrawLatex(0.65,0.6,tt);
  sprintf(tt,"K #topfoo %3.3f", fNofK*scale);
  tex1->DrawLatex(0.65,0.5,tt);
  sprintf(tt,"p  #topfoo %3.3f", fNofP*scale);
  tex1->DrawLatex(0.65,0.4,tt);
  
  if(fQaType == 1)
  {
   if(fMuchPIdType == 0)tex1->DrawLatex(0.8,0.8,"MuCh ID (70% true hits)");
   else {sprintf(tt,"MuCh ID (%d last detectors)",fNofLastDetectors);
         tex1->DrawLatex(0.8,0.8,tt);}
   sprintf(tt,"#mu #topfoo  %3.3f", fNofMuMuch*scale);
   tex1->DrawLatex(0.8,0.7,tt);
   sprintf(tt,"#pi #topfoo  %3.3f", fNofPiMuch*scale);
   tex1->DrawLatex(0.8,0.6,tt);
   sprintf(tt,"K #topfoo %3.3f", fNofKMuch*scale);
   tex1->DrawLatex(0.8,0.5,tt);
   sprintf(tt,"p  #topfoo %3.3f", fNofPMuch*scale);
   tex1->DrawLatex(0.8,0.4,tt);  
   sprintf(tt,"e  #topfoo %3.3f", fNofEMuch*scale);
   tex1->DrawLatex(0.8,0.3,tt);  
  } 
  
  tex1->SetTextSize(0.055);  
  tex1->DrawLatex(0.025,0.4,"Transport file:");
  sprintf(tt,"%s", fInFileName);
  tex1->DrawLatex(0.025,0.3,tt);
  tex1->SetTextSize(0.08);
  sprintf(tt,"Qa file: %s", fQaFileName);
  tex1->DrawLatex(0.025,0.05,tt);
   
  MC_QA_1->Modified();
  MC_QA->cd();    
  
  TPad *MC_QA_2 = new TPad("MC_QA_2", "MC_QA_2",0.008,0.01,0.99,0.7);
  MC_QA_2->Draw();
  MC_QA_2->cd();
  MC_QA_2->Divide(3,2);
  
//-------------- Rapidity
  
  TVirtualPad *MC_QA_3 = MC_QA_2->cd(1);
  MC_QA_3->SetFillColor(13);
  MC_QA_3->Divide(2,1);
  
  MC_QA_3->cd(1);gPad->SetLogy(); gPad->SetFillColor(0);
  DrawHistograms(fRapidityBg,fRapidityS);
  
  TLatex * tex2 = new TLatex();
  tex2->SetTextSize(0.08); 
  
  MC_QA_3->cd(2);  gPad->SetFillColor(0);
  tex2->DrawLatex(0.06,0.9,"Rapidity");
  tex2->DrawLatex(0.06,0.8,"distribution:");
  DrawStatText(fRapidityBg,fRapidityS);
  
//-------------- Momentum
  
  TVirtualPad *MC_QA_4 = MC_QA_2->cd(2);
  MC_QA_4->SetFillColor(13);
  MC_QA_4->Divide(2,1); 
    
  MC_QA_4->cd(1); gPad->SetLogy();	gPad->SetFillColor(0);
  DrawHistograms(fMomentumBg,fMomentumS);
  
  MC_QA_4->cd(2); gPad->SetFillColor(0);
  tex2->DrawLatex(0.06,0.9,"Momentum");
  tex2->DrawLatex(0.06,0.8,"distribution:");
  DrawStatText(fMomentumBg, fMomentumS);
  
//-------------- Transverse momentum
  
  TVirtualPad *MC_QA_5 = MC_QA_2->cd(3);
  MC_QA_5->SetFillColor(13);
  MC_QA_5->Divide(2,1); 
  
  MC_QA_5->cd(1); gPad->SetLogy();	gPad->SetFillColor(0);
  DrawHistograms(fTransMomentumBg,fTransMomentumS);
  
  MC_QA_5->cd(2);gPad->SetFillColor(0);
  tex2->DrawLatex(0.06,0.9,"Transverse momentum");
  tex2->DrawLatex(0.06,0.8,"distribution:");
  DrawStatText(fTransMomentumBg,fTransMomentumS);
  
//-------------- Theta vs. Momentum: Background
  
  TVirtualPad *MC_QA_6 = MC_QA_2->cd(4);
  MC_QA_6->SetFillColor(13);
  MC_QA_6->Divide(2,1); 
    
  MC_QA_6->cd(1); gPad->SetFillColor(0);
  if(fMomentumThetaBg->GetEntries() == 0)tex2->DrawLatex(0.06,0.9,"No background");
  else{fMomentumThetaBg->SetStats(kFALSE); fMomentumThetaBg->Draw("colz");} 
  
  MC_QA_6->cd(2); gPad->SetFillColor(0);
  if(fMomentumThetaBg->GetEntries() == 0)tex2->DrawLatex(0.06,0.9,"No background");
  else 
  {
   tex2->DrawLatex(0.06,0.9,"Background");
   DrawStatText(fMomentumThetaBg);
  }
  
//-------------- Theta vs. Momentum: Signal

  TVirtualPad *MC_QA_7 = MC_QA_2->cd(5);
  MC_QA_7->SetFillColor(13);
  MC_QA_7->Divide(2,1); 
    
  MC_QA_7->cd(1); gPad->SetFillColor(0);
  if(fMomentumThetaS->GetEntries() == 0)tex2->DrawLatex(0.06,0.9,"No signal");
  else{fMomentumThetaS->SetStats(kFALSE); fMomentumThetaS->Draw("colz");}
  
  MC_QA_7->cd(2); gPad->SetFillColor(0);
  if(fMomentumThetaS->GetEntries() == 0)tex2->DrawLatex(0.06,0.9,"No signal");
  else 
  {
   tex2->DrawLatex(0.06,0.9,"Signal");
   DrawStatText(fMomentumThetaS);
  }    
  
//-------------- Number of tracks
  
  TVirtualPad *MC_QA_8 = MC_QA_2->cd(6);
  MC_QA_8->SetFillColor(13);
  MC_QA_8->Divide(2,1); 
  
  MC_QA_8->cd(1); gPad->SetLogy();	gPad->SetFillColor(0);
  DrawHistograms(fNofTracksBg,fNofTracksS);
  
  MC_QA_8->cd(2);gPad->SetFillColor(0);
  tex2->DrawLatex(0.06,0.9,"Number of tracks");
  tex2->DrawLatex(0.06,0.8,"in one event:");
  DrawStatText(fNofTracksBg,fNofTracksS);
  tex2->SetTextColor(2);
  tex2->DrawLatex(0.06,0.14,"NofTacks/event:"); 
  Double_t stats[4]; 
  fNofTracksBg->GetStats(stats);
  sprintf(tt,"             Bg        = %3.3f",	stats[2]*scale);  
  fNofTracksS->GetStats(stats);
  tex2->DrawLatex(0.06,0.09,tt);  
  sprintf(tt,"             Signal  = %3.3f",stats[2]*scale);  
  tex2->DrawLatex(0.06,0.04,tt);  
  
//-------------- save all histograms into PS file
  
  MC_QA->cd();
  MC_QA->SaveAs(fQaFileName);  
}
// -------------------------------------------------------------------------
Double_t CbmMuchPhysicsQa::ReadGeo(CbmGeoNode *node){
  
  TString name = node->getName();
  TString Sname = node->getShapePointer()->GetName();	
  CbmGeoVector nodeV = node->getLabTransform()->getTranslation(); //in cm
  CbmGeoVector centerV = node->getCenterPosition().getTranslation();
  TArrayD *P = node->getParameters();
  Int_t NP = node->getShapePointer()->getNumParam();
 
  Double_t Znode  = nodeV.Z() + centerV.Z();
 
  if(Sname == "SPHE")Znode += 0.5*( P->At(0) + P->At(1) ); // inner & outer radius
  else if(Sname == "PCON")
  {
   Int_t Nz = (NP-3)/3;
   double Z = -100000, z = 100000;
   for( Int_t i=0; i<Nz; i++ )
	  {
	   double z1 = P->At(3+i*3+0);
	   if( z1<z ) z = z1;
	   if( z1>Z ) Z = z1;		  
	  }
   Znode += (z+Z)/2.;
  }  
  else if(Sname=="PGON")
  {
   Int_t Nz = (NP-4)/3;
   double Z = -100000, z = 100000;
   for( Int_t i=0; i<Nz; i++ )
	  {
	   double z1 = P->At(4+i*3+0);
	   if( z1<z ) z = z1;
	   if( z1>Z ) Z = z1;		  
	  }      
   Znode += (z+Z)/2.;
  } 
   
  return Znode;
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::FillHistogram(Int_t i){
 
 CbmMCTrack* mct = (CbmMCTrack*)fMCTracks->At(i);
 TLorentzVector mom;
 mct->Get4Momentum(mom);
 if(mct->GetMotherId() < 0 && TMath::Abs(mct->GetPdgCode()) == 13)
 {
  fRapidityS->Fill(mom.Rapidity());
  fTransMomentumS->Fill(mom.Pt());
  fMomentumS->Fill(mom.P());
  fMomentumThetaS->Fill(mom.P(),mom.Theta());
 }
 else
 {
  fRapidityBg->Fill(mom.Rapidity());
  fTransMomentumBg->Fill(mom.Pt());
  fMomentumBg->Fill(mom.P());
  fMomentumThetaBg->Fill(mom.P(),mom.Theta());  
  ParticleCounter(i, kSTS);
 }
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::DrawStatText(TH1D *h1, TH1D *h2){
  TLatex * tex = new TLatex();
  char tt[256]; 
  tex->SetTextSize(0.08); 
  sprintf(tt,"Bg:       Tracks  = %d",(Int_t)h1->GetEntries());
  tex->DrawLatex(0.06,0.7,tt);
  sprintf(tt,"             Mean    = %3.3f",h1->GetMean());
  tex->DrawLatex(0.06,0.6,tt);
  sprintf(tt,"             RMS     = %3.3f",h1->GetRMS());
  tex->DrawLatex(0.06,0.5,tt);
  sprintf(tt,"Signal: Tracks  = %d",(Int_t)h2->GetEntries());
  tex->DrawLatex(0.06,0.4,tt);
  sprintf(tt,"             Mean    = %3.3f",h2->GetMean());
  tex->DrawLatex(0.06,0.3,tt);
  sprintf(tt,"             RMS     = %3.3f",h2->GetRMS());
  tex->DrawLatex(0.06,0.2,tt);  
 }
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::DrawStatText(TH2D *h){
  TLatex * tex = new TLatex();
  char tt[256]; 
  tex->SetTextSize(0.08); 
  tex->DrawLatex(0.06,0.8,"Theta vs. Momentum");  
  tex->DrawLatex(0.06,0.7,"distribution:");
  sprintf(tt,"             Tracks  = %d",(Int_t)h->GetEntries());
  tex->DrawLatex(0.06,0.6,tt);
  sprintf(tt,"             Mean    = %3.3f",h->GetMean());
  tex->DrawLatex(0.06,0.5,tt);
  sprintf(tt,"             RMS     = %3.3f",h->GetRMS());
  tex->DrawLatex(0.06,0.4,tt);
 } 
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::DrawHistograms(TH1D *h1, TH1D *h2){
if(h2->GetEntries() < h1->GetEntries())
  {
   h1->SetStats(kFALSE); h1->Draw(); if(h2->GetEntries() != 0)h2->Draw("same");
  }
  else
  {
   h2->SetStats(kFALSE); h2->Draw(); h1->Draw("same");
  }
}
// -------------------------------------------------------------------------
void CbmMuchPhysicsQa::ParticleCounter(Int_t i, DetectorId detId){

 CbmMCTrack* mct = (CbmMCTrack*)fMCTracks->At(i);   
 if(detId == kMUCH)  
 {
  if(TMath::Abs(mct->GetPdgCode()) == 13){if(mct->GetMotherId() > 0)fNofMuMuch++;}
  else if(TMath::Abs(mct->GetPdgCode()) == 211)fNofPiMuch++;
  else if(TMath::Abs(mct->GetPdgCode()) == 321)fNofKMuch++;
  else if(TMath::Abs(mct->GetPdgCode()) == 2212)fNofPMuch++;
  else if(TMath::Abs(mct->GetPdgCode()) == 11)fNofEMuch++;
 }
 else
 {
  if(TMath::Abs(mct->GetPdgCode()) == 13){if(mct->GetMotherId() > 0)fNofMu++;}
  else if(TMath::Abs(mct->GetPdgCode()) == 211)fNofPi++;
  else if(TMath::Abs(mct->GetPdgCode()) == 321)fNofK++;
  else if(TMath::Abs(mct->GetPdgCode()) == 2212)fNofP++;
 }
}
// -------------------------------------------------------------------------


   
   
