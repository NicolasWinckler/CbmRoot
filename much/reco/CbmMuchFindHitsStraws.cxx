/** CbmMuchFindHitsStraws.cxx
 **/

#include "CbmMuchFindHitsStraws.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchStation.h"
#include "CbmMuchStrawHit.h"
#include "FairRootManager.h"
#include "FairGeoRotation.h"
#include "TRandom.h" 
#include "TVector3.h" 
#include <iomanip>

using std::cout;
using std::endl;
using std::setw;
using std::fixed;
using std::left;
using std::right;
using std::setprecision;

// -----   Default constructor   ------------------------------------------
CbmMuchFindHitsStraws::CbmMuchFindHitsStraws() 
  : FairTask("MuchFindHits", 1),
    fEffic(0),fMerge(0),fMirror(0)
{
  fDigiFile    = NULL;
  fDigis   = fDigiMatches = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  //SetPhis(0., 90., 45.);
  SetPhis(-10., 0., 10.);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchFindHitsStraws::CbmMuchFindHitsStraws(Int_t iVerbose)
  : FairTask("MuchFindHits", iVerbose), 
    fEffic(0),fMerge(0),fMirror(0)
{
  fDigiFile    = NULL;
  fDigis   = fDigiMatches = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  //SetPhis(0., 90., 45.);
  SetPhis(-10., 0., 10.);
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchFindHitsStraws::CbmMuchFindHitsStraws(const char* name, const char* digiFileName, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fEffic(0),fMerge(0),fMirror(0)
{
  fDigiFile    = new TFile(digiFileName);
  fDigis   = fDigiMatches = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  //SetPhis(0., 90., 45.);
  SetPhis(-10., 0, 10.);
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
  fDigis = (TClonesArray*) ioman->GetObject("MuchStrawDigi");
  fDigiMatches = (TClonesArray*) ioman->GetObject("MuchStrawDigiMatch");

  // Initialize GeoScheme
  TObjArray* stations = (TObjArray*) fDigiFile->Get("stations");
  fGeoScheme->Init(stations);

  // Register hit array
  fHits = new TClonesArray("CbmMuchStrawHit", 1000);
  ioman->Register("MuchStrawHit", "Straw hit in MUCH", fHits, kTRUE);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchFindHitsStraws::ReInit() {
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmMuchFindHitsStraws::Exec(Option_t* opt)
{
  //*
  // Make hits in straw tubes

  fTimer.Start();
  // Clear output array
  if(fHits) fHits->Clear();

  static Int_t first = 1;
  static Double_t radIn[6];
  Double_t diam[6] = {0.42, 0.42, 0.42, 0.42, 0.42, 0.42}; // tube diameters
  Double_t sigmaX = 0.02, sigmaY = 0.02; // 200um

  Double_t phi[3] = {fPhis[0] * TMath::DegToRad(), fPhis[1] * TMath::DegToRad(), fPhis[2] * TMath::DegToRad()}; // rotation angles of views (doublets)

  if (first) {
    // Some initialization - should go somewhere else
    cout << " Processing straws ... " << endl;
    first = 0;
    // Get inner radia of stations
    Int_t nSt = fGeoScheme->GetNStations();
    for (Int_t i = 0; i < nSt; ++i) {
      CbmMuchStation *st = fGeoScheme->GetStation(i);
      radIn[i] = st->GetRmin();
      cout << i << " " << radIn[i] << endl;
    }
  }

  Int_t iarray[5] = {0}, nDigis = fDigis->GetEntriesFast(), nHits = fHits->GetEntriesFast();
  TVector3 pos, dpos;
  Double_t xyz[3] = {0}, array[5] = {0};
  for (Int_t idig = 0; idig < nDigis; ++idig) {
    CbmMuchDigi *digi = (CbmMuchDigi*) fDigis->UncheckedAt(idig);
    xyz[0] = digi->GetTime();
    xyz[1] = digi->GetDTime();
    UInt_t iz = digi->GetADCCharge();
    xyz[2] = *((Float_t*) &iz);
    //cout << xyz[0] << " " << xyz[1] << " " << xyz[2] << endl;

    Int_t detId =  digi->GetDetectorId();
    Int_t station3 = fGeoScheme->GetStationIndex(detId);
    Int_t layer = fGeoScheme->GetLayerIndex(detId);
    Int_t rot = layer % 3;
    Double_t plocX = xyz[0] * TMath::Cos(phi[rot]) + xyz[1] * TMath::Sin(phi[rot]);
    Double_t plocY = -xyz[0] * TMath::Sin(phi[rot]) + xyz[1] * TMath::Cos(phi[rot]);
//    std::cout << "phi=" << phi[rot] << " x=" << xyz[0] << " y=" << xyz[1] << " z=" << xyz[2] << " plocX=" << plocX << " plocY=" << plocY << std::endl;
    Double_t xloc = plocX;
    //cout << " Local: " << ploc.getX() << " " << ploc.getY() << " " << ploc.getZ() << endl;
    if (layer % 2 != 0) xloc += diam[station3] / 2. * TMath::Sign(1.,xloc); // half-tube shift
    Int_t itube = (Int_t) (xloc / diam[station3]), iSegment;
    if (xloc < 0) itube--;
//    cout << itube << " " << station3 << endl;
    Double_t xwire = (itube + 0.5) * diam[station3]; // wire position
    Double_t times[3] = {0};

    if (TMath::Abs(plocX) < radIn[station3]) {
      if (plocY > 0) iSegment = 1;
      else iSegment = -1;
    } else iSegment = 0;

    // Global coordinates in rotated coordinate system
    Double_t errU = gRandom->Gaus(0,sigmaX);
    Double_t wXY = TMath::Sin(phi[rot]);
    //cout << station3 << " " << layer << " " << " " << wXY << endl;

    pos.SetXYZ(xyz[0], xyz[1], xyz[2]);
    dpos.SetXYZ(sigmaX, sigmaY, 0.);

    //CbmMuchStrawHit(detectorId,u,phi,z,du,dphi,dz,refId,planeId);
    //cout << " Local: " << ploc.getX()+errU << " " << ploc.getY() << " " << ploc.getZ() << endl;
    CbmMuchStrawHit *hit = new ((*fHits)[nHits++]) CbmMuchStrawHit(detId,
			   plocX + errU, TMath::ASin(wXY), pos[2], sigmaX, 0, 0,
                           idig, fGeoScheme->GetLayerSideNr(detId));
    hit->SetX(pos[0]);
    hit->SetY(pos[1]);
    hit->SetTube(itube);
    hit->SetSegment(iSegment);
    //hit->SetZ(fGeoScheme->GetLayerSideByDetId(detId)->GetZ());
    iarray[0] = station3;
    iarray[1] = itube;
    array[0] = xwire;
    array[1] = xloc - xwire; // drift distance
    array[2] = array[1] + errU; // drift distance with error
    hit->SetDouble(3, array);
    hit->SetInt(2, iarray);
  }
  //*/

  // Apply inefficiency (currently inside tube walls)
  if (fEffic) Effic(diam);
  // Merge hits inside the same tube
  if (fMerge) Merge();
  // Add mirror hits (left/right ambiguity)
  if (fMirror) Mirror();

  fTimer.Stop();
  cout << "+ ";
  cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
       << fixed << right << fTimer.RealTime()
       << " s, digis " << fDigis->GetEntriesFast() << ", hits: "
       << fHits->GetEntriesFast() << endl;
}
// -------------------------------------------------------------------------


// ---------   Private method Effic   -------------------------------
void CbmMuchFindHitsStraws::Effic(Double_t *diam) 
{
  // Apply straw inefficiency (currently inside tube walls)

  Int_t nHits = fHits->GetEntriesFast();
  for (Int_t ihit = 0; ihit < nHits; ++ihit) {
    CbmMuchStrawHit *hit = (CbmMuchStrawHit*) fHits->UncheckedAt(ihit);

    // Apply inefficiency
    Double_t drift = hit->GetDouble()[1];
    Int_t station = hit->GetInt()[0];
    if (TMath::Abs(drift) < diam[station]/2-0.01) continue; // outside tube wall
    fHits->RemoveAt(ihit);
  }
  fHits->Compress();
  cout << " *** Effic - Number of original hits: " << nHits << endl;
  cout << " *** Effic - Number of efficiency corrected hits: " << fHits->GetEntriesFast() << endl;
}

// ---------   Private method Merge   -----------------------------
void CbmMuchFindHitsStraws::Merge() 
{
  // Merge hits inside the same tube

  Int_t nHits = fHits->GetEntriesFast();
  for (Int_t ihit = 0; ihit < nHits; ++ihit) {
    CbmMuchStrawHit *hit = (CbmMuchStrawHit*) fHits->UncheckedAt(ihit);
    if (hit == 0x0) continue;
    Double_t drift = TMath::Abs (hit->GetDouble()[1]);
    //CbmMuchDigiMatch *digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hit->GetDigi());
    CbmMuchDigiMatch *digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(ihit);

    for (Int_t jhit = ihit+1; jhit < nHits; ++jhit) {
      CbmMuchStrawHit *hit1 = (CbmMuchStrawHit*) fHits->UncheckedAt(jhit);
      if (hit1 == 0x0) continue;

      if (hit1->GetTube() != hit->GetTube()) continue; // different tubes
      if (hit1->GetDetectorId() != hit->GetDetectorId()) continue; // different layers
      if (hit1->GetSegment() != hit->GetSegment()) continue; // upper and lower tubes

      Double_t drift1 = TMath::Abs (hit1->GetDouble()[1]);
      //CbmMuchDigiMatch *digiM1 = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hit1->GetDigi());
      CbmMuchDigiMatch *digiM1 = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(jhit);
      if (drift < drift1) {
	fHits->RemoveAt(jhit);
	hit->SetFlag(hit->GetFlag()+(1<<1)); // increase overlap multiplicity
	digiM->AddPoint(digiM1->GetRefIndex()); // add point
      } else {
	fHits->RemoveAt(ihit);
	hit1->SetFlag(hit1->GetFlag()+(1<<1)); 
	digiM1->AddPoint(digiM->GetRefIndex());  
	break;
      }
    }
  }
  fHits->Compress();
  cout << " *** Merge - Number of original hits: " << nHits << endl;
  cout << " *** Merge - Number of merged hits: " << fHits->GetEntriesFast() << endl;
}

// ---------   Private method Mirror   ----------------------------
void CbmMuchFindHitsStraws::Mirror() 
{
  // Add mirror hits (left/right ambiguity)

  Int_t nHits0 = fHits->GetEntriesFast();
  Int_t nHits = nHits0;
  for (Int_t ihit = 0; ihit < nHits0; ++ihit) {
    CbmMuchStrawHit *hit = (CbmMuchStrawHit*) fHits->UncheckedAt(ihit);

    // Add mirror hit
    Double_t xwire = hit->GetDouble()[0];
    Double_t drift = hit->GetDouble()[2];
    CbmMuchStrawHit *hitM = new ((*fHits)[nHits++]) CbmMuchStrawHit(*hit);
    hitM->SetU(hit->GetU()-2*drift);
    hitM->SetFlag(hitM->GetFlag()+1); // flag mirror hit
    cout << " Mirror: " << hit->GetU() << " " << xwire << " " << hitM->GetU() << endl;
  }
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchFindHitsStraws)
