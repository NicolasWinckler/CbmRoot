
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
//#include "../../littrack/utils/CbmLitMemoryManagment.h"
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

CbmRichRingFinderHoughImpl::CbmRichRingFinderHoughImpl()
{
}

void CbmRichRingFinderHoughImpl::Init()
{
    SetParameters();

    fHist.resize(fNofBinsXY);
    fHistR.resize(fNofBinsR);
	fHitInd.resize(fNofParts);

    fFitCOP = new CbmRichRingFitterCOPLight();

    TString richSelectNNFile = gSystem->Getenv("VMCWORKDIR");
    richSelectNNFile += "/parameters/rich/NeuralNet_RingSelection_Weights_Compact.txt";
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

void CbmRichRingFinderHoughImpl::SetParameters()
{
	fMaxDistance = 11.5;
	fMinDistance = 3.;
	fMinDistanceSq = fMinDistance*fMinDistance;
	fMaxDistanceSq = fMaxDistance*fMaxDistance;

	fMinRadius = 3.3;
	fMaxRadius = 5.7;

//  if (fNofParts == 1){
//		fHTCut = 50;
//		fHitCut = 5;
//		fHTCutR = 20;
//		fHitCutR = 3;
//		fMinNofHitsInArea = 6;
//  }else if (fNofParts == 2){
	fHTCut = 5;
	fHitCut = 2;
	fHTCutR = 5;
	fHitCutR = 2;
	fMinNofHitsInArea = 4;
//  }

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

	for (Int_t j = 0; j < fNofBinsXY; j++){
		fHist[j] = 0;
	}

	for (Int_t k = 0; k < fNofBinsR; k++) {
		fHistR[k] = 0;
	}
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
