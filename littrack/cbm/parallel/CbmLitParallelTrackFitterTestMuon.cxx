/**
 * \file CbmLitParallelTrackFitterTestMuon.cxx
 * \brief Parallel track fitter used to test propagation and track fit.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "cbm/parallel/CbmLitParallelTrackFitterTestMuon.h"

#include "cbm/base/CbmLitEnvironment.h"
#include "std/data/CbmLitTrack.h"
#include "std/data/CbmLitFitNode.h"
#include "std/data/CbmLitHit.h"
#include "std/data/CbmLitPixelHit.h"
#include "std/data/CbmLitTrackParam.h"
#include "parallel/muon/LitDetectorGeometryMuon.h"
#include "parallel/LitExtrapolation.h"
#include "parallel/LitFiltration.h"
#include "parallel/LitAddMaterial.h"
#include "parallel/LitTrackParam.h"
#include "parallel/LitField.h"
#include "parallel/LitHit.h"
#include "parallel/LitConverter.h"
#include "parallel/LitMath.h"

CbmLitParallelTrackFitterTestMuon::CbmLitParallelTrackFitterTestMuon()
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   env->GetMuchLayoutScal(fLayout);
   std::cout << fLayout;
}

CbmLitParallelTrackFitterTestMuon::~CbmLitParallelTrackFitterTestMuon()
{

}

LitStatus CbmLitParallelTrackFitterTestMuon::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitParallelTrackFitterTestMuon::Finalize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitParallelTrackFitterTestMuon::Fit(
   CbmLitTrack* track,
   bool downstream)
{
   track->SortHits(downstream);
   track->SetChi2(0.0);
   int nofHits = track->GetNofHits();
   FitNodeVector nodes(nofHits);
   CbmLitTrackParam par;

   track->SetParamLast(track->GetParamFirst());
   par = *track->GetParamLast();

   lit::parallel::LitTrackParam<fscal> lpar;
   CbmLitTrackParamToLitTrackParamScal(&par, &lpar);

   int ihit = 0;
   for (unsigned char isg = 0; isg < fLayout.GetNofStationGroups(); isg++) {
      const lit::parallel::LitStationGroupMuon<fscal>& stationGroup = fLayout.GetStationGroup(isg);
      //Propagation through the absorber
      const lit::parallel::LitAbsorber<fscal>& absorber = stationGroup.GetAbsorber();
      lit::parallel::LitRK4Extrapolation(lpar, absorber.GetZ(),
    		  absorber.GetFieldGridFront(), absorber.GetFieldGridMiddle(), absorber.GetFieldGridBack());
      lit::parallel::LitAddMaterial(lpar, absorber.GetMaterial());
      //propagate through the absorber using steps
      // first extrapolate input track parameters to the absorber
//     LitRK4Extrapolation(lpar, absorber.Z - absorber.material.Thickness, field);
//     int nofSteps = 8;
//     fscal step = (absorber.Z - lpar.Z) / nofSteps;
//     fscal z = lpar.Z;
//     LitMaterialInfo<fscal> mat = absorber.material;
//     mat.Thickness = step;
//     for (int iStep = 0; iStep < nofSteps; iStep++) {
//       z += step;
//       LitRK4Extrapolation(lpar, z, field);
//       LitAddMaterial(lpar, mat);
//     }
      //end propagate through the absorber using steps

      //Approximate the field between the absorbers
      lit::parallel::LitFieldRegion<fscal> field;
      stationGroup.GetFieldRegion(lpar.X, lpar.Y, field);
      for (unsigned char ist = 0; ist < stationGroup.GetNofStations(); ist++) {
         const lit::parallel::LitStationMuon<fscal>& station = stationGroup.GetStation(ist);
         for (unsigned char iss = 0; iss < station.GetNofSubstations(); iss++) {
            const lit::parallel::LitSubstationMuon<fscal>& substation = station.GetSubstation(iss);

            // Propagation through station
            lit::parallel::LitRK4Extrapolation(lpar, substation.GetZ(), field);
            lit::parallel::LitAddMaterial(lpar, substation.GetMaterial());

            LitTrackParamScalToCbmLitTrackParam(&lpar, &par);

            if (CheckHit(isg, ist, iss, fLayout, track)) {
               const CbmLitHit* hit = track->GetHit(ihit);
               lit::parallel::LitTrackParam<fscal> ulpar = lpar;
               lit::parallel::LitPixelHit<fscal> lhit;
               CbmLitPixelHit* pixelHit = (CbmLitPixelHit*) hit;
               SerialHitToParallel(*pixelHit, lhit);
               fscal chisq = 0;
               lit::parallel::LitFiltration(ulpar, lhit, chisq);

               CbmLitTrackParam upar;
               LitTrackParamScalToCbmLitTrackParam(&ulpar, &upar);

               nodes[ihit].SetPredictedParam(&par);
               nodes[ihit].SetUpdatedParam(&upar);
               nodes[ihit].SetChiSqFiltered(chisq);

               ihit++;
            }
         }
      }
   }

   track->SetParamLast(&par);
   track->SetFitNodes(nodes);
// track->SetNDF(NDF(track));

   return kLITSUCCESS;
}

unsigned char CbmLitParallelTrackFitterTestMuon::PlaneId(
   unsigned char stationGroup,
   unsigned char station,
   unsigned char substation,
   const lit::parallel::LitDetectorLayoutMuon<fscal>& layout) const
{
   int counter = 0;
   for(unsigned char i = 0; i < stationGroup; i++) {
      for(unsigned char j = 0; j < layout.GetStationGroup(i).GetNofStations(); j++) {
         counter += layout.GetStationGroup(i).GetStation(j).GetNofSubstations();
      }
   }
   for(unsigned char j = 0; j < station; j++) {
      counter += layout.GetStationGroup(stationGroup).GetStation(j).GetNofSubstations();
   }
   counter += substation;

   return counter;
}

bool CbmLitParallelTrackFitterTestMuon::CheckHit(
   unsigned char stationGroup,
   unsigned char station,
   unsigned char substation,
   const lit::parallel::LitDetectorLayoutMuon<fscal>& layout,
   CbmLitTrack* track) const
{
   unsigned char planeId = PlaneId(stationGroup, station, substation, layout);
   for (int i = 0; i < track->GetNofHits(); i++) {
      const CbmLitHit* hit = track->GetHit(i);
      if (hit->GetPlaneId() == planeId) { return true; }
   }
   return false;
}

void CbmLitParallelTrackFitterTestMuon::SerialHitToParallel(
   const CbmLitPixelHit& hit,
   lit::parallel::LitPixelHit<fscal>& lhit) const
{
   lhit.X = hit.GetX();
   lhit.Y = hit.GetY();
   lhit.Dx = hit.GetDx();
   lhit.Dy = hit.GetDy();
   lhit.Dxy = hit.GetDxy();
}
