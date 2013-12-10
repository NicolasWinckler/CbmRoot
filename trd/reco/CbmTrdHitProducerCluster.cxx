#include "CbmTrdHitProducerCluster.h"
#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdPoint.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TGeoManager.h"
#include "TMath.h"


CbmTrdHitProducerCluster::CbmTrdHitProducerCluster()
  :FairTask("CbmTrdHitProducerCluster",1),
   fDigis(NULL),
   fClusters(NULL),
   fHits(NULL),
   fDigiPar(NULL),
   fTrianglePads(false)
{
}

CbmTrdHitProducerCluster::~CbmTrdHitProducerCluster()
{
   fHits->Delete();
   delete fHits;
}

void CbmTrdHitProducerCluster::SetParContainers()
{
   fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdHitProducerCluster::Init()
{
   FairRootManager *ioman = FairRootManager::Instance();

   fDigis = (TClonesArray *) ioman->GetObject("TrdDigi");
   if (fDigis == NULL) LOG(FATAL) << "CbmTrdHitProducerCluster::Init No TrdDigi array." << FairLogger::endl;

   fClusters = (TClonesArray *) ioman->GetObject("TrdCluster");
   if (fClusters == NULL) LOG(FATAL) << "CbmTrdHitProducerCluster::Init No TrdCluster array." << FairLogger::endl;

   fHits = new TClonesArray("CbmTrdHit", 100);
   ioman->Register("TrdHit","TRD Hit",fHits,kTRUE);

   return kSUCCESS;
}
void CbmTrdHitProducerCluster::SetTriangularPads(Bool_t triangles)
{
  fTrianglePads = triangles;
}
void CbmTrdHitProducerCluster::Exec(Option_t * option)
{
  fHits->Delete();

  TStopwatch timer;
  timer.Start();

  Int_t nofCluster = fClusters->GetEntries();
  for (Int_t iCluster = 0; iCluster < nofCluster; iCluster++) {
    if(fTrianglePads)
      TriangularPadReconstruction(iCluster);
    else
      CenterOfGravity(iCluster);
  }

  LOG(INFO) << "CbmTrdHitProducerCluster::Exec nofHits=" << fHits->GetEntriesFast()
	    << " nofClusters=" << fClusters->GetEntriesFast() << " nofDigis="
	    << fDigis->GetEntriesFast() << FairLogger::endl;

  timer.Stop();
  LOG(INFO) << "CbmTrdHitProducerCluster::Exec real time=" << timer.RealTime()
	    << " CPU time=" << timer.CpuTime() << FairLogger::endl;
}
Double_t CbmTrdHitProducerCluster::CalcDisplacement(Double_t Q_left, Double_t Q_center, Double_t Q_right, Double_t padWidth){
  if (Q_left > 0.0 && Q_center > 0.0 && Q_right > 0.0)
    return  0.5 * padWidth * TMath::Log(Q_right / Q_left) / TMath::Log((Q_center * Q_center) / (Q_left * Q_right));
  else
    return 0.0;
}
Double_t CbmTrdHitProducerCluster::CalcY(Double_t padWidth, Double_t padHeight, Double_t rect_Dis, Double_t para_Dis, Int_t digiRow){
  Double_t alpha = TMath::ATan(padWidth / padHeight);
  if (digiRow%2 == 0)
    return (para_Dis - (rect_Dis - 0.5 * padWidth) * TMath::Cos(alpha)) / (-1. * TMath::Sin(alpha));
  else
    return (para_Dis - (rect_Dis + 0.5 * padWidth) * TMath::Cos(alpha)) / (-1. * TMath::Sin(alpha));
}

void CbmTrdHitProducerCluster::TriangularPadReconstruction(Int_t clusterId){
  const CbmTrdCluster* cluster = static_cast<const CbmTrdCluster*>(fClusters->At(clusterId));

  TVector3 hit_posV;
  TVector3 local_pad_posV;
  TVector3 local_pad_dposV;
  Double_t totalCharge = 0;
  Int_t moduleAddress = 0;
  Int_t nofDigis = cluster->GetNofDigis();
  Int_t DigiMaxId = -1;
  Double_t maxCharge = 0;
  for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++) {
    const CbmTrdDigi* digi = static_cast<const CbmTrdDigi*>(fDigis->At(cluster->GetDigi(iDigi)));
    moduleAddress = CbmTrdAddress::GetModuleAddress(digi->GetAddress());
    Double_t charge = digi->GetCharge();
    if (charge > maxCharge){
      maxCharge = charge;
      DigiMaxId = cluster->GetDigi(iDigi);
    }
  }
  /*
    for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++) {
    const CbmTrdDigi* digi = static_cast<const CbmTrdDigi*>(fDigis->At(cluster->GetDigi(iDigi)));

    moduleAddress = CbmTrdAddress::GetModuleAddress(digi->GetAddress());
    CbmTrdModule* moduleInfo = fDigiPar->GetModule(moduleAddress);
    gGeoManager->FindNode(moduleInfo->GetX(), moduleInfo->GetY(), moduleInfo->GetZ());

    if (moduleInfo == NULL) {
    LOG(WARNING) << "CbmTrdHitProducerCluster::CenterOfCharge No CbmTrdModule found digiAddress="
    << digi->GetAddress() << " moduleAddress=" << CbmTrdAddress::GetModuleAddress(digi->GetAddress()) << FairLogger::endl;
    return;
    }
    totalCharge += digi->GetCharge();
    moduleInfo->GetPadPosition(digi->GetAddress(), local_pad_posV, local_pad_dposV);//local_pad_pos[0], local_pad_pos[1], local_pad_pos[2]);
    moduleInfo->TransformHitError(local_pad_dposV);
    for (Int_t iDim = 0; iDim < 3; iDim++) {
    hit_posV[iDim] += local_pad_posV[iDim] * digi->GetCharge();
    }
    }
    Double_t hit_pos[3];
    for (Int_t iDim = 0; iDim < 3; iDim++) {
    hit_posV[iDim] /= totalCharge;
    hit_pos[iDim] = hit_posV[iDim];
    }
    Double_t global_hit[3];
    gGeoManager->LocalToMaster(hit_pos, global_hit);

    for (Int_t iDim = 0; iDim < 3; iDim++){
    hit_posV[iDim] = global_hit[iDim];
    }
  */
  Int_t nofHits = fHits->GetEntriesFast();
  new ((*fHits)[nofHits]) CbmTrdHit(moduleAddress, hit_posV, local_pad_dposV, 0, clusterId, 0, 0, totalCharge);
}


