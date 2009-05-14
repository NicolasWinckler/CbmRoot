/** CbmMuchDigitizeStraws.cxx
 * Add comment
 **/

// Includes from MUCH
#include "CbmMuchDigitizeStraws.h"
#include "CbmMuchPoint.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"

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

using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::fixed;
using std::left;
using std::right;
using std::setprecision;

// -----   Default constructor   ------------------------------------------
CbmMuchDigitizeStraws::CbmMuchDigitizeStraws() :
    FairTask("MuchDigitize", 1) {
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
CbmMuchDigitizeStraws::CbmMuchDigitizeStraws(Int_t iVerbose) :
    FairTask("MuchDigitize", iVerbose) {
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
CbmMuchDigitizeStraws::CbmMuchDigitizeStraws(const char* name, const char* digiFileName,
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
CbmMuchDigitizeStraws::~CbmMuchDigitizeStraws() {
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

        CbmMuchStation* station = fGeoScheme->GetStationByDetId(point->GetDetectorID());


        if (station->GetDetectorType()==2) {
            ExecStraws(point, iPoint);
        }


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
    if (!ioman)
        Fatal("Init", "No FairRootManager");

    // Initialize GeoScheme
    TObjArray* stations = (TObjArray*) fDigiFile->Get("stations");
    fGeoScheme->Init(stations);
    fGeoScheme->InitGrid();

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


ClassImp(CbmMuchDigitizeStraws)

