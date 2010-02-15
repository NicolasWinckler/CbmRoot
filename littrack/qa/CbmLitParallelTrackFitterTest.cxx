/*
 * CbmLitParallelTrackFitterTest.cxx
 *
 *  Created on: Sep 24, 2009
 *      Author: andrey
 */

#include "CbmLitParallelTrackFitterTest.h"
#include "CbmLitEnvironment.h"
#include "CbmLitTrack.h"
#include "CbmLitFitNode.h"
#include "CbmLitHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitTrackParam.h"
#include "parallel/LitDetectorGeometry.h"
#include "parallel/LitExtrapolation.h"
#include "parallel/LitFiltration.h"
#include "parallel/LitAddMaterial.h"
#include "parallel/LitTrackParam.h"
#include "parallel/LitField.h"
#include "parallel/LitHit.h"
#include "parallel/LitConverter.h"
#include "parallel/LitMath.h"

CbmLitParallelTrackFitterTest::CbmLitParallelTrackFitterTest()
{
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	env->GetMuchLayoutScal(fLayout);
	std::cout << fLayout;
}

CbmLitParallelTrackFitterTest::~CbmLitParallelTrackFitterTest()
{

}

LitStatus CbmLitParallelTrackFitterTest::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitParallelTrackFitterTest::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitParallelTrackFitterTest::Fit(
		CbmLitTrack *track,
		bool downstream)
{
	track->SortHits(downstream);
	track->SetChi2(0.0);
	int nofHits = track->GetNofHits();
	FitNodeVector nodes(nofHits);
	CbmLitTrackParam par;

    track->SetParamLast(track->GetParamFirst());
    par = *track->GetParamLast();

    LitTrackParam<fscal> lpar;
    CbmLitTrackParamToLitTrackParamScal(&par, &lpar);

    int ihit = 0;

	for (int isg = 0; isg < fLayout.GetNofStationGroups(); isg++) {
		LitStationGroup<fscal> stationGroup = fLayout.stationGroups[isg];
	    //Propagation through the absorber
	    LitFieldRegion<fscal> field;
	    LitFieldValue<fscal> v1, v2;
	    LitAbsorber<fscal> absorber = stationGroup.absorber;
	    absorber.fieldSliceFront.GetFieldValue(lpar.X, lpar.Y, v1);
	    absorber.fieldSliceBack.GetFieldValue(lpar.X, lpar.Y, v2);
	    field.Set(v1, absorber.fieldSliceFront.Z, v2, absorber.fieldSliceBack.Z);
	    LitRK4Extrapolation(lpar, absorber.Z, field);
	    LitAddMaterial(lpar, absorber.material);

	    //Approximate the field between the absorbers
	    LitSubstation<fscal> ss1 = stationGroup.stations[0].substations[0];
	    LitSubstation<fscal> ss2 = stationGroup.stations[1].substations[0];
	    ss1.fieldSlice.GetFieldValue(lpar.X, lpar.Y, v1);
	    ss2.fieldSlice.GetFieldValue(lpar.X, lpar.Y, v2);
	    field.Set(v1, ss1.fieldSlice.Z, v2, ss2.fieldSlice.Z);
	    for (int ist = 0; ist < stationGroup.GetNofStations(); ist++) {
	    	 LitStation<fscal> station = stationGroup.stations[ist];
	    	 for (int iss = 0; iss < station.GetNofSubstations(); iss++) {
	    		LitSubstation<fscal> substation = station.substations[iss];
				// Propagation through station
				LitRK4Extrapolation(lpar, substation.Z, field);
				LitAddMaterial(lpar, substation.material);

				LitTrackParamScalToCbmLitTrackParam(&lpar, &par);

				if (CheckHit(isg, ist, iss, fLayout, track)) {
					const CbmLitHit* hit = track->GetHit(ihit);
					LitTrackParam<fscal> ulpar = lpar;
					LitPixelHit<fscal> lhit;
					CbmLitPixelHit* pixelHit = (CbmLitPixelHit*) hit;
					SerialHitToParallel(*pixelHit, lhit);
					LitFiltration(ulpar, lhit);
					fscal chisq = ChiSq(ulpar, lhit);

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
//	track->SetNDF(NDF(track));

	return kLITSUCCESS;
}

int CbmLitParallelTrackFitterTest::PlaneId(
		int stationGroup,
		int station,
		int substation,
		LitDetectorLayout<fscal>& layout) const
{
	int counter = 0;
	for(int i = 0; i < stationGroup; i++) {
		for(int j = 0; j < layout.stationGroups[i].GetNofStations(); j++) {
			counter += layout.stationGroups[i].stations[j].GetNofSubstations();
		}
//		counter++; // count for absorber
	}
//	counter++;//again count for absorber
	for(int j = 0; j < station; j++) {
		counter += layout.stationGroups[stationGroup].stations[j].GetNofSubstations();
	}
	counter += substation;

//	std::cout << "PlaneId: " << stationGroup << " " << station << " " << substation
//		<< " " << counter << std::endl;
	return counter;
}

bool CbmLitParallelTrackFitterTest::CheckHit(
		int stationGroup,
		int station,
		int substation,
		LitDetectorLayout<fscal>& layout,
		CbmLitTrack* track)
{
	int planeId = PlaneId(stationGroup, station, substation, layout);
	for (int i = 0; i < track->GetNofHits(); i++) {
		const CbmLitHit* hit = track->GetHit(i);
		if (hit->GetPlaneId() == planeId) return true;
	}
	return false;
}

void CbmLitParallelTrackFitterTest::SerialHitToParallel(
		const CbmLitPixelHit& hit,
		LitPixelHit<fscal>& lhit)
{
	lhit.X = hit.GetX();
	lhit.Y = hit.GetY();
	lhit.Dx = hit.GetDx();
	lhit.Dy = hit.GetDy();
	lhit.Dxy = hit.GetDxy();
}
