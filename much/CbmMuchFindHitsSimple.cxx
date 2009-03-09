/** CbmMuchFindHitsSimple.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** CBM task class for finding hits in the MUCH by simple algorithm
 ** Task level RECO
 ** Produces objects of type CbmMuchHit out of CbmMuchDigi.
 **/

#include "CbmMuchFindHitsSimple.h"
#include "CbmMuchDigi.h"
#include "CbmMuchHit.h"

#include "FairRootManager.h"

#include "TMath.h"
#include "TVector3.h"

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
CbmMuchFindHitsSimple::CbmMuchFindHitsSimple() : CbmMuchTask("MuchFindHits", 1) {
  fDigiFile    = NULL;
  fDigis   = NULL;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchFindHitsSimple::CbmMuchFindHitsSimple(Int_t iVerbose)
  : CbmMuchTask("MuchFindHits", iVerbose) {
  fDigiFile    = NULL;
  fDigis   = NULL;
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchFindHitsSimple::CbmMuchFindHitsSimple(const char* name, TFile* digiFile, Int_t iVerbose)
  : CbmMuchTask(name, iVerbose) {
  fDigiFile    = digiFile;
  fDigis   = NULL;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchFindHitsSimple::~CbmMuchFindHitsSimple() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmMuchFindHitsSimple::Exec(Option_t* opt) {
  Bool_t warn = kFALSE;

  fTimer.Start();

  // Clear output array
  if(fHits) fHits->Clear();
  Int_t nHits=0;

  Int_t nDigis = fDigis->GetEntriesFast();
  for(Int_t iDigi = 0; iDigi < nDigis; ++iDigi){
    CbmMuchDigi* digi = (CbmMuchDigi*)fDigis->At(iDigi);
    if(!digi) continue;
    Int_t detectorId = digi->GetDetectorId();
    Int_t channelId  = digi->GetChannelId();
    // Get sector and its properties
    CbmMuchSector* sector = fGeoScheme->GetSectorByDetId(detectorId, channelId);
    if(!sector) continue;
    Double_t xc     = sector->GetPosition()[0];
    Double_t yc     = sector->GetPosition()[1];
    Double_t z      = sector->GetPosition()[2];
    Double_t lx     = sector->GetSize()[0];
    Double_t ly     = sector->GetSize()[1];
    Int_t nColumns  = sector->GetNCols();
    Int_t nRows     = sector->GetNRows();
    Double_t dx     = lx/nColumns;
    Double_t dy     = ly/nRows;
    Double_t sigmaX = dx/TMath::Sqrt(12.);
    Double_t sigmaY = dy/TMath::Sqrt(12.);

    // Find hit coordinates
    Int_t iChannel = CbmMuchGeoScheme::GetChannelIndex(channelId);
    Int_t iRow  = Int_t( iChannel / nColumns );
    Int_t iCol  = iChannel - iRow * nColumns;
    Double_t xint = ( Double_t(iCol) + 0.5 ) * dx;
    Double_t yint = ( Double_t(iRow) + 0.5 ) * dy;

    // Translation to center of sector
    xint = xint - lx/2.;
    yint = yint - ly/2.;

    // Translation into global c.s.
    Double_t x = xint + xc;
    Double_t y = yint + yc;

    // Make new hit
    TVector3 pos, dpos;
    pos.SetXYZ(x, y, z);
    dpos.SetXYZ(sigmaX, sigmaY, 0.);
    new ((*fHits)[nHits++]) CbmMuchHit(detectorId, pos, dpos, 0, iDigi, digi->GetTimes(), digi->GetDTime());
  }

  fTimer.Stop();
  if ( fVerbose ) {
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Active channels           : " << fDigis->GetEntriesFast() << endl;
    cout << "    Hits created              : " << fHits->GetEntriesFast()   << endl;
    cout << "    Real time                 : " << fTimer.RealTime() << endl;
  }
  else {
    if ( warn ) cout << "- ";
    else        cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
	 << fixed << right << fTimer.RealTime()
	 << " s, digis " << fDigis->GetEntriesFast() << ", hits: "
	 << fHits->GetEntriesFast() << endl;
  }
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void CbmMuchFindHitsSimple::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindHitsSimple::Init() {
  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");

  // Initialize GeoScheme
  TObjArray* stations = (TObjArray*) fDigiFile->Get("stations");
  fGeoScheme->Init(stations);

  // Register output array
  fHits = new TClonesArray("CbmMuchHit", 1000);
  ioman->Register("MuchHit", "Hit in MUCH", fHits, kTRUE);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchFindHitsSimple::ReInit() {
  return kSUCCESS;
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchFindHitsSimple)
