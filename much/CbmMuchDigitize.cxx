/** CbmMuchDigitize.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 19.03.07
 *@version 1.0
 **
 ** CBM task class for digitizing MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchDigi out of CbmMuchPoint.
 **/

// Includes from MUCH
#include "CbmMuchDigitize.h"
#include "CbmMuchPoint.h"
#include "CbmMuchSector.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchPad.h"

// Includes from base
#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"

// Includes from ROOT
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TFile.h"

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
CbmMuchDigitize::CbmMuchDigitize() :
  FairTask("MuchDigitize", 1) {
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fDigiFile = NULL;
  fPoints = NULL;
  fDigis = NULL;
  fDigiMatches = NULL;
  fDTime = 8e-2;
  fNADCChannels = 256;
  fQMax = 440000;
  SetQThreshold(3);
  fMeanNoise = 0; //1500;
  fUseAvalanche = 0;
  SetDetectorType(kMICROMEGAS);
  fMeanGasGain = 1e4;
  fRnd = new TRandom3();
  fLandauRnd = new TRandom();

  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigitize::CbmMuchDigitize(Int_t iVerbose) :
  FairTask("MuchDigitize", iVerbose) {
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fDigiFile = NULL;
  fPoints = NULL;
  fDigis = NULL;
  fDigiMatches = NULL;
  fDTime = 8e-2;
  fNADCChannels = 256;
  fQMax = 440000;
  SetQThreshold(3);
  fMeanNoise = 0;//1500;
  fUseAvalanche = 0;
  SetDetectorType(kMICROMEGAS);
  fMeanGasGain = 1e4;
  fRnd = new TRandom3();
  fLandauRnd = new TRandom();

  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchDigitize::CbmMuchDigitize(const char* name, const char* digiFileName,
    Int_t iVerbose) :
  FairTask(name, iVerbose) {
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fDigiFile = new TFile(digiFileName);
  fPoints = NULL;
  fDigis = NULL;
  fDigiMatches = NULL;
  fDTime = 8e-2;
  fNADCChannels = 256;
  fQMax = 440000;
  SetQThreshold(3);
  fMeanNoise = 0;//1500;
  fUseAvalanche = 0;
  SetDetectorType(kMICROMEGAS);
  fMeanGasGain = 1e4;
  fRnd = new TRandom3();
  fLandauRnd = new TRandom();

  Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchDigitize::~CbmMuchDigitize() {
  if (fDigiFile)
    delete fDigiFile;
  if (fDigis) {
    fDigis->Delete();
    delete fDigis;
  }
  if (fDigiMatches) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }
  fUseAvalanche = 0;
  SetDetectorType(kMICROMEGAS);

  Reset();
}
// -------------------------------------------------------------------------


// ------- Private method ExecSimple ---------------------------------------
Bool_t CbmMuchDigitize::ExecSimple(CbmMuchPoint* point, Int_t iPoint) {
  // Get module for the point
  Int_t detectorId = point->GetDetectorID();
  CbmMuchModule* module = fGeoScheme->GetModuleByDetId(detectorId);
  if (!module)
    return kFALSE;
  if (module->GetNSectors() == 0) {
    fNOutside++;
    return kFALSE;
  }
  TVector3 modSize = module->GetSize();
  Double_t modLx = modSize[0];
  Double_t modLy = modSize[1];
  Double_t gridDx = module->GetGridDx();
  Double_t gridDy = module->GetGridDy();

  // Get entrance and exit coordinates of the point
  Double_t xIn = point->GetXIn();
  Double_t yIn = point->GetYIn();
  Double_t xOut = point->GetXOut();
  Double_t yOut = point->GetYOut();

  // Get avalanche center position
  Double_t x0 = (xIn + xOut) / 2;
  Double_t y0 = (yIn + yOut) / 2;

  // Translate to module center system
  TVector3 modPos = module->GetPosition();
  Double_t x0_mod = x0 - modPos[0];
  Double_t y0_mod = y0 - modPos[1];

  Int_t iGridColumn = (Int_t) ((x0_mod + modLx / 2.) / gridDx);
  Int_t iGridRow = (Int_t) ((y0_mod + modLy / 2.) / gridDy);

  if (iGridRow > module->GetGridRows() - 1 || iGridColumn
      > module->GetGridCols() - 1)
    return kFALSE;
  CbmMuchSector* sector = module->GetSector(iGridColumn, iGridRow);
  Int_t iChannel = -1;
  if (sector) {
    iChannel = sector->GetChannel(x0, y0);
    // Ask neighbour sectors if needed
    if (iChannel < 0) {
      vector<CbmMuchSector*> neighbours = sector->GetNeighbours();
      for (vector<CbmMuchSector*>::iterator it = neighbours.begin(); it
          != neighbours.end(); it++) {
        sector = *it;
        if (!sector)
          continue;
        iChannel = sector->GetChannel(x0, y0);
        if (iChannel >= 0)
          break;
      }
    }
  }
  if (iChannel < 0) {
    fNOutside++;
    return kFALSE;
  }

  Int_t iSector = sector->GetSectorIndex();                            // Sector index within the module
  Int_t channelId = CbmMuchGeoScheme::GetChannelId(iSector, iChannel); // Channel id within the module
  pair<Int_t, Int_t> uniqueId(detectorId, channelId);                  // Unique id of the channel within the MUCH
  Int_t iDigi = -1;
  if (fChannelMap.find(uniqueId) == fChannelMap.end()) {
    // Channel not yet active. Create new Digi and Match.
    iDigi = fDigis->GetEntriesFast();
    Double_t time = point->GetTime() + gRandom->Gaus(0, fDTime);
    new ((*fDigis)[iDigi]) CbmMuchDigi(detectorId, channelId, time, fDTime);
    new ((*fDigiMatches)[iDigi]) CbmMuchDigiMatch();
    CbmMuchDigiMatch* match =
        dynamic_cast<CbmMuchDigiMatch*> (fDigiMatches->At(iDigi));
    if (match)
      match->AddPoint(iPoint);
    // Match channelId to index of the Digi.
    fChannelMap[uniqueId] = iDigi;
  } else {
    // Channel already active.
    iDigi = fChannelMap[uniqueId];
    CbmMuchDigi* digi = dynamic_cast<CbmMuchDigi*> (fDigis->At(iDigi));
    Double_t time = point->GetTime() + gRandom->Gaus(0, fDTime);
    digi->AddTime(time); // add time info
    CbmMuchDigiMatch* match =
        dynamic_cast<CbmMuchDigiMatch*> (fDigiMatches->At(iDigi));
    if (match) {
      match->AddPoint(iPoint);
      fNMulti++;
    }
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// ------- Private method ExecAdvanced -------------------------------------
Bool_t CbmMuchDigitize::ExecAdvanced(CbmMuchPoint* point, Int_t iPoint) {
  // Get module for the point
  Int_t detectorId = point->GetDetectorID();
  CbmMuchModule* module = fGeoScheme->GetModuleByDetId(detectorId);
  if (!module)
    return kFALSE;
  if (module->GetNSectors() == 0) {
    fNOutside++;
    return kFALSE;
  }

  TVector3 modSize = module->GetSize();
  Double_t modLx = modSize[0];
  Double_t modLy = modSize[1];
  Double_t gridDx = module->GetGridDx();
  Double_t gridDy = module->GetGridDy();

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
  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(trackID);
  if (!mcTrack) {
    return kFALSE;
  }
  Int_t pdgCode = mcTrack->GetPdgCode();

  // Reject funny particles
  TParticlePDG *particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
  if (!particle) {
    return kFALSE;
  }
  if (TMath::Abs(particle->Charge()) < 0.1)
    return kFALSE;
  TVector3 momentum;                                            // 3-momentum of the particle
  point->Momentum(momentum);
  Double_t mom = momentum.Mag() * 1e3;                         // absolute momentum value [MeV/c]
  Double_t mom2 = mom * mom;                                    // squared momentum of the particle
  Double_t mass = particle->Mass() * 1e3;                       // mass of the particle [MeV/c^2]
  Double_t mass2 = mass * mass;                                 // squared mass of the particle
  Double_t Tkin = TMath::Sqrt(mom2 + mass2) - mass;            // kinetic energy of the particle
  Double_t sigma = CbmMuchDigitize::Sigma_n_e(Tkin, mass);      // sigma for Landau distribution
  Double_t mpv = CbmMuchDigitize::MPV_n_e(Tkin, mass);          // most probable value for Landau distr.
  UInt_t nElectrons = (UInt_t) fLandauRnd->Landau(mpv, sigma); // number of prim. electrons per 0.3 cm gap
  while (nElectrons > 100000)
    nElectrons = (UInt_t) (fLandauRnd->Landau(mpv, sigma));     // restrict Landau tail to increase performance
  // Number of electrons for current track length
  if (mass < 100.)
    nElectrons = (UInt_t) (nElectrons * lTrack / 0.47);
  else
    nElectrons = (UInt_t) (nElectrons * lTrack / 0.36);
  //********** Primary electrons from the track (end)   ***********************//

  Double_t hypotenuse = TMath::Sqrt(deltaX * deltaX + deltaY * deltaY);
  Double_t cosphi_tr = deltaX / hypotenuse; // cos of track azim. angle
  Double_t sinphi_tr = deltaY / hypotenuse; // sin of track azim. angle

  map<Int_t, CbmMuchDigi*> chargedPads;     // map from a channel id within the module to a fired digi
  Double_t time = point->GetTime();
  UInt_t nTrackCharge = 0;                  // total charge left by a track
  for (Int_t iElectron = 0; iElectron < nElectrons; iElectron++) {
    // Coordinates of primary electrons along the track
    Double_t aL = fRnd->Rndm() * hypotenuse;
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
      // Translate to module center
      TVector3 modPos = module->GetPosition();
      Double_t xVertMod = xVertex[iVertex] - modPos[0];
      Double_t yVertMod = yVertex[iVertex] - modPos[1];
      Int_t iGridColumn = (Int_t) ((xVertMod + modLx / 2.) / gridDx);
      Int_t iGridRow = (Int_t) ((yVertMod + modLy / 2.) / gridDy);
      if (iGridRow > module->GetGridRows() - 1 || iGridColumn
          > module->GetGridCols() - 1)
        continue;

      CbmMuchSector* sector = module->GetSector(iGridColumn, iGridRow);

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
        Double_t deltaX = xe - sector->GetPadX0(iChannel);
        Double_t deltaY = ye - sector->GetPadY0(iChannel);
        Double_t dist = TMath::Sqrt(deltaX * deltaX + deltaY * deltaY);
        if (dist > padRad + fSpotRadius)
          continue; // rough search

        TPolyLine* padPolygon = sector->GetPad(iChannel);
        Double_t area;
        if (!PolygonsIntersect(sector, *padPolygon, spotPolygon, area))
          continue; // detailed search
        UInt_t iCharge = (UInt_t) (nSecElectrons * area / spotArea);
        Int_t channelId = CbmMuchGeoScheme::GetChannelId(iSector, iChannel); // channel id within the module

        if (chargedPads.find(channelId) == chargedPads.end()) {
          chargedPads[channelId] = new CbmMuchDigi(detectorId,
              channelId, time, fDTime);
        }
        chargedPads[channelId]->AddCharge(iCharge); // add charge to digi
      } // loop channels
    } // loop fired sectors
  } // loop primary electrons

  if (chargedPads.size() == 0) {
    fNOutside++;
    return kFALSE;
  }

  for (map<Int_t, CbmMuchDigi*>::iterator it = chargedPads.begin(); it
      != chargedPads.end(); it++) {
    Int_t channelId = (*it).first;                      // Channel id within the module
    pair<Int_t, Int_t> uniqueId(detectorId, channelId); // Unique channel id within the MUCH
    CbmMuchDigi* digi = (*it).second;
    if (!digi)
      continue;
    Int_t iCharge = digi->GetCharge();
    if (iCharge < 0)
      iCharge = (Int_t) (TMath::Power(2, 31) - 2);
    if (fChargedPads.find(uniqueId) == fChargedPads.end()) {
      fChargedPads[uniqueId] = new CbmMuchDigi(digi);
      fChargedMatches[uniqueId] = new CbmMuchDigiMatch();
      fChargedMatches[uniqueId]->AddPoint(iPoint);
      fChargedMatches[uniqueId]->AddCharge(iCharge);
    } else {
      fChargedPads[uniqueId]->AddTime(time);
      fChargedPads[uniqueId]->AddCharge(iCharge);
      fChargedMatches[uniqueId]->AddPoint(iPoint);
      fChargedMatches[uniqueId]->AddCharge(iCharge);
      fNMulti++;
    }

    // Clear memory
    delete digi;
  }

  chargedPads.clear();

  return kTRUE;
  //**************  Simulate avalanche (end) **********************************//
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmMuchDigitize::Exec(Option_t* opt) {
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

  // Check for input arrays
  if (!fMCTracks) {
    cout << "-W- " << fName << "::Exec: No input array (MCTrack)" << endl;
    cout << "- " << fName << endl;
    return;
  }
  if (!fPoints) {
    cerr << "-W- " << fName << "::Exec: No input array (MuchPoint) "
        << endl;
    cout << "- " << fName << endl;
    return;
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

    CbmMuchStation* station = fGeoScheme->GetStationByDetId(point->GetDetectorID());


    if (station->GetDetectorType()==2) {
      ExecStraws(point, iPoint);
    }


    if (station->GetDetectorType()==1) {

      // Get the module the point is in
      CbmMuchModule* module = fGeoScheme->GetModuleByDetId(
          point->GetDetectorID());
      if (!module) {
        fNFailed++;
        continue;
      }

      // Produce Digis
      if (!fUseAvalanche) {
        if (!ExecSimple(point, iPoint))
          continue;
      } else {
        if (!ExecAdvanced(point, iPoint))
          continue;
      }

    }
  } // MuchPoint loop

  if (fUseAvalanche)
    FirePads();

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
void CbmMuchDigitize::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Finish   -----------------------------------------
void CbmMuchDigitize::FinishTask() {
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchDigitize::Init() {
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");

  // Initialize GeoScheme
  TObjArray* stations = (TObjArray*) fDigiFile->Get("stations");
  fGeoScheme->Init(stations);
  fGeoScheme->InitGrid();

  // Get input array of MuchPoints
  fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");

  // Get input array of MC tracks
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

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
InitStatus CbmMuchDigitize::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmMuchDigitize::Reset() {
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
void CbmMuchDigitize::FirePads() {
  // Add electronics noise
  if (fMeanNoise)
    AddNoise();

  // Apply threshold
  for (map<pair<Int_t, Int_t> , CbmMuchDigi*>::iterator it =
      fChargedPads.begin(); it != fChargedPads.end(); it++) {
    pair<Int_t, Int_t> uniqueId = (*it).first;
    CbmMuchDigi* digi = (*it).second;
    CbmMuchDigiMatch* match = fChargedMatches[uniqueId];
    if (digi->GetCharge() > fQThreshold) {
      Int_t iDigi = -1;
      if (fChannelMap.find(uniqueId) == fChannelMap.end()) {
        iDigi = fDigis->GetEntriesFast();
        digi->SetADCCharge(digi->GetCharge() / fNADCChannels);
        new ((*fDigis)[iDigi]) CbmMuchDigi(digi);
        new ((*fDigiMatches)[iDigi]) CbmMuchDigiMatch(match);
        fChannelMap[uniqueId] = iDigi;
      }
    }
    delete digi;
    delete match;
  }
}
// -------------------------------------------------------------------------

// -----   Private method AddNoise   ---------------------------------------
void CbmMuchDigitize::AddNoise() {
  vector<CbmMuchPad*> pads = fGeoScheme->GetPads();
  for (vector<CbmMuchPad*>::iterator it = pads.begin(); it != pads.end(); ++it) {
    AddNoise(*it);
  }
}
// -------------------------------------------------------------------------

// -----   Private method AddNoise   ---------------------------------------
void CbmMuchDigitize::AddNoise(CbmMuchPad* pad) {
  Double_t rndGaus = TMath::Abs(fMeanNoise * fRnd->Gaus());
  UInt_t iCharge = (UInt_t) rndGaus;
  Int_t detectorId = pad->GetDetectorId();
  Int_t channelId  = pad->GetChannelId();
  pair<Int_t, Int_t> uniqueId(detectorId, channelId);
  if (fChargedPads.find(uniqueId) == fChargedPads.end()) {
    if (iCharge <= fQThreshold)
      return;
    fChargedPads[uniqueId] = new CbmMuchDigi(pad->GetDetectorId(),
        channelId, 0, 0); // No time and Dtime info
    fChargedMatches[uniqueId] = new CbmMuchDigiMatch();
  }
  fChargedMatches[uniqueId]->AddCharge(iCharge);
  fChargedPads[uniqueId]->AddCharge(iCharge);
}
// -------------------------------------------------------------------------

// -----   Private method FirePads   ---------------------------------------
TPolyLine CbmMuchDigitize::GetPolygon(Double_t x0, Double_t y0, Double_t width,
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

Bool_t CbmMuchDigitize::ProjectionsIntersect(Double_t x11, Double_t x12,
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

Bool_t CbmMuchDigitize::PolygonsIntersect(CbmMuchSector* sector,
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

Int_t CbmMuchDigitize::GasGain() {
  //  const Double_t q_mean  = 1.e4;  // mean gas gain, arbitrary value
  Double_t gasGain = -fMeanGasGain * TMath::Log(1 - fRnd->Rndm());
  if (gasGain < 0.)
    gasGain = 1e6;
  return (Int_t) gasGain;
}

Double_t CbmMuchDigitize::Sigma_n_e(Double_t Tkin, Double_t mass) {
  Double_t logT;
  if (mass < 100) {
    logT = log(Tkin * 0.511 / mass);
    return CbmMuchDigitize::e_sigma_n_e(logT);
  } else if (mass >= 100 && mass < 200) {
    logT = log(Tkin * 105.658 / mass);
    return CbmMuchDigitize::mu_sigma_n_e(logT);
  } else {
    logT = log(Tkin * 938.272 / mass);
    return CbmMuchDigitize::p_sigma_n_e(logT);
  }
}

Double_t CbmMuchDigitize::MPV_n_e(Double_t Tkin, Double_t mass) {
  Double_t logT;
  if (mass < 100.) {
    logT = log(Tkin * 0.511 / mass);
    return CbmMuchDigitize::e_MPV_n_e(logT);
  } else if (mass >= 100. && mass < 200.) {
    logT = log(Tkin * 105.658 / mass);
    return CbmMuchDigitize::mu_MPV_n_e(logT);
  } else {
    logT = log(Tkin * 938.272 / mass);
    return CbmMuchDigitize::p_MPV_n_e(logT);
  }
}

Double_t CbmMuchDigitize::mu_sigma_n_e(Double_t &logT) {
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

Double_t CbmMuchDigitize::p_sigma_n_e(Double_t &logT) {
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

Double_t CbmMuchDigitize::e_sigma_n_e(Double_t &logT) {
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

Double_t CbmMuchDigitize::mu_MPV_n_e(Double_t &logT) {
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

Double_t CbmMuchDigitize::p_MPV_n_e(Double_t &logT) {
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

Double_t CbmMuchDigitize::e_MPV_n_e(Double_t &logT) {
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
void CbmMuchDigitize::SetDetectorType(DetectorType type) {
  switch (type) {
  case kGEM:
    fSpotRadius = 0.15;
    break;
  case kMICROMEGAS:
    fSpotRadius = 0.03;
    break;
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Bool_t CbmMuchDigitize::ExecStraws(CbmMuchPoint* point,Int_t iPoint){
  // Digitize straw tube MC point
  Int_t detectorId = point->GetDetectorID();

  //new ((*fDigis)[fNDigis]) CbmMuchDigi(stationNr, sectorNr, iChannel, time, fDTime);
  Int_t iDigi = fDigis->GetEntriesFast();
  CbmMuchDigi *digi = new ((*fDigis)[iDigi]) CbmMuchDigi(detectorId, 0, -1, 0);
  CbmMuchDigiMatch* match = new ((*fDigiMatches)[iDigi]) CbmMuchDigiMatch();
  match->AddPoint(iPoint);
  //cout << detID << " " << stationNr << " " << layer << " " << digi->GetDetectorId() << endl;
  Double_t coord[3];
  coord[0] = (point->GetXIn() + point->GetXOut()) / 2.;
  coord[1] = (point->GetYIn() + point->GetYOut()) / 2.;
  coord[2] = (point->GetZIn() + point->GetZOut()) / 2.;
  // Pass abs values of coordinates and their sign in some stupid manner
  Int_t signs = 0;
  for (Int_t i = 0; i < 3; ++i) {
    digi->AddTime(TMath::Abs(coord[i]));
    if (coord[i] < 0) signs |= (1 << i);
  }
  digi->SetUniqueID(signs);
  //cout << " Straw digi: " << coord[0] << " " << coord[1] << " " << coord[2] << " " << signs << endl;
  return kTRUE;
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchDigitize)

