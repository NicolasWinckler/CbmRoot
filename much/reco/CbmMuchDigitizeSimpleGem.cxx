/** CbmMuchDigitizeSimpleGem.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 19.03.07
 *@version 1.0
 **
 ** CBM task class for digitizing MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchDigi out of CbmMuchPoint.
 **/

// Includes from MUCH
#include "CbmMuchDigitizeSimpleGem.h"
#include "CbmMuchPoint.h"
#include "CbmMuchSector.h"
#include "CbmMuchStation.h"
#include "CbmMuchModuleGem.h"

// Includes from base
#include "FairRootManager.h"

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
CbmMuchDigitizeSimpleGem::CbmMuchDigitizeSimpleGem() :
  FairTask("MuchDigitizeAdvancedGem", 1) {
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fDigiFile = NULL;
  fPoints = NULL;
  fDigis = NULL;
  fDigiMatches = NULL;
  fDTime = 8e-2;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigitizeSimpleGem::CbmMuchDigitizeSimpleGem(Int_t iVerbose) :
  FairTask("MuchDigitizeAdvancedGem", iVerbose) {
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fDigiFile = NULL;
  fPoints = NULL;
  fDigis = NULL;
  fDigiMatches = NULL;
  fDTime = 8e-2;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchDigitizeSimpleGem::CbmMuchDigitizeSimpleGem(const char* name, const char* digiFileName,
    Int_t iVerbose) :
      FairTask(name, iVerbose) {
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fDigiFile = new TFile(digiFileName);
  fPoints = NULL;
  fDigis = NULL;
  fDigiMatches = NULL;
  fDTime = 8e-2;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchDigitizeSimpleGem::~CbmMuchDigitizeSimpleGem() {
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
  Reset();
}
// -------------------------------------------------------------------------


// ------- Private method ExecSimple ---------------------------------------
Bool_t CbmMuchDigitizeSimpleGem::ExecSimple(CbmMuchPoint* point, Int_t iPoint) {
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

  // Get entrance and exit coordinates of the point
  Double_t xIn = point->GetXIn();
  Double_t yIn = point->GetYIn();
  Double_t xOut = point->GetXOut();
  Double_t yOut = point->GetYOut();

  // Get avalanche center position
  Double_t x0 = (xIn + xOut) / 2;
  Double_t y0 = (yIn + yOut) / 2;

  CbmMuchSector* sector = module->GetSector(x0, y0);
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
  Long64_t channelId = CbmMuchModuleGem::GetChannelId(iSector, iChannel); // Channel id within the module
  pair<Int_t, Long64_t> uniqueId(detectorId, channelId);                  // Unique id of the channel within the MUCH
  Int_t iDigi = -1;
  CbmMuchDigiMatch* match;
  if (fChannelMap.find(uniqueId) == fChannelMap.end()) {
    // Channel not yet active. Create new Digi and Match.
    iDigi = fDigis->GetEntriesFast();
    Double_t time = point->GetTime() + gRandom->Gaus(0, fDTime);
    new ((*fDigis)[iDigi]) CbmMuchDigi(detectorId, channelId, time, fDTime);
    new ((*fDigiMatches)[iDigi]) CbmMuchDigiMatch();
    match = dynamic_cast<CbmMuchDigiMatch*> (fDigiMatches->At(iDigi));
      if (match){
        match->AddPoint(iPoint);
      }
      // Match channelId to index of the Digi.
      fChannelMap[uniqueId] = iDigi;

      CbmMuchDigi* digi = dynamic_cast<CbmMuchDigi*> (fDigis->At(iDigi));

      // Set ADC charge (for visualization)
      digi->SetADCCharge(100);
  } else {
    // Channel already active.
    iDigi = fChannelMap[uniqueId];
    CbmMuchDigi* digi = dynamic_cast<CbmMuchDigi*> (fDigis->At(iDigi));
    Double_t time = point->GetTime() + gRandom->Gaus(0, fDTime);
    match = dynamic_cast<CbmMuchDigiMatch*> (fDigiMatches->At(iDigi));
      if (match) {
        match->AddPoint(iPoint);
        fNMulti++;
      }
  }

  CbmMuchDigi* digi = dynamic_cast<CbmMuchDigi*> (fDigis->At(iDigi));
  if(digi) digi->SetADCCharge(100);

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmMuchDigitizeSimpleGem::Exec(Option_t* opt) {
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
    ExecSimple(point, iPoint);
  } // MuchPoint loop

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
void CbmMuchDigitizeSimpleGem::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Finish   -----------------------------------------
void CbmMuchDigitizeSimpleGem::FinishTask() {
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchDigitizeSimpleGem::Init() {
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");

  // Initialize GeoScheme
  TObjArray* stations = (TObjArray*) fDigiFile->Get("stations");
  fGeoScheme->Init(stations);

  // Get input array of MuchPoints
  fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");

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
InitStatus CbmMuchDigitizeSimpleGem::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmMuchDigitizeSimpleGem::Reset() {
  fNFailed = fNOutside = fNMulti = 0;
  fChannelMap.clear();
  if (fDigis)
    fDigis->Clear();
  if (fDigiMatches)
    fDigiMatches->Delete(); // Delete because of memory leaks
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchDigitizeSimpleGem)

