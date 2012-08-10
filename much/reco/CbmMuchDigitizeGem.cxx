/** CbmMuchDigitizeGem.cxx
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
#include "CbmMuchDigitizeGem.h"
#include "CbmMuchPoint.h"
#include "CbmMuchSector.h"
#include "CbmMuchStation.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchModuleGemRadial.h"
#include "CbmMuchModuleGemRectangular.h"
#include "CbmMuchPad.h"
#include "CbmMuchPadRadial.h"
#include "CbmMuchPadRectangular.h"
#include "CbmMuchSectorRadial.h"
#include "CbmMuchSectorRectangular.h"

// Includes from base
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmMCEpoch.h"

// Includes from ROOT
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TFile.h"
#include "TRandom.h"
#include "TChain.h"

#include <vector>
using std::map;

// -------------------------------------------------------------------------
CbmMuchDigitizeGem::CbmMuchDigitizeGem(const char* digiFileName) 
  : FairTask("MuchDigitizeGem",1),
    fAlgorithm(1),
    fGeoScheme(CbmMuchGeoScheme::Instance()),
    fDigiFile(digiFileName),
    fPoints(NULL),
    fMCTracks(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fMcEpoch(NULL),
    fNFailed(0),
    fNOutside(0),
    fNMulti(0),
    fNADCChannels(256),
    fQMax(500000),
    fQThreshold(3),
    fMeanNoise(0),//(1500),
    fSpotRadius(0.05),
    fMeanGasGain(1e4),
    fDTime(3),
    fEvent(0),
    fDeadPadsFrac(0),
    fTimer(),
    fEpoch(0),
    fMcChain(NULL),
    fDeadTime(400),
    fDriftVelocity(100),
    fPeakingTime(20),
    fRemainderTime(40),
    fTimeBinWidth(1),
    fChainEventId(0),
    fTotalDriftTime(0.)
{
  Double_t driftVolumeWidth = 0.4; // cm // TODO
  fTotalDriftTime = driftVolumeWidth/fDriftVelocity*10000; // [ns];
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchDigitizeGem::~CbmMuchDigitizeGem() {
  if (fDigis) {
    fDigis->Delete();
    delete fDigis;
  }
  if (fDigiMatches) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchDigitizeGem::Init() {
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
void CbmMuchDigitizeGem::Exec(Option_t* opt) {
  fTimer.Start();
  fDigis->Clear();
  fDigiMatches->Delete();

  if (fEpoch) fPoints = fMcEpoch->GetPoints(kMUCH);
  Int_t nPoints = fPoints->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(iPoint);
    if (!point || !point->IsUsable())  continue;
    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(point->GetDetectorID());
    if (!module) continue;
    ExecPoint(point, iPoint);
  }

  // Add remaining digis
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules();
  for (Int_t im=0;im<modules.size();im++){
    if (modules[im]->GetDetectorType()!=1 && modules[im]->GetDetectorType()!=3) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) modules[im];
    vector<CbmMuchPad*> pads = module->GetPads();
    for (Int_t ip=0;ip<pads.size();ip++) AddDigi(pads[ip]);
  }

  fTimer.Stop();
  printf("-I- MuchDigitizer: event=%3i, time=%5.2f s, points=%i, digis=%i\n",fEvent++,fTimer.RealTime(),nPoints,fDigis->GetEntriesFast());
}
// -------------------------------------------------------------------------


// ------- Private method ExecAdvanced -------------------------------------
Bool_t CbmMuchDigitizeGem::ExecPoint(CbmMuchPoint* point, Int_t iPoint) {
  TVector3 v1,v2,dv;
  point->PositionIn(v1);
  point->PositionOut(v2);
  dv = v2-v1;

  Int_t detectorId = point->GetDetectorID();
  CbmMuchModule* module = fGeoScheme->GetModuleByDetId(detectorId);
  
  if (fAlgorithm==0){    // Simple digitization
    TVector3 v = 0.5*(v1+v2);
    CbmMuchPad* pad = 0;
    if (module->GetDetectorType()==1){
      CbmMuchModuleGemRectangular* module1 = (CbmMuchModuleGemRectangular*) module;
      pad = module1->GetPad(v[0],v[1]);
      if (pad) printf("x0=%f,y0=%f\n",pad->GetX(),pad->GetY());
    } else if (module->GetDetectorType()==3){
      CbmMuchModuleGemRadial* module3 = (CbmMuchModuleGemRadial*) module;
      pad = module3->GetPad(v[0],v[1]);
    }
    if (!pad) return kFALSE;
    AddCharge(pad,fQMax,iPoint,point->GetTime(),0);
    return kTRUE;
  }
  
  // Start of advanced digitization
  Int_t nElectrons = Int_t(GetNPrimaryElectronsPerCm(point)*dv.Mag());
  if (nElectrons<0) return kFALSE;
  
  Double_t time = -1;
  // TODO
  while(time < 0) time = point->GetTime() + gRandom->Gaus(0, fDTime);
  
  if (module->GetDetectorType()==1) {
    CbmMuchModuleGemRectangular* module1 = (CbmMuchModuleGemRectangular*) module;
    map<CbmMuchSector*,Int_t> firedSectors;
    for (Int_t i=0;i<nElectrons;i++) {
      Double_t aL = gRandom->Rndm();
      Double_t driftTime = (1-aL)*fTotalDriftTime;
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
          CbmMuchPad* pad = sector->GetPadByChannelIndex(iPad);
          Double_t xp0 = pad->GetX();
          Double_t xpd = pad->GetDx()/2.;
          Double_t xp1 = xp0-xpd;
          Double_t xp2 = xp0+xpd;
          if (x1>xp2 || x2<xp1) continue;
          Double_t yp0 = pad->GetY();
          Double_t ypd = pad->GetDy()/2.;
          Double_t yp1 = yp0-ypd;
          Double_t yp2 = yp0+ypd;
          if (y1>yp2 || y2<yp1) continue;
          Double_t lx = x1>xp1 ? (x2<xp2 ? x2-x1 : xp2-x1) : x2-xp1;
          Double_t ly = y1>yp1 ? (y2<yp2 ? y2-y1 : yp2-y1) : y2-yp1;
          AddCharge(pad,UInt_t(ne*lx*ly/s),iPoint,time,driftTime);
        }
      } // loop fired sectors
      firedSectors.clear();
    }
  }
  
  if (module->GetDetectorType()==3) {
    CbmMuchModuleGemRadial* module3 = (CbmMuchModuleGemRadial*) module;
    CbmMuchSectorRadial* sFirst = (CbmMuchSectorRadial*) module3->GetSectorByIndex(0);
    CbmMuchSectorRadial* sLast  = (CbmMuchSectorRadial*) module3->GetSectorByIndex(module3->GetNSectors()-1);

    Double_t rMin = sFirst->GetR1();
    Double_t rMax = sLast->GetR2();

    for (Int_t i=0;i<nElectrons;i++) {
      Double_t aL   = gRandom->Rndm();
      Double_t driftTime = (1-aL)*fTotalDriftTime;
      TVector3 ve   = v1 + dv*aL;
      UInt_t ne     = GasGain();
      Double_t r    = ve.Perp();
      Double_t phi  = ve.Phi();
      Double_t r1   = r-fSpotRadius;
      Double_t r2   = r+fSpotRadius;
      Double_t phi1 = phi-fSpotRadius/r;
      Double_t phi2 = phi+fSpotRadius/r;
      if (r1<rMin && r2>rMin) {
        AddCharge(sFirst,UInt_t(ne*(r2-rMin)/(r2-r1)),iPoint,time,driftTime,phi1,phi2);
        continue;
      }  
      if (r1<rMax && r2>rMax) {
        AddCharge(sLast,UInt_t(ne*(rMax-r1)/(r2-r1)),iPoint,time,driftTime,phi1,phi2);
        continue;
      }

      CbmMuchSectorRadial* s1 = module3->GetSectorByRadius(r1);
      CbmMuchSectorRadial* s2 = module3->GetSectorByRadius(r2);
      if (s1==s2) AddCharge(s1,ne,iPoint,time,driftTime,phi1,phi2); 
      else {
        AddCharge(s1,UInt_t(ne*(s1->GetR2()-r1)/(r2-r1)),iPoint,time,driftTime,phi1,phi2);
        AddCharge(s2,UInt_t(ne*(r2-s2->GetR1())/(r2-r1)),iPoint,time,driftTime,phi1,phi2);
      }
    }
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t CbmMuchDigitizeGem::GasGain() {
  Double_t gasGain = -fMeanGasGain * TMath::Log(1 - gRandom->Rndm());
  if (gasGain < 0.) gasGain = 1e6;
  return (Int_t) gasGain;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchDigitizeGem::Sigma_n_e(Double_t Tkin, Double_t mass) {
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
Double_t CbmMuchDigitizeGem::MPV_n_e(Double_t Tkin, Double_t mass) {
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
Double_t CbmMuchDigitizeGem::GetNPrimaryElectronsPerCm(CbmMuchPoint* point){
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


  TParticlePDG *particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
  // Assign proton hypothesis for unknown particles
  if (!particle) particle = TDatabasePDG::Instance()->GetParticle(2212);
  if (TMath::Abs(particle->Charge()) < 0.1) return -1;

  Double_t m = particle->Mass();
  TLorentzVector p;
  p.SetXYZM(point->GetPx(),point->GetPy(),point->GetPz(),m);
  Double_t Tkin = p.E()-m; // kinetic energy of the particle
  Double_t sigma = CbmMuchDigitizeGem::Sigma_n_e(Tkin,m); // sigma for Landau distribution
  Double_t mpv   = CbmMuchDigitizeGem::MPV_n_e(Tkin,m);   // most probable value for Landau distr.
  Double_t n = gRandom->Landau(mpv, sigma);
  while (n > 5e4) n = gRandom->Landau(mpv, sigma); // restrict Landau tail to increase performance
  return m<0.1 ? n/l_e : n/l_not_e;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t CbmMuchDigitizeGem::AddCharge(CbmMuchSectorRadial* s,UInt_t ne, Int_t iPoint, Double_t time, Double_t driftTime, 
    Double_t phi1, Double_t phi2){
  CbmMuchPadRadial* pad1 = s->GetPadByPhi(phi1);
  CbmMuchPadRadial* pad2 = s->GetPadByPhi(phi2);
  if (pad1==pad2) 
    AddCharge(pad1,ne,iPoint,time,driftTime);
  else {
    Double_t phi = pad1 ? pad1->GetPhi2() : pad2 ? pad2->GetPhi1() : 0;
    UInt_t pad1_ne = UInt_t(ne*(phi-phi1)/(phi2-phi1));
    AddCharge(pad1,pad1_ne   ,iPoint,time,driftTime);
    AddCharge(pad2,ne-pad1_ne,iPoint,time,driftTime);
  }
  return kFALSE;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchDigitizeGem::AddCharge(CbmMuchPad* pad, UInt_t charge, Int_t iPoint, Double_t time, Double_t driftTime){
  if (!pad) return;
  CbmMuchDigiMatch* match = pad->GetMatch();
  CbmMuchDigi* digi = pad->GetDigi();
  if (match->GetNPoints()==0) {
    digi->SetTime(time);
    digi->SetDeadTime(fDeadTime);
  }
  if (time>digi->GetTime()+digi->GetDeadTime()) {
    AddDigi(pad);
    digi->SetTime(time);
    digi->SetDeadTime(fDeadTime);
  }
  match->AddCharge(iPoint,charge,driftTime);
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
Bool_t CbmMuchDigitizeGem::AddDigi(CbmMuchPad* pad) {
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

ClassImp(CbmMuchDigitizeGem)
