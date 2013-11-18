/** CbmMuchDigitizeStraws.cxx
 * Add comment
 **/

// Includes from MUCH
#include "CbmMuchDigitizeStraws.h"
#include "CbmMuchPoint.h"
#include "CbmMuchModuleStraws.h"
#include "TRandom.h"
#include "CbmMuchStrawDigi.h"
// Includes from base
#include "FairRootManager.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::fixed;
using std::left;
using std::right;
using std::setprecision;

// -----   Default constructor   ------------------------------------------
CbmMuchDigitizeStraws::CbmMuchDigitizeStraws() 
  : FairTask("MuchDigitize", 1),
    fGeoScheme(CbmMuchGeoScheme::Instance()),
    fDigiFile(""),
    fPoints(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fNFailed(0),
    fNOutside(0),
    fNMulti(0),
    fDTime(8e-2),
    fEvent(0),
    fTimer()
{
  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigitizeStraws::CbmMuchDigitizeStraws(Int_t iVerbose) 
   : FairTask("MuchDigitize", iVerbose),
     fGeoScheme(CbmMuchGeoScheme::Instance()),
     fDigiFile(""),
     fPoints(NULL),
     fDigis(NULL),
     fDigiMatches(NULL),
     fNFailed(0),
     fNOutside(0),
     fNMulti(0),
     fDTime(8e-2),
     fEvent(0),
     fTimer()
{
  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchDigitizeStraws::CbmMuchDigitizeStraws(const char* name, const char* digiFileName,
    Int_t iVerbose) 
  : FairTask(name, iVerbose),
    fGeoScheme(CbmMuchGeoScheme::Instance()),
    fDigiFile(digiFileName),
    fPoints(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fNFailed(0),
    fNOutside(0),
    fNMulti(0),
    fDTime(8e-2),
    fEvent(0),
    fTimer()
{
  Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchDigitizeStraws::~CbmMuchDigitizeStraws() {
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
  Reset();
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmMuchDigitizeStraws::Exec(Option_t* opt) {
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

    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(point->GetDetectorID());
    if (!module) {
      fNFailed++;
      continue;
    }
    // Process only appropriate module types
    if(module->GetDetectorType()!=2) continue;
    // Produce Digis
    ExecStraws(point, iPoint);
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
void CbmMuchDigitizeStraws::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Finish   -----------------------------------------
void CbmMuchDigitizeStraws::FinishTask() {
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchDigitizeStraws::Init() {
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman) Fatal("Init", "No FairRootManager");

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

  // Get Digi array from memory
  fDigis = (TClonesArray*) ioman->GetObject("MuchStrawDigi");
  if (! fDigis) {
    fDigis = new TClonesArray("CbmMuchStrawDigi", 1000);
    ioman->Register("MuchStrawDigi", "Digi in MUCH", fDigis, kTRUE);
  }

  // Get DigiMatch array from memory
  fDigiMatches = (TClonesArray*) ioman->GetObject("MuchStrawDigiMatch");
  if (! fDigiMatches) {
    fDigiMatches = new TClonesArray("CbmMuchDigiMatch", 1000);
    ioman->Register("MuchStrawDigiMatch", "Digi Match in MUCH", fDigiMatches, kTRUE);
  }

  fEvent = 0;

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchDigitizeStraws::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmMuchDigitizeStraws::Reset() {
  fNFailed = fNOutside = fNMulti = 0;
  if (fDigis)
    fDigis->Clear();
  if (fDigiMatches)
    fDigiMatches->Delete(); // Delete because of memory leaks
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t CbmMuchDigitizeStraws::ExecStraws(CbmMuchPoint* point,Int_t iPoint){
  // Digitize straw tube MC point
  // (Pass coordinates in even mode stupid manner than before)
  Int_t detectorId = point->GetDetectorID();

  Float_t coord[3];
  coord[0] = (point->GetXIn() + point->GetXOut()) / 2.;
  coord[1] = (point->GetYIn() + point->GetYOut()) / 2.;
  coord[2] = (point->GetZIn() + point->GetZOut()) / 2.;
  //cout << "Digitizer: " << coord[0] << " " << coord[1] << " " << coord[2] << endl;
  Int_t iDigi = fDigis->GetEntriesFast();
  //cout << iDigi << endl; //AZ
  CbmMuchStrawDigi* digi  = new ((*fDigis)[iDigi]) CbmMuchStrawDigi(detectorId,coord[0],coord[1],coord[2],point->GetTime());
  CbmMuchDigiMatch* match = new ((*fDigiMatches)[iDigi]) CbmMuchDigiMatch();
  match->AddReference(iPoint);
  return kTRUE;
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchDigitizeStraws)

