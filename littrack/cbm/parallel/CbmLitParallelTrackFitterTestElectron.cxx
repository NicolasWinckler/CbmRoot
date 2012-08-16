/** CbmLitParallelTrackFitterTestElectron.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **/
#include "cbm/parallel/CbmLitParallelTrackFitterTestElectron.h"

#include "base/CbmLitEnvironment.h"
#include "base/CbmLitToolFactory.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitFitNode.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrackParam.h"
#include "parallel/LitExtrapolation.h"
#include "parallel/LitFiltration.h"
#include "parallel/LitAddMaterial.h"
#include "parallel/LitTrackParam.h"
#include "parallel/LitFieldGrid.h"
#include "parallel/LitHit.h"
#include "parallel/LitConverter.h"
#include "parallel/LitMath.h"

#include "FairField.h"
#include "../std/interface/CbmLitField.h"
#include "../base/CbmLitMapField.h"

CbmLitParallelTrackFitterTestElectron::CbmLitParallelTrackFitterTestElectron()
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   env->GetTrdLayoutScal(fLayout);
   std::cout << fLayout;

   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fExtrapolator = factory->CreateTrackExtrapolator("rk4");
   fPropagator = factory->CreateTrackPropagator("lit");

   fField = new CbmLitMapField(env->GetField());
}

CbmLitParallelTrackFitterTestElectron::~CbmLitParallelTrackFitterTestElectron()
{

}

