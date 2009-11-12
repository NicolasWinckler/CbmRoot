
// --------------------------------------------------------------------------------------
// CbmRichRingFinderHoughBase source file
// Base class for ring finders based on on HT method
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

#include "CbmRichRingFinderHoughImpl.h"
//#include "CbmRichFuzzyKE.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "FairTrackParam.h"
#include "../../littrack/utils/CbmLitMemoryManagment.h"

#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"

#include <iostream>
#include <cmath>
#include <set>
#include <algorithm>
#include <iostream>


using std::cout;
using std::endl;
using std::vector;

// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderHoughImpl::CbmRichRingFinderHoughImpl  (TString geometry)
{
    cout << "-I- CbmRichRingFinderHoughImpl constructor for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHoughImpl::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }
    fGeometryType = geometry;
}

void CbmRichRingFinderHoughImpl::Init()
{
    SetParameters(fGeometryType);

    fHist.resize(fNofBinsXY);

//    fRingHits.resize(fNofBinsXY);
//    for (Int_t j = 0; j < fNofBinsXY; j++)
//		fRingHits[j].resize(kMAX_NOF_HITS_IN_AREA);

    fHistR.resize(fNofBinsR);

//    fRingHitsR.resize(fNofBinsR);
//	for (Int_t k = 0; k < fNofBinsR; k++){
//		fRingHitsR[k].resize(kMAX_NOF_HITS_IN_AREA);
//	}

	fHitInd.resize(fNofParts);

    fFitCOP = new CbmRichRingFitterCOP(0, 0);
    fFitCOP->Init();

    fFitEllipseTau = new CbmRichRingFitterEllipseTau(0, 0, fGeometryType);
    fFitEllipseTau->Init();
}

CbmRichRingFinderHoughImpl::CbmRichRingFinderHoughImpl()
{


}
// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderHoughImpl::~CbmRichRingFinderHoughImpl()
{
	delete fFitCOP;
	delete fFitEllipseTau;
	delete fANNSelect;
}

void CbmRichRingFinderHoughImpl::DoFind()
{

	if (fData.size() > kMAX_NOF_HITS) {
		cout<< "-E- CbmRichRingFinderHoughImpl::DoFind: Number of hits is more than "<< kMAX_NOF_HITS << endl;
		return ;
	}

	for_each(fFoundRings.begin(), fFoundRings.end(), DeleteObject());
	fFoundRings.clear();
	fFoundRings.reserve(100);

	std::sort(fData.begin(), fData.end(), CbmRichHoughHitCmpUp());
	HoughTransformReconstruction();
	RingSelection();
}

void CbmRichRingFinderHoughImpl::SetParameters( Int_t nofParts,
		Float_t maxDistance, Float_t minDistance,
		Float_t minRadius, Float_t maxRadius,
		Int_t HTCut, Int_t hitCut,
		Int_t HTCutR, Int_t hitCutR,
		Int_t nofBinsX, Int_t nofBinsY,
		Int_t nofBinsR, Float_t annCut,
		Float_t usedHitsCut, Float_t usedHitsAllCut,
		Float_t rmsCoeffEl, Float_t maxCutEl,
		Float_t rmsCoeffCOP, Float_t maxCutCOP)
{
    fNofParts = nofParts;

    fMaxDistance = maxDistance;
    fMinDistance = minDistance;

    fMinRadius = minRadius;
    fMaxRadius = maxRadius;

    fHTCut = HTCut;
    fHitCut = hitCut;
    fHTCutR = HTCutR;
    fHitCutR = hitCutR;

    fNofBinsX = nofBinsX;
    fNofBinsY = nofBinsY;
    fNofBinsR = nofBinsR;

    fAnnCut = annCut;
    fUsedHitsCut = usedHitsCut;
    fUsedHitsAllCut = usedHitsAllCut;

	fRmsCoeffEl = rmsCoeffEl;
	fMaxCutEl = maxCutEl;
	fRmsCoeffCOP = rmsCoeffCOP;
	fMaxCutCOP = maxCutCOP;

    kMAX_NOF_HITS_IN_AREA = 300;
    fMinNofHitsInArea = 6;
    fMinDistanceSq = fMinDistance*fMinDistance;
    fMaxDistanceSq = fMaxDistance*fMaxDistance;

    fDx = 2*fMaxDistance / (Float_t)fNofBinsX;
    fDy = 2*fMaxDistance / (Float_t)fNofBinsY;
    fDr = fMaxRadius / (Float_t)fNofBinsR;
    fNofBinsXY = fNofBinsX * fNofBinsY;
}

