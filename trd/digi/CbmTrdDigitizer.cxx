#include "CbmTrdDigitizer.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdGeoHandler.h"

#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::pair;
using std::make_pair;
using std::map;
using std::vector;

CbmTrdDigitizer::CbmTrdDigitizer(
      CbmTrdRadiator* radiator):
    FairTask("CbmTrdDigitizer"),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fTrdDigis(NULL),
    fTrdDigiMatches(NULL),
    fMCTracks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiator(radiator),
    fGeoHandler(new CbmTrdGeoHandler()),
    fDigiMap()
{
}

CbmTrdDigitizer::~CbmTrdDigitizer()
{
   fTrdDigis->Clear("C");
   delete fTrdDigis;
   fTrdDigiMatches->Clear("C");
   delete fTrdDigiMatches;
}

void CbmTrdDigitizer::SetParContainers()
{
   fDigiPar = static_cast<CbmTrdDigiPar*>(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdDigitizer::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   Fatal("CbmTrdDigitizer::Init()", "No TrdPoint array!");

   fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");
   Fatal("CbmTrdDigitizer::Init()", "No MCTrack array!");

   fTrdDigis = new TClonesArray("CbmTrdDigi", 100);
   ioman->Register("TrdDigi", "TRD Digis", fTrdDigis, kTRUE);

   fTrdDigiMatches = new TClonesArray("CbmTrdDigiMatch", 100);
   ioman->Register("TrdDigiMatch", "TRD Digis", fTrdDigiMatches, kTRUE);

   fGeoHandler->Init();

   fRadiator->Init();

   return kSUCCESS;
}

void CbmTrdDigitizer::Exec(Option_t * option)
{
   fDigiMap.clear();
   fTrdDigis->Clear();
   fTrdDigiMatches->Clear();

   Int_t nofTrdPoints = fTrdPoints->GetEntriesFast();
   for (Int_t iPoint = 0; iPoint < nofTrdPoints ; iPoint++ ) {
      // If random value above fEfficency reject point
      if (gRandom->Rndm() > fEfficiency ) continue;

      AddDigi(iPoint);
   }

   // Fill data from internally used stl map into output TClonesArray
   Int_t iDigi=0;
   map<pair< Int_t, pair< Int_t, Int_t > >, CbmTrdDigi* >::iterator it;
   for (it = fDigiMap.begin() ; it != fDigiMap.end(); it++) {
      CbmTrdDigi* digi = it->second;
      new ((*fTrdDigis)[iDigi]) CbmTrdDigi(digi->GetDetId(), digi->GetCol(), digi->GetRow(), digi->GetCharge(), digi->GetTime());

      CbmTrdDigiMatch* digiMatch = new ((*fTrdDigiMatches)[iDigi]) CbmTrdDigiMatch();

      vector<Int_t> arr = digi->GetMCIndex();
      vector<Int_t>::iterator itvec;
      for (itvec = arr.begin() ; itvec <arr.end(); itvec++  ) {
         Int_t bla = digiMatch->AddPoint((Int_t)*itvec);
      }
      iDigi++;
   }
}

void CbmTrdDigitizer::Finish()
{


}

void CbmTrdDigitizer::AddDigi(
      Int_t pointId)
{
   CbmTrdPoint* mcPoint = static_cast<CbmTrdPoint*>(fTrdPoints->At(pointId));
   if(NULL == mcPoint) return;

   CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcPoint->GetTrackID());
   if(NULL == mcTrack) return;

   TVector3 mom;
   mcPoint->Momentum(mom);

   Bool_t isElectron = abs(mcTrack->GetPdgCode()) == 11;
   Double_t energyLoss = (isElectron) ? mcPoint->GetEnergyLoss() + fRadiator->GetTR(mom) :  mcPoint->GetEnergyLoss();
   Int_t time = mcPoint->GetTime();

   Int_t col, row, sector;
   fModuleInfo = fDigiPar->GetModule(mcPoint->GetDetectorID());
   fModuleInfo->GetPadInfo(mcPoint, col, row, sector);
   Int_t detectorId = fGeoHandler->SetSector(mcPoint->GetDetectorID(), sector);

  // Add digi for pixel(x,y) in module to map for intermediate storage.
  // In case the pixel for this pixel/module combination does not exists it is added to the map.
  // In case it exists already the information about another hit in this
  // pixel is added. Also the additional energy loss is added to the pixel.

   // Look for pixel in charge map
   pair<Int_t, pair<Int_t,Int_t> > b(detectorId, make_pair(col, row));
   map<pair< Int_t, pair< Int_t, Int_t > >, CbmTrdDigi* >::iterator it = fDigiMap.find(b);

   if (it == fDigiMap.end()) { // Pixel not yet in map -> Add new pixel
      fDigiMap[b] = new CbmTrdDigi(detectorId, col, row, energyLoss, time, pointId);
   } else { // Pixel already in map -> Add charge
      CbmTrdDigi* digi = (CbmTrdDigi*) it->second;
      if (NULL == digi) Fatal("CbmTrdDigitizer::AddDigi", "Zero pointer in digi map!");
      digi->AddCharge(energyLoss);
      digi->AddMCIndex(pointId);
      if (time > (digi->GetTime())) digi->SetTime(time);
   }
}

ClassImp(CbmTrdDigitizer)
