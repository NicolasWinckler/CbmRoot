/** CbmMuchFindHitsStraws.cxx
 **/

#include "CbmMuchFindHitsStraws.h"
#include "CbmMuchDigi.h"
#include "CbmMuchHit.h"
#include "FairRootManager.h"



// -----   Default constructor   ------------------------------------------
CbmMuchFindHitsStraws::CbmMuchFindHitsStraws() : FairTask("MuchFindHits", 1) {
  fDigiFile    = NULL;
  fDigis   = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchFindHitsStraws::CbmMuchFindHitsStraws(Int_t iVerbose)
  : FairTask("MuchFindHits", iVerbose) {
  fDigiFile    = NULL;
  fDigis   = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchFindHitsStraws::CbmMuchFindHitsStraws(const char* name, const char* digiFileName, Int_t iVerbose)
  : FairTask(name, iVerbose) {
  fDigiFile    = new TFile(digiFileName);
  fDigis   = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchFindHitsStraws::~CbmMuchFindHitsStraws() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
}
// -------------------------------------------------------------------------


// -----   Private method SetParContainers   -------------------------------
void CbmMuchFindHitsStraws::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindHitsStraws::Init() {
  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");

  // Initialize GeoScheme
  TObjArray* stations = (TObjArray*) fDigiFile->Get("stations");
  fGeoScheme->Init(stations);

  // Get Hit array from memory
  fHits = (TClonesArray*) ioman->GetObject("MuchHit");
  // register if not in memory
  if (! fHits) {
    // Register hit array
    fHits = new TClonesArray("CbmMuchHit", 1000);
    ioman->Register("MuchHit", "Hit in MUCH", fHits, kTRUE);
  }

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchFindHitsStraws::ReInit() {
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmMuchFindHitsStraws::Exec(Option_t* opt) {
/*
  // Make hits in straw tubes

  static Int_t first = 1;
  static FairGeoRotation rotMatr[3];
  static Double_t radIn[4];
  Double_t diam[4] = {0.4, 0.4, 0.4, 0.4}; // tube diameters
  Double_t sigmaX = 0.02, sigmaY = 0.02; // 200um

  if (first) {
    // Some initialization - should go somewhere else
    cout << " Straws ... " << endl;
    first = 0;
    Double_t phi[3] = {0., 90., 45. }; // view rotation angles
    for (Int_t i = 0; i < 3; ++i) {
      phi[i] *= TMath::DegToRad();
      rotMatr[i].setElement(TMath::Cos(phi[i]),0);
      rotMatr[i].setElement(TMath::Cos(phi[i]),4);
      rotMatr[i].setElement(TMath::Sin(phi[i]),1);
      rotMatr[i].setElement(-TMath::Sin(phi[i]),3);
      rotMatr[i].print();
      rotMatr[i].invert();
      rotMatr[i].print();
    }
    // Get inner radia
    TObjArray* sensNodes = fGeoPar->GetGeoSensitiveNodes();
    for (Int_t i = 0; i < 3; ++i) {
      TString statName = Form("muchstation%02iL#1",begStation+3*i);
      FairGeoNode *node = (FairGeoNode*) sensNodes->FindObject(statName);
      TArrayD *pars = node->getParameters();
      //cout << pars->At(4) << endl;
      radIn[i] = pars->At(4);
    }
  }

  Int_t nDigis = fDigis->GetEntriesFast(), nHits = fHits->GetEntriesFast();
  TVector3 pos, dpos;
  Double_t xyz[3] = {0};
  for (Int_t idig = 0; idig < nDigis; ++idig) {
    CbmMuchDigi *digi = (CbmMuchDigi*) fDigis->UncheckedAt(idig);
    if (digi->GetStationNr() < begStation) continue; // not straw
    Int_t signs = digi->GetUniqueID();
    for (Int_t i = 0; i < 3; ++i) {
      xyz[i] = digi->GetTimes()[i];
      if (signs & (1<<i)) xyz[i] = -xyz[i];
      //cout << xyz[i] << " ";
      //if (i == 2) cout << endl;
    }

    Int_t layer = digi->GetSectorNr();
    Int_t station = digi->GetStationNr() - begStation;
    Int_t rot = station % 3;
    Int_t station3 = station / 3; // triple station
    FairGeoVector p(xyz[0], xyz[1], xyz[2]);
    FairGeoVector ploc = rotMatr[rot] * p;
    Double_t xloc = ploc.getX();
    if (layer % 2 != 0) xloc += diam[station3] / 2.; // half-tube shift
    Int_t itube = (Int_t) (xloc / diam[station3]);
    if (xloc < 0) itube--;
//    cout << itube << " " << station3 << endl;
    Double_t *times = digi->GetTimes();

    if (TMath::Abs(ploc.getX()) < radIn[station3]) {
      if (ploc.getY() > 0) times[1] = 1;
      else times[1] = -1;
    } else times[1] = 0;

    // Global coordinates in rotated coordinate system
    Double_t errU = gRandom->Gaus(0,sigmaX);
    //hit->SetU(ploc.getX()+errU);
    times[0] = ploc.getX() + errU;
    Double_t wXY = rotMatr[rot](1);

    pos.SetXYZ(xyz[0], xyz[1], xyz[2]);
    dpos.SetXYZ(sigmaX, sigmaY, 0.);
    // Change station No.
    station = begStation + station * 2 + layer;
    Int_t detId = digi->GetDetectorId();
    detId = detId & ~(255 << 16); // reset station No.
    detId |= (station << 16);   // station No. on bits 16-23

    times[2] = -77777;
    CbmMuchHit *hit =
      new ((*fHits)[nHits++]) CbmMuchHit(detId, pos, dpos, wXY, idig,
           times, digi->GetDTime());
    hit->SetCluster(itube); // tube number
    //new ((*fHits)[nHits++]) CbmMuchHit(detId, pos, dpos, wXY, iDigi, digi->GetTimes(), digi->GetDTime());
  }
*/
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchFindHitsStraws)
