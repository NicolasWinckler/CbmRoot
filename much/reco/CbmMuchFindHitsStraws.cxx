/**
 * \file CbmMuchFindHitsStraws.cxx
 **/

#include "CbmMuchFindHitsStraws.h"
#include "CbmMuchStrawDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchAddress.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchStation.h"
#include "CbmMuchStrawHit.h"
#include "FairRootManager.h"
#include "FairGeoRotation.h"
#include "TRandom.h" 
#include "TVector3.h" 
#include "TClonesArray.h"

#include <cassert>

CbmMuchFindHitsStraws::CbmMuchFindHitsStraws(const char* digiFileName)
  : FairTask("CbmMuchFindHitsStraws"),
    fGeoScheme( CbmMuchGeoScheme::Instance()),
    fDigiFile(digiFileName),
    fDigis(NULL),
    fDigiMatches(NULL),
    fHits(NULL),
    fEffic(0),
    fMerge(0),
    fMirror(0),
    fBinary(1),
    fPhis()
{
   SetPhis(0., 10, -10.);
}

CbmMuchFindHitsStraws::~CbmMuchFindHitsStraws()
{
   if ( fHits ) {
      fHits->Delete();
      delete fHits;
   }
}

InitStatus CbmMuchFindHitsStraws::Init()
{
  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchStrawDigi");
  fDigiMatches = (TClonesArray*) ioman->GetObject("MuchStrawDigiMatch");

  // Initialize GeoScheme
  TFile* oldfile=gFile;
  TFile* file=new TFile(fDigiFile);
  TObjArray* stations = (TObjArray*) file->Get("stations");
  file->Close();
  file->Delete();
  gFile=oldfile;
  fGeoScheme->Init(stations);

  // Register hit array
  fHits = new TClonesArray("CbmMuchStrawHit", 1000);
  ioman->Register("MuchStrawHit", "Straw hit in MUCH", fHits, kTRUE);

  return kSUCCESS;
}