///Set Parameters for specify geometry
void CbmRichRingFinderHoughImpl::SetParameters(TString geometry)
{
    cout << "-I- CbmRichRingFinderHough::SetParameters for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHough::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }

    TString richSelectNNFile = gSystem->Getenv("VMCWORKDIR");

    if (geometry == "large"){
        fNofParts = 1;
        kMAX_NOF_HITS_IN_AREA = 300;
        fMinNofHitsInArea = 6;

        fMaxDistance = 14;
        fMinDistance = 3.;
        fMinDistanceSq = fMinDistance*fMinDistance;
        fMaxDistanceSq = fMaxDistance*fMaxDistance;

        fMinRadius = 4.3;
        fMaxRadius = 7.0;

        fHTCut = 80;
        fHitCut = 9;
        fHTCutR = 35;
        fHitCutR = 9;

        fNofBinsX = 15;
        fNofBinsY = 15;
        fNofBinsR = 40;

        fAnnCut = 0.0;
        fUsedHitsCut = 0.35;
        fUsedHitsAllCut = 0.4;

    	fRmsCoeffEl = 1.;
    	fMaxCutEl = 0.3;
    	fRmsCoeffCOP = 3.;
    	fMaxCutCOP = 1.2;

        richSelectNNFile += "/parameters/rich/NeuralNet_RingSelection_Weights.txt";
    }

    if (geometry == "compact"){
        fNofParts = 2;

        fMaxDistance = 11.5;
        fMinDistance = 2.5;
        fMinDistanceSq = fMinDistance*fMinDistance;
        fMaxDistanceSq = fMaxDistance*fMaxDistance;

        fMinRadius = 3.3;
        fMaxRadius = 5.7;

        if (fNofParts == 1){
			fHTCut = 50;
			fHitCut = 5;
			fHTCutR = 20;
			fHitCutR = 3;
			kMAX_NOF_HITS_IN_AREA = 300;
			fMinNofHitsInArea = 6;
        }else if (fNofParts == 2){
			fHTCut = 5;
			fHitCut = 2;
			fHTCutR = 5;
			fHitCutR = 2;
			kMAX_NOF_HITS_IN_AREA = 300;
			fMinNofHitsInArea = 4;
        }

        fNofBinsX = 25;
        fNofBinsY = 25;
        fNofBinsR = 32;

		fAnnCut = -0.15;
		fUsedHitsCut = 0.35;
		fUsedHitsAllCut = 0.4;

    	fRmsCoeffEl = 2.;
    	fMaxCutEl = 0.8;
    	fRmsCoeffCOP = 3.;
    	fMaxCutCOP = 1.2;

	    richSelectNNFile += "/parameters/rich/NeuralNet_RingSelection_Weights_Compact.txt";
    }


    fDx = 2.*fMaxDistance / (Float_t)fNofBinsX;
    fDy = 2.*fMaxDistance / (Float_t)fNofBinsY;
    fDr = fMaxRadius / (Float_t)fNofBinsR;
    fNofBinsXY = fNofBinsX * fNofBinsY;

    fANNSelect = new CbmRichRingSelectNeuralNet(0, richSelectNNFile);
    fANNSelect->Init();
}

