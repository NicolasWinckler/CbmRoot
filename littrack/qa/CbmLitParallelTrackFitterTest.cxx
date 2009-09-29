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
//#include "parallel/LitConverter.h"
#include "parallel/LitMath.h"

CbmLitParallelTrackFitterTest::CbmLitParallelTrackFitterTest()
{
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	env->GetMuchLayout(fLayout);
	std::cout << fLayout;
}

CbmLitParallelTrackFitterTest::~CbmLitParallelTrackFitterTest() {
	// TODO Auto-generated destructor stub
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

    LitTrackParam lpar;
    SerialParamToParallel(par, lpar);
//    CbmLitTrackParamToLitScalTrackParam(&par, &lpar);

    int ihit = 0;

	for (int isg = 0; isg < fLayout.GetNofStationGroups(); isg++) {
		LitStationGroup stationGroup = fLayout.stationGroups[isg];
	    //Propagation through the absorber
	    LitFieldRegion field;
	    LitFieldValue v1, v2;
	    LitAbsorber absorber = stationGroup.absorber;
	    absorber.fieldSliceFront.GetFieldValue(lpar.X, lpar.Y, v1);
	    absorber.fieldSliceBack.GetFieldValue(lpar.X, lpar.Y, v2);
	    field.Set(v1, absorber.fieldSliceBack.Z, v2, absorber.fieldSliceFront.Z);
	    LitRK4Extrapolation(lpar, absorber.Z, field);
	    LitAddMaterial(lpar, absorber.material);

	    //Approximate the field between the absorbers
	    LitSubstation ss1 = stationGroup.stations[0].substations[0];
	    LitSubstation ss2 = stationGroup.stations[1].substations[0];
	    ss1.fieldSlice.GetFieldValue(lpar.X, lpar.Y, v1);
	    ss2.fieldSlice.GetFieldValue(lpar.X, lpar.Y, v2);
	    field.Set(v1, ss1.fieldSlice.Z, v2, ss2.fieldSlice.Z);
	    for (int ist = 0; ist < stationGroup.GetNofStations(); ist++) {
	    	 LitStation station = stationGroup.stations[ist];
	    	 for (int iss = 0; iss < station.GetNofSubstations(); iss++) {
	    		LitSubstation substation = station.substations[iss];
				// Propagation through station
				LitRK4Extrapolation(lpar, substation.Z, field);
				LitAddMaterial(lpar, substation.material);

				ParallelParamToSerial(lpar, par);

				if (CheckHit(isg, ist, iss, fLayout, track)) {
					const CbmLitHit* hit = track->GetHit(ihit);
					LitTrackParam ulpar = lpar;
					LitPixelHit lhit;
					CbmLitPixelHit* pixelHit = (CbmLitPixelHit*) hit;
					SerialHitToParallel(*pixelHit, lhit);
					LitFiltration(ulpar, lhit);
					fvec chisq = ChiSq(ulpar, lhit);

					CbmLitTrackParam upar;
					ParallelParamToSerial(ulpar, upar);

					nodes[ihit].SetPredictedParam(&par);
					nodes[ihit].SetUpdatedParam(&upar);
					nodes[ihit].SetChiSqFiltered(chisq[0]);

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

void CbmLitParallelTrackFitterTest::SerialParamToParallel(
		const CbmLitTrackParam& par,
		LitTrackParam& lpar)
{
	lpar.X = par.GetX();
	lpar.Y = par.GetY();
	lpar.Tx = par.GetTx();
	lpar.Ty = par.GetTy();
	lpar.Qp = par.GetQp();
	lpar.Z = par.GetZ();
	lpar.C0 = par.GetCovariance(0);
	lpar.C1 = par.GetCovariance(1);
	lpar.C2 = par.GetCovariance(2);
	lpar.C3 = par.GetCovariance(3);
	lpar.C4 = par.GetCovariance(4);
	lpar.C5 = par.GetCovariance(5);
	lpar.C6 = par.GetCovariance(6);
	lpar.C7 = par.GetCovariance(7);
	lpar.C8 = par.GetCovariance(8);
	lpar.C9 = par.GetCovariance(9);
	lpar.C10 = par.GetCovariance(10);
	lpar.C11 = par.GetCovariance(11);
	lpar.C12 = par.GetCovariance(12);
	lpar.C13 = par.GetCovariance(13);
	lpar.C14 = par.GetCovariance(14);
}

void CbmLitParallelTrackFitterTest::ParallelParamToSerial(
		const LitTrackParam& lpar,
		CbmLitTrackParam& par)
{
	par.SetX(lpar.X[0]);
	par.SetY(lpar.Y[0]);
	par.SetTx(lpar.Tx[0]);
	par.SetTy(lpar.Ty[0]);
	par.SetQp(lpar.Qp[0]);
	par.SetZ(lpar.Z[0]);
	par.SetCovariance(0, lpar.C0[0]);
	par.SetCovariance(1, lpar.C1[0]);
	par.SetCovariance(2, lpar.C2[0]);
	par.SetCovariance(3, lpar.C3[0]);
	par.SetCovariance(4, lpar.C4[0]);
	par.SetCovariance(5, lpar.C5[0]);
	par.SetCovariance(6, lpar.C6[0]);
	par.SetCovariance(7, lpar.C7[0]);
	par.SetCovariance(8, lpar.C8[0]);
	par.SetCovariance(9, lpar.C9[0]);
	par.SetCovariance(10, lpar.C10[0]);
	par.SetCovariance(11, lpar.C11[0]);
	par.SetCovariance(12, lpar.C12[0]);
	par.SetCovariance(13, lpar.C13[0]);
	par.SetCovariance(14, lpar.C14[0]);
}

int CbmLitParallelTrackFitterTest::PlaneId(
		int stationGroup,
		int station,
		int substation,
		LitDetectorLayout& layout) const
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
		LitDetectorLayout& layout,
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
		LitPixelHit& lhit)
{
	lhit.X = hit.GetX();
	lhit.Y = hit.GetY();
	lhit.Dx = hit.GetDx();
	lhit.Dy = hit.GetDy();
	lhit.Dxy = hit.GetDxy();
}
