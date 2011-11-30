/**
 * \file CbmLitMCTrackCreator.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#include "CbmLitMCTrackCreator.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairMCPoint.h"
#include "CbmMvdPoint.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDetectorId.h"
#include "CbmMuchGeoScheme.h"
#include "CbmTrdDetectorId.h"
#include "CbmMCTrack.h"

#include "TDatabasePDG.h"

CbmLitMCTrackCreator::CbmLitMCTrackCreator():
   fMCTracks(NULL),
   fMvdPoints(NULL),
   fStsPoints(NULL),
   fTrdPoints(NULL),
   fMuchPoints(NULL),
   fTofPoints(NULL),
   fLitMCTracks()
{
   ReadDataBranches();
}

CbmLitMCTrackCreator::~CbmLitMCTrackCreator()
{

}

CbmLitMCTrackCreator* CbmLitMCTrackCreator::Instance()
{
   static CbmLitMCTrackCreator instance;
   return &instance;
}

void CbmLitMCTrackCreator::Create()
{
   FillStationMaps();

   fLitMCTracks.clear();

   AddPoints(kMVD, fMvdPoints);
   AddPoints(kSTS, fStsPoints);
   AddPoints(kTRD, fTrdPoints);
   AddPoints(kMUCH, fMuchPoints);
   AddPoints(kTOF, fTofPoints);

//   std::cout << "CbmLitMCTrackCreator: nof MC tracks=" << fLitMCTracks.size() << std::endl;
//   std::map<int, CbmLitMCTrack>::iterator it;
//   for (it = fLitMCTracks.begin(); it != fLitMCTracks.end(); it++)
//       std::cout << (*it).first << " => " << (*it).second;
}

void CbmLitMCTrackCreator::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
   fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
   fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
}

void CbmLitMCTrackCreator::AddPoints(
      DetectorId detId,
      const TClonesArray* array)
{
   if (!array) return;
   int nofPoints = array->GetEntriesFast();
   for (int iPoint = 0; iPoint < nofPoints; iPoint++) {
      FairMCPoint* fairPoint = static_cast<FairMCPoint*>(array->At(iPoint));
      CbmLitMCPoint litPoint;
      int stationId = -1;
      if (detId == kMVD) stationId = fMvdStationsMap[iPoint]; else
      if (detId == kSTS) stationId = fStsStationsMap[iPoint]; else
      if (detId == kTRD) stationId = fTrdStationsMap[iPoint]; else
      if (detId == kMUCH) stationId = fMuchStationsMap[iPoint];
      if (stationId < 0) continue;
      FairMCPointToLitMCPoint(fairPoint, &litPoint, iPoint, stationId);
      fLitMCTracks[fairPoint->GetTrackID()].AddPoint(detId, litPoint);
   }
}

void CbmLitMCTrackCreator::FairMCPointToLitMCPoint(
      const FairMCPoint* fairPoint,
      CbmLitMCPoint* litPoint,
      int refId,
      int stationId)
{
   litPoint->SetX(fairPoint->GetX());
   litPoint->SetY(fairPoint->GetY());
   litPoint->SetZ(fairPoint->GetZ());
   litPoint->SetPx(fairPoint->GetPx());
   litPoint->SetPy(fairPoint->GetPy());
   litPoint->SetPz(fairPoint->GetPz());
   litPoint->SetRefId(refId);
   litPoint->SetStationId(stationId);
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(fairPoint->GetTrackID()));
   double charge = TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode())->Charge();
   myf q = (charge > 0) ? 1. : -1;
   litPoint->SetQ(q);
}

void CbmLitMCTrackCreator::FillStationMaps()
{
   fMvdStationsMap.clear();
   fStsStationsMap.clear();
   fTrdStationsMap.clear();
   fMuchStationsMap.clear();

   // MVD
   if (NULL != fMvdPoints) {
      Int_t nofMvdPoints = fMvdPoints->GetEntriesFast();
       for (Int_t iPoint = 0; iPoint < nofMvdPoints; iPoint++) {
          CbmMvdPoint* point = static_cast<CbmMvdPoint*>(fMvdPoints->At(iPoint));
          fMvdStationsMap[iPoint] = point->GetStationNr() - 1;
       }
   }
   // end MVD

   // STS
   if (NULL != fStsPoints) {
      FairRunAna* run = FairRunAna::Instance();
      FairRuntimeDb* runDb = run->GetRuntimeDb();
      CbmGeoStsPar* stsGeoPar = (CbmGeoStsPar*) runDb->getContainer("CbmGeoStsPar");
      TObjArray* stsNodes = stsGeoPar->GetGeoSensitiveNodes();
      Int_t nofStsStations = stsNodes->GetEntries();
      std::map<Int_t, Int_t> stsStationNrFromMcId;
      for (Int_t ist = 0; ist < nofStsStations; ist++) {
        FairGeoNode* stsNode = (FairGeoNode*) stsNodes->At(ist);
        std::string stsNodeName(stsNode->GetName());
        std::string stsStationNr = stsNodeName.substr(10, 2);
        int stationNr = atoi(stsStationNr.c_str());
        stsStationNrFromMcId[stsNode->getMCid()] = stationNr - 1;
      }

      CbmStsDetectorId stsDetectorId;
      Int_t nofStsPoints = fStsPoints->GetEntriesFast();
      for (Int_t iPoint = 0; iPoint < nofStsPoints; iPoint++) {
         FairMCPoint* point = static_cast<FairMCPoint*>(fStsPoints->At(iPoint));
         Int_t stationId = stsStationNrFromMcId[point->GetDetectorID()];
         fStsStationsMap[iPoint] = stationId;
      }
   }
   // end STS

   // MUCH
   if (NULL != fMuchPoints) {
      Int_t nofMuchPoints = fMuchPoints->GetEntriesFast();
      for (Int_t iPoint = 0; iPoint < nofMuchPoints; iPoint++) {
         FairMCPoint* point = static_cast<FairMCPoint*>(fMuchPoints->At(iPoint));
         Int_t stationId = 100 * CbmMuchGeoScheme::GetStationIndex(point->GetDetectorID())
            + 10 * CbmMuchGeoScheme::GetLayerIndex(point->GetDetectorID())
            + CbmMuchGeoScheme::GetLayerSideIndex(point->GetDetectorID());
//         Int_t stationId = CbmMuchGeoScheme::Instance()->GetLayerSideNr(point->GetDetectorID());
         fMuchStationsMap[iPoint] = stationId;
      }
   }
   // end MUCH

   // TRD
   if (NULL != fTrdPoints) {
      CbmTrdDetectorId trdDetectorId;
      Int_t nofTrdPoints = fTrdPoints->GetEntriesFast();
      for (Int_t iPoint = 0; iPoint < nofTrdPoints; iPoint++) {
         FairMCPoint* point = static_cast<FairMCPoint*>(fTrdPoints->At(iPoint));
         Int_t* detInfo = trdDetectorId.GetDetectorInfo(point->GetDetectorID());
         Int_t stationId = 10 * detInfo[1] + detInfo[2];
         fTrdStationsMap[iPoint] = stationId;
      }
   }
   // end TRD
}