void CbmRichRingFinderHoughImpl::HoughTransformReconstruction()
{
    Int_t indmin, indmax;
    for (UInt_t iHit = 0; iHit < fData.size(); iHit++)
    {
        if (fData[iHit].fIsUsed == true) continue;

        fCurMinX = fData[iHit].fX  - fMaxDistance;
		fCurMinY = fData[iHit].fY - fMaxDistance;

		DefineLocalAreaAndHits(fData[iHit].fX, fData[iHit].fY , &indmin, &indmax);
		HoughTransform(indmin, indmax);
		FindPeak(indmin, indmax);

    }//main loop over hits
}

void CbmRichRingFinderHoughImpl::DefineLocalAreaAndHits(Float_t x0, Float_t y0,
		Int_t *indmin, Int_t *indmax)
{
    CbmRichHoughHit mpnt;
    std::vector<CbmRichHoughHit>::iterator itmin, itmax;

	//find all hits which are in the corridor
	mpnt.fX = x0 - fMaxDistance;
	itmin = std::lower_bound(fData.begin(), fData.end(), mpnt, CbmRichHoughHitCmpUp());

	mpnt.fX = x0 + fMaxDistance;
	itmax = std::lower_bound(fData.begin(), fData.end(), mpnt, CbmRichHoughHitCmpUp()) - 1;

	*indmin = itmin - fData.begin();
	*indmax = itmax - fData.begin();

	Int_t arSize = *indmax - *indmin + 1;
	if (arSize <= fMinNofHitsInArea) return;


	for (Int_t i = 0; i < fNofParts; i++){
		fHitInd[i].clear();
		fHitInd[i].reserve( (*indmax-*indmin) / fNofParts);
	}

	register Float_t rx, ry;
    for (Int_t i = *indmin; i <= *indmax; i++) {
    	if (fData[i].fIsUsed == true) continue;
    	ry = y0 - fData[i].fY;
    	if (fabs(ry) > fMaxDistance) continue;
    	rx = x0 - fData[i].fX;
    	Float_t d = rx	* rx +ry * ry;
    	if (d > fMaxDistanceSq) continue;

    	fHitInd[i % fNofParts].push_back(i);
    }

    Int_t nofHits = 0;
    Int_t nofHitsNorm = fHitInd[0].size() + 1;
    Int_t iPmulNofHits;

	for (Int_t j = 0; j < fNofBinsXY; j++){
		fHist[j] = 0;
	}

//	for (Int_t iP = 0; iP < fNofParts; iP++){
//		nofHits = fHitInd[iP].size();
//		if (nofHits > kMAX_NOF_HITS_IN_AREA) nofHits = kMAX_NOF_HITS_IN_AREA - 1;
//		iPmulNofHits = iP * nofHitsNorm;
//		for (Int_t j = 0; j < fNofBinsXY; j++){
//		    for (Int_t iH = 0; iH < (nofHits & ~ 3); iH+=4) {
//			fRingHits[j][iPmulNofHits + iH] = 0;
//			fRingHits[j][iPmulNofHits + iH+1] = 0;
//			fRingHits[j][iPmulNofHits + iH+2] = 0;
//                        fRingHits[j][iPmulNofHits + iH+3] = 0;
//		    }
//		    for (Int_t iH = (nofHits & ~ 3); iH < nofHits; iH++) {
//                       fRingHits[j][iPmulNofHits + iH] = 0;
//		    }
//		}
//	}

	for (Int_t k = 0; k < fNofBinsR; k++) {
		fHistR[k] = 0;
	}

//	for (Int_t iP = 0; iP < fNofParts; iP++){
//		nofHits = fHitInd[iP].size();
//		if (nofHits > kMAX_NOF_HITS_IN_AREA) nofHits = kMAX_NOF_HITS_IN_AREA - 1;
//		iPmulNofHits = iP * nofHitsNorm;
//		for (Int_t k = 0; k < fNofBinsR; k++) {
//			for (Int_t iH = 0; iH < nofHits; iH++)
//				fRingHitsR[k][iPmulNofHits + iH] = 0;
//		}
//	}

}

