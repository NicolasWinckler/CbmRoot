// --------------------------------------------------------------------------------------
// CbmRichRingFinderHoughSimd source file
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

#include "../L1/L1Algo/L1Types.h"
#include <emmintrin.h>
#include "CbmRichRingFinderHoughSimd.h"

// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderHoughSimd::CbmRichRingFinderHoughSimd  ( TString geometry )
{
    cout << "-I- CbmRichRingFinderHough constructor for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHough::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }
    fGeometryType = geometry;

}

void CbmRichRingFinderHoughSimd::HoughTransformGroup(unsigned short int indmin,
		unsigned short int indmax, Int_t iPart)
{
//	register Float_t r12, r13, r23;
//    register Float_t rx0, rx1, rx2, ry0, ry1,ry2; //rx[3], ry[3];//, x[3], y[3];
    //register Float_t xc, yc, r;
    //register Float_t xcs, ycs; // xcs = xc - fCurMinX
    register Int_t intX, intY, intR;
    register Int_t indXY;

    register unsigned short int iH11, iH12, iH13, iH14, iH2, iH3;
    register Int_t nofHitsNorm = fHitInd[0].size() + 1;
    register Int_t iPmulNofHits;

    //register Float_t t5, t10, t19, det, t6, t7;
    register Float_t dx = 1.0f/fDx, dy = 1.0f/fDy, dr = 1.0f/fDr;
    //register Float_t iH1X, iH1Y, iH2X, iH2Y, iH3X, iH3Y;

    fvec xcs, ycs;
    fvec fCurMinXV = fvec(fCurMinX), fCurMinYV = fvec(fCurMinY);
    fvec xc, yc, r;
	fvec r12, r13, r23;
    fvec rx0, rx1, rx2, ry0, ry1,ry2; //rx[3], ry[3];//, x[3], y[3];
    fvec t5, t10, t19, det, t6, t7;
    fvec iH1X, iH1Y, iH2X, iH2Y, iH3X, iH3Y;
    fvec intXv, intYv, intRv;
    fvec dxv = fvec(1.0f/fDx), dyv = fvec(1.0f/fDy), drv = fvec(1.0f/fDr);
    fvec fvec05 = fvec(0.5f);
	float* intXf;
	float* intYf;
	float* intRf;
    Int_t nofHits = fHitInd[iPart].size();
    if (nofHits <= fMinNofHitsInArea) return;
    iPmulNofHits = iPart * nofHitsNorm;

    vector<unsigned short> hitIndPart;
    hitIndPart.assign(fHitInd[iPart].begin(), fHitInd[iPart].end());

	for (unsigned short int iHit1 = 0; iHit1 < (nofHits & ~3); iHit1+=4) {
		iH11 = hitIndPart[iHit1];
		iH12 = hitIndPart[iHit1+1];
		iH13 = hitIndPart[iHit1+2];
		iH14 = hitIndPart[iHit1+3];

		iH1X = fvec(fData[iH11].fX, fData[iH12].fX, fData[iH13].fX, fData[iH14].fX);
		iH1Y = fvec(fData[iH11].fY, fData[iH12].fY, fData[iH13].fY, fData[iH14].fY);

		for (unsigned short int iHit2 = iHit1 + 1; iHit2 < nofHits; iHit2++) {
			iH2 = hitIndPart[iHit2];
			iH2X = fvec(fData[iH2].fX);
			iH2Y = fvec(fData[iH2].fY);

			rx0 = iH1X - iH2X;//rx12
			ry0 = iH1Y- iH2Y;//ry12
			//r12 = rx0 * rx0 + ry0 * ry0;
			//if (r12 < fMinDistanceSq || r12 > fMaxDistanceSq)	continue;

			t10 = fvec(fData[iH11].fX2plusY2 - fData[iH2].fX2plusY2,
					fData[iH12].fX2plusY2 - fData[iH2].fX2plusY2,
					fData[iH13].fX2plusY2 - fData[iH2].fX2plusY2,
					fData[iH14].fX2plusY2 - fData[iH2].fX2plusY2);
			for (unsigned short int iHit3 = iHit2 + 1; iHit3 < nofHits; iHit3++) {
				iH3 = hitIndPart[iHit3];
				iH3X = fvec(fData[iH3].fX);
				iH3Y = fvec(fData[iH3].fY);
				t5 = fvec(fData[iH2].fX2plusY2 - fData[iH3].fX2plusY2);

				//rx1 = iH1X - iH3X;//rx13
				//ry1 = iH1Y - iH3Y;//ry13
				//r13 = rx1 * rx1 + ry1 * ry1;
				//if (r13 < fMinDistanceSq || r13 > fMaxDistanceSq)continue;

				rx2 = iH2X - iH3X;//rx23
				ry2 = iH2Y - iH3Y;//ry23
				//r23 = rx2 * rx2 + ry2 * ry2;
				//if (r23	< fMinDistanceSq || r23 > fMaxDistanceSq)continue;

			    //det = rx2*ry0 - rx0*ry2;
			    //if (det == 0.0f) continue;
				t19 = fvec05 / (rx2*ry0 - rx0*ry2);

				xc = (t5 * ry0 - t10 * ry2) * t19;
				xcs = xc - fCurMinXV;
				//intX = int( xcs *dx);
				//if (intX < 0 || intX >= fNofBinsX ) continue;

				yc = (t10 * rx2 - t5 * rx0) * t19;
				ycs = yc - fCurMinYV;
				//intY = int( ycs *dy);
				//if (intY < 0 || intY >= fNofBinsY ) continue;

				 //radius calculation
				t6 = iH1X - xc;
				t7 = iH1Y - yc;
				r = sqrt(t6 * t6 + t7 * t7);

				//intR = int(r *dr);
				//if (intR < 0 || intR > fNofBinsR) continue;
				//indXY = intX * fNofBinsX + intY;
				intXv = xcs *dxv;
				intYv = ycs *dyv;
				intRv = r *drv;
				intXf = (float*)&intXv.v;
				intYf = (float*)&intYv.v;
				intRf = (float*)&intRv.v;
				for (register unsigned short int iv = 0; iv < 4; iv++){
					intX = int(intXf[iv]);
					intY = int(intYf[iv]);
					intR = int(intRf[iv]);

					//if (r12 < fMinDistanceSq || r12 > fMaxDistanceSq)	continue;
					//if (r13 < fMinDistanceSq || r13 > fMaxDistanceSq)continue;
					//if (r23	< fMinDistanceSq || r23 > fMaxDistanceSq)continue;
					if (intR < 10 || intR>= fNofBinsR) continue;
					//if (intX < 0 || intX >= fNofBinsX ) continue;
					//if (intY < 0 || intY >= fNofBinsY ) continue;
					//__m128i im;
					//_mm_cvtps_epi32(intXv);

					indXY = intX * fNofBinsX + intY;
					if (indXY < 0 || indXY >= fNofBinsXY ) continue;
					fHist[indXY]++;
					fHistR[intR]++;
				}
			}//iHit1
		}//iHit2
	}//iHit3
}



void CbmRichRingFinderHoughSimd::HoughTransformReconstruction()
{
    Int_t indmin, indmax;
    Float_t x0, y0;

    for (UInt_t iHit = 0; iHit < fData.size(); iHit++)
    {
        if (fData[iHit].fIsUsed == true) continue;

		x0 = fData[iHit].fX;
		y0 = fData[iHit].fY;

        fCurMinX = x0  - fMaxDistance;
		fCurMinY = y0 - fMaxDistance;

		DefineLocalAreaAndHits(x0, y0, &indmin, &indmax);
		HoughTransform(indmin, indmax);
		FindPeak(indmin, indmax);

    }//main loop over hits
}


//ClassImp(CbmRichRingFinderHoughSimd)
