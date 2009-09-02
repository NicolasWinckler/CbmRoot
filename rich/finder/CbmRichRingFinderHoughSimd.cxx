// --------------------------------------------------------------------------------------
// CbmRichRingFinderHoughSimd source file
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

#include "../L1/L1Algo/vectors/P4_F32vec4.h"

#include "CbmRichRingFinderHoughSimd.h"

fvec m0_5 = fvec(0.5f);
fvec m1 = fvec(1.0f);

void CalculateRingParametersOldVec(fvec x[],
			fvec y[],
			fvec *xc,
			fvec *yc,
			fvec *r){
	fvec t1, t2, t3, t4, t5, t6, t8, t9,
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
    t19 = m1 / (t14 * t6 - t16 * t11);

    *xc = m0_5 * (t5 * t6 - t10 * t11) * t19;
    *yc = m0_5 * (t10 * t14 - t5 * t16) * t19;

    t21 = (x[0] - *xc)*(x[0] - *xc);
    t41 = (y[0] - *yc)*(y[0] - *yc);
    *r = sqrt(t21 + t41);

}

// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderHoughSimd::CbmRichRingFinderHoughSimd  ( Int_t verbose, TString geometry )
{
    cout << "-I- CbmRichRingFinderHough constructor for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHough::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }
    fGeometryType = geometry;

}

// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderHoughSimd::~CbmRichRingFinderHoughSimd()
{

}


void CbmRichRingFinderHoughSimd::HoughTransformSimd(unsigned short int indmin, unsigned short int indmax)
{
    fvec xcv, ycv, rv;
    fvec xv[3], yv[3];
    fvec rx13v, rx23v, ry13v, ry23v, rx12v, ry12v;
    fvec r12v, r13v, r23v;
    fvec curMinX = fvec(fCurMinX);
    fvec curMinY = fvec(fCurMinY);
    fvec t5, t10, t19, t21, t41, det;
    fvec t4, t6;
    Float_t r12, rx12, ry12;
    Int_t intX, intY, intR, indXY;
    Float_t xc, yc, r;
    Int_t nofHits;

    register unsigned short int iH1, iH2, iH3;
    register unsigned short int iH1_1, iH2_1, iH3_1;
    register Int_t nofHitsNorm = fHitInd[0].size() + 1;
    Int_t iPmulNofHits;


    for (Int_t iPart = 0; iPart < fNofParts; iPart++){
    Int_t nofHits = fHitInd[iPart].size();
    if (nofHits <= fMinNofHitsInArea) return;
    iPmulNofHits = iPart * nofHitsNorm;

	for (Int_t iHit1 = 0; iHit1 < nofHits; iHit1++)
	{
		iH1 = fHitInd[iPart][iHit1];
		iH1_1 = iPmulNofHits + iHit1;

		xv[0] = fvec(fData[iH1].fX);
		yv[0] = fvec(fData[iH1].fY);

		for (Int_t iHit2 = iHit1 + 1; iHit2 < nofHits; iHit2++)
		{
			iH2 = fHitInd[iPart][iHit2];
			iH2_1 = iPmulNofHits + iHit2;

			rx12 = fData[iH1].fX - fData[iH2].fX;//rx12
			ry12 = fData[iH1].fY - fData[iH2].fY;//ry12
			r12 = rx12 * rx12 + ry12 * ry12;
			if (r12 < fMinDistanceSq || r12 > fMaxDistanceSq) continue;

			xv[1] = fvec(fData[iH2].fX);
			yv[1] = fvec(fData[iH2].fY);
			t10 = fvec(fData[iH1].fX2plusY2 - fData[iH2].fX2plusY2);
			r12v = fvec(r12);
			t4 = fvec(fData[iH2].fX2plusY2);

			for (Int_t iHit3 = iHit2 + 1; iHit3 < nofHits - 3; iHit3+=4)
			{
				iH3 = fHitInd[iPart][iHit3];
				iH3_1 = iPmulNofHits + iHit3;

				xv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fX,fData[ fHitInd[iPart][iHit3+1] ].fX,
						fData[ fHitInd[iPart][iHit3+2] ].fX,fData[ fHitInd[iPart][iHit3+3] ].fX);

				yv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fY,
						fData[ fHitInd[iPart][iHit3+1] ].fY,
						fData[ fHitInd[iPart][iHit3+2] ].fY,
						fData[ fHitInd[iPart][iHit3+3] ].fY);

				t6 = fvec(fData[ fHitInd[iPart][iHit3] ].fX2plusY2,
						fData[ fHitInd[iPart][iHit3+1] ].fX2plusY2,
						fData[ fHitInd[iPart][iHit3+2] ].fX2plusY2,
						fData[ fHitInd[iPart][iHit3+3] ].fX2plusY2);

				t5 = t4 - t6;

				rx13v = xv[0] - xv[2];//fData[iH1].fX - fData[iH3].fX;//rx13
				rx23v = xv[1] - xv[2];//fData[iH2].fX - fData[iH3].fX;//rx23

				ry13v = yv[0] - yv[2];// fData[iH1].fY - fData[iH3].fY;//ry13
				ry23v = yv[1] - yv[2];// fData[iH2].fY - fData[iH3].fY;//ry23

				r13v = rx13v * rx13v + ry13v * ry13v;
				r23v = rx23v * rx23v + ry23v * ry23v;

			    det = rx23v*ry12v - rx12v*ry23v;
			   // if (det == 0.0f) continue;
				t19 = m0_5 / det;
				xcv = (t5 * ry12v - t10 * ry23v) * t19;
				ycv = (t10 * rx23v - t5 * rx12v) * t19;

				 //radius calculation
				t21 = (xv[0] - xcv) * (xv[0] - xcv);
				t41 = (yv[0] - ycv) * (yv[0] - ycv);
				rv = sqrt(t21 + t41);

				xcv = xcv - curMinX;
				ycv = ycv - curMinY;

				for (int iv = 0; iv < 4; iv++){
					if (r13v[iv] < fMinDistanceSq || r13v[iv] > fMaxDistanceSq ||
					 r23v[iv] < fMinDistanceSq || r23v[iv] > fMaxDistanceSq ) continue;

					intX = (int)(xcv[iv] / fDx);
					intY = (int)(ycv[iv] / fDy);
					intR = (int)(rv[iv] / fDr);

					if (intX < 0 || intX >= fNofBinsX ||
						intY < 0 || intY >= fNofBinsY) continue;
					if (rv[iv] < fMinRadius || rv[iv] > fMaxRadius) continue;
					cout << rv[iv] << " ";
					indXY = intX * fNofBinsX + intY;
					fHist[indXY]++;
					fRingHits[indXY][iH1_1]++;
					fRingHits[indXY][iH2_1]++;
					fRingHits[indXY][iH3_1+ iv]++;
					fHistR[intR]++;
					fRingHitsR[intR][iH1_1]++;
					fRingHitsR[intR][iH2_1]++;
					fRingHitsR[intR][iH3_1+ iv]++;
				}//iv
			}//iHit1
		}//iHit2
	}//iHit3
    }//iPart
}