void CbmRichRingFinderHoughImpl::HoughTransform(unsigned short int indmin, unsigned short int indmax)
{
    for (Int_t iPart = 0; iPart < fNofParts; iPart++){
    	HoughTransformGroup(indmin, indmax, iPart);
    }//iPart
}

void CbmRichRingFinderHoughImpl::HoughTransformGroup(unsigned short int indmin,
		unsigned short int indmax, Int_t iPart)
{
	register Float_t r12, r13, r23;
    register Float_t rx0, rx1, rx2, ry0, ry1,ry2; //rx[3], ry[3];//, x[3], y[3];
    register Float_t xc, yc, r;
    register Float_t xcs, ycs; // xcs = xc - fCurMinX
    register Int_t intX, intY, intR;
    register Int_t indXY;

    register unsigned short int iH1, iH2, iH3;
    register unsigned short int iH1_1, iH2_1, iH3_1;
    register Int_t nofHitsNorm = fHitInd[0].size() + 1;
    register Int_t iPmulNofHits;

    register Float_t t5, t10, t19, det, t6, t7;
    register Float_t dx = 1.0f/fDx, dy = 1.0f/fDy, dr = 1.0f/fDr;
    register Float_t histXY;

    Int_t nofHits = fHitInd[iPart].size();
    if (nofHits <= fMinNofHitsInArea) return;
    iPmulNofHits = iPart * nofHitsNorm;

    register Float_t iH1X, iH1Y, iH2X, iH2Y, iH3X, iH3Y;

	for (unsigned short int iHit1 = 0; iHit1 < nofHits; iHit1++) {
		iH1 = fHitInd[iPart][iHit1];
		iH1_1 = iPmulNofHits + iHit1;
		iH1X = fData[iH1].fX;
		iH1Y = fData[iH1].fY;

		for (unsigned short int iHit2 = iHit1 + 1; iHit2 < nofHits; iHit2++) {
			iH2 = fHitInd[iPart][iHit2];
			iH2_1 = iPmulNofHits + iHit2;
			iH2X = fData[iH2].fX;
			iH2Y = fData[iH2].fY;

			rx0 = iH1X - iH2X;//rx12
			ry0 = iH1Y- iH2Y;//ry12
			r12 = rx0 * rx0 + ry0 * ry0;
			if (r12 < fMinDistanceSq || r12 > fMaxDistanceSq)	continue;

			t10 = fData[iH1].fX2plusY2 - fData[iH2].fX2plusY2;
			iH3_1= iPmulNofHits+iHit2;
			for (unsigned short int iHit3 = iHit2 + 1; iHit3 < nofHits; iHit3++) {
				iH3 = fHitInd[iPart][iHit3];
				iH3_1++;//iH3_1 = iPmulNofHits + iHit3;
				iH3X = fData[iH3].fX;
				iH3Y = fData[iH3].fY;

				rx1 = iH1X - iH3X;//rx13
				ry1 = iH1Y - iH3Y;//ry13
				r13 = rx1 * rx1 + ry1 * ry1;
				if (r13 < fMinDistanceSq || r13 > fMaxDistanceSq)continue;

				rx2 = iH2X - iH3X;//rx23
				ry2 = iH2Y - iH3Y;//ry23
				r23 = rx2 * rx2 + ry2 * ry2;
				if (r23	< fMinDistanceSq || r23 > fMaxDistanceSq)continue;

			    det = rx2*ry0 - rx0*ry2;
			    if (det == 0.0f) continue;
				t19 = 0.5f / det;
				t5 = fData[iH2].fX2plusY2 - fData[iH3].fX2plusY2;

				xc = (t5 * ry0 - t10 * ry2) * t19;
				xcs = xc - fCurMinX;
				intX = int( xcs *dx);
				if (intX < 0 || intX >= fNofBinsX ) continue;

				yc = (t10 * rx2 - t5 * rx0) * t19;
				ycs = yc - fCurMinY;
				intY = int( ycs *dy);
				if (intY < 0 || intY >= fNofBinsY ) continue;

				 //radius calculation
				t6 = iH1X - xc;
				t7 = iH1Y - yc;
				r = sqrt(t6 * t6 + t7 * t7);

				intR = int(r *dr);
				if (intR < 0 || intR >= fNofBinsR) continue;
				indXY = intX * fNofBinsX + intY;

				fHist[indXY]++;
//				fRingHits[indXY][iH1_1]++;
//				fRingHits[indXY][iH2_1]++;
//				fRingHits[indXY][iH3_1]++;
				fHistR[intR]++;

//				fRingHitsR[intR][iH1_1]++;
//				fRingHitsR[intR][iH2_1]++;
//				fRingHitsR[intR][iH3_1]++;
			}//iHit1
		}//iHit2
	}//iHit3
}


