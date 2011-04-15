
// --------------------------------------------------------------------------------------
// CbmRichRingFinderHoughBase source file
// Base class for ring finders based on on HT method
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

#include "CbmRichRingFinderHoughImpl.h"
//#include "CbmRichFuzzyKE.h"
#include "CbmRichRingLight.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "FairTrackParam.h"
#include "../../littrack/std/utils/CbmLitMemoryManagment.h"

#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TMath.h"

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

CbmRichRingFinderHoughImpl::CbmRichRingFinderHoughImpl()
{


}

void CbmRichRingFinderHoughImpl::Init()
{
    SetParameters(fGeometryType);

    fHist.resize(fNofBinsXY);
    fHistR.resize(fNofBinsR);
	fHitInd.resize(fNofParts);

    fFitCOP = new CbmRichRingFitterCOPLight();

    TString richSelectNNFile = gSystem->Getenv("VMCWORKDIR");
    if (fGeometryType == "large"){
        richSelectNNFile += "/parameters/rich/NeuralNet_RingSelection_Weights.txt";
    }
    if (fGeometryType == "compact"){
	    richSelectNNFile += "/parameters/rich/NeuralNet_RingSelection_Weights_Compact.txt";
        //richSelectNNFile = "/u/slebedev/JUL09/trunk/macro/rich/NeuralNet_RingSelection_Weights1.txt";
    }
    fANNSelect = new CbmRichRingSelectNeuralNet(richSelectNNFile);
    fANNSelect->Init();

}

// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderHoughImpl::~CbmRichRingFinderHoughImpl()
{
	delete fFitCOP;
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
	fData.clear();
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
    //fNofParts = nofParts;

    fMaxDistance = maxDistance;
    fMinDistance = minDistance;
    fMinDistanceSq = fMinDistance*fMinDistance;
    fMaxDistanceSq = fMaxDistance*fMaxDistance;

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

    fMinNofHitsInArea = 6;

    fDx = 2.f*fMaxDistance / (Float_t)fNofBinsX;
    fDy = 2.f*fMaxDistance / (Float_t)fNofBinsY;
    fDr = fMaxRadius / (Float_t)fNofBinsR;
    fNofBinsXY = fNofBinsX * fNofBinsY;
}

///Set Parameters for specify geometry
void CbmRichRingFinderHoughImpl::SetParameters(TString geometry)
{
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHough::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }

    if (geometry == "large"){
        //fNofParts = 1;
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
    }

    if (geometry == "compact"){
        //fNofParts = 2;

        fMaxDistance = 11.5;
        fMinDistance = 3.;
        fMinDistanceSq = fMinDistance*fMinDistance;
        fMaxDistanceSq = fMaxDistance*fMaxDistance;

        fMinRadius = 3.3;
        fMaxRadius = 5.7;

//        if (fNofParts == 1){
//			fHTCut = 50;
//			fHitCut = 5;
//			fHTCutR = 20;
//			fHitCutR = 3;
//			fMinNofHitsInArea = 6;
//        }else if (fNofParts == 2){
			fHTCut = 5;
			fHitCut = 2;
			fHTCutR = 5;
			fHitCutR = 2;
			fMinNofHitsInArea = 4;
//        }

        fNofBinsX = 17;
        fNofBinsY = 25;
        fNofBinsR = 32;

		fAnnCut = -0.6;
		fUsedHitsCut = 0.4;
		fUsedHitsAllCut = 0.4;

    	fRmsCoeffEl = 2.5;
    	fMaxCutEl = 1.2;
    	fRmsCoeffCOP = 3.;
    	fMaxCutCOP = 1.2;
    }
	kMAX_NOF_HITS = 65500;
	fNofParts = 2;
    fDx = 1.36*fMaxDistance / (Float_t)fNofBinsX;
    fDy = 2.*fMaxDistance / (Float_t)fNofBinsY;
    fDr = fMaxRadius / (Float_t)fNofBinsR;
    fNofBinsXY = fNofBinsX * fNofBinsY;
}