LitStatus CbmLitParallelTrackFitterTestElectron::Fit(
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

   if (par.GetTx() == 0. && par.GetTy() == 0.) { return kLITERROR; }

   lit::parallel::LitTrackParam<fscal> lpar;
   CbmLitTrackParamToLitTrackParamScal(&par, &lpar);

   int ihit = 0;

   for (unsigned char ivp = 0; ivp < fLayout.GetNofVirtualPlanes()-1; ivp++) {
      const lit::parallel::LitVirtualPlaneElectron<fscal>& vp1 = fLayout.GetVirtualPlane(ivp);
      const lit::parallel::LitVirtualPlaneElectron<fscal>& vp2 = fLayout.GetVirtualPlane(ivp+1);

//      lit::parallel::LitFieldValue<fscal> v1, v2, v3;
//      myf bx[3], by[3], bz[3];
//      myf z1 = lpar.Z;
//      myf z2 = lpar.Z + (vp2.GetZ() - lpar.Z) / 2.0;
//      myf z3 = vp2.GetZ();
////      std::cout << (int)ivp << " " << z1 << " " << z2 << " " << z3 << std::endl;
//      fField->GetFieldValue(lpar.X, lpar.Y, z1, bx[0], by[0], bz[0]);
//      fField->GetFieldValue(lpar.X, lpar.Y, z2, bx[1], by[1], bz[1]);
//      fField->GetFieldValue(lpar.X, lpar.Y, z3, bx[2], by[2], bz[2]);
//      v1.Bx = bx[0];
//      v1.By = by[0];
//      v1.Bz = bz[0];
//      v2.Bx = bx[1];
//      v2.By = by[1];
//      v2.Bz = bz[1];
//      v3.Bx = bx[2];
//      v3.By = by[2];
//      v3.Bz = bz[2];
//      if (vp2.GetZ() < 420.)
//         lit::parallel::LitRK4Extrapolation(lpar, vp2.GetZ(), v1, v2, v3);
//      else lit::parallel::LitLineExtrapolation(lpar, vp2.GetZ());
//      lit::parallel::LitAddMaterial(lpar, vp2.GetMaterial());


      if (vp2.GetZ() < 420.) lit::parallel::LitRK4Extrapolation(lpar, vp2.GetZ(),
              vp1.GetFieldGrid(), vp1.GetFieldGridMid(), vp2.GetFieldGrid());
      else lit::parallel::LitLineExtrapolation(lpar, vp2.GetZ());
//      lit::parallel::LitAddMaterialElectron(lpar, vp2.GetMaterial());
      lit::parallel::LitAddMaterial(lpar, vp2.GetMaterial());


//       fExtrapolator->Extrapolate(&par, vp2.GetZ());
//       LitStatus propStatus = fPropagator->Propagate(&par, vp2.GetZ(), 13, NULL);
//       if (propStatus != kLITSUCCESS) return kLITERROR;
   }

//    CbmLitTrackParamToLitTrackParamScal(&par, &lpar);


   for (unsigned char isg = 0; isg < fLayout.GetNofStationGroups(); isg++) {
      const lit::parallel::LitStationGroupElectron<fscal>& stationGroup = fLayout.GetStationGroup(isg);

      for (unsigned char ist = 0; ist < stationGroup.GetNofStations(); ist++) {
         const lit::parallel::LitStationElectron<fscal>& station = stationGroup.GetStation(ist);

//       LitStatus propStatus = fPropagator->Propagate(&par, station.Z, 211, NULL);
//       if (propStatus != kLITSUCCESS) return kLITERROR;

         lit::parallel::LitLineExtrapolation(lpar, station.GetZ());

         for (unsigned char im = 0; im < station.GetNofMaterialsBefore(); im++) {
//            lit::parallel::LitAddMaterialElectron(lpar, station.GetMaterialBefore(im));
            lit::parallel::LitAddMaterial(lpar, station.GetMaterialBefore(im));
         }

         LitTrackParamScalToCbmLitTrackParam(&lpar, &par);

         if (CheckHit(isg, ist, fLayout, track)) {
            const CbmLitHit* hit = track->GetHit(ihit);
            lit::parallel::LitTrackParam<fscal> ulpar = lpar;
            lit::parallel::LitPixelHit<fscal> lhit;
            CbmLitPixelHit* pixelHit = (CbmLitPixelHit*) hit;
            SerialHitToParallel(*pixelHit, lhit);
            fscal chisq = 0;
            lit::parallel::LitFiltration(ulpar, lhit, chisq);

            lpar = ulpar;

            CbmLitTrackParam upar;
            LitTrackParamScalToCbmLitTrackParam(&ulpar, &upar);

            nodes[ihit].SetPredictedParam(&par);
            nodes[ihit].SetUpdatedParam(&upar);
            nodes[ihit].SetChiSqFiltered(chisq);

            ihit++;
         }

         for (unsigned char im = 0; im < station.GetNofMaterialsAfter(); im++) {
//            lit::parallel::LitAddMaterialElectron(lpar, station.GetMaterialAfter(im));
            lit::parallel::LitAddMaterial(lpar, station.GetMaterialAfter(im));
         }
      }
   }

   track->SetParamLast(&par);
   track->SetFitNodes(nodes);
// track->SetNDF(NDF(track));

   return kLITSUCCESS;
}

unsigned char CbmLitParallelTrackFitterTestElectron::PlaneId(
   unsigned char stationGroup,
   unsigned char station,
   lit::parallel::LitDetectorLayoutElectronScal& layout) const
{
   int counter = 0;
   for(unsigned char i = 0; i < stationGroup; i++) {
      counter += layout.GetStationGroup(i).GetNofStations();
   }

   counter += station;

   return counter;
}

bool CbmLitParallelTrackFitterTestElectron::CheckHit(
   unsigned char stationGroup,
   unsigned char station,
   lit::parallel::LitDetectorLayoutElectronScal& layout,
   CbmLitTrack* track)
{
   unsigned char planeId = PlaneId(stationGroup, station, layout);
   for (unsigned int i = 0; i < track->GetNofHits(); i++) {
      const CbmLitHit* hit = track->GetHit(i);
      //if (hit->GetPlaneId() == planeId) { return true; } // TODO no planeIDs now
   }
   return false;
}

void CbmLitParallelTrackFitterTestElectron::SerialHitToParallel(
   const CbmLitPixelHit& hit,
   lit::parallel::LitPixelHit<fscal>& lhit)
{
   lhit.X = hit.GetX();
   lhit.Y = hit.GetY();
   lhit.Dx = hit.GetDx();
   lhit.Dy = hit.GetDy();
   lhit.Dxy = hit.GetDxy();
}