void CbmRichRingFinderHoughImpl::CalculateRingParameters(Float_t x2y2[],
                             Float_t rx[],
                             Float_t ry[],
                             Float_t x0,
                             Float_t y0,
                             Float_t *xc,
                             Float_t *yc,
                             Float_t *r)
{
    register Float_t t5, t10, t19, t21, t41, det;

    det = rx[2]*ry[0] - rx[0]*ry[2];

    if (det != 0.0f){
        t19 = 0.5f / det;
        t5 = x2y2[1] - x2y2[2];
        t10 = x2y2[0] - x2y2[1];
        *xc = (t5 * ry[0] - t10 * ry[2]) * t19;
        *yc = (t10 * rx[2] - t5 * rx[0]) * t19;

         //radius calculation
         t21 = (x0 - *xc) * (x0 - *xc);
         t41 = (y0 - *yc) * (y0 - *yc);
         *r = sqrt(t21 + t41);

    }else {
        *xc = -1.0f;
        *yc = -1.0f;
        *r = -1.0f;
    }
}

void CbmRichRingFinderHoughImpl::CalculateRingParametersOld(Float_t x[],
			Float_t y[],
			Float_t *xc,
			Float_t *yc,
			Float_t *r)
{
    register Float_t t1, t2, t3, t4, t5, t6, t8, t9,
        t10, t11, t14, t16, t19, t21, t41;

    t1 = x[1] * x[1];
    t2 = x[2] * x[2];
    t3 = y[1] * y[1];
    t4 = y[2] * y[2];
    t5 = t1 - t2 + t3 - t4;
    t6 = y[0] - y[1];
    t8 = x[0] * x[0];
    t9 = y[0] * y[0];
    t10 = t8 - t1 + t9 - t3;
    t11 = y[1] - y[2];
    t14 = x[1] - x[2];
    t16 = x[0] - x[1];
    t19 = 1.0f / (t14 * t6 - t16 * t11);

    *xc = 0.5e0 * (t5 * t6 - t10 * t11) * t19;
    *yc = 0.5e0 * (t10 * t14 - t5 * t16) * t19;

    t21 = (x[0] - *xc)*(x[0] - *xc);
    t41 = (y[0] - *yc)*(y[0] - *yc);
    *r = sqrt(t21 + t41);
}