void CbmTrdHitProducerCluster::CenterOfGravity(Int_t clusterId)
{
   const CbmTrdCluster* cluster = static_cast<const CbmTrdCluster*>(fClusters->At(clusterId));

   TVector3 hit_posV;
   TVector3 local_pad_posV;
   TVector3 local_pad_dposV;
   Double_t totalCharge = 0;
   Int_t moduleAddress = 0;
   Int_t nofDigis = cluster->GetNofDigis();
   for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++) {
      const CbmTrdDigi* digi = static_cast<const CbmTrdDigi*>(fDigis->At(cluster->GetDigi(iDigi)));

      moduleAddress = CbmTrdAddress::GetModuleAddress(digi->GetAddress());
      CbmTrdModule* moduleInfo = fDigiPar->GetModule(moduleAddress);
      gGeoManager->FindNode(moduleInfo->GetX(), moduleInfo->GetY(), moduleInfo->GetZ());

      if (moduleInfo == NULL) {
         LOG(WARNING) << "CbmTrdHitProducerCluster::CenterOfCharge No CbmTrdModule found digiAddress="
               << digi->GetAddress() << " moduleAddress=" << CbmTrdAddress::GetModuleAddress(digi->GetAddress()) << FairLogger::endl;
         return;
      }
      totalCharge += digi->GetCharge();
      moduleInfo->GetPadPosition(digi->GetAddress(), local_pad_posV, local_pad_dposV);//local_pad_pos[0], local_pad_pos[1], local_pad_pos[2]);
      moduleInfo->TransformHitError(local_pad_dposV);

      for (Int_t iDim = 0; iDim < 3; iDim++) {
         hit_posV[iDim] += local_pad_posV[iDim] * digi->GetCharge();
      }
   }
   Double_t hit_pos[3];
   for (Int_t iDim = 0; iDim < 3; iDim++) {
      hit_posV[iDim] /= totalCharge;
      hit_pos[iDim] = hit_posV[iDim];
   }
   Double_t global_hit[3];
   gGeoManager->LocalToMaster(hit_pos, global_hit);

   for (Int_t iDim = 0; iDim < 3; iDim++){
      hit_posV[iDim] = global_hit[iDim];
   }
   Int_t nofHits = fHits->GetEntriesFast();
   new ((*fHits)[nofHits]) CbmTrdHit(moduleAddress, hit_posV, local_pad_dposV, 0, clusterId, 0, 0, totalCharge);
}
ClassImp(CbmTrdHitProducerCluster)
