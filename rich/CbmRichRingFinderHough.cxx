// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHough source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Simeon Lebedev (salebedev@jinr.ru)                            -----
// ----- Implementation: Simeon Lebedev (salebedev@jinr.ru)  and Andrei Lebedev (alebedev@jinr.ru)-----

//This program performs the procedure of the ring recognition from data obtained from
//the CBM RICH detector.

#include "CbmRichRingFinderHough.h"
//#include "CbmRichFuzzyKE.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"

#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"

#include <iostream>
#include <cmath>
#include <set>
#include <algorithm>


using std::cout;
using std::endl;
using std::vector;
//--------------------------------------------------------------------------
// -----   Default constructor   -------------------------------------------
CbmRichRingFinderHough::CbmRichRingFinderHough()
{


}

// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderHough::CbmRichRingFinderHough  ( Int_t verbose )
{

}

// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderHough::~CbmRichRingFinderHough()
{

}


void CbmRichRingFinderHough::Init()
{
    if (fVerbose) cout << "CbmRichRingFinderHough::init() "<<endl;
    fNEvent = 0;
    String geometryType = "compact";
    SetParameters(geometryType);

    fHist.resize(fNofBinsX);
    for (Int_t i = 0; i < fNofBinsX; i++) fHist[i].resize(fNofBinsY);

    fRingHits.resize(fNofBinsX);
    for (Int_t i = 0; i < fNofBinsX; i++) fRingHits[i].resize(fNofBinsY);


    fHistR.resize(fNofBinsR);
    fRingHitsR.resize(fNofBinsR);

    fFitCOP = new CbmRichRingFitterCOP(0, 0);
    fFitCOP->Init();

    fFitEllipseTau = new CbmRichRingFitterEllipseTau(0, 0, geometryType);
    fFitEllipseTau->Init();

}

Int_t CbmRichRingFinderHough::DoFind(TClonesArray* rHitArray,
                                      TClonesArray* rProjArray,
                                         TClonesArray* rRingArray) {
	  TStopwatch timer;
	  timer.Start();

	  ++fNEvent;
	  if (fVerbose) cout << "-------------------------    Event no. " << fNEvent << "   -------------------------"<<endl;

	  fData.clear();
	  fFoundRings.clear();
	  for (Int_t i = 0;  i < fFoundRingsHitId.size();i++)
		  fFoundRingsHitId[i].clear();
	  fFoundRingsHitId.clear();

	  std::vector<CbmRichMyPoint> UpH;
	  std::vector<CbmRichMyPoint> DownH;

	  if ( !rHitArray) {
	      cout << "-E- CbmRichRingFinderHough::DoFind: Hit array missing! " << rHitArray << endl;
	      return -1;
	  }
	  const Int_t nhits = rHitArray->GetEntriesFast();
	  if(!nhits) {
	      cout << "-E- CbmRichRingFinderHough::DoFind:No hits in this event." << endl;
	      return -1;
	  }
      for(Int_t iHit = 0; iHit < nhits; iHit++) {
    	  CbmRichHit * hit = (CbmRichHit*)rHitArray->At(iHit);
    	  if(hit){
    		  CbmRichMyPoint tempPoint;
    		  tempPoint.fX = hit->X();
    		  tempPoint.fY = hit->Y();
    		  tempPoint.fX2plusY2 = hit->X() * hit->X() + hit->Y() * hit->Y();
    		  tempPoint.fId = iHit;
    		  tempPoint.fRefIndex = hit->GetRefIndex();
              tempPoint.fIsUsed = false;
            if (hit->Y() >= 0)
                UpH.push_back(tempPoint);
            else DownH.push_back(tempPoint);
        }
    }

    fData = UpH;
    std::sort(fData.begin(), fData.end(), CbmRichMyPoint::CmpUp);
    HoughTransformReconstruction();

    fData.clear();
    fData = DownH;
    std::sort(fData.begin(), fData.end(), CbmRichMyPoint::CmpUp);
    HoughTransformReconstruction();

    RingSelection();

    //FuzzyKE(rHitArray);

    //Add found rings to output file
    cout  << " Number of found ring = " << fFoundRings.size() << endl;
    Int_t ringCount = 0;
    for(UInt_t iRing=0; iRing < fFoundRings.size(); iRing++) {
        if (fFoundRings[iRing].GetRecFlag() == -1) continue;

    	new ((*rRingArray)[ringCount]) CbmRichRing();
        CbmRichRing * r = (CbmRichRing *)rRingArray->At(ringCount);
        r->SetCenterX(fFoundRings[iRing].GetCenterX());
        r->SetCenterY(fFoundRings[iRing].GetCenterY());
        r->SetRadius(fFoundRings[iRing].GetRadius());
        for (Int_t ith = 0; ith < fFoundRings[iRing].GetNofHits();ith ++){
            r->AddHit(fFoundRings[iRing].GetHit(ith));
        }
        ringCount++;
    }

    timer.Stop();
    cout << "TIME  CPU time " <<  timer.CpuTime() << " s" << endl;

    if (fVerbose) cout << "CbmRichRingFinderHough: Number of output rings: " << rRingArray->GetEntriesFast() << endl;


    return 1;
}