void CbmRichRingFinderHoughImpl::RemoveHitsAroundEllipse(Int_t indmin, Int_t indmax, CbmRichRing * ring)
{
	Double_t A = ring->GetAPar();
	Double_t B = ring->GetBPar();
	Double_t C = ring->GetCPar();
	Double_t D = ring->GetDPar();
	Double_t E = ring->GetEPar();
	Double_t F = ring->GetFPar();

	Double_t rms = TMath::Sqrt( ring->GetChi2()/ring->GetNofHits() );
	Double_t dCut = fRmsCoeffEl * rms;
	if (dCut > fMaxCutEl) dCut = fMaxCutEl;

	for(Int_t j = 0; j < indmax - indmin + 1; j++){
		Double_t x = fData[j + indmin].fX;
		Double_t y = fData[j + indmin].fY;

        Double_t d1 = TMath::Abs(A*x*x + B*x*y + C*y*y + D*x + E*y + F);
        Double_t d2 = sqrt( pow(2*A*x + B*y + D, 2) + pow(B*x + 2*C*y + E, 2) );

        Double_t d = d1/d2;
        if (d < dCut) fData[j+indmin].fIsUsed = true;
	}
}

void CbmRichRingFinderHoughImpl::RemoveHitsAroundRing(Int_t indmin, Int_t indmax, CbmRichRing * ring)
{
	Double_t drHitCut = sqrt(ring->GetChi2()/ring->GetNofHits());
	if (drHitCut > 0.3)	drHitCut = 0.3;

	for (Int_t j = 0; j < indmax - indmin + 1; j++) {
		Double_t rx = fData[j + indmin].fX - ring->GetCenterX();
		Double_t ry = fData[j + indmin].fY - ring->GetCenterY();

		Double_t dr = fabs(sqrt(rx * rx + ry * ry) - ring->GetRadius());
		if (dr < drHitCut) {
			fData[j+indmin].fIsUsed = true;
		}
	}
}


void CbmRichRingFinderHoughImpl::FindPeak(Int_t indmin, Int_t indmax)
{
//Find MAX bin XY and compare it with CUT
    Int_t maxBinXY = -1, maxXY = -1;
    UInt_t size = fHist.size();
    for (UInt_t k = 0; k < size; k++){
        if (fHist[k] > maxBinXY){
            maxBinXY = fHist[k];
            maxXY = k;
        }
    }
    if (maxBinXY < fHTCut) return;

//Find MAX bin R and compare it with CUT
    Int_t maxBinR = -1, maxR = -1;
    size = fHistR.size();
    for (UInt_t k = 0; k < size; k++){
        if (fHistR[k] > maxBinR){
            maxBinR = fHistR[k];
            maxR = k;
        }
    }
	if (maxBinR < fHTCutR) return;
	CbmRichRing* ring1 = new CbmRichRing();
//	Double_t hitCut = maxBinXY/10.;
//	if (hitCut <= fHitCut)	hitCut = fHitCut;
//	Int_t nofHitsNorm = fHitInd[0].size() + 1;
//    Int_t iPmulNofHits;
//    Int_t ind;
//    for (Int_t iP = 0; iP < fNofParts; iP++){
//    	Int_t nofHits = fHitInd[iP].size();
//    	iPmulNofHits = iP * nofHitsNorm;
//    	for (Int_t iH = 0; iH < nofHits; iH++){
//    		ind = iPmulNofHits + iH;
//    		Int_t hitCutR = fRingHitsR[maxR][ind];
//    		//if (maxR >=1 && maxR <= fHistR.size())
//    		//	hitCutR += fRingHitsR[maxR+1][ind] + fRingHitsR[maxR-1][ind];
//
//    		if (fRingHits[maxXY][ind] >= hitCut && hitCutR >= fHitCutR) {
//    			ring1.AddHit(fData[ fHitInd[iP][iH] ].fId);
//    		}
//    	}
//    }


//Find Preliminary Xc, Yc, R
    Float_t xc, yc, r;
	xc = (maxXY/fNofBinsX + 0.5)* fDx + fCurMinX;
	yc = (maxXY%fNofBinsX + 0.5)* fDy + fCurMinY;
	r = (maxR + 0.5)* fDr;
	for (Int_t j = 0; j < indmax - indmin + 1; j++) {
		Float_t rx = fData[j + indmin].fX - xc;
		Float_t ry = fData[j + indmin].fY - yc;

		Float_t dr = fabs(sqrt(rx * rx + ry * ry) - r);
		if (dr < 0.6) ring1->AddHit(fData[j + indmin].fId);
	}

	fFitCOP->DoFit(ring1);
	Double_t drCOPCut = fRmsCoeffCOP*sqrt(ring1->GetChi2()/ring1->GetNofHits());
	if (drCOPCut > fMaxCutCOP)	drCOPCut = fMaxCutCOP;

	xc = ring1->GetCenterX();
	yc = ring1->GetCenterY();
	r = ring1->GetRadius();
	CbmRichRing* ring2 = new CbmRichRing();
	for (Int_t j = 0; j < indmax - indmin + 1; j++) {
		Float_t rx = fData[j + indmin].fX - xc;
		Float_t ry = fData[j + indmin].fY - yc;

		Float_t dr = fabs(sqrt(rx * rx + ry * ry) - r);
		if (dr < drCOPCut) {
			//fData[j+indmin].fIsUsed = true;
			ring2->AddHit(fData[j + indmin].fId);
		}
	}
	fFitEllipseTau->DoFit(ring2);

	fANNSelect->DoSelect(ring2);
	//remove found hits only for good quality rings
	if (ring2->GetSelectionNN() > fAnnCut) {
		//RemoveHitsAroundRing(indmin, indmax, &ring2);
		RemoveHitsAroundEllipse(indmin, indmax, ring2);

		fFoundRings.push_back(ring2);
	}
	//fFoundRings.push_back(ring2);
}

