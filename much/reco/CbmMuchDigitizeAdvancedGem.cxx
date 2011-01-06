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
  fDeadTime(3.e+5)
{
    SetQThreshold(3);
    SetSpotRadius();
    Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigitizeAdvancedGem::CbmMuchDigitizeAdvancedGem(Int_t iVerbose) :
  FairTask("MuchDigitize", iVerbose),
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
  fDeadTime(3.e+5)
{
    SetQThreshold(3);
    SetSpotRadius();
    Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
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
  fDeadTime(3.e+5)
{
  SetQThreshold(3),
  SetSpotRadius(),
  Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchDigitizeAdvancedGem::~CbmMuchDigitizeAdvancedGem() {
  /*
  if (fDigiFile)
    delete fDigiFile;
  */
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
  if (!module)
    return kFALSE;
  if (module->GetNSectors() == 0) {
    fNOutside++;
    return kFALSE;
  }

  TVector3 modSize = module->GetSize();
  Double_t modLx = modSize[0];
  Double_t modLy = modSize[1];

  // Get track length within the module
  Double_t xIn = point->GetXIn();
  Double_t yIn = point->GetYIn();
  Double_t xOut = point->GetXOut();
  Double_t yOut = point->GetYOut();
  Double_t zIn = point->GetZIn();
  Double_t zOut = point->GetZOut();
  Double_t deltaX = xOut - xIn;
  Double_t deltaY = yOut - yIn;
  Double_t deltaZ = zOut - zIn;
  Double_t lTrack = TMath::Sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ); // track length

  //********** Primary electrons from the track (begin) ************************//
  // Get particle's characteristics
  Int_t trackID = point->GetTrackID();
  if (trackID < 0) {
    return kFALSE;
  }
  CbmMCTrack* mcTrack;
  if (!fEpoch) {
    mcTrack = (CbmMCTrack*) fMCTracks->At(trackID);
  } else {
    Int_t eventId = point->GetEventID();
    fMcChain->GetEntry(eventId);
    mcTrack = (CbmMCTrack*) fMCTracks->At(trackID);
  }
  if (!mcTrack) return kFALSE;
  
  Int_t pdgCode = mcTrack->GetPdgCode();
  // Debug
  Bool_t isSignalMuon = TMath::Abs(pdgCode) == 13 && mcTrack->GetMotherId() < 0;

  // Reject funny particles
  TParticlePDG *particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
  if (!particle) {
    return kFALSE;
  }
  if (TMath::Abs(particle->Charge()) < 0.1)
    return kFALSE;
  TVector3 momentum;                                // 3-momentum of the particle
  point->Momentum(momentum);
  Double_t mom = momentum.Mag() * 1e3;              // absolute momentum value [MeV/c]
  Double_t mom2 = mom * mom;                        // squared momentum of the particle
  Double_t mass = particle->Mass() * 1e3;           // mass of the particle [MeV/c^2]
  Double_t mass2 = mass * mass;                     // squared mass of the particle
  Double_t Tkin = TMath::Sqrt(mom2 + mass2) - mass; // kinetic energy of the particle
  Double_t sigma = CbmMuchDigitizeAdvancedGem::Sigma_n_e(Tkin, mass);      // sigma for Landau distribution
  Double_t mpv = CbmMuchDigitizeAdvancedGem::MPV_n_e(Tkin, mass);          // most probable value for Landau distr.
  UInt_t nElectrons = (UInt_t) gRandom->Landau(mpv, sigma);  // number of prim. electrons per 0.3 cm gap
  while (nElectrons > 50000)
    nElectrons = (UInt_t) (gRandom->Landau(mpv, sigma));     // restrict Landau tail to increase performance
  // Number of electrons for current track length
  if (mass < 100.)
    nElectrons = (UInt_t) (nElectrons * lTrack / 0.47);
  else
    nElectrons = (UInt_t) (nElectrons * lTrack / 0.36);
  //********** Primary electrons from the track (end)   ***********************//

  Double_t hypotenuse = TMath::Sqrt(deltaX * deltaX + deltaY * deltaY);
  Double_t cosphi_tr = deltaX / hypotenuse; // cos of track azim. angle
  Double_t sinphi_tr = deltaY / hypotenuse; // sin of track azim. angle

  map<Long64_t, CbmMuchDigi*> chargedPads;     // map from a channel id within the module to a fired digi
  map<Long64_t, CbmMuchDigiMatch*> chargedMatches;     // the same for digi matches
  Double_t time = -1;
  while(time < 0) time = point->GetTime()  + gRandom->Gaus(0, fDTime);
  UInt_t nTrackCharge = 0;                  // total charge left by a track
  for (Int_t iElectron = 0; iElectron < nElectrons; iElectron++) {
    // Coordinates of primary electrons along the track
    Double_t aL = gRandom->Rndm() * hypotenuse;
    Double_t xe = xIn + aL * cosphi_tr;
    Double_t ye = yIn + aL * sinphi_tr;

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

      if (sector) {
        Int_t iSector = sector->GetSectorIndex();
        if (firedSectors.find(iSector) == firedSectors.end())
          firedSectors[iSector] = sector;
      }
    }

    // Fire pads in intersected sectors
    for (map<Int_t, CbmMuchSector*>::iterator it = firedSectors.begin(); it
    != firedSectors.end(); it++) {
      // Get sector and its parameters
      CbmMuchSector* sector = (*it).second;
      Int_t iSector = (*it).first;
      // Find fired pads
      for (Int_t iChannel = 0; iChannel < sector->GetNChannels(); iChannel++) {
        Double_t padRad = sector->GetPadRadius();
        Double_t distX = xe - sector->GetPadX0(iChannel);
        Double_t distY = ye - sector->GetPadY0(iChannel);
        Double_t dist = TMath::Sqrt(distX * distX + distY * distY);
        if (dist > padRad + fSpotRadius)
          continue; // rough search

        TPolyLine* padPolygon = sector->GetPad(iChannel);
        Double_t area;
        if (!PolygonsIntersect(sector, *padPolygon, spotPolygon, area))
          continue; // detailed search
        UInt_t iCharge = (UInt_t) (nSecElectrons * area / spotArea);
        Long64_t channelId = CbmMuchModuleGem::GetChannelId(iSector, iChannel); // channel id within the module

        if (chargedPads.find(channelId) == chargedPads.end()) {
          chargedPads[channelId] = new CbmMuchDigi(detectorId,
              channelId, time, fDTime);
          chargedMatches[channelId] = new CbmMuchDigiMatch();
        }
        chargedMatches[channelId]->AddPoint(iPoint);
        chargedMatches[channelId]->AddCharge(iCharge);

      } // loop channels
    } // loop fired sectors
  } // loop primary electrons

  if (chargedPads.size() == 0) {
    fNOutside++;
    return kFALSE;
  }

  for (map<Long64_t, CbmMuchDigi*>::iterator it = chargedPads.begin(); it
  != chargedPads.end(); it++) {
    Long64_t channelId = (*it).first;                      // Channel id within the module
    pair<Int_t, Long64_t> uniqueId(detectorId, channelId); // Unique channel id within the MUCH
    CbmMuchDigi* digi = chargedPads[channelId];
    CbmMuchDigiMatch* match = chargedMatches[channelId];
    if(!match) continue;
    if (!digi) continue;
    Int_t iCharge = match->GetTotalCharge();
    if (fChargedPads.find(uniqueId) == fChargedPads.end()) {
      fChargedPads[uniqueId] = new CbmMuchDigi(digi);
      fChargedMatches[uniqueId] = new CbmMuchDigiMatch();
      fChargedMatches[uniqueId]->AddPoint(iPoint);
      fChargedMatches[uniqueId]->AddCharge(iCharge);
    } else {
      fChargedMatches[uniqueId]->AddPoint(iPoint);
      fChargedMatches[uniqueId]->AddCharge(iCharge);
      fNMulti++;
    }

    // Clear memory
    delete digi;
    delete match;
  }

  chargedPads.clear();
  chargedMatches.clear();

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

  // Verbose screen output
  if (fVerbose > 2) {
    cout << endl << "-I- " << fName << ": executing event" << endl;
    cout << "----------------------------------------------" << endl;
  }

  // Check for input arrays (Event-by-event approach)
  if (!fEpoch && !fMCTracks) {
    cout << "-W- " << fName << "::Exec: No input array (MCTrack)" << endl;
    cout << "- " << fName << endl;
    return;
  }
  if (!fEpoch && !fPoints) {
    cerr << "-W- " << fName << "::Exec: No input array (MuchPoint) "
    << endl;
    cout << "- " << fName << endl;
    return;
  }

  // Check for input arrays (Epoch approach)
  if (fEpoch) {
    fPoints = fMcEpoch->GetPoints(kMUCH);
  }
  
  
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
    CbmMuchModuleGem* module = (CbmMuchModuleGem*)fGeoScheme->GetModuleByDetId(
        point->GetDetectorID());
    if (!module) {
      fNFailed++;
      continue;
    }
    // Process only appropriate module types
    if(module->GetDetectorType()!=1) continue;
    // Produce Digis
    ExecAdvanced(point, iPoint);
  } // MuchPoint loop

  FirePads();

//  for(Int_t iDigi = 0; iDigi < fDigis->GetEntriesFast(); ++iDigi){
//    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
//  }

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

// -----   Private method SetParContainers   -------------------------------
void CbmMuchDigitizeAdvancedGem::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Finish   -----------------------------------------
void CbmMuchDigitizeAdvancedGem::FinishTask() {
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

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchDigitizeAdvancedGem::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmMuchDigitizeAdvancedGem::Reset() {
  fNFailed = fNOutside = fNMulti = 0;
  fChannelMap.clear();
  fChargedPads.clear();
  fChargedMatches.clear();
  if (fDigis)
    fDigis->Clear();
  if (fDigiMatches)
    fDigiMatches->Delete(); // Delete because of memory leaks
}
// -------------------------------------------------------------------------

// -----   Private method FirePads   ---------------------------------------
void CbmMuchDigitizeAdvancedGem::FirePads() {
  // Add electronics noise // TODO Add noise in time
  if (fMeanNoise && !fEpoch) AddNoise(); 

  vector<CbmMuchDigi*> vdigi;
  vector<CbmMuchDigiMatch*> vmatch;
  
  for (map<pair<Int_t, Long64_t> , CbmMuchDigi*>::iterator it =
    fChargedPads.begin(); it != fChargedPads.end(); it++) {
    // Randomly kill the pads
    if (gRandom->Rndm() < fDeadPadsFrac) continue;
    
    CbmMuchDigi* digi = it->second;
    CbmMuchDigiMatch* match = fChargedMatches[it->first];
    
    if (!fEpoch){
      vdigi.push_back(digi);
      vmatch.push_back(match);
      continue;
    }
    
    match->SortPointsInTime(fPoints);
    Double_t last_time=-1.e+10;
    CbmMuchDigi* new_digi = 0;
    CbmMuchDigiMatch* new_match = 0;
    for (Int_t i=0;i<match->GetNPoints();i++){
      Int_t index  = match->GetRefIndex(i);
      Int_t charge = match->GetCharge(i);
      // Get point time
      assert(fPoints);
      CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(index);
      assert(point);
      Double_t time = point->GetTime();
      // Create new digi if time interval between points is larger than dead time
      if (time-last_time>fDeadTime) {
        new_digi  = new CbmMuchDigi(digi);
        new_match = new CbmMuchDigiMatch();
        new_digi->SetTime(time);
        vdigi.push_back(new_digi);
        vmatch.push_back(new_match);
      }
      assert(new_digi);
      assert(new_match);
      new_digi->SetDeadTime(new_digi->GetTime()-time+fDeadTime);
      new_match->AddPoint(index);
      new_match->AddCharge(charge);
    }
  }
  
  // Apply threshold
  
  for (UInt_t i=0,iDigi=0;i<vdigi.size();i++){
    CbmMuchDigi* digi = vdigi[i];
    CbmMuchDigiMatch* match = vmatch[i];
    if (match->GetTotalCharge() < fQThreshold) continue;
    Int_t adcChannel = match->GetTotalCharge() * fNADCChannels/ fQMax;
    digi->SetADCCharge(adcChannel > fNADCChannels ? fNADCChannels-1 : adcChannel);
    new ((*fDigis)[iDigi]) CbmMuchDigi(digi);
    new ((*fDigiMatches)[iDigi++]) CbmMuchDigiMatch(match);
    delete digi;
    delete match;
  }
  vdigi.clear();
  vmatch.clear();
}
// -------------------------------------------------------------------------

// -----   Private method AddNoise   ---------------------------------------
void CbmMuchDigitizeAdvancedGem::AddNoise() {
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules();
  for(vector<CbmMuchModule*>::iterator it = modules.begin(); it!=modules.end(); it++){
    CbmMuchModule* mod = (*it);
    if(mod->GetDetectorType() != 1) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) mod;
    vector<CbmMuchPad*> pads = module->GetPads();
    for (vector<CbmMuchPad*>::iterator iter = pads.begin(); iter != pads.end(); ++iter) {
      AddNoise(*iter);
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method AddNoise   ---------------------------------------
void CbmMuchDigitizeAdvancedGem::AddNoise(CbmMuchPad* pad) {
  Double_t rndGaus = TMath::Abs(fMeanNoise * gRandom->Gaus());
  UInt_t iCharge = (UInt_t) rndGaus;
  Int_t detectorId = pad->GetDetectorId();
  Long64_t channelId  = pad->GetChannelId();
  pair<Int_t, Long64_t> uniqueId(detectorId, channelId);
  if (fChargedPads.find(uniqueId) == fChargedPads.end()) {
    if (iCharge <= fQThreshold) return;
    fChargedPads[uniqueId] = new CbmMuchDigi(pad->GetDetectorId(),
        channelId, 0, 0); // No time and Dtime info   // TODO Add noise in time
    fChargedMatches[uniqueId] = new CbmMuchDigiMatch();
  }
  fChargedMatches[uniqueId]->AddPoint(-1);
  fChargedMatches[uniqueId]->AddCharge(iCharge);
}
// -------------------------------------------------------------------------

// -----   Private method FirePads   ---------------------------------------
TPolyLine CbmMuchDigitizeAdvancedGem::GetPolygon(Double_t x0, Double_t y0, Double_t width,
    Double_t height) {

  Double_t x[5], y[5];
  x[1] = -width / 2;
  y[1] = +height / 2;
  x[2] = +width / 2;
  y[2] = +height / 2;
  x[0] = -width / 2;
  y[0] = -height / 2;
  x[3] = +width / 2;
  y[3] = -height / 2;
  x[4] = x[0];
  y[4] = y[0];
  for (int i = 0; i < 5; i++) {
    x[i] += x0;
    y[i] += y0;
  }
  TPolyLine pline(5, x, y);
  return pline;
}

Bool_t CbmMuchDigitizeAdvancedGem::ProjectionsIntersect(Double_t x11, Double_t x12,
    Double_t x21, Double_t x22, Double_t& length) {
  if (x11 > x22 || x12 < x21)
    return kFALSE;
  if (x22 < x12) {
    if (x21 > x11)
      length = x22 - x21;
    else
      length = x22 - x11;
  } else {
    if (x11 > x21)
      length = x12 - x11;
    else
      length = x12 - x21;
  }
  return kTRUE;
}

Bool_t CbmMuchDigitizeAdvancedGem::PolygonsIntersect(CbmMuchSector* sector,
    TPolyLine polygon1, TPolyLine polygon2, Double_t& area) {
  Double_t length, width;
  Double_t* x1 = polygon1.GetX();
  Double_t* y1 = polygon1.GetY();
  Double_t* x2 = polygon2.GetX();
  Double_t* y2 = polygon2.GetY();

  if (!ProjectionsIntersect(x1[0], x1[3], x2[0], x2[3], width))
    return kFALSE;
  if (!ProjectionsIntersect(y1[0], y1[1], y2[0], y2[1], length))
    return kFALSE;
  area = width * length;
  return kTRUE;
}

Int_t CbmMuchDigitizeAdvancedGem::GasGain() {
  //  const Double_t q_mean  = 1.e4;  // mean gas gain, arbitrary value
  Double_t gasGain = -fMeanGasGain * TMath::Log(1 - gRandom->Rndm());
  if (gasGain < 0.)
    gasGain = 1e6;
  return (Int_t) gasGain;
}

Double_t CbmMuchDigitizeAdvancedGem::Sigma_n_e(Double_t Tkin, Double_t mass) {
  Double_t logT;
  if (mass < 100) {
    logT = log(Tkin * 0.511 / mass);
    return CbmMuchDigitizeAdvancedGem::e_sigma_n_e(logT);
  } else if (mass >= 100 && mass < 200) {
    logT = log(Tkin * 105.658 / mass);
    return CbmMuchDigitizeAdvancedGem::mu_sigma_n_e(logT);
  } else {
    logT = log(Tkin * 938.272 / mass);
    return CbmMuchDigitizeAdvancedGem::p_sigma_n_e(logT);
  }
}

Double_t CbmMuchDigitizeAdvancedGem::MPV_n_e(Double_t Tkin, Double_t mass) {
  Double_t logT;
  if (mass < 100.) {
    logT = log(Tkin * 0.511 / mass);
    return CbmMuchDigitizeAdvancedGem::e_MPV_n_e(logT);
  } else if (mass >= 100. && mass < 200.) {
    logT = log(Tkin * 105.658 / mass);
    return CbmMuchDigitizeAdvancedGem::mu_MPV_n_e(logT);
  } else {
    logT = log(Tkin * 938.272 / mass);
    return CbmMuchDigitizeAdvancedGem::p_MPV_n_e(logT);
  }
}

Double_t CbmMuchDigitizeAdvancedGem::mu_sigma_n_e(Double_t &logT) {
  if (logT < -0.916291)
    logT = -0.916291;
  if (logT > 9.21034)
    logT = 9.21034;
  const int n = 7;
  double val = 0;
  double arg = 1;
  double p[n] = { 74.5272, -49.7648, 14.4886, -2.23059, 0.188254,
      -0.00792744, 0.00011976 };
  for (int i = 0; i < n; i++) {
    val = val + arg * p[i];
    arg = arg * logT;
  }
  return val;
}

Double_t CbmMuchDigitizeAdvancedGem::p_sigma_n_e(Double_t &logT) {
  if (logT < 1.09861)
    logT = 1.09861;
  if (logT > 9.21034)
    logT = 9.21034;
  const int n = 7;
  double val = 0;
  double arg = 1;
  double p[n] = { 175.879, -15.016, -34.6513, 13.346, -2.08732, 0.153678,
      -0.00440115 };
  for (int i = 0; i < n; i++) {
    val = val + arg * p[i];
    arg = arg * logT;
  }
  return val;
}

Double_t CbmMuchDigitizeAdvancedGem::e_sigma_n_e(Double_t &logT) {
  if (logT < -3.21888)
    logT = -3.21888;
  if (logT > 9.21034)
    logT = 9.21034;
  const int n = 7;
  double val = 0;
  double arg = 1;
  double p[n] = { 4.06815, -0.225699, 0.464502, -0.141208, 0.0226821,
      -0.00195697, 6.87497e-05 };
  for (int i = 0; i < n; i++) {
    val = val + arg * p[i];
    arg = arg * logT;
  }
  return val;
}

Double_t CbmMuchDigitizeAdvancedGem::mu_MPV_n_e(Double_t &logT) {
  if (logT < -0.916291)
    logT = -0.916291;
  if (logT > 9.21034)
    logT = 9.21034;
  const int n = 7;
  double val = 0;
  double arg = 1;
  double p[n] = { 660.746, -609.335, 249.011, -55.6658, 7.04607, -0.472135,
      0.0129834 };
  for (int i = 0; i < n; i++) {
    val = val + arg * p[i];
    arg = arg * logT;
  }
  return val;
}

Double_t CbmMuchDigitizeAdvancedGem::p_MPV_n_e(Double_t &logT) {
  if (logT < 1.09861)
    logT = 1.09861;
  if (logT > 9.21034)
    logT = 9.21034;
  const int n = 7;
  double val = 0;
  double arg = 1;
  double p[n] = { 4152.73, -3123.98, 1010.85, -178.092, 17.8764, -0.963169,
      0.0216643 };
  for (int i = 0; i < n; i++) {
    val = val + arg * p[i];
    arg = arg * logT;
  }
  return val;
}

Double_t CbmMuchDigitizeAdvancedGem::e_MPV_n_e(Double_t &logT) {
  if (logT < -3.21888)
    logT = -3.21888;
  if (logT > 9.21034)
    logT = 9.21034;
  const int n = 7;
  double val = 0;
  double arg = 1;
  double p[n] = { 14.654, -0.786582, 2.32435, -0.875594, 0.167237,
      -0.0162335, 0.000616855 };
  for (int i = 0; i < n; i++) {
    val = val + arg * p[i];
    arg = arg * logT;
  }
  return val;
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchDigitizeAdvancedGem)

