/** LitConverter.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Functions to convert Littrack serial data classes
 * to data classes used in the Littrack parallel version.
 **/

#ifndef LITCONVERTER_H_
#define LITCONVERTER_H_

#include "CbmLitPixelHit.h"
#include "CbmLitTrackParam.h"
#include "CbmLitTrack.h"

#include "CbmPixelHit.h"

#include "LitTrackParam.h"
#include "LitHit.h"
#include "LitTrack.h"

/* Converts CbmLitPixelHit to LitScalPixelHit.
 * @param hit Pointer to CbmlitPixelHit.
 * @param lhit Pointer to LitScalPixelHit.
 */
inline void CbmLitPixelHitToLitScalPixelHit(
		const CbmLitPixelHit* hit,
		LitScalPixelHit* lhit)
{
		lhit->X = hit->GetX();
		lhit->Y = hit->GetY();
		lhit->Dx = hit->GetDx();
		lhit->Dy = hit->GetDy();
		lhit->Dxy = hit->GetDxy();
		lhit->planeId = hit->GetPlaneId();
		lhit->refId = hit->GetRefId();
		lhit->Z = hit->GetZ();
}

/* Converts CbmPixelHit to LitScalPixelHit.
 * @param hit Pointer to CbmPixelHit.
 * @param lhit Pointer to LitScalPixelHit.
 */
inline void CbmPixelHitToLitScalPixelHit(
		const CbmPixelHit* hit,
		LitScalPixelHit* lhit)
{
		lhit->X = hit->GetX();
		lhit->Y = hit->GetY();
		lhit->Dx = hit->GetDx();
		lhit->Dy = hit->GetDy();
		lhit->Dxy = hit->GetDxy();
		lhit->planeId = hit->GetPlaneId() - 1;
		lhit->refId = hit->GetRefId();
		lhit->Z = hit->GetZ();
}

/* Converts CbmLitTrackParam to LitTrackParamScal.
 * @param par Pointer to CbmLitTrackParam.
 * @param lpar Pointer to LitTrackParamScal.
 */
inline void CbmLitTrackParamToLitTrackParamScal(
		const CbmLitTrackParam* par,
		LitTrackParamScal* lpar)
{
	lpar->X = par->GetX();
	lpar->Y = par->GetY();
	lpar->Tx = par->GetTx();
	lpar->Ty = par->GetTy();
	lpar->Qp = par->GetQp();
	lpar->Z = par->GetZ();
	lpar->C0 = par->GetCovariance(0);
	lpar->C1 = par->GetCovariance(1);
	lpar->C2 = par->GetCovariance(2);
	lpar->C3 = par->GetCovariance(3);
	lpar->C4 = par->GetCovariance(4);
	lpar->C5 = par->GetCovariance(5);
	lpar->C6 = par->GetCovariance(6);
	lpar->C7 = par->GetCovariance(7);
	lpar->C8 = par->GetCovariance(8);
	lpar->C9 = par->GetCovariance(9);
	lpar->C10 = par->GetCovariance(10);
	lpar->C11 = par->GetCovariance(11);
	lpar->C12 = par->GetCovariance(12);
	lpar->C13 = par->GetCovariance(13);
	lpar->C14 = par->GetCovariance(14);
}

/* Converts LitTrackParamScal to CbmLitTrackParam.
 * @param lpar Pointer to LitTrackParamScal.
 * @param par Pointer to CbmLitTrackParam.
 */
