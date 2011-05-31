/** CbmLitParallelTrackFitterTestElectron.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **/
#include "cbm/parallel/CbmLitParallelTrackFitterTestElectron.h"

#include "base/CbmLitEnvironment.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitFitNode.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrackParam.h"
#include "parallel/LitExtrapolation.h"
#include "parallel/LitFiltration.h"
#include "parallel/LitAddMaterial.h"
#include "parallel/LitTrackParam.h"
#include "parallel/LitField.h"
#include "parallel/LitHit.h"
#include "parallel/LitConverter.h"
#include "parallel/LitMath.h"

//#include "CbmLitToolFactory.h"
//#include "FairField.h"
//#include "CbmLitField.h"
//#include "CbmLitMapField.h"

CbmLitParallelTrackFitterTestElectron::CbmLitParallelTrackFitterTestElectron()
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   env->GetTrdLayoutScal(fLayout);
   std::cout << fLayout;

// CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
// fExtrapolator = factory->CreateTrackExtrapolator("rk4");
// fPropagator = factory->CreateTrackPropagator("lit");

// fField = new CbmLitMapField(env->GetField());
}

CbmLitParallelTrackFitterTestElectron::~CbmLitParallelTrackFitterTestElectron()
{

}

LitStatus CbmLitParallelTrackFitterTestElectron::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitParallelTrackFitterTestElectron::Finalize()
{
   return kLITSUCCESS;
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
      lit::parallel::LitVirtualPlaneElectron<fscal>& vp1 = fLayout.virtualPlanes[ivp];
      lit::parallel::LitVirtualPlaneElectron<fscal>& vp2 = fLayout.virtualPlanes[ivp+1];

//       LitFieldRegion<fscal> field;
//       LitFieldValue<fscal> v1, v2;
//       vp1.fieldSlice.GetFieldValue(lpar.X, lpar.Y, v1);
//       vp2.fieldSlice.GetFieldValue(lpar.X, lpar.Y, v2);
//       field.Set(v1, vp1.fieldSlice.Z, v2, vp2.fieldSlice.Z);
//       LitRK4Extrapolation(lpar, vp2.Z, field);
//       LitAddMaterial(lpar, vp2.material);

//       LitRK4ExtrapolationTest(lpar, vp2.Z, fField);
//       LitAddMaterial(lpar, vp2.material);

      lit::parallel::LitRK4ExtrapolationElectron(lpar, vp2.Z, vp1.fieldSlice, vp1.fieldSliceMid, vp2.fieldSlice);
      lit::parallel::LitAddMaterialElectron(lpar, vp2.material);

//       if (vp2.Z < 200.)
//          fExtrapolator->Extrapolate(&par, vp2.Z);
//       LitStatus propStatus = fPropagator->Propagate(&par, vp2.Z, 211, NULL);
//       if (propStatus != kLITSUCCESS) return kLITERROR;
   }

//    CbmLitTrackParamToLitTrackParamScal(&par, &lpar);


   for (unsigned char isg = 0; isg < fLayout.GetNofStationGroups(); isg++) {
      lit::parallel::LitStationGroupElectron<fscal>& stationGroup = fLayout.stationGroups[isg];

      for (unsigned char ist = 0; ist < stationGroup.GetNofStations(); ist++) {
         lit::parallel::LitStationElectron<fscal>& station = stationGroup.stations[ist];

//       LitStatus propStatus = fPropagator->Propagate(&par, station.Z, 211, NULL);
//       if (propStatus != kLITSUCCESS) return kLITERROR;

         lit::parallel::LitLineExtrapolation(lpar, station.Z);

         for (unsigned char im = 0; im < station.GetNofMaterialsBefore(); im++) {
            lit::parallel::LitAddMaterialElectron(lpar, station.materialsBefore[im]);
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
//            fscal chisq = ChiSq(ulpar, lhit);

            lpar = ulpar;

            CbmLitTrackParam upar;
            LitTrackParamScalToCbmLitTrackParam(&ulpar, &upar);

            nodes[ihit].SetPredictedParam(&par);
            nodes[ihit].SetUpdatedParam(&upar);
            nodes[ihit].SetChiSqFiltered(chisq);

            ihit++;
         }

         for (unsigned char im = 0; im < station.GetNofMaterialsAfter(); im++) {
            lit::parallel::LitAddMaterialElectron(lpar, station.materialsAfter[im]);
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
      counter += layout.stationGroups[i].GetNofStations();
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
      if (hit->GetPlaneId() == planeId) { return true; }
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
