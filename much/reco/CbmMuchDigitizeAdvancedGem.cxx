/** CbmMuchDigitizeAdvancedGem.cxx
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
#include "CbmMuchPad.h"

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
  fQMax(440000),
  fMeanNoise(0),//(1500),
  fMeanGasGain(1e4),
  fDeadPadsFrac(0),
  fGeoScheme(CbmMuchGeoScheme::Instance()),
  fEpoch(0),
  fMcChain(NULL),
  fDeadTime(200),
  fChainEventId(0)
{
    SetQThreshold(3);
    SetSpotRadius();
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
  fQMax(440000),
  fMeanNoise(0),//(1500),
  fMeanGasGain(1e4),
  fDeadPadsFrac(0),
  fGeoScheme(CbmMuchGeoScheme::Instance()),
  fEpoch(0),
  fMcChain(NULL),
  fDeadTime(200),
  fChainEventId(0)
{
  SetQThreshold(3),
  SetSpotRadius(),
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
  SetSpotRadius();
  Reset();
}
// -------------------------------------------------------------------------

// ------- Private method ExecAdvanced -------------------------------------
Bool_t CbmMuchDigitizeAdvancedGem::ExecAdvanced(CbmMuchPoint* point, Int_t iPoint) {
  // Get module for the point
  Int_t detectorId = point->GetDetectorID();
  CbmMuchModuleGem* module = (CbmMuchModuleGem*)fGeoScheme->GetModuleByDetId(detectorId);
  if (!module) return kFALSE;
  if (module->GetNSectors() == 0) {
    fNOutside++;
    return kFALSE;
  }

  TVector3 posIn,posOut;
  point->PositionIn(posIn);
  point->PositionOut(posOut);
  TVector3 delta = posOut-posIn;
  Double_t lTrack = delta.Mag();

  //********** Primary electrons from the track (begin) ************************//
  // Get particle's characteristics
  Int_t trackID = point->GetTrackID();
  if (trackID < 0) return kFALSE;

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
  if (!mcTrack) return kFALSE;
  
  Int_t pdgCode = mcTrack->GetPdgCode();
  // Debug
  Bool_t isSignalMuon = TMath::Abs(pdgCode) == 13 && mcTrack->GetMotherId() < 0;

  // Reject funny particles
  TParticlePDG *particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
  if (!particle) return kFALSE;
  if (TMath::Abs(particle->Charge()) < 0.1) return kFALSE;

  Double_t m = particle->Mass();
  TLorentzVector p;
  p.SetXYZM(point->GetPx(),point->GetPy(),point->GetPz(),m);
  Double_t Tkin = p.E()-m; // kinetic energy of the particle
  Double_t sigma = CbmMuchDigitizeAdvancedGem::Sigma_n_e(Tkin,m); // sigma for Landau distribution
  Double_t mpv   = CbmMuchDigitizeAdvancedGem::MPV_n_e(Tkin,m);   // most probable value for Landau distr.
  UInt_t nElectrons = (UInt_t) gRandom->Landau(mpv, sigma);  // number of prim. electrons per 3 mm gap
  while (nElectrons > 50000) nElectrons = (UInt_t) (gRandom->Landau(mpv, sigma));     // restrict Landau tail to increase performance
  // Number of electrons for current track length
  if (m < 0.1) nElectrons = (UInt_t) (nElectrons * lTrack / 0.47);
  else         nElectrons = (UInt_t) (nElectrons * lTrack / 0.36);
  //********** Primary electrons from the track (end)   ***********************//

  Double_t time = -1;
  while(time < 0) time = point->GetTime() + gRandom->Gaus(0, fDTime);
  for (Int_t iElectron = 0; iElectron < nElectrons; iElectron++) {
    // Coordinates of primary electrons along the track
    Double_t aL = gRandom->Rndm();
    Double_t xe = posIn[0]+aL*delta[0];
    Double_t ye = posIn[1]+aL*delta[1];

    // Calculate number of secondary electrons for each primary electron
    UInt_t nSecElectrons = GasGain(); // number of secondary electrons
    Double_t spotL = fSpotRadius * 2.; // diameter of the spot
    Double_t spotArea = spotL * spotL; // spot area

    // Find intersection of spot with sectors
    TPolyLine spotPolygon = GetPolygon(xe, ye, spotL, spotL);
    Double_t* xVertex = spotPolygon.GetX();
    Double_t* yVertex = spotPolygon.GetY();
    map<Int_t, CbmMuchSector*> firedSectors; // map from a sector index to a fired sector
    for (Int_t iVertex = 0; iVertex < spotPolygon.GetN() - 1; iVertex++) {
      CbmMuchSector* sector = module->GetSector(xVertex[iVertex], yVertex[iVertex]);
      if (!sector) continue;
      Int_t iSector = sector->GetSectorIndex();
      if (firedSectors.find(iSector) != firedSectors.end()) continue;
      firedSectors[iSector] = sector;
    }

    // Fire pads in intersected sectors
    for (map<Int_t, CbmMuchSector*>::iterator it = firedSectors.begin(); it!= firedSectors.end(); it++) {
      // Get sector and its parameters
      CbmMuchSector* sector  = (*it).second;
      Double_t padRad = sector->GetPadRadius();
      
      for (Int_t iPad=0;iPad<sector->GetNChannels();iPad++){
        CbmMuchPad* pad = sector->GetPad(iPad);
        // rough search
        if (pow(pad->GetX0()-xe,2.)+pow(pad->GetY0()-ye,2.)>pow(padRad+fSpotRadius,2.)) continue; 
        // detailed search
        Double_t area;
        if (!PolygonsIntersect(*pad,spotPolygon,area)) continue; 
        UInt_t iCharge = (UInt_t) (nSecElectrons * area / spotArea);
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
        match->AddCharge(iPoint,iCharge);
      }
    } // loop fired sectors
    firedSectors.clear();
  } // loop over electrons
  return kTRUE;
  //**************  Simulate avalanche (end) **********************************//
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
    CbmMuchModuleGem* module = (CbmMuchModuleGem*)fGeoScheme->GetModuleByDetId(point->GetDetectorID());
    if (!module) {
      fNFailed++;
      continue;
    }
    // Process only appropriate module types
    if(module->GetDetectorType()!=1) continue;
    // Produce Digis
    ExecAdvanced(point, iPoint);
  } // MuchPoint loop

  // Add remaining digis
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules();
  for (Int_t im=0;im<modules.size();im++){
    if (modules[im]->GetDetectorType()!=1) continue;
    vector<CbmMuchPad*> pads = ((CbmMuchModuleGem*) modules[im])->GetPads();
    for (Int_t ip=0;ip<pads.size();ip++) AddDigi(pads[ip]);
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


// -----   Private method Reset   ------------------------------------------
void CbmMuchDigitizeAdvancedGem::Reset() {
  fNFailed = fNOutside = fNMulti = 0;
  if (fDigis) fDigis->Clear();
  if (fDigiMatches) fDigiMatches->Delete(); // Delete because of memory leaks
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
TPolyLine CbmMuchDigitizeAdvancedGem::GetPolygon(Double_t x0, Double_t y0, Double_t width,
    Double_t height) {

  Double_t x[5], y[5];
  x[1] = -width /2;
  y[1] = +height/2;
  x[2] = +width /2;
  y[2] = +height/2;
  x[0] = -width /2;
  y[0] = -height/2;
  x[3] = +width /2;
  y[3] = -height/2;
  x[4] = x[0];
  y[4] = y[0];
  for (int i = 0; i < 5; i++) {
    x[i] += x0;
    y[i] += y0;
  }
  TPolyLine pline(5, x, y);
  return pline;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t CbmMuchDigitizeAdvancedGem::ProjectionsIntersect(Double_t x11, Double_t x12,
    Double_t x21, Double_t x22, Double_t& length) {
  if (x11 > x22 || x12 < x21)
    return kFALSE;
  if (x22 < x12) {
    if (x21 > x11) length = x22 - x21;
    else           length = x22 - x11;
  } else {
    if (x11 > x21) length = x12 - x11;
    else           length = x12 - x21;
  }
  return kTRUE;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t CbmMuchDigitizeAdvancedGem::PolygonsIntersect(TPolyLine polygon1, TPolyLine polygon2, Double_t& area) {
  Double_t length, width;
  Double_t* x1 = polygon1.GetX();
  Double_t* y1 = polygon1.GetY();
  Double_t* x2 = polygon2.GetX();
  Double_t* y2 = polygon2.GetY();

  if (!ProjectionsIntersect(x1[0], x1[3], x2[0], x2[3], width))  return kFALSE;
  if (!ProjectionsIntersect(y1[0], y1[1], y2[0], y2[1], length)) return kFALSE;
  area = width * length;
  return kTRUE;
}

Int_t CbmMuchDigitizeAdvancedGem::GasGain() {
  Double_t gasGain = -fMeanGasGain * TMath::Log(1 - gRandom->Rndm());
  if (gasGain < 0.) gasGain = 1e6;
  return (Int_t) gasGain;
}

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

ClassImp(CbmMuchDigitizeAdvancedGem)