void CbmMuchFindHitsStraws::Exec(Option_t* opt)
{
   if (fHits) fHits->Clear();

   static Int_t first = 1;
   static Double_t radIn[6];
   Double_t diam[6] = {0.62, 0.62, 0.62, 0.62, 0.62, 0.62}; // tube diameters
   Double_t sigmaX = 0.02, sigmaY = 0.02; // 200um
   const Double_t sigmaBin = diam[0] / TMath::Sqrt(12.);

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
         CbmMuchModule *mod = fGeoScheme->GetModule(i,0,0,0);
         cout << i << " " << radIn[i] << " " << mod->GetSize().X() << " " << mod->GetSize().Y() << " " << mod->GetSize().Z() << " " << mod->GetDetectorType() << endl;
         if (mod->GetDetectorType() == 2) assert (TMath::Abs(mod->GetSize().Z()-diam[i]) < 0.1); // tube diameter
      }
   }

   Int_t iarray[5] = {0}, nDigis = fDigis->GetEntriesFast(), nHits = fHits->GetEntriesFast();
   TVector3 pos, dpos;
   Double_t xyz[3] = {0}, array[5] = {0};
   for (Int_t idig = 0; idig < nDigis; ++idig) {
      CbmMuchStrawDigi *digi = (CbmMuchStrawDigi*) fDigis->UncheckedAt(idig);
      xyz[0] = digi->GetX();
      xyz[1] = digi->GetY();
      xyz[2] = digi->GetZ();
      UInt_t address =  CbmMuchAddress::GetElementAddress(digi->GetAddress(),kMuchModule);
      Int_t station3 = CbmMuchAddress::GetStationIndex(address);
      Int_t layer = CbmMuchAddress::GetLayerIndex(address);
      Int_t side = CbmMuchAddress::GetLayerSideIndex(address);
      Int_t rot = layer % 3;
      Double_t plocX =  xyz[0] * TMath::Cos(phi[rot]) + xyz[1] * TMath::Sin(phi[rot]);
      Double_t plocY = -xyz[0] * TMath::Sin(phi[rot]) + xyz[1] * TMath::Cos(phi[rot]);
      //    std::cout << "phi=" << phi[rot] << " x=" << xyz[0] << " y=" << xyz[1] << " z=" << xyz[2] << " plocX=" << plocX << " plocY=" << plocY << std::endl;
      Double_t xloc = plocX;
      //cout << " Local: " << ploc.getX() << " " << ploc.getY() << " " << ploc.getZ() << endl;
      if (side) xloc += diam[station3] / 2.; // half-tube shift
      Int_t itube = (Int_t) (xloc / diam[station3]), iSegment;
      if (xloc < 0) itube--;
      Double_t xwire = (itube + 0.5) * diam[station3]; // wire position
      //cout << itube << " " << layer << " " << side << " " << xloc << " " << xwire << endl;
      Double_t times[3] = {0};

      /* No segmentation outside beam hole region
      if (TMath::Abs(plocX) < radIn[station3]) {
      if (plocY > 0) iSegment = 1;
      else iSegment = -1;
      } else iSegment = 0;
      */
      if (plocY > 0) iSegment = 1;
      else iSegment = -1;

      // Global coordinates in rotated coordinate system
      Double_t errU = gRandom->Gaus(0,sigmaX);
      Double_t wXY = TMath::Sin(phi[rot]);
      //cout << station3 << " " << layer << " " << " " << wXY << endl;

      pos.SetXYZ(xyz[0], xyz[1], xyz[2]);
      dpos.SetXYZ(sigmaX, sigmaY, 0.);

      //CbmMuchStrawHit(detectorId,u,phi,z,du,dphi,dz,refId,planeId);
      //cout << " Local: " << ploc.getX()+errU << " " << ploc.getY() << " " << ploc.getZ() << endl;
      CbmMuchStrawHit *hit = new ((*fHits)[nHits++]) CbmMuchStrawHit(address,
            plocX + errU, TMath::ASin(wXY), pos[2], sigmaX, 0, 0,
               idig, fGeoScheme->GetLayerSideNr(address));
      hit->SetX(pos[0]);
      hit->SetY(pos[1]);
      hit->SetTube(itube);
      hit->SetSegment(iSegment);
      //hit->SetZ(fGeoScheme->GetLayerSideByDetId(detId)->GetZ());
      iarray[0] = station3;
      iarray[1] = itube;
      array[0] = xwire - diam[station3] / 2. * side; // Xwire in global system
      array[1] = xloc - xwire; // drift distance
      array[2] = array[1] + errU; // drift distance with error
      hit->SetDouble(3, array);
      hit->SetInt(2, iarray);
      hit->SetFlag(0);
      if (fBinary) {
         hit->SetU (array[0]);
         hit->SetDu (sigmaBin);
      }
   }

   // Apply inefficiency (currently inside tube walls)
   if (fEffic) Effic(diam);
   // Merge hits inside the same tube
   if (fMerge) Merge();
   // Add mirror hits (left/right ambiguity)
   if (fMirror) Mirror();

   static Int_t eventNo = 0;
   LOG(INFO) << "CbmMuchFindHitsStraws::Exec eventNo=" << eventNo << " hits="
            << fHits->GetEntriesFast() << " digis=" << fDigis->GetEntriesFast() << FairLogger::endl;
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
   LOG(INFO) << "CbmMuchFindHitsStraws::Effic hits=" << nHits << " corrected hits="
         << fHits->GetEntriesFast() << FairLogger::endl;
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
      CbmMuchDigiMatch *digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hit->GetRefId());
      //CbmMuchDigiMatch *digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(ihit);

      for (Int_t jhit = ihit+1; jhit < nHits; ++jhit) {
      CbmMuchStrawHit *hit1 = (CbmMuchStrawHit*) fHits->UncheckedAt(jhit);
      if (hit1 == 0x0) continue;

      if (hit1->GetTube() != hit->GetTube()) continue; // different tubes
      if (hit1->GetAddress() != hit->GetAddress()) continue; // different layers
      if (hit1->GetSegment() != hit->GetSegment()) continue; // upper and lower tubes

      Double_t drift1 = TMath::Abs (hit1->GetDouble()[1]);
      CbmMuchDigiMatch *digiM1 = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hit1->GetRefId());
      //CbmMuchDigiMatch *digiM1 = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(jhit);
      //cout << jhit << " " << hit1->GetRefId() << endl;
      if (drift < drift1) {
         fHits->RemoveAt(jhit);
         Int_t nPoints = digiM1->GetNofLinks();
         for (Int_t j = 0; j < nPoints; ++j) {
            hit->SetFlag(hit->GetFlag()+(1<<1)); // increase overlap multiplicity
            digiM->AddLink(digiM1->GetMatchedLink()); // add point
         }
      } else {
         fHits->RemoveAt(ihit);
         Int_t nPoints = digiM->GetNofLinks();
         for (Int_t j = 0; j < nPoints; ++j) {
            hit1->SetFlag(hit1->GetFlag()+(1<<1));
            digiM1->AddLink(digiM->GetMatchedLink());
         }
         break;
      }
      }
   }
   fHits->Compress();
   LOG(INFO) << "CbmMuchFindHitsStraws::Merge hits=" << nHits << " merged hits="
         << fHits->GetEntriesFast() << FairLogger::endl;
}

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
   }
}

ClassImp(CbmMuchFindHitsStraws)