void CbmRichRingFinderHoughImpl::RingSelection()
{
	std::sort(fFoundRings.begin(), fFoundRings.end(), CbmRichRingComparatorMore());
	std::vector<std::set<Int_t> > usedHits;
	usedHits.reserve(300);
	std::set<Int_t> usedHitsAll;

	Int_t nofRings = fFoundRings.size();
	for (Int_t iRing = 0; iRing < nofRings; iRing++){
		fFoundRings[iRing]->SetRecFlag(-1);
		CbmRichRing* ring = fFoundRings[iRing];
		Int_t nofHits = ring->GetNofHits();
		Bool_t isGoodRing = true;

		for (Int_t iRSet = 0; iRSet < usedHits.size(); iRSet++){
			Int_t nofUsedHits = 0;
			for(Int_t iHit = 0; iHit < nofHits; iHit++){
				std::set<Int_t>::iterator it = usedHits[iRSet].find(ring->GetHit(iHit));
				if(it != usedHits[iRSet].end()){
					nofUsedHits++;
				}
			}
			if ((Double_t)nofUsedHits/(Double_t)nofHits > fUsedHitsCut){
				isGoodRing = false;
				break;
			}
		}// iRSet


		Bool_t isGoodRingAll = true;
		Int_t nofUsedHitsAll = 0;
		for(Int_t iHit = 0; iHit < nofHits; iHit++){
			std::set<Int_t>::iterator it = usedHitsAll.find(ring->GetHit(iHit));
			if(it != usedHitsAll.end()){
				nofUsedHitsAll++;
			}
		}
		if ((Double_t)nofUsedHitsAll/(Double_t)nofHits > fUsedHitsAllCut){
			isGoodRingAll = false;
		}


		if (isGoodRing && isGoodRingAll){
			fFoundRings[iRing]->SetRecFlag(1);
			std::set<Int_t> usedHitsT;
			for(Int_t iHit = 0; iHit < nofHits; iHit++){
				usedHitsT.insert(ring->GetHit(iHit));
				usedHitsAll.insert(ring->GetHit(iHit));
			}
			usedHits.push_back(usedHitsT);
		}
	}
}