void CbmRichRingFinderHoughImpl::HoughTransformReconstruction()
{
    Int_t indmin, indmax;
    for (UInt_t iHit = 0; iHit < fData.size(); iHit++)
    {
        if (fData[iHit].fIsUsed == true) continue;

        fCurMinX = fData[iHit].fHit.fX  - 0.36f*fMaxDistance;
		fCurMinY = fData[iHit].fHit.fY - fMaxDistance;

		DefineLocalAreaAndHits(fData[iHit].fHit.fX, fData[iHit].fHit.fY , &indmin, &indmax);
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
	mpnt.fHit.fX = x0 - 0.36f*fMaxDistance;
	itmin = std::lower_bound(fData.begin(), fData.end(), mpnt, CbmRichHoughHitCmpUp());

	mpnt.fHit.fX = x0 + fMaxDistance;
	itmax = std::lower_bound(fData.begin(), fData.end(), mpnt, CbmRichHoughHitCmpUp()) - 1;

	*indmin = itmin - fData.begin();
	*indmax = itmax - fData.begin();

	Int_t arSize = *indmax - *indmin + 1;
	if (arSize <= fMinNofHitsInArea) return;

//	for (Int_t i = 0; i < fNofParts; i++){
//		fHitInd[i].clear();
//		fHitInd[i].reserve( (*indmax-*indmin) / fNofParts);
//	}

	fDataPart1.clear();
	fDataPart1.reserve( (*indmax-*indmin) / fNofParts);
	fDataPart2.clear();
	fDataPart2.reserve( (*indmax-*indmin) / fNofParts);

	register Int_t indmin1=*indmin;
	register Int_t indmax1=*indmax;
	register Float_t maxDistance08 = 0.8f*fMaxDistance;

	if (fNofParts == 2){
		for (Int_t i = indmin1; i <= indmax1; i+=2) {
			if (fData[i].fIsUsed == true) continue;
			register Float_t ry = y0 - fData[i].fHit.fY;
			if (fabs(ry) > maxDistance08) continue;
			register Float_t rx = x0 - fData[i].fHit.fX;
			register Float_t d = rx	* rx +ry * ry;
			if (d > fMaxDistanceSq) continue;
//			fHitInd[0].push_back(i);
			fDataPart1.push_back(fData[i]);
		}
		for (Int_t i = indmin1+1; i <= indmax1; i+=2) {
			if (fData[i].fIsUsed == true) continue;
			register Float_t ry = y0 - fData[i].fHit.fY;
			if (fabs(ry) > maxDistance08) continue;
			register Float_t rx = x0 - fData[i].fHit.fX;
			register Float_t d = rx	* rx +ry * ry;
			if (d > fMaxDistanceSq) continue;
//			fHitInd[1].push_back(i);
			fDataPart2.push_back(fData[i]);
		}
	}else { //fNofParts!=2
//		for (Int_t i = indmin1; i <= indmax1; i++) {
//			if (fData[i]->fIsUsed == true) continue;
//			ry = y0 - fData[i]->fY;
//			if (fabs(ry) > maxDistance08) continue;
//			rx = x0 - fData[i]->fX;
//			Float_t d = rx	* rx +ry * ry;
//			if (d > fMaxDistanceSq) continue;
//
//			fHitInd[i % fNofParts].push_back(i);
//		}
	}

//	for (Int_t j = 0; j < fNofBinsXY; j++){
//		fHist[j] = 0;
//		cout << "fHist["<<j<<"]=0;" <<endl;
//	}

	InitHist();

//	for (Int_t k = 0; k < fNofBinsR; k++) {
//		fHistR[k] = 0;
//	}
}

void CbmRichRingFinderHoughImpl::HoughTransform(unsigned short int indmin, unsigned short int indmax)
{
    for (Int_t iPart = 0; iPart < fNofParts; iPart++){
//    	Int_t nofHits = fHitInd[iPart].size();
//	    if (nofHits <= fMinNofHitsInArea) continue;
    	HoughTransformGroup(indmin, indmax, iPart);
    }//iPart
}

void CbmRichRingFinderHoughImpl::HoughTransformGroup(unsigned short int indmin,
		unsigned short int indmax, Int_t iPart)
{
	register unsigned short nofHits;// = fHitInd[iPart].size();
    register float xcs, ycs; // xcs = xc - fCurMinX
    register float dx = 1.0f/fDx, dy = 1.0f/fDy, dr = 1.0f/fDr;

    register vector<CbmRichHoughHit> data;
    if (iPart == 0){
    	data = fDataPart1;
    	nofHits = fDataPart1.size();
    } else if (iPart == 1) {
    	data = fDataPart2;
    	nofHits = fDataPart2.size();
    }
	typedef vector<CbmRichHoughHit>::iterator iH;

	for (iH iHit1 = data.begin(); iHit1 != data.end(); iHit1++) {
		register float iH1X = iHit1->fHit.fX;
		register float iH1Y = iHit1->fHit.fY;

		  for (iH iHit2 = iHit1 + 1; iHit2 != data.end(); iHit2++) {
			register Float_t iH2X = iHit2->fHit.fX;
			register Float_t iH2Y = iHit2->fHit.fY;

			register float rx0 = iH1X - iH2X;//rx12
			register float ry0 = iH1Y- iH2Y;//ry12
			register float r12 = rx0 * rx0 + ry0 * ry0;
			if (r12 < fMinDistanceSq || r12 > fMaxDistanceSq)	continue;

			register float t10 = iHit1->fX2plusY2 - iHit2->fX2plusY2;
			for (iH iHit3 = iHit2 + 1; iHit3 != data.end(); iHit3++) {
				register Float_t iH3X = iHit3->fHit.fX;
				register Float_t iH3Y = iHit3->fHit.fY;

				register float rx1 = iH1X - iH3X;//rx13
				register float ry1 = iH1Y - iH3Y;//ry13
				register float r13 = rx1 * rx1 + ry1 * ry1;
				if (r13 < fMinDistanceSq || r13 > fMaxDistanceSq)continue;

				register float rx2 = iH2X - iH3X;//rx23
				register float ry2 = iH2Y - iH3Y;//ry23
				register float r23 = rx2 * rx2 + ry2 * ry2;
				if (r23	< fMinDistanceSq || r23 > fMaxDistanceSq)continue;

				register float det = rx2*ry0 - rx0*ry2;
			    if (det == 0.0f) continue;
			    register float t19 = 0.5f / det;
			    register float t5 = iHit2->fX2plusY2 - iHit3->fX2plusY2;

			    register float xc = (t5 * ry0 - t10 * ry2) * t19;
				xcs = xc - fCurMinX;
				register int intX = int( xcs *dx);
				if (intX < 0 || intX >= fNofBinsX ) continue;

				register float yc = (t10 * rx2 - t5 * rx0) * t19;
				ycs = yc - fCurMinY;
				register int intY = int( ycs *dy);
				if (intY < 0 || intY >= fNofBinsY ) continue;

				 //radius calculation
				register float t6 = iH1X - xc;
				register float t7 = iH1Y - yc;
				//if (t6 > fMaxRadius || t7 > fMaxRadius) continue;
				register float r = sqrt(t6 * t6 + t7 * t7);
				//if (r < fMinRadius) continue;
				register int intR = int(r *dr);
				if (intR < 0 || intR >= fNofBinsR) continue;

				fHist[intX * fNofBinsX + intY]++;
				fHistR[intR]++;
			}//iHit1
		}//iHit2
	}//iHit3
	//hitIndPart.clear();
}

void CbmRichRingFinderHoughImpl::FindPeak(Int_t indmin, Int_t indmax)
{
//Find MAX bin R and compare it with CUT
    Int_t maxBinR = -1, maxR = -1;
    register UInt_t size = fHistR.size();
    for (UInt_t k = 0; k < size; k++){
        if (fHistR[k] > maxBinR){
            maxBinR = fHistR[k];
            maxR = k;
        }
    }
	if (maxBinR < fHTCutR) return;

//Find MAX bin XY and compare it with CUT
    Int_t maxBinXY = -1, maxXY = -1;
    size = fHist.size();
    for (UInt_t k = 0; k < size; k++){
        if (fHist[k] > maxBinXY){
            maxBinXY = fHist[k];
            maxXY = k;
        }
    }
    if (maxBinXY < fHTCut) return;

	CbmRichRingLight* ring1 = new CbmRichRingLight();

//Find Preliminary Xc, Yc, R
    register Float_t xc, yc, r;
    register Float_t rx, ry, dr;
	xc = (maxXY/fNofBinsX + 0.5f)* fDx + fCurMinX;
	yc = (maxXY%fNofBinsX + 0.5f)* fDy + fCurMinY;
	r = (maxR + 0.5f)* fDr;
	for (Int_t j = indmin; j < indmax + 1; j++) {
		rx = fData[j].fHit.fX - xc;
		ry = fData[j].fHit.fY - yc;

		dr = fabs(sqrt(rx * rx + ry * ry) - r);
		if (dr > 0.6f) continue;
		ring1->AddHit(fData[j].fHit, fData[j].fId);
	}

	if (ring1->GetNofHits() < 7) {
	  delete ring1;
	  return;
	}

	fFitCOP->DoFit(ring1);
	Float_t drCOPCut = fRmsCoeffCOP*sqrt(ring1->GetChi2()/ring1->GetNofHits());
	if (drCOPCut > fMaxCutCOP)	drCOPCut = fMaxCutCOP;

	xc = ring1->GetCenterX();
	yc = ring1->GetCenterY();
	r = ring1->GetRadius();

	delete ring1;

	CbmRichRingLight* ring2 = new CbmRichRingLight();
	for (Int_t j = indmin; j < indmax + 1; j++) {
		rx = fData[j].fHit.fX - xc;
		ry = fData[j].fHit.fY - yc;

		dr = fabs(sqrt(rx * rx + ry * ry) - r);
		if (dr > drCOPCut) continue;
		//fData[j+indmin].fIsUsed = true;
		ring2->AddHit(fData[j].fHit, fData[j].fId);
	}

	if (ring2->GetNofHits() < 7) {
	  delete ring2;
	  return;
	}

	fFitCOP->DoFit(ring2);

	fANNSelect->DoSelect(ring2);
	Float_t select = ring2->GetSelectionNN();
	//cout << ring2->GetRadius() << " " << ring2->GetSelectionNN() << endl;
	//remove found hits only for good quality rings
	if (select > fAnnCut) {

		RemoveHitsAroundRing(indmin, indmax, ring2);
		//RemoveHitsAroundEllipse(indmin, indmax, ring2);

		//fFoundRings.push_back(ring2);
	}

	if (select > -0.7) {
		fFoundRings.push_back(ring2);
	}
	//fFoundRings.push_back(ring2);
        ring2=NULL;
	delete ring2;
}

void CbmRichRingFinderHoughImpl::RemoveHitsAroundRing(Int_t indmin, Int_t indmax,
		CbmRichRingLight* ring)
{
	Float_t rms = TMath::Sqrt(ring->GetChi2()/ring->GetNofHits());
	Float_t dCut = fRmsCoeffEl * rms;
	if (dCut > fMaxCutEl) dCut = fMaxCutEl;

	for (Int_t j = indmin; j < indmax + 1; j++) {
		Float_t rx = fData[j].fHit.fX - ring->GetCenterX();
		Float_t ry = fData[j].fHit.fY - ring->GetCenterY();

		Float_t dr = fabs(sqrt(rx * rx + ry * ry) - ring->GetRadius());
		if (dr < dCut) {
			fData[j].fIsUsed = true;
		}
	}
}

void CbmRichRingFinderHoughImpl::RingSelection()
{
	Int_t nofRings = fFoundRings.size();
	std::sort(fFoundRings.begin(), fFoundRings.end(), CbmRichRingComparatorMore());
//	std::vector<std::set<UShort_t> > usedHits;
//	usedHits.reserve(nofRings);
	std::set<UShort_t> usedHitsAll;
	std::vector<UShort_t> goodRingIndex;
	goodRingIndex.reserve(nofRings);
	CbmRichRingLight* ring2;

	for (Int_t iRing = 0; iRing < nofRings; iRing++){
		CbmRichRingLight* ring = fFoundRings[iRing];
		ring->SetRecFlag(-1);
		Int_t nofHits = ring->GetNofHits();
//		Bool_t isGoodRing = true;
//		for (Int_t iRSet = 0; iRSet < usedHits.size(); iRSet++){
//			ring2 = fFoundRings[goodRingIndex[iRSet]];
//			if (AreRingsCloseEnough(ring2, ring)) continue;
//			Int_t nofUsedHits = 0;
//			for(Int_t iHit = 0; iHit < nofHits; iHit++){
//				std::set<UShort_t>::iterator it = usedHits[iRSet].find(ring->GetHit(iHit));
//				if(it != usedHits[iRSet].end()){
//					nofUsedHits++;
//				}
//			}
//			if ((Float_t)nofUsedHits/(Float_t)nofHits > fUsedHitsCut){
//				isGoodRing = false;
//				break;
//			}
//		}// iRSet

		Bool_t isGoodRingAll = true;
		Int_t nofUsedHitsAll = 0;
		for(Int_t iHit = 0; iHit < nofHits; iHit++){
			std::set<UShort_t>::iterator it = usedHitsAll.find(ring->GetHitId(iHit));
			if(it != usedHitsAll.end()){
				nofUsedHitsAll++;
			}
		}
		if ((Float_t)nofUsedHitsAll/(Float_t)nofHits > fUsedHitsAllCut){
			isGoodRingAll = false;
		}

		if (/*isGoodRing && */ isGoodRingAll){
			//fFitEllipse->DoFit(fFoundRings[iRing]);
			fFoundRings[iRing]->SetRecFlag(1);
			for (Int_t iRSet = 0; iRSet < goodRingIndex.size(); iRSet++){
				//ring2 = fFoundRings[goodRingIndex[iRSet]];
				//if (AreRingsCloseEnough(ring2, ring)) continue;

				ReAssingSharedHits(goodRingIndex[iRSet]	,iRing);
			}
			goodRingIndex.push_back(iRing);
			//std::set<UShort_t> usedHitsT;
			for(Int_t iHit = 0; iHit < nofHits; iHit++){
				//usedHitsT.insert(ring->GetHit(iHit));
				usedHitsAll.insert(ring->GetHitId(iHit));
			}
//			usedHits.push_back(usedHitsT);
//			usedHitsT.clear();
		}// isGoodRing
	}// iRing

//	usedHits.clear();
	usedHitsAll.clear();
	goodRingIndex.clear();

}

void CbmRichRingFinderHoughImpl::ReAssingSharedHits(Int_t ringInd1, Int_t ringInd2)
{
	CbmRichRingLight* ring1 = fFoundRings[ringInd1];
	CbmRichRingLight* ring2 = fFoundRings[ringInd2];
	Int_t nofHits1 = ring1->GetNofHits();
	Int_t nofHits2 = ring2->GetNofHits();

	for(Int_t iHit1 = 0; iHit1 < nofHits1; iHit1++){
		unsigned short hitInd1 = ring1->GetHitId(iHit1);
		for(Int_t iHit2 = 0; iHit2 < nofHits2; iHit2++){
			unsigned short hitInd2 = ring2->GetHitId(iHit2);
			if(hitInd1 != hitInd2) continue;
			Int_t hitIndData =  GetHitIndex(hitInd1);
			Float_t hitX = fData[hitIndData].fHit.fX;
			Float_t hitY = fData[hitIndData].fHit.fY;
			Float_t rx1 = hitX - ring1->GetCenterX();
			Float_t ry1 = hitY - ring1->GetCenterY();
			Float_t dr1 = fabs(sqrt(rx1 * rx1 + ry1 * ry1) - ring1->GetRadius());

			Float_t rx2 = hitX - ring2->GetCenterX();
			Float_t ry2 = hitY - ring2->GetCenterY();
			Float_t dr2 = fabs(sqrt(rx2 * rx2 + ry2 * ry2) - ring2->GetRadius());

			if (dr1 > dr2){
				ring1->RemoveHit(hitInd1);
			} else {
				ring2->RemoveHit(hitInd2);
			}

		}//iHit2
	}//iHit1
}

void CbmRichRingFinderHoughImpl::ReAssingSharedHitsEllipse(Int_t ringInd1, Int_t ringInd2)
{
//	CbmRichRing* ring1 = fFoundRings[ringInd1];
//	CbmRichRing* ring2 = fFoundRings[ringInd2];
//	Int_t nofHits1 = ring1->GetNofHits();
//	Int_t nofHits2 = ring2->GetNofHits();
//
//	for(Int_t iHit1 = 0; iHit1 < nofHits1; iHit1++){
//		UShort_t hitInd1 = ring1->GetHit(iHit1);
//		for(Int_t iHit2 = 0; iHit2 < nofHits2; iHit2++){
//			UShort_t hitInd2 = ring2->GetHit(iHit2);
//			if(hitInd1 == hitInd2){
//				Int_t hitIndData =  GetHitIndex(hitInd1);
//				Float_t x = fData[hitIndData].fX;
//				Float_t y = fData[hitIndData].fY;
//
//				Double_t A = ring1->GetAPar();
//				Double_t B = ring1->GetBPar();
//				Double_t C = ring1->GetCPar();
//				Double_t D = ring1->GetDPar();
//				Double_t E = ring1->GetEPar();
//				Double_t F = ring1->GetFPar();
//		        Double_t d1 = TMath::Abs(A*x*x + B*x*y + C*y*y + D*x + E*y + F);
//			    Double_t d2 = sqrt( pow(2*A*x + B*y + D, 2) + pow(B*x + 2*C*y + E, 2) );
//			    Double_t dr1 = d1/d2;
//
//				A = ring2->GetAPar();
//				B = ring2->GetBPar();
//				C = ring2->GetCPar();
//				D = ring2->GetDPar();
//				E = ring2->GetEPar();
//				F = ring2->GetFPar();
//		        d1 = TMath::Abs(A*x*x + B*x*y + C*y*y + D*x + E*y + F);
//			    d2 = sqrt( pow(2*A*x + B*y + D, 2) + pow(B*x + 2*C*y + E, 2) );
//			    Double_t dr2 = d1/d2;
//
//				if (dr1 > dr2){
//					ring1->RemoveHit(hitInd1);
//				} else {
//					ring2->RemoveHit(hitInd2);
//				}
//			}
//		}
//	}
}

Int_t CbmRichRingFinderHoughImpl::GetHitIndex(UShort_t hitInd)
{
	UInt_t size = fData.size();
	for (UInt_t i = 0; i < size; i++){
		if (fData[i].fId == hitInd) return i;
	}

}

Bool_t CbmRichRingFinderHoughImpl::AreRingsCloseEnough(CbmRichRingLight* ring1,
		CbmRichRingLight* ring2)
{
	float rx = ring1->GetCenterX() - ring2->GetCenterX();
	float ry = ring1->GetCenterY() - ring2->GetCenterY();

	float r = sqrt(rx*rx + ry*ry);
	if (r > 1.3f * (ring1->GetRadius() + ring2->GetRadius())) return false;
	return true;

}

void CbmRichRingFinderHoughImpl::InitHist()
{
	fHist[0]=0;
	fHist[1]=0;
	fHist[2]=0;
	fHist[3]=0;
	fHist[4]=0;
	fHist[5]=0;
	fHist[6]=0;
	fHist[7]=0;
	fHist[8]=0;
	fHist[9]=0;
	fHist[10]=0;
	fHist[11]=0;
	fHist[12]=0;
	fHist[13]=0;
	fHist[14]=0;
	fHist[15]=0;
	fHist[16]=0;
	fHist[17]=0;
	fHist[18]=0;
	fHist[19]=0;
	fHist[20]=0;
	fHist[21]=0;
	fHist[22]=0;
	fHist[23]=0;
	fHist[24]=0;
	fHist[25]=0;
	fHist[26]=0;
	fHist[27]=0;
	fHist[28]=0;
	fHist[29]=0;
	fHist[30]=0;
	fHist[31]=0;
	fHist[32]=0;
	fHist[33]=0;
	fHist[34]=0;
	fHist[35]=0;
	fHist[36]=0;
	fHist[37]=0;
	fHist[38]=0;
	fHist[39]=0;
	fHist[40]=0;
	fHist[41]=0;
	fHist[42]=0;
	fHist[43]=0;
	fHist[44]=0;
	fHist[45]=0;
	fHist[46]=0;
	fHist[47]=0;
	fHist[48]=0;
	fHist[49]=0;
	fHist[50]=0;
	fHist[51]=0;
	fHist[52]=0;
	fHist[53]=0;
	fHist[54]=0;
	fHist[55]=0;
	fHist[56]=0;
	fHist[57]=0;
	fHist[58]=0;
	fHist[59]=0;
	fHist[60]=0;
	fHist[61]=0;
	fHist[62]=0;
	fHist[63]=0;
	fHist[64]=0;
	fHist[65]=0;
	fHist[66]=0;
	fHist[67]=0;
	fHist[68]=0;
	fHist[69]=0;
	fHist[70]=0;
	fHist[71]=0;
	fHist[72]=0;
	fHist[73]=0;
	fHist[74]=0;
	fHist[75]=0;
	fHist[76]=0;
	fHist[77]=0;
	fHist[78]=0;
	fHist[79]=0;
	fHist[80]=0;
	fHist[81]=0;
	fHist[82]=0;
	fHist[83]=0;
	fHist[84]=0;
	fHist[85]=0;
	fHist[86]=0;
	fHist[87]=0;
	fHist[88]=0;
	fHist[89]=0;
	fHist[90]=0;
	fHist[91]=0;
	fHist[92]=0;
	fHist[93]=0;
	fHist[94]=0;
	fHist[95]=0;
	fHist[96]=0;
	fHist[97]=0;
	fHist[98]=0;
	fHist[99]=0;
	fHist[100]=0;
	fHist[101]=0;
	fHist[102]=0;
	fHist[103]=0;
	fHist[104]=0;
	fHist[105]=0;
	fHist[106]=0;
	fHist[107]=0;
	fHist[108]=0;
	fHist[109]=0;
	fHist[110]=0;
	fHist[111]=0;
	fHist[112]=0;
	fHist[113]=0;
	fHist[114]=0;
	fHist[115]=0;
	fHist[116]=0;
	fHist[117]=0;
	fHist[118]=0;
	fHist[119]=0;
	fHist[120]=0;
	fHist[121]=0;
	fHist[122]=0;
	fHist[123]=0;
	fHist[124]=0;
	fHist[125]=0;
	fHist[126]=0;
	fHist[127]=0;
	fHist[128]=0;
	fHist[129]=0;
	fHist[130]=0;
	fHist[131]=0;
	fHist[132]=0;
	fHist[133]=0;
	fHist[134]=0;
	fHist[135]=0;
	fHist[136]=0;
	fHist[137]=0;
	fHist[138]=0;
	fHist[139]=0;
	fHist[140]=0;
	fHist[141]=0;
	fHist[142]=0;
	fHist[143]=0;
	fHist[144]=0;
	fHist[145]=0;
	fHist[146]=0;
	fHist[147]=0;
	fHist[148]=0;
	fHist[149]=0;
	fHist[150]=0;
	fHist[151]=0;
	fHist[152]=0;
	fHist[153]=0;
	fHist[154]=0;
	fHist[155]=0;
	fHist[156]=0;
	fHist[157]=0;
	fHist[158]=0;
	fHist[159]=0;
	fHist[160]=0;
	fHist[161]=0;
	fHist[162]=0;
	fHist[163]=0;
	fHist[164]=0;
	fHist[165]=0;
	fHist[166]=0;
	fHist[167]=0;
	fHist[168]=0;
	fHist[169]=0;
	fHist[170]=0;
	fHist[171]=0;
	fHist[172]=0;
	fHist[173]=0;
	fHist[174]=0;
	fHist[175]=0;
	fHist[176]=0;
	fHist[177]=0;
	fHist[178]=0;
	fHist[179]=0;
	fHist[180]=0;
	fHist[181]=0;
	fHist[182]=0;
	fHist[183]=0;
	fHist[184]=0;
	fHist[185]=0;
	fHist[186]=0;
	fHist[187]=0;
	fHist[188]=0;
	fHist[189]=0;
	fHist[190]=0;
	fHist[191]=0;
	fHist[192]=0;
	fHist[193]=0;
	fHist[194]=0;
	fHist[195]=0;
	fHist[196]=0;
	fHist[197]=0;
	fHist[198]=0;
	fHist[199]=0;
	fHist[200]=0;
	fHist[201]=0;
	fHist[202]=0;
	fHist[203]=0;
	fHist[204]=0;
	fHist[205]=0;
	fHist[206]=0;
	fHist[207]=0;
	fHist[208]=0;
	fHist[209]=0;
	fHist[210]=0;
	fHist[211]=0;
	fHist[212]=0;
	fHist[213]=0;
	fHist[214]=0;
	fHist[215]=0;
	fHist[216]=0;
	fHist[217]=0;
	fHist[218]=0;
	fHist[219]=0;
	fHist[220]=0;
	fHist[221]=0;
	fHist[222]=0;
	fHist[223]=0;
	fHist[224]=0;
	fHist[225]=0;
	fHist[226]=0;
	fHist[227]=0;
	fHist[228]=0;
	fHist[229]=0;
	fHist[230]=0;
	fHist[231]=0;
	fHist[232]=0;
	fHist[233]=0;
	fHist[234]=0;
	fHist[235]=0;
	fHist[236]=0;
	fHist[237]=0;
	fHist[238]=0;
	fHist[239]=0;
	fHist[240]=0;
	fHist[241]=0;
	fHist[242]=0;
	fHist[243]=0;
	fHist[244]=0;
	fHist[245]=0;
	fHist[246]=0;
	fHist[247]=0;
	fHist[248]=0;
	fHist[249]=0;
	fHist[250]=0;
	fHist[251]=0;
	fHist[252]=0;
	fHist[253]=0;
	fHist[254]=0;
	fHist[255]=0;
	fHist[256]=0;
	fHist[257]=0;
	fHist[258]=0;
	fHist[259]=0;
	fHist[260]=0;
	fHist[261]=0;
	fHist[262]=0;
	fHist[263]=0;
	fHist[264]=0;
	fHist[265]=0;
	fHist[266]=0;
	fHist[267]=0;
	fHist[268]=0;
	fHist[269]=0;
	fHist[270]=0;
	fHist[271]=0;
	fHist[272]=0;
	fHist[273]=0;
	fHist[274]=0;
	fHist[275]=0;
	fHist[276]=0;
	fHist[277]=0;
	fHist[278]=0;
	fHist[279]=0;
	fHist[280]=0;
	fHist[281]=0;
	fHist[282]=0;
	fHist[283]=0;
	fHist[284]=0;
	fHist[285]=0;
	fHist[286]=0;
	fHist[287]=0;
	fHist[288]=0;
	fHist[289]=0;
	fHist[290]=0;
	fHist[291]=0;
	fHist[292]=0;
	fHist[293]=0;
	fHist[294]=0;
	fHist[295]=0;
	fHist[296]=0;
	fHist[297]=0;
	fHist[298]=0;
	fHist[299]=0;
	fHist[300]=0;
	fHist[301]=0;
	fHist[302]=0;
	fHist[303]=0;
	fHist[304]=0;
	fHist[305]=0;
	fHist[306]=0;
	fHist[307]=0;
	fHist[308]=0;
	fHist[309]=0;
	fHist[310]=0;
	fHist[311]=0;
	fHist[312]=0;
	fHist[313]=0;
	fHist[314]=0;
	fHist[315]=0;
	fHist[316]=0;
	fHist[317]=0;
	fHist[318]=0;
	fHist[319]=0;
	fHist[320]=0;
	fHist[321]=0;
	fHist[322]=0;
	fHist[323]=0;
	fHist[324]=0;
	fHist[325]=0;
	fHist[326]=0;
	fHist[327]=0;
	fHist[328]=0;
	fHist[329]=0;
	fHist[330]=0;
	fHist[331]=0;
	fHist[332]=0;
	fHist[333]=0;
	fHist[334]=0;
	fHist[335]=0;
	fHist[336]=0;
	fHist[337]=0;
	fHist[338]=0;
	fHist[339]=0;
	fHist[340]=0;
	fHist[341]=0;
	fHist[342]=0;
	fHist[343]=0;
	fHist[344]=0;
	fHist[345]=0;
	fHist[346]=0;
	fHist[347]=0;
	fHist[348]=0;
	fHist[349]=0;
	fHist[350]=0;
	fHist[351]=0;
	fHist[352]=0;
	fHist[353]=0;
	fHist[354]=0;
	fHist[355]=0;
	fHist[356]=0;
	fHist[357]=0;
	fHist[358]=0;
	fHist[359]=0;
	fHist[360]=0;
	fHist[361]=0;
	fHist[362]=0;
	fHist[363]=0;
	fHist[364]=0;
	fHist[365]=0;
	fHist[366]=0;
	fHist[367]=0;
	fHist[368]=0;
	fHist[369]=0;
	fHist[370]=0;
	fHist[371]=0;
	fHist[372]=0;
	fHist[373]=0;
	fHist[374]=0;
	fHist[375]=0;
	fHist[376]=0;
	fHist[377]=0;
	fHist[378]=0;
	fHist[379]=0;
	fHist[380]=0;
	fHist[381]=0;
	fHist[382]=0;
	fHist[383]=0;
	fHist[384]=0;
	fHist[385]=0;
	fHist[386]=0;
	fHist[387]=0;
	fHist[388]=0;
	fHist[389]=0;
	fHist[390]=0;
	fHist[391]=0;
	fHist[392]=0;
	fHist[393]=0;
	fHist[394]=0;
	fHist[395]=0;
	fHist[396]=0;
	fHist[397]=0;
	fHist[398]=0;
	fHist[399]=0;
	fHist[400]=0;
	fHist[401]=0;
	fHist[402]=0;
	fHist[403]=0;
	fHist[404]=0;
	fHist[405]=0;
	fHist[406]=0;
	fHist[407]=0;
	fHist[408]=0;
	fHist[409]=0;
	fHist[410]=0;
	fHist[411]=0;
	fHist[412]=0;
	fHist[413]=0;
	fHist[414]=0;
	fHist[415]=0;
	fHist[416]=0;
	fHist[417]=0;
	fHist[418]=0;
	fHist[419]=0;
	fHist[420]=0;
	fHist[421]=0;
	fHist[422]=0;
	fHist[423]=0;
	fHist[424]=0;

	fHistR[0]=0;
	fHistR[1]=0;
	fHistR[2]=0;
	fHistR[3]=0;
	fHistR[4]=0;
	fHistR[5]=0;
	fHistR[6]=0;
	fHistR[7]=0;
	fHistR[8]=0;
	fHistR[9]=0;
	fHistR[10]=0;
	fHistR[11]=0;
	fHistR[12]=0;
	fHistR[13]=0;
	fHistR[14]=0;
	fHistR[15]=0;
	fHistR[16]=0;
	fHistR[17]=0;
	fHistR[18]=0;
	fHistR[19]=0;
	fHistR[20]=0;
	fHistR[21]=0;
	fHistR[22]=0;
	fHistR[23]=0;
	fHistR[24]=0;
	fHistR[25]=0;
	fHistR[26]=0;
	fHistR[27]=0;
	fHistR[28]=0;
	fHistR[29]=0;
	fHistR[30]=0;
	fHistR[31]=0;

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

	for(Int_t j = indmin; j < indmax + 1; j++){
		Double_t x = fData[j].fHit.fX;
		Double_t y = fData[j].fHit.fY;

        Double_t d1 = TMath::Abs(A*x*x + B*x*y + C*y*y + D*x + E*y + F);
        Double_t d2 = sqrt( pow(2*A*x + B*y + D, 2) + pow(B*x + 2*C*y + E, 2) );

        Double_t d = d1/d2;
        if (d < dCut) fData[j].fIsUsed = true;
	}
}