void CbmRichRingFinderHoughSimd::HoughTransformSimd2(unsigned short int indmin, unsigned short int indmax)
{
    Float_t r12, r13, r23;
    register Float_t rx[3], ry[3], x[3], y[3];
    register Float_t xc, yc, r;
    register Float_t xcs, ycs; // xcs = xc - fCurMinX
    Int_t intX, intY, intR;
    Int_t indXY;
    register unsigned short int iH1, iH2, iH3;
    register unsigned short int iH1_1, iH2_1, iH3_1;
    register Int_t nofHitsNorm = fHitInd[0].size() + 1;
    Int_t iPmulNofHits;
    //register Float_t t5, t10, t19, t21, t41, det;

    fvec xv[3], yv[3];
    fvec rxv[3], ryv[3];
    fvec r12v, r13v, r23v;
    fvec t5v, t10v, t19v, t21v, t41v, detv;
    fvec xcv, ycv, rv;
    fvec curMinXv = fvec(fCurMinX);
    fvec curMinYv = fvec(fCurMinY);
    fvec intXv, intYv, intRv;
    fvec dxv = fvec(1.0f/fDx);
    fvec dyv = fvec(1.0f/fDy);
    fvec drv = fvec(1.0f/fDr);


    for (Int_t iPart = 0; iPart < fNofParts; iPart++){
    Int_t nofHits = fHitInd[iPart].size();
    if (nofHits <= fMinNofHitsInArea) return;
    iPmulNofHits = iPart * nofHitsNorm;

	for (unsigned short int iHit1 = 0; iHit1 < nofHits; iHit1++) {
		iH1 = fHitInd[iPart][iHit1];
		iH1_1 = iPmulNofHits + iHit1;

		xv[0] = fvec(fData[iH1].fX);
		yv[0] = fvec(fData[iH1].fY);
		for (unsigned short int iHit2 = iHit1 + 1; iHit2 < nofHits; iHit2++) {
			iH2 = fHitInd[iPart][iHit2];
			iH2_1 = iPmulNofHits + iHit2;

			xv[1] = fvec(fData[iH2].fX);
			yv[1] = fvec(fData[iH2].fY);

			rx[0] = fData[iH1].fX - fData[iH2].fX;//rx12
			ry[0] = fData[iH1].fY - fData[iH2].fY;//ry12
			r12 = rx[0] * rx[0] + ry[0] * ry[0];
			if (r12 < fMinDistanceSq || r12 > fMaxDistanceSq)	continue;

			t10v = fvec(fData[iH1].fX2plusY2 - fData[iH2].fX2plusY2);
			rxv[0] = fvec(rx[0]);
			ryv[0] = fvec(ry[0]);

			for (unsigned short int iHit3 = iHit2 + 1; iHit3 < nofHits + 4; iHit3+=4) {
				iH3 = fHitInd[iPart][iHit3];
				iH3_1 = iPmulNofHits + iHit3;

				if (iHit3+3 < nofHits){
					xv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fX,	fData[ fHitInd[iPart][iHit3+1] ].fX,
							fData[ fHitInd[iPart][iHit3+2] ].fX, fData[ fHitInd[iPart][iHit3+3] ].fX);
					yv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fY,	fData[ fHitInd[iPart][iHit3+1] ].fY,
							fData[ fHitInd[iPart][iHit3+2] ].fY, fData[ fHitInd[iPart][iHit3+3] ].fY);
				}else if (iHit3+2 < nofHits){
					xv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fX,	fData[ fHitInd[iPart][iHit3+1] ].fX,
							fData[ fHitInd[iPart][iHit3+2] ].fX, 0.0f);
					yv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fY,	fData[ fHitInd[iPart][iHit3+1] ].fY,
							fData[ fHitInd[iPart][iHit3+2] ].fY, 0.0f);
				}else if (iHit3+1 < nofHits){
					xv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fX,	fData[ fHitInd[iPart][iHit3+1] ].fX,
							0.0f, 0.0f);
					yv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fY,	fData[ fHitInd[iPart][iHit3+1] ].fY,
							0.0f, 0.0f);
				}else if (iHit3 < nofHits){
					xv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fX, 0.0f, 0.0f, 0.0f);
					yv[2] = fvec(fData[ fHitInd[iPart][iHit3] ].fY,	0.0f, 0.0f, 0.0f);
				}

				rxv[1] = xv[0]- xv[2];//rx13
				ryv[1] = yv[0] - yv[2];//ry13
				r13v = rxv[1] * rxv[1] + ryv[1] * ryv[1];

				rxv[2] = xv[1] - xv[2];//rx23
				ryv[2] = yv[1] - yv[2];//ry23
				r23v = rxv[2] * rxv[2] + ryv[2] * ryv[2];

			    detv = rxv[2]*ryv[0] - rxv[0]*ryv[2];
				t19v = m0_5 / detv;
				t5v = xv[1]*xv[1]+ yv[1]*yv[1] - xv[2]*xv[2]- yv[2]*yv[2];

				xcv = (t5v * ryv[0] - t10v * ryv[2]) * t19v;
				ycv = (t10v * rxv[2] - t5v * rxv[0]) * t19v;
				 //radius calculation
				t21v = (xv[0] - xcv) * (xv[0] - xcv);
				t41v = (yv[0] - ycv) * (yv[0] - ycv);
				rv = sqrt(t21v + t41v);

				xcv = xcv - curMinXv;
				ycv = ycv - curMinYv;

				intXv = xcv * dxv;
				intYv = ycv * dyv;
				intRv = rv * drv;

				for (Int_t iv = 0; iv < 4; iv++){
					if (xv[0][iv] == 0.0f) continue;

					if (r13v[iv] < fMinDistanceSq || r13v[iv] > fMaxDistanceSq)continue;
					if (r23v[iv]	< fMinDistanceSq || r23v[iv] > fMaxDistanceSq)continue;

					intX = (int)intXv[iv];
					if (intX < 0 || intX >= fNofBinsX ) continue;
					intY = (int)intYv[iv];
					if (intY < 0 || intY >= fNofBinsY ) continue;
					intR = (int)intRv[iv];
					if (rv[iv] < fMinRadius || rv[iv] > fMaxRadius) continue;

					indXY = intX * fNofBinsX + intY;

					fHist[indXY]++;
					fRingHits[indXY][iH1_1]++;
					fRingHits[indXY][iH2_1]++;
					fRingHits[indXY][iH3_1 + iv]++;
					fHistR[intR]++;
					fRingHitsR[intR][iH1_1]++;
					fRingHitsR[intR][iH2_1]++;
					fRingHitsR[intR][iH3_1 + iv]++;
				}//iv
			}//iHit1
		}//iHit2
	}//iHit3
    }//iPart

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
		HoughTransformSimd2(indmin, indmax);
		FindPeak(indmin, indmax);

    }//main loop over hits
}


ClassImp(CbmRichRingFinderHoughSimd)