void CbmRichRingFinderHoughImpl::FuzzyKE(TClonesArray* rHitArray)
{
/*	Int_t nofRings = fFoundRings.size();
	vector<CbmRichRing> foundRingsFuzzy;
	vector<Ellipse> rings;
	vector<Int_t> ringsId;
	vector<Hit> hits;
	rings.clear();
	std::set<Int_t> hitSet;



	for (Int_t iR = 0; iR < nofRings; iR++){
		CbmRichRing ring = fFoundRings[iR];
		if (fabs(ring.GetAaxis()) > 10) continue;
		Double_t x0 = ring.GetCenterX();
		Double_t y0 = ring.GetCenterY();
		rings.clear();
		ringsId.clear();
		hits.clear();
		hitSet.clear();

		rings.push_back(CbmRichFuzzyKE::ParTransform(&ring));
		ringsId.push_back(iR);
		Int_t nofHits = ring.GetNofHits();
		for (Int_t iH = 0; iH < nofHits; iH++){
			hitSet.insert(ring.GetHit(iH));
		}
		for (Int_t iR1 = 0; iR1 < nofRings; iR1++){
			if (iR == iR1) continue;
			CbmRichRing ring1 = fFoundRings[iR1];
			Double_t x01 = ring1.GetCenterX();
			Double_t y01 = ring1.GetCenterY();
			Double_t d = sqrt ( (x0-x01)*(x0-x01) + (y0-y01)*(y0-y01) );
			if (d > 2.*fMaxRadius) continue;
			rings.push_back(CbmRichFuzzyKE::ParTransform(&ring1));
			ringsId.push_back(iR1);

			Int_t nofHits1 = ring1.GetNofHits();
			for (Int_t iH = 0; iH < nofHits1; iH++){
				hitSet.insert(ring1.GetHit(iH));
			}
		}
		std::set<Int_t>::iterator it;
		//cout << "hitSet.size() = " << hitSet.size()<<endl;
		for (it=hitSet.begin(); it!=hitSet.end(); it++){
	        CbmRichHit* hit = (CbmRichHit*)rHitArray->At(*it);
	        Hit hit1;
	        hit1.fX = hit->GetX();
	        hit1.fY = hit->GetY();
	        hit1.fHitId = *it;
	        hits.push_back(hit1);
		}

		CbmRichFuzzyKE* fuzzy = new CbmRichFuzzyKE();
		fuzzy->SetHits(hits);
		fuzzy->SetEllipses(rings);
		fuzzy->Minimize();

		std::vector<vector<Double_t> > u;
		std::vector<Ellipse> minEllipses;
		u = fuzzy->GetU();
		minEllipses = fuzzy->GetEllipses();

		CbmRichRing ringFuzzy;
		//we need only first ellipse parameters, because this is our main ellipse
		for (Int_t iH = 0; iH < u[0].size(); iH++){
			//find out if there is a u[i][j] more than u[0][j]
			Bool_t u0Max=true;
			for (Int_t iUR = 1; iUR < u.size(); iUR++){
				//if (u[0][iH] < 0.5) continue;
				if (u[0][iH] < u[iUR][iH]){
					u0Max = false;
					break;
				}
			}//iUR
			if(u0Max){
				ringFuzzy.AddHit(hits[iH].fHitId);
				for (Int_t iUR = 1; iUR < u.size(); iUR++){
					fFoundRings[ringsId[iUR]].RemoveHit(hits[iH].fHitId);
				}//iUR
			}
		}//iH

		fFitEllipse->DoFit(&ringFuzzy);
		fFoundRings[iR] = ringFuzzy;
		foundRingsFuzzy.push_back(ringFuzzy);

		//cout << "rings.size() = "<< rings.size()<< endl;
	}//iR

	//fFoundRings.clear();
	//fFoundRings.assign(foundRingsFuzzy.begin(), foundRingsFuzzy.end());*/
}

ClassImp(CbmRichRingFinderHoughImpl)
