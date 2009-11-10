/** CbmMuchFindHitsSimpleGem.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** CBM task class for finding hits in the MUCH by simple algorithm
 ** Task level RECO
 ** Produces objects of type CbmMuchHit out of CbmMuchDigi.
 **/

#include "CbmMuchFindHitsSimpleGem.h"
#include "CbmMuchSector.h"
#include "CbmMuchDigi.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchCluster.h"
#include "CbmMuchModuleGem.h"
#include "FairRootManager.h"

#include "TMath.h"

#include <iostream>
#include <iomanip>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::fixed;
using std::left;
using std::right;
using std::setprecision;
using std::vector;

// -----   Default constructor   ------------------------------------------
CbmMuchFindHitsSimpleGem::CbmMuchFindHitsSimpleGem() : FairTask("MuchFindHitsSimpleGem", 1) {
  fDigiFile    = NULL;
  fDigis   = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchFindHitsSimpleGem::CbmMuchFindHitsSimpleGem(Int_t iVerbose)
  : FairTask("MuchFindHitsSimpleGem", iVerbose) {
  fDigiFile    = NULL;
  fDigis   = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchFindHitsSimpleGem::CbmMuchFindHitsSimpleGem(const char* name, const char* digiFileName, Int_t iVerbose)
  : FairTask(name, iVerbose) {
  fDigiFile    = new TFile(digiFileName);
  fDigis   = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchFindHitsSimpleGem::~CbmMuchFindHitsSimpleGem() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
  if ( fClusters ) {
    fClusters->Delete();
    delete fClusters;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmMuchFindHitsSimpleGem::Exec(Option_t* opt) {
  Bool_t warn = kFALSE;

  fTimer.Start();

  // Clear output array
  if(fHits) fHits->Clear();
  if(fClusters) fClusters->Clear();
  Int_t nHits=0;

  if(!fDigis) printf("No digi array!!!\n");
  Int_t nDigis = fDigis->GetEntriesFast();
  for(Int_t iDigi = 0; iDigi < nDigis; ++iDigi){
    CbmMuchDigi* digi = (CbmMuchDigi*)fDigis->At(iDigi);
    if(!digi) continue;
    Int_t detectorId = digi->GetDetectorId();
    Long64_t channelId  = digi->GetChannelId();

    // Skip if digi was generated by straw digitizer
    CbmMuchModuleGem* module = (CbmMuchModuleGem*)fGeoScheme->GetModuleByDetId(detectorId);
    if(!module) continue;

    // Get sector and its properties
    CbmMuchSector* sector = module->GetSector(channelId);
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
    Int_t iChannel = CbmMuchModuleGem::GetChannelIndex(channelId);
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
    Int_t planeId = fGeoScheme->GetLayerSideNr(detectorId);
    new ((*fHits)[nHits]) CbmMuchPixelHit(detectorId, pos, dpos, 0, nHits, planeId);

    // Create a cluster consisting of only one digi
    vector<Int_t> digiIndices;
    digiIndices.push_back(iDigi);
    new ((*fClusters)[nHits++]) CbmMuchCluster(digiIndices, 0, 0);
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
void CbmMuchFindHitsSimpleGem::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindHitsSimpleGem::Init() {
  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");

  // Initialize GeoScheme
  TObjArray* stations = (TObjArray*) fDigiFile->Get("stations");
  fGeoScheme->Init(stations);

  // Register output array
  fHits = new TClonesArray("CbmMuchPixelHit", 1000);
  ioman->Register("MuchPixelHit", "Hit in MUCH", fHits, kTRUE);
  fClusters = new TClonesArray("CbmMuchCluster", 1000);
  ioman->Register("MuchCluster", "Cluster in MUCH", fClusters, kTRUE);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchFindHitsSimpleGem::ReInit() {
  return kSUCCESS;
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchFindHitsSimpleGem)