void CbmRichRingFinderHough::Finish()
{

}

///Set Parameters for specify geometry
void CbmRichRingFinderHough::SetParameters(TString geometry)
{
    cout << "-I- CbmRichRingFinderHough::SetParameters for " << geometry << " RICH geometry"<<endl;
    if (geometry != "small" && geometry != "compact" && geometry != "standard" && geometry != "large"){
        geometry = "standard";
        cout << "-E- CbmRichRingFinderHough::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }

    TString richSelectNNFile = gSystem->Getenv("VMCWORKDIR");

    if (geometry == "standard" || geometry == "large"){
        fMaxDistance = 14;
        fMinDistance = 3.;
        fMinDistance2 = fMinDistance*fMinDistance;
        fMaxDistance2 = fMaxDistance*fMaxDistance;

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

        richSelectNNFile += "/parameters/rich/NeuralNet_RingSelection_Weights.txt";
    }

    if (geometry == "small" || geometry == "compact"){
        fMaxDistance = 11.5;
        fMinDistance = 2.5;
        fMinDistance2 = fMinDistance*fMinDistance;
        fMaxDistance2 = fMaxDistance*fMaxDistance;

        fMinRadius = 3.3;
        fMaxRadius = 5.7;

        fHTCut = 50;
        fHitCut = 5;

        fHTCutR = 20;
        fHitCutR = 3;

        fNofBinsX = 15;
        fNofBinsY = 15;
        fNofBinsR = 40;

		fAnnCut = 0.0;
		fUsedHitsCut = 0.4;
		fUsedHitsAllCut = 0.4;

	    richSelectNNFile += "/parameters/rich/NeuralNet_RingSelection_Weights_Compact.txt";
    }

    fANNSelect = new CbmRichRingSelectNeuralNet(0, richSelectNNFile);
    fANNSelect->Init();
}

void CbmRichRingFinderHough::HoughTransformReconstruction()
{
    if (fVerbose > 1) cout << "void CbmRichRingFinderHough::HoughTransformReconstruction()"<< endl;

    CbmRichMyPoint mpnt;
    std::vector<CbmRichMyPoint>::iterator itmin, itmax;
    Int_t indmin, indmax;
    Double_t r12, r13, r23;
    Double_t x2y2[3], rx[3], ry[3];
    Double_t xc, yc, r;
    ///to store distances from (X0, Y0) hit to others hits in the corridor;
    std::vector<Double_t> distX0Y0;

    Double_t kMINX, kMINY;
    Double_t ix, iy, ir;
    Int_t intX, intY, intR;

    Double_t dx = 2*fMaxDistance / (Double_t)fNofBinsX;
    Double_t dy = fMaxDistance / (Double_t)fNofBinsY;
    Double_t dr = (fMaxRadius+1.) / (Double_t)fNofBinsR;

    if (fVerbose > 1) cout << "fData.size() = "<< fData.size() << endl;

    for (UInt_t iHit = 0; iHit < fData.size(); iHit++)
    {
        if (fData[iHit].fIsUsed == true) continue;

        kMINX = fData[iHit].fX - fMaxDistance;
        kMINY = fData[iHit].fY - fMaxDistance;

        //find all hits which are in the corridor
        mpnt.fX = fData[iHit].fX - fMaxDistance;
        itmin = std::lower_bound(fData.begin(),
                    fData.end(), mpnt, CbmRichMyPoint::CmpUp);

        mpnt.fX = fData[iHit].fX + fMaxDistance;
        itmax = std::lower_bound(fData.begin(),
                    fData.end(), mpnt, CbmRichMyPoint::CmpUp)- 1 ;

        indmin = itmin - fData.begin();
        indmax = itmax - fData.begin();

        for (Int_t j = 0; j < fNofBinsX; j++)
            for (Int_t k = 0; k < fNofBinsY; k++){
                fHist[j][k] = 0;
                fRingHits[j][k].clear();
                fRingHits[j][k].resize(indmax - indmin + 1);
            }

        for (Int_t k = 0; k < fNofBinsR; k++){
            fHistR[k] = 0;
            fRingHitsR[k].clear();
            fRingHitsR[k].resize(indmax - indmin + 1);
        }

        ///fill distX0Y0
        distX0Y0.clear();
        distX0Y0.reserve(200);
        for (Int_t i = indmin; i <=indmax; i++)
        {
            distX0Y0.push_back( sqrt( (fData[iHit].fX - fData[i].fX)*
                                       (fData[iHit].fX - fData[i].fX)+
                                       (fData[iHit].fY - fData[i].fY)*
                                       (fData[iHit].fY - fData[i].fY) ) );
        }
        Int_t nofPart = 1;
        for (Int_t iPart = 0; iPart < nofPart; iPart++)
        for (Int_t iHit1 = indmin + iPart; iHit1 <= indmax; iHit1 +=nofPart)
        {
            //if (iHit1 % 2 == iPart) continue;
            if (fData[iHit1].fIsUsed == true) continue;
            //if ( fabs(fData[iHit].fY - fData[iHit1].fY) > 0.7*fMaxDistance  ) continue;
            if (distX0Y0[iHit1 - indmin] > fMaxDistance) continue;

            for (Int_t iHit2 = iHit1 + 1; iHit2 <= indmax; iHit2+=nofPart)
            {
                //if (iHit1 % 2 == iPart) continue;
                if (fData[iHit2].fIsUsed == true) continue;
                //if ( fabs(fData[iHit].fY - fData[iHit2].fY) > 0.7*fMaxDistance ) continue;
                if (distX0Y0[iHit2 - indmin] > fMaxDistance) continue;

                rx[0] = fData[iHit1].fX - fData[iHit2].fX;//rx12
                ry[0] = fData[iHit1].fY - fData[iHit2].fY;//ry12
                r12 = rx[0] * rx[0] + ry[0] * ry[0];
                if (r12 < fMinDistance2 || r12 > fMaxDistance2) continue;

                for (Int_t iHit3 = iHit2 + 1; iHit3 <= indmax; iHit3+=nofPart)
                {
                    //if (iHit1 % 2 == iPart) continue;
                    if (fData[iHit3].fIsUsed == true) continue;
                    //if ( fabs(fData[iHit].fY - fData[iHit3].fY) > 0.7*fMaxDistance ) continue;
                    if (distX0Y0[iHit3 - indmin] > fMaxDistance) continue;

                    rx[1] = fData[iHit1].fX - fData[iHit3].fX;//rx13
                    rx[2] = fData[iHit2].fX - fData[iHit3].fX;//rx23

                    ry[1] = fData[iHit1].fY - fData[iHit3].fY;//ry13
                    ry[2] = fData[iHit2].fY - fData[iHit3].fY;//ry23

                    r13 = rx[1] * rx[1] + ry[1] * ry[1];
                    r23 = rx[2] * rx[2] + ry[2] * ry[2];

                    if (r13 < fMinDistance2 || r13 > fMaxDistance2 ||
                        r23 < fMinDistance2 || r23 > fMaxDistance2 ) continue;
                    //if (r13 < fMinDistance || r13 > fMaxDistance ||
                    //    r23 < fMinDistance || r23 > fMaxDistance ) continue;


                    x2y2[0] = fData[iHit1].fX2plusY2;
                    x2y2[1] = fData[iHit2].fX2plusY2;
                    x2y2[2] = fData[iHit3].fX2plusY2;

                   // x[0] = fData[iHit1].fX;
                   // x[1] = fData[iHit2].fX;
                    //x[2] = fData[iHit3].fX;

                  //  y[0] = fData[iHit1].fY;
                   // y[1] = fData[iHit2].fY;
                    //y[2] = fData[iHit3].fY;

                    CalculateRingParameters(x2y2, rx, ry, fData[iHit1].fX, fData[iHit1].fY, &xc, &yc, &r);
                    //CalculateRingParametersOld(x, y, &xc, &yc, &r);

                    xc = xc - kMINX;
                    yc = yc - kMINY;

                    if (xc < 0 || xc > 2*fMaxDistance ||
                        yc < 0 || yc > 2*fMaxDistance) continue;

                    modf(xc / dx, &ix);
                    modf(yc / dy, &iy);
                    modf(r / dr, &ir);

                    intX = (int)ix;
                    intY = (int)iy;
                    intR = (int)ir;

                    if (intX < 0 || intX >= fNofBinsX ||
                        intY < 0 || intY >= fNofBinsY) continue;
                    if (r < fMinRadius || r > fMaxRadius) continue;

                    fHist[intX][intY]++;
                    fRingHits[intX][intY][iHit1-indmin]++;
                    fRingHits[intX][intY][iHit2-indmin]++;
                    fRingHits[intX][intY][iHit3-indmin]++;
                    fHistR[intR]++;
                    fRingHitsR[intR][iHit1-indmin]++;
                    fRingHitsR[intR][iHit2-indmin]++;
                    fRingHitsR[intR][iHit3-indmin]++;

                }//iHit1
            }//iHit2
        }//iHit3
        //FindAllPeaks(indmin, indmax);
        FindPeak(indmin, indmax);
    }//main loop over hits

}


void CbmRichRingFinderHough::CalculateRingParameters(Double_t x2y2[],
                             Double_t rx[],
                             Double_t ry[],
                             Double_t x0,
                             Double_t y0,
                             Double_t *xc,
                             Double_t *yc,
                             Double_t *r)
{
    register Double_t t5, t10, t19, t21, t41, det;

    t5 = x2y2[1] - x2y2[2];
    t10 = x2y2[0] - x2y2[1];
    det = rx[2]*ry[0] - rx[0]*ry[2];

    if (det != 0){
        t19 = 0.10e1 / det;
        *xc = 0.5e0 * (t5 * ry[0] - t10 * ry[2]) * t19;
        *yc = 0.5e0 * (t10 * rx[2] - t5 * rx[0]) * t19;

         //radius calculation
         t21 = (x0 - *xc) * (x0 - *xc);
         t41 = (y0 - *yc) * (y0 - *yc);
         *r = sqrt(t21 + t41);

    }else {
        *xc = -1;
        *yc = -1;
        *r = -1;
    }
}

void CbmRichRingFinderHough::CalculateRingParametersOld(Double_t x[],
                            Double_t y[],
                            Double_t *xc,
                            Double_t *yc,
                            Double_t *r)
{
    register Double_t t1, t2, t3, t4, t5, t6, t8, t9,
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
    t19 = 0.10e1 / (t14 * t6 - t16 * t11);

    *xc = 0.5e0 * (t5 * t6 - t10 * t11) * t19;
    *yc = 0.5e0 * (t10 * t14 - t5 * t16) * t19;

    t21 = pow(x[0] - *xc, 0.2e1);
    t41 = pow(y[0] - *yc, 0.2e1);
    *r = pow(t21 + t41, 0.5e0);
}

void CbmRichRingFinderHough::FindMaxBinsXYR(Int_t *maxBinX, Int_t *maxBinY, Int_t *maxBinR)
{
    Int_t maxBin = -1, binX = -1, binY = -1, binR = -1;

    for (UInt_t j = 0; j < fHist.size(); j++)
    for (UInt_t k = 0; k < fHist[j].size(); k++){
        if (fHist[j][k] > maxBin){
            maxBin = fHist[j][k];
            binX = j;
            binY = k;
        }
    }

    maxBin = -1;
    for (UInt_t k = 0; k < fHistR.size(); k++){
        if (fHistR[k] > maxBin){
            maxBin = fHistR[k];
            binR = k;
        }
    }

    *maxBinX = binX;
    *maxBinY = binY;
    *maxBinR = binR;
}

void CbmRichRingFinderHough::RemoveHitsAroundEllipse(Int_t indmin, Int_t indmax, CbmRichRing * ring)
{
	Double_t A = ring->GetAPar();
	Double_t B = ring->GetBPar();
	Double_t C = ring->GetCPar();
	Double_t D = ring->GetDPar();
	Double_t E = ring->GetEPar();
	Double_t F = ring->GetFPar();

	Double_t rms = TMath::Sqrt( ring->GetChi2()/ring->GetNofHits() );
	Double_t dCut = rms;
	if (dCut > 0.3) dCut = 0.3;

	for(Int_t j = 0; j < indmax - indmin + 1; j++){
		Double_t x = fData[j + indmin].fX;
		Double_t y = fData[j + indmin].fY;

        Double_t d1 = TMath::Abs(A*x*x + B*x*y + C*y*y + D*x + E*y + F);
        Double_t d2 = sqrt( pow(2*A*x + B*y + D, 2) + pow(B*x + 2*C*y + E, 2) );

        Double_t d = d1/d2;
        if (d < dCut) fData[j+indmin].fIsUsed = true;
	}
}

void CbmRichRingFinderHough::RemoveHitsAroundRing(Int_t indmin, Int_t indmax, CbmRichRing * ring)
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


void CbmRichRingFinderHough::FindPeak(Int_t indmin, Int_t indmax)
{
    Int_t maxBinNumX, maxBinNumY, maxBinNumR;
	FindMaxBinsXYR(&maxBinNumX, &maxBinNumY, &maxBinNumR);

	Int_t maxBin = fHist[maxBinNumX][maxBinNumY];
	Int_t maxBinR = fHistR[maxBinNumR ];
	if (maxBinNumR >=1 && maxBinNumR <=fHistR.size())
		maxBinR += fHistR[maxBinNumR -1] + fHistR[maxBinNumR + 1];

	if (maxBin < fHTCut || maxBinR < fHTCutR) return;

	CbmRichRing ring1, ring2;
	Double_t hitCut = maxBin/10.;
	if (hitCut <= fHitCut)	hitCut = fHitCut;

	for (Int_t j = 0; j < indmax - indmin + 1; j++) {
		Int_t hitCutR = fRingHitsR[maxBinNumR][j];
		if (maxBinNumR >=1 && maxBinNumR <=fHistR.size())
			hitCutR += fRingHitsR[maxBinNumR+1][j] + fRingHitsR[maxBinNumR-1][j];

		if (fRingHits[maxBinNumX][maxBinNumY][j] >= hitCut
				&& hitCutR >= fHitCutR) {
			ring1.AddHit(fData[j + indmin].fId);
		}
	}

	fFitCOP->DoFit(&ring1);
	Double_t drCOPCut = 3*sqrt(ring1.GetChi2());
	if (drCOPCut > 1.2)	drCOPCut = 1.2;

	for (Int_t j = 0; j < indmax - indmin + 1; j++) {
		Double_t rx = fData[j + indmin].fX - ring1.GetCenterX();
		Double_t ry = fData[j + indmin].fY - ring1.GetCenterY();

		Double_t dr = fabs(sqrt(rx * rx + ry * ry) - ring1.GetRadius());
		if (dr < drCOPCut) {
			//fData[j+indmin].fIsUsed = true;
			ring2.AddHit(fData[j + indmin].fId);
		}
	}

	fFitEllipseTau->DoFit(&ring2);

//	fFitEllipse->DoFit(&ring2);

	fANNSelect->DoSelect(&ring2);
	//remove found hits only for good quality rings
	if (ring2.GetSelectionNN() > fAnnCut) {
		//RemoveHitsAroundRing(indmin, indmax, &ring2);
		RemoveHitsAroundEllipse(indmin, indmax, &ring2);

		//fFoundRings.push_back(ring2);
	}
	fFoundRings.push_back(ring2);

}

void CbmRichRingFinderHough::RingSelection()
{
	std::sort(fFoundRings.begin(), fFoundRings.end(), CbmRichRingComparatorMore());
	std::vector<std::set<Int_t> > usedHits;
	std::set<Int_t> usedHitsAll;

	Int_t nofRings = fFoundRings.size();
	for (Int_t iRing = 0; iRing < nofRings; iRing++){
		fFoundRings[iRing].SetRecFlag(-1);
		CbmRichRing ring = fFoundRings[iRing];
		Int_t nofHits = ring.GetNofHits();
		Bool_t isGoodRing = true;

		for (Int_t iRSet = 0; iRSet < usedHits.size(); iRSet++){
			Int_t nofUsedHits = 0;
			for(Int_t iHit = 0; iHit < nofHits; iHit++){
				std::set<Int_t>::iterator it = usedHits[iRSet].find(ring.GetHit(iHit));
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
			std::set<Int_t>::iterator it = usedHitsAll.find(ring.GetHit(iHit));
			if(it != usedHitsAll.end()){
				nofUsedHitsAll++;
			}
		}
		if ((Double_t)nofUsedHitsAll/(Double_t)nofHits > fUsedHitsAllCut){
			isGoodRingAll = false;
		}


		if (isGoodRing && isGoodRingAll){
			fFoundRings[iRing].SetRecFlag(1);
			std::set<Int_t> usedHitsT;
			for(Int_t iHit = 0; iHit < nofHits; iHit++){
				usedHitsT.insert(ring.GetHit(iHit));
				usedHitsAll.insert(ring.GetHit(iHit));
			}
			usedHits.push_back(usedHitsT);
		}
	}
}


void CbmRichRingFinderHough::FuzzyKE(TClonesArray* rHitArray)
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

ClassImp(CbmRichRingFinderHough)
