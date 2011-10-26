/**
 * \file CbmLitMCTrackCreator.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#include "CbmLitMCTrackCreator.h"
#include "FairRootManager.h"
#include "FairMCPoint.h"

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
   fLitMCTracks.clear();

   AddPoints(kMVD, fMvdPoints);
   AddPoints(kSTS, fStsPoints);
   AddPoints(kTRD, fTrdPoints);
   AddPoints(kMUCH, fMuchPoints);
   AddPoints(kTOF, fTofPoints);

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
      FairMCPointToLitMCPoint(fairPoint, &litPoint, iPoint);
      fLitMCTracks[fairPoint->GetTrackID()].AddPoint(detId, litPoint);
   }
}

void CbmLitMCTrackCreator::FairMCPointToLitMCPoint(
      const FairMCPoint* fairPoint,
      CbmLitMCPoint* litPoint,
      int refId)
{
   litPoint->SetX(fairPoint->GetX());
   litPoint->SetY(fairPoint->GetY());
   litPoint->SetZ(fairPoint->GetZ());
   litPoint->SetPx(fairPoint->GetPx());
   litPoint->SetPy(fairPoint->GetPy());
   litPoint->SetPz(fairPoint->GetPz());
   litPoint->SetRefId(refId);
   litPoint->SetStationId(-1);
}
