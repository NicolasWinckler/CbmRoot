#include "CbmLitMath.h"

#include "CbmLitTrackParam.h"
#include "CbmLitHit.h"

Double_t ChiSq(						
		const CbmLitTrackParam* par, 						
		const CbmLitHit* hit)
{
	Double_t dxx = hit->GetDx() * hit->GetDx();
	Double_t dxy = hit->GetDxy();
	Double_t dyy = hit->GetDy() * hit->GetDy();
	Double_t C0 = par->GetCovariance(0);
	Double_t C5 = par->GetCovariance(5);
	Double_t C1 = par->GetCovariance(1);
	Double_t dx = hit->GetX() - par->GetX();
	Double_t dy = hit->GetY() - par->GetY();

	Double_t norm = -dxx * dyy + dxx * C5 + dyy * C0 - C0 * C5 + dxy * dxy -
						2 * dxy * C1 + C1 * C1;
	if (norm == 0.) norm = 1e-10;
	
	Double_t chi2 = (-dx * dx * (dyy - C5) - dy * dy * (dxx - C0) + 2 * dx * dy * (dxy - C1)) / norm;

	return chi2;
}

Int_t NDF(
		Int_t nofHits)
{
   // TODO check NDF
   if (nofHits > 2) return (2 * nofHits - 5);
   else return 1; 
}