inline void LitTrackParamScalToCbmLitTrackParam(
		const LitTrackParamScal* lpar,
		CbmLitTrackParam* par)
{
	par->SetX(lpar->X);
	par->SetY(lpar->Y);
	par->SetTx(lpar->Tx);
	par->SetTy(lpar->Ty);
	par->SetQp(lpar->Qp);
	par->SetZ(lpar->Z);
	par->SetCovariance(0, lpar->C0);
	par->SetCovariance(1, lpar->C1);
	par->SetCovariance(2, lpar->C2);
	par->SetCovariance(3, lpar->C3);
	par->SetCovariance(4, lpar->C4);
	par->SetCovariance(5, lpar->C5);
	par->SetCovariance(6, lpar->C6);
	par->SetCovariance(7, lpar->C7);
	par->SetCovariance(8, lpar->C8);
	par->SetCovariance(9, lpar->C9);
	par->SetCovariance(10, lpar->C10);
	par->SetCovariance(11, lpar->C11);
	par->SetCovariance(12, lpar->C12);
	par->SetCovariance(13, lpar->C13);
	par->SetCovariance(14, lpar->C14);
}

/* Concerts LitTrack to CbmLitTrack.
 * @param ltrack Pointer to LitTrack.
 * @param track Pointer to CbmLitTrack.
 */
inline void LitScalTrackToCbmLitTrack(
		LitScalTrack* ltrack,
		CbmLitTrack* track)
{
	// Convert track parameters
	CbmLitTrackParam parFirst, parLast;
	LitTrackParamScalToCbmLitTrackParam(&(ltrack->paramFirst), &parFirst);
	LitTrackParamScalToCbmLitTrackParam(&(ltrack->paramLast), &parLast);
	track->SetParamLast(&parLast);
	track->SetParamFirst(&parFirst);

	// Copy other track parameters
	track->SetChi2(ltrack->chiSq);
	track->SetNDF(ltrack->NDF);
	track->SetPreviousTrackId(ltrack->previouseTrackId);
	track->SetQuality(kLITGOOD);
//	track->SetRefId(std::distance(ltracks.begin(), it));

	// Convert hits
	for (int i = 0; i < ltrack->nofHits; i++){
		CbmLitPixelHit* newHit = new CbmLitPixelHit;
		newHit->SetX(ltrack->hits[i]->X);
		newHit->SetY(ltrack->hits[i]->Y);
		newHit->SetDx(ltrack->hits[i]->Dx);
		newHit->SetDy(ltrack->hits[i]->Dy);
		newHit->SetDxy(ltrack->hits[i]->Dxy);
		newHit->SetPlaneId(ltrack->hits[i]->planeId);
		newHit->SetHitType(kLITPIXELHIT);
		newHit->SetDetectorId(kLITMUCH);
		newHit->SetRefId(ltrack->hits[i]->refId);
		newHit->SetZ(ltrack->hits[i]->Z);
//		std::cout << ltrack->hits[i];
		track->AddHit(newHit);
	}
}

/* Converts CbmLitTrack to LitTrack.
 * @param track Pointer to CbmLitTrack.
 * @param ltrack Pointer to LitTrack.
 */
inline void CbmLitTrackToLitTrackScal(
		CbmLitTrack* track,
		LitTrackScal* ltrack)
{
	// Convert track parameters
//	CbmLitTrackParamToLitTrackParamScal(track->GetParamFirst(), &(ltrack->paramFirst));
	CbmLitTrackParamToLitTrackParamScal(track->GetParamLast(), &(ltrack->paramLast));

	// Copy other track parameters
//	ltrack->chiSq = track->GetChi2();
//	ltrack->NDF = track->GetNDF();
//	ltrack->previouseTrackId = track->GetPreviousTrackId();

	// Convert hits
	for (int i = 0; i < track->GetNofHits(); i++){
		LitPixelHit<fscal>* newHit = new LitPixelHit<fscal>;
		CbmLitPixelHit* hit = (CbmLitPixelHit*) track->GetHit(i);
		newHit->X = hit->GetX();
		newHit->Y = hit->GetY();
		newHit->Dx = hit->GetDx();
		newHit->Dy = hit->GetDy();
		newHit->Dxy = hit->GetDxy();
//		newHit->planeId = hit->GetPlaneId();
//		newHit->refId = hit->GetRefId();
//		newHit->Z = hit->GetZ();
		ltrack->AddHit(newHit);
	}
}

#endif /* LITCONVERTER_H_ */
