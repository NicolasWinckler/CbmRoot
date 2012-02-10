/** CbmMuchDigitizeAdvancedGem.cxx
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 01.05.11
 *@version 2.0
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 19.03.07
 *@version 1.0
 **
 ** CBM task class for digitizing MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchDigi out of CbmMuchPoint.
 **/

// Includes from MUCH
#include "CbmMuchDigitizeAdvancedGem.h"
#include "CbmMuchPoint.h"
#include "CbmMuchSector.h"
#include "CbmMuchStation.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchModuleSector.h"
#include "CbmMuchPad.h"
#include "CbmMuchRadialSector.h"
#include "CbmMuchRadialPad.h"

// Includes from base
#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"
#include "CbmMCEpoch.h"

// Includes from ROOT
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TFile.h"
#include "TRandom.h"
#include "TChain.h"

#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::fixed;
using std::left;
using std::right;
using std::setprecision;
using std::map;
using std::pair;

void CbmMuchDigitizeAdvancedGem::SetParContainers() {}
void CbmMuchDigitizeAdvancedGem::FinishTask() {}
InitStatus CbmMuchDigitizeAdvancedGem::ReInit() { return kSUCCESS; }

// -----   Default constructor   ------------------------------------------
CbmMuchDigitizeAdvancedGem::CbmMuchDigitizeAdvancedGem() :
  FairTask("MuchDigitize", 1),
  fDigiFile(""),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fDTime(3),
  fNADCChannels(256),
  fQMax(500000),
  fMeanNoise(0),//(1500),
  fMeanGasGain(1e4),
  fDeadPadsFrac(0),
  fGeoScheme(CbmMuchGeoScheme::Instance()),
  fEpoch(0),
  fMcChain(NULL),
  fDeadTime(400),
  fDriftVelocity(100),
  fPeakingTime(20),
  fRemainderTime(40),
  fTimeBinWidth(1),
  fChainEventId(0)
{
  SetQThreshold(3);
  SetSpotRadius();
  Double_t driftVolumeWidth = 0.4; // cm // TODO
  fTotalDriftTime = driftVolumeWidth/fDriftVelocity*10000; // [ns];

    Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor    -----------------------------------------
CbmMuchDigitizeAdvancedGem::CbmMuchDigitizeAdvancedGem(const char* name, const char* digiFileName, Int_t iVerbose) :
  FairTask(name, iVerbose),
  fDigiFile(digiFileName),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fDTime(3),
  fNADCChannels(256),
  fQMax(500000),
  fMeanNoise(0),//(1500),
  fMeanGasGain(1e4),
  fDeadPadsFrac(0),
  fGeoScheme(CbmMuchGeoScheme::Instance()),
  fEpoch(0),
  fMcChain(NULL),
  fDeadTime(400),
  fDriftVelocity(100),
  fPeakingTime(20),
  fRemainderTime(40),
  fTimeBinWidth(1),
  fChainEventId(0)
{
  SetQThreshold(3);
  SetSpotRadius();
  Double_t driftVolumeWidth = 0.4; // cm // TODO
  fTotalDriftTime = driftVolumeWidth/fDriftVelocity*10000; // [ns];
  Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchDigitizeAdvancedGem::~CbmMuchDigitizeAdvancedGem() {
  if (fDigis) {
    fDigis->Delete();
    delete fDigis;
  }
  if (fDigiMatches) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }
  Reset();
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmMuchDigitizeAdvancedGem::Reset() {
  fNFailed = fNOutside = fNMulti = 0;
  if (fDigis) fDigis->Clear();
  if (fDigiMatches) fDigiMatches->Delete(); // Delete because of memory leaks
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchDigitizeAdvancedGem::Init() {
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");

  // Initialize GeoScheme
  TFile* oldfile=gFile;
  TFile* file=new TFile(fDigiFile);
  TObjArray* stations = (TObjArray*) file->Get("stations");
  file->Close();
  file->Delete();
  gFile=oldfile;
  fGeoScheme->Init(stations);

  // Get input array of MuchPoints
  fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");

  // Get input array of MC tracks
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

  if (fEpoch) {
    fMcEpoch = (CbmMCEpoch*) ioman->GetObject("MCEpoch.");
    if (!fMcEpoch) {
      Fatal("Init","No MC epoch branch found in file");
      return kFATAL;
    }
    // Check for the chain of MC files
    if (!fMcChain) {
      Fatal("Init","MC chain pointer is NULL"); 
      return kFATAL;
    }
    
    if (!fMcChain->GetEntries()) {
      Fatal("Init","No entries in the MC chain");
      return kFATAL;
    }
    
    fMCTracks = new TClonesArray("CbmMCTrack");
    fMcChain->SetBranchAddress("MCTrack",&fMCTracks);
    fMcChain->GetEntry(fChainEventId);
  }
  
  // Register output array MuchDigi
  fDigis = new TClonesArray("CbmMuchDigi", 1000);
  ioman->Register("MuchDigi", "Digital response in MUCH", fDigis, kTRUE);

  // Register output array MuchDigiMatches
  fDigiMatches = new TClonesArray("CbmMuchDigiMatch", 1000);
  ioman->Register("MuchDigiMatch", "Digi Match in MUCH", fDigiMatches, kTRUE);

  fEvent = 0;

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmMuchDigitizeAdvancedGem::Exec(Option_t* opt) {
  // Reset all eventwise counters
  fTimer.Start();
  Reset();
  cout << endl;
  cout << "-I- " << fName << "   :   Event " << ++fEvent << endl;

  if (fEpoch) fPoints = fMcEpoch->GetPoints(kMUCH);
  
  Int_t notUsable = 0; // DEBUG: counter for not usable points

  // Loop over all MuchPoints
  Int_t nPoints = fPoints->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(iPoint);

    // Take only usable points
    if (!point || !point->IsUsable()) {
      notUsable++;
      continue;
    }

    // Get the module the point is in
    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(point->GetDetectorID());
    if (!module) {
      fNFailed++;
      continue;
    }
    if (!module->GetNSectors()) {
      fNOutside++;
      continue;
    }
    
    ExecAdvanced(point, iPoint);
  } // MuchPoint loop

  // Add remaining digis
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules();
  for (Int_t im=0;im<modules.size();im++){
    CbmMuchModule* module = modules[im];
    if(module->GetDetectorType()==1){
      vector<CbmMuchPad*> pads = ((CbmMuchModuleGem*) modules[im])->GetPads();
      for (Int_t ip=0;ip<pads.size();ip++) AddDigi(pads[ip]);
    }
    if(module->GetDetectorType()==3){
      vector<CbmMuchRadialPad*> pads = ((CbmMuchModuleSector*) modules[im])->GetPads();
      for (Int_t ip=0;ip<pads.size();ip++) AddDigi(pads[ip]);
    }
  }

  // Screen output
  fTimer.Stop();

  if (!fVerbose)
    cout << "+ ";
  else
    cout << "-I- ";
  cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
  << fixed << right << fTimer.RealTime() << " s, points " << nPoints
  << ", failed " << fNFailed << ", not usable " << notUsable
  << ", outside " << fNOutside << ", multihits " << fNMulti
  << ", digis " << fDigis->GetEntriesFast() << endl;
}
// -------------------------------------------------------------------------


// ------- Private method ExecAdvanced -------------------------------------
Bool_t CbmMuchDigitizeAdvancedGem::ExecAdvanced(CbmMuchPoint* point, Int_t iPoint) {
  TVector3 v1,v2,dv;
  point->PositionIn(v1);
  point->PositionOut(v2);
  dv = v2-v1;

  Int_t detectorId = point->GetDetectorID();
  CbmMuchModule* module = fGeoScheme->GetModuleByDetId(detectorId);

  Int_t nElectrons = Int_t(GetNPrimaryElectronsPerCm(point)*dv.Mag());
  if (nElectrons<0) return kFALSE;
  
  Double_t time = -1;
  while(time < 0) time = point->GetTime() + gRandom->Gaus(0, fDTime);
  
  if (module->GetDetectorType()==1) {
    CbmMuchModuleGem* module1 = (CbmMuchModuleGem*) module;
    map<CbmMuchSector*,Int_t> firedSectors;
    for (Int_t i=0;i<nElectrons;i++) {
      Double_t aL = gRandom->Rndm();
      TVector3 ve = v1 + dv*aL;
      UInt_t ne   = GasGain();
      Double_t x  = ve.X();
      Double_t y  = ve.Y();
      Double_t x1 = x-fSpotRadius;
      Double_t x2 = x+fSpotRadius;
      Double_t y1 = y-fSpotRadius;
      Double_t y2 = y+fSpotRadius;
      Double_t s  = 4*fSpotRadius*fSpotRadius;
      firedSectors[module1->GetSector(x1,y1)]=0;
      firedSectors[module1->GetSector(x1,y2)]=0;
      firedSectors[module1->GetSector(x2,y1)]=0;
      firedSectors[module1->GetSector(x2,y2)]=0;
      for (map<CbmMuchSector*, Int_t>::iterator it = firedSectors.begin(); it!= firedSectors.end(); it++) {
        CbmMuchSector* sector  = (*it).first;
        if (!sector) continue;
        for (Int_t iPad=0;iPad<sector->GetNChannels();iPad++){
          CbmMuchPad* pad = sector->GetPad(iPad);
          Double_t xp0 = pad->GetX0();
          Double_t xpd = pad->GetLx()/2.;
          Double_t xp1 = xp0-xpd;
          Double_t xp2 = xp0+xpd;
          if (x1>xp2 || x2<xp1) continue;
          Double_t yp0 = pad->GetY0();
          Double_t ypd = pad->GetLy()/2.;
          Double_t yp1 = yp0-ypd;
          Double_t yp2 = yp0+ypd;
          if (y1>yp2 || y2<yp1) continue;
          Double_t lx = x1>xp1 ? (x2<xp2 ? x2-x1 : xp2-x1) : x2-xp1;
          Double_t ly = y1>yp1 ? (y2<yp2 ? y2-y1 : yp2-y1) : y2-yp1;
          AddCharge(pad,UInt_t(ne*lx*ly/s),iPoint,time,aL);
        }
      } // loop fired sectors
      firedSectors.clear();
    }
  }
  
  if (module->GetDetectorType()==3) {
    CbmMuchModuleSector* module3 = (CbmMuchModuleSector*) module;
    Double_t rMin = module3->GetSectorByIndex(0)->GetR1();
    Double_t rMax = module3->GetSectorByIndex(module->GetNSectors()-1)->GetR2();

    for (Int_t i=0;i<nElectrons;i++) {
      Double_t aL   = gRandom->Rndm();
      TVector3 ve   = v1 + dv*aL;
      UInt_t ne     = GasGain();
      Double_t r    = ve.Perp();
      Double_t phi  = ve.Phi();
      Double_t r1   = r-fSpotRadius;
      Double_t r2   = r+fSpotRadius;
      Double_t phi1 = phi-fSpotRadius/r;
      Double_t phi2 = phi+fSpotRadius/r;
      if (r1<rMin && r2>rMin) {
        CbmMuchRadialSector* s = module3->GetSectorByIndex(0);
        AddCharge(s,UInt_t(ne*(r2-rMin)/(r2-r1)),iPoint,time,aL,phi1,phi2);
        continue;
      }  
      if (r1<rMax && r2>rMax) {
        CbmMuchRadialSector* s = module3->GetSectorByIndex(module->GetNSectors()-1);
        AddCharge(s,UInt_t(ne*(rMax-r1)/(r2-r1)),iPoint,time,aL,phi1,phi2);
        continue;
      }

      CbmMuchRadialSector* s1 = module3->GetSectorByRadius(r1);
      CbmMuchRadialSector* s2 = module3->GetSectorByRadius(r2);
      if (s1==s2) AddCharge(s1,ne,iPoint,time,aL,phi1,phi2); 
      else {
        AddCharge(s1,UInt_t(ne*(s1->GetR2()-r1)/(r2-r1)),iPoint,time,aL,phi1,phi2);
        AddCharge(s2,UInt_t(ne*(r2-s2->GetR1())/(r2-r1)),iPoint,time,aL,phi1,phi2);
      }
    }
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t CbmMuchDigitizeAdvancedGem::GasGain() {
  Double_t gasGain = -fMeanGasGain * TMath::Log(1 - gRandom->Rndm());
  if (gasGain < 0.) gasGain = 1e6;
  return (Int_t) gasGain;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchDigitizeAdvancedGem::Sigma_n_e(Double_t Tkin, Double_t mass) {
  Double_t logT;
  TF1 fPol6("fPol6","pol6",-5,10);
  if (mass < 0.1) {
    logT = log(Tkin * 0.511 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_e) logT = min_logT_e;
    return fPol6.EvalPar(&logT,sigma_e);
  } else if (mass >= 0.1 && mass < 0.2) {
    logT = log(Tkin * 105.658 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_mu) logT = min_logT_mu;
    return fPol6.EvalPar(&logT,sigma_mu);
  } else {
    logT = log(Tkin * 938.272 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_p) logT = min_logT_p;
    return fPol6.EvalPar(&logT,sigma_p);
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchDigitizeAdvancedGem::MPV_n_e(Double_t Tkin, Double_t mass) {
  Double_t logT;
  TF1 fPol6("fPol6","pol6",-5,10);
  if (mass < 0.1) {
    logT = log(Tkin * 0.511 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_e) logT = min_logT_e;
    return fPol6.EvalPar(&logT,mpv_e);
  } else if (mass >= 0.1 && mass < 0.2) {
    logT = log(Tkin * 105.658 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_mu) logT = min_logT_mu;
    return fPol6.EvalPar(&logT,mpv_mu);
  } else {
    logT = log(Tkin * 938.272 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_p) logT = min_logT_p;
    return fPol6.EvalPar(&logT,mpv_p);
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t CbmMuchDigitizeAdvancedGem::AddDigi(CbmMuchPad* pad) {
  CbmMuchDigiMatch* match = pad->GetMatch();
  CbmMuchDigi* digi = pad->GetDigi();
  
  // Add noise
  if (fMeanNoise){
    Double_t rndGaus = TMath::Abs(fMeanNoise * gRandom->Gaus());
    UInt_t noiseCharge = (UInt_t) rndGaus;
    match->AddCharge(-1,noiseCharge);
  }
  
  // Check for threshold 
  if (match->GetTotalCharge() < fQThreshold) {
    match->Reset();
    return kFALSE;
  }
  
  Int_t adc = match->GetTotalCharge() * fNADCChannels/ fQMax;
  digi->SetADCCharge(adc > fNADCChannels ? fNADCChannels-1 : adc);
  new ((*fDigis)[fDigis->GetEntriesFast()]) CbmMuchDigi(digi);
  new ((*fDigiMatches)[fDigiMatches->GetEntriesFast()]) CbmMuchDigiMatch(match);
  match->Reset();
  return kTRUE;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t CbmMuchDigitizeAdvancedGem::AddDigi(CbmMuchRadialPad* pad) {
  CbmMuchDigiMatch* match = pad->GetMatch();
  CbmMuchDigi* digi = pad->GetDigi();
  
  // Add noise
  if (fMeanNoise){
    Double_t rndGaus = TMath::Abs(fMeanNoise * gRandom->Gaus());
    UInt_t noiseCharge = (UInt_t) rndGaus;
    match->AddCharge(-1,noiseCharge);
  }
  
  // Check for threshold 
  if (match->GetTotalCharge() < fQThreshold) {
    match->Reset();
    return kFALSE;
  }
  
  Int_t adc = match->GetTotalCharge() * fNADCChannels/ fQMax;
  digi->SetADCCharge(adc > fNADCChannels ? fNADCChannels-1 : adc);
  new ((*fDigis)[fDigis->GetEntriesFast()]) CbmMuchDigi(digi);
  new ((*fDigiMatches)[fDigiMatches->GetEntriesFast()]) CbmMuchDigiMatch(match);
  match->Reset();
  return kTRUE;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchDigitizeAdvancedGem::GetNPrimaryElectronsPerCm(CbmMuchPoint* point){
  Int_t trackID = point->GetTrackID();
  if (trackID < 0) return -1;

  CbmMCTrack* mcTrack;
  if (!fEpoch) {
    mcTrack = (CbmMCTrack*) fMCTracks->At(trackID);
  } else {
    Int_t eventId = point->GetEventID();
    if (eventId!=fChainEventId) {
      fChainEventId=eventId;
      fMcChain->GetEntry(eventId);
    }
    mcTrack = (CbmMCTrack*) fMCTracks->At(trackID);
  }
  if (!mcTrack) return -1;
  
  Int_t pdgCode = mcTrack->GetPdgCode();

  // Reject funny particles
  TParticlePDG *particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
  if (!particle) return -1;
  if (TMath::Abs(particle->Charge()) < 0.1) return -1;

  Double_t m = particle->Mass();
  TLorentzVector p;
  p.SetXYZM(point->GetPx(),point->GetPy(),point->GetPz(),m);
  Double_t Tkin = p.E()-m; // kinetic energy of the particle
  Double_t sigma = CbmMuchDigitizeAdvancedGem::Sigma_n_e(Tkin,m); // sigma for Landau distribution
  Double_t mpv   = CbmMuchDigitizeAdvancedGem::MPV_n_e(Tkin,m);   // most probable value for Landau distr.
  Double_t n = gRandom->Landau(mpv, sigma);
  while (n > 5e4) n = gRandom->Landau(mpv, sigma); // restrict Landau tail to increase performance
  return m<0.1 ? n/l_e : n/l_not_e;
}


Bool_t CbmMuchDigitizeAdvancedGem::AddCharge(CbmMuchRadialPad* pad, UInt_t charge, Int_t iPoint, Double_t time, Double_t aL){
  CbmMuchDigi*      digi  = pad->GetDigi();
  CbmMuchDigiMatch* match = pad->GetMatch();
  if (match->GetNPoints()==0) {
    digi->SetTime(time);
    digi->SetDeadTime(fDeadTime);
  }
  if (time>digi->GetTime()+digi->GetDeadTime()) {
    AddDigi(pad);
    digi->SetTime(time);
    digi->SetDeadTime(fDeadTime);
  }
  match->AddCharge(iPoint,charge,(1-aL)*fTotalDriftTime);
  return kFALSE;
}

Bool_t CbmMuchDigitizeAdvancedGem::AddCharge(CbmMuchRadialSector* s,UInt_t ne, Int_t iPoint, Double_t time, Double_t aL, 
    Double_t phi1, Double_t phi2){
  
  CbmMuchRadialPad* pad1 = s->GetPadByPhi(phi1);
  CbmMuchRadialPad* pad2 = s->GetPadByPhi(phi2);
  if (pad1==pad2) 
    AddCharge(pad1,ne,iPoint,time,aL);
  else {
    UInt_t pad1_ne = UInt_t(ne*(pad1->GetPhimax()/180.*TMath::Pi()-phi1)/(phi2-phi1));
    AddCharge(pad1,pad1_ne   ,iPoint,time,aL);
    AddCharge(pad2,ne-pad1_ne,iPoint,time,aL);
  }
  return kFALSE;
}

Bool_t CbmMuchDigitizeAdvancedGem::AddCharge(CbmMuchPad* pad, UInt_t charge, Int_t iPoint, Double_t time, Double_t aL){
  CbmMuchDigi*      digi  = pad->GetDigi();
  CbmMuchDigiMatch* match = pad->GetMatch();
  if (match->GetNPoints()==0) {
    digi->SetTime(time);
    digi->SetDeadTime(fDeadTime);
  }
  if (time>digi->GetTime()+digi->GetDeadTime()) {
    AddDigi(pad);
    digi->SetTime(time);
    digi->SetDeadTime(fDeadTime);
  }
  match->AddCharge(iPoint,charge,(1-aL)*fTotalDriftTime);
  return kFALSE;
}

ClassImp(CbmMuchDigitizeAdvancedGem)
