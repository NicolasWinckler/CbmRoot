// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderTrack source file                         -----
// ----- implementation: Simeon Lebedev (salebedev@jinr.ru)  -----
// -------------------------------------------------------------------------

#include "CbmRichRingFinderTrack.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"

#include "CbmTrackParam.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

//--------------------------------------------------------------------------
// -----   Default constructor   -------------------------------------------
CbmRichRingFinderTrack::CbmRichRingFinderTrack()
{
	fVerbose = 1;
}

// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderTrack::CbmRichRingFinderTrack ( Int_t verbose )
{
	fVerbose = verbose;
}

// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderTrack::~CbmRichRingFinderTrack()
{}

void CbmRichRingFinderTrack::Init ()
{
	cout << "CbmRichRingFinderTrack::init() "<<endl;
	InitArray();
}


Int_t CbmRichRingFinderTrack::DoFind(TClonesArray* rHitArray,
				     TClonesArray* rProjArray,
		       		     TClonesArray* rRingArray) {
	fData.clear();
	fGuidance.clear();
	fFoundRings.clear();

	if ( !rHitArray) {
    		cout << "-E- CbmRichRingFinderTrack::DoFind: Hit array missing! " << rHitArray << endl;
		return -1;
	}
	const Int_t nhits = rHitArray->GetEntriesFast();
	if(!nhits) {
		cout << "No hits in this event." << endl;
		return -1;
	}

	for(Int_t iHit = 0; iHit < nhits; iHit++) {
		CbmRichHit * hit = (CbmRichHit*)rHitArray->At(iHit);
		if(hit){
			CbmRichTrackMyPoint tempPoint;
			tempPoint.fX = hit->X();
			tempPoint.fY = hit->Y();
			tempPoint.fId = iHit;
			tempPoint.fRefIndex = hit->GetRefIndex();
			fData.push_back(tempPoint);
		}
	}
	cout << "fData.size() = "<< fData.size()<<endl;
	sort(fData.begin(), fData.end(),CbmRichTrackMyPoint::CmpUp);
	
	//for (int i = 0; i< fData.size(); i++) cout << fData[i].fX <<" " ;
	
	const Int_t npoints = rProjArray->GetEntriesFast();
	if(!npoints) {
		cout << "No projections in this event." << endl;
		return -1;
	}
	for(Int_t iPoint = 0; iPoint < npoints; iPoint++) {
		CbmTrackParam * point = (CbmTrackParam*)rProjArray->At(iPoint);
		if(point) {
			CbmRichTrackMyPoint tempPoint;
			tempPoint.fX = point->GetX();
			tempPoint.fY = point->GetY();
			tempPoint.fId = iPoint;
			fGuidance.push_back(tempPoint);
			//cout<<"X="<<fGuidance[iPoint].fX <<" Y="<< fGuidance[iPoint].fY<<"    ";
		}
	}
	cout <<"fGuidance.size() = "<< fGuidance.size()<< endl;

	int foundRingsCount = 0;
	
	FindCenters(fNBins);
	//return -1;
 	for(unsigned int iRing=0; iRing < fFoundRings.size(); iRing++) {
		//cout<<"111111"<<endl;
		//new ((*rRingArray)[foundRingsCount]) CbmRichRing(fFoundRings[iRing]);
		
		
		//cout<<"222222"<<endl;
		new ((*rRingArray)[foundRingsCount]) CbmRichRing();
		CbmRichRing * r = (CbmRichRing *)rRingArray->At(foundRingsCount);
		double xc = fFoundRings[iRing].GetCenterX();
		double yc = fFoundRings[iRing].GetCenterY();
		double radius = fFoundRings[iRing].GetRadius();
		//cout << "(X,Y,R, NHits) = "<<xc << " "
		//     << yc<< " "<<radius << " "<< 
		//     fFoundRings[iRing].GetNofHits() << endl;;
		r->SetCenterX(xc);
		r->SetCenterY(yc);
		r->SetRadius(radius);
		for (int ith = 0; ith < fFoundRings[iRing].GetNofHits();ith ++){
			r->AddHit(fFoundRings[iRing].GetHit(ith));
		}
		foundRingsCount++;
		
  	}
	cout<<"found Rings = "<<foundRingsCount<<endl;

        return foundRingsCount;
}

void CbmRichRingFinderTrack::SetParameters(int NBins, double MinRadius,double MaxRadius, int Cut)
{
	cout <<"SetNewParameters begin"<< endl;
	fNBins = NBins;
	fMinRadius = MinRadius;
	fMaxRadius = MaxRadius;
	fCut = Cut;
	/*fNBins = 20;
	fMinRadius = 2;
	fMaxRadius = 6;
	fCut = 12;*/

	cout <<"SetNewParameters end"<< endl;
}
void  CbmRichRingFinderTrack::InitArray()
{
	cout<<"InitArray begin" << endl;

	fHist.resize(fNBins);

	cout<<"InitArray end" << endl;
}

void CbmRichRingFinderTrack::InitHist1D(double nbins, int cpinc)
{
	Int_t i;
	Double_t ir;
	Double_t dx = (fMaxRadius) / nbins;
	for (i = 0; i < nbins; i++) fHist[i].clear();

	CbmRichTrackMyPoint mpnt;
   std::vector<CbmRichTrackMyPoint>::iterator it1;
   std::vector<CbmRichTrackMyPoint>::iterator it2;
   std::vector<CbmRichTrackMyPoint>::iterator it3;
   std::vector<CbmRichTrackMyPoint>::iterator it4;

   //find all hits which are in the corridor
   mpnt.fX = fGuidance[cpinc].fX - fMaxRadius;
   it1 = std::lower_bound(fData.begin(),
	fData.end(), mpnt, CbmRichTrackMyPoint::CmpUp);

	mpnt.fX = fGuidance[cpinc].fX - fMinRadius;
   it2 = std::lower_bound(fData.begin(),
	fData.end(), mpnt, CbmRichTrackMyPoint::CmpUp);

	mpnt.fX = fGuidance[cpinc].fX + fMinRadius;
   it3 = std::lower_bound(fData.begin(),
	fData.end(), mpnt, CbmRichTrackMyPoint::CmpUp)- 1 ;

	mpnt.fX = fGuidance[cpinc].fX + fMaxRadius;
   it4 = std::lower_bound(fData.begin(),
	fData.end(), mpnt, CbmRichTrackMyPoint::CmpUp)- 1 ;

   //first index of the hit in the corridor
   Int_t ind1 = it1 - fData.begin();
   Int_t ind2 = it2 - fData.begin();
   Int_t ind3 = it3 - fData.begin();
   Int_t ind4 = it4 - fData.begin();
	//cout << ind1 <<" "<<ind2 <<" "<<ind3<<" "<< ind4;
        //count number of points in the corridor
   Int_t hit_cnt = ind4 - ind1 + 1 ;//+ ind4 - ind3 + 1;
	//cout<< " hitCnt ="<<hit_cnt;
	if (hit_cnt >= fCut){
	Int_t hitCntY = 0;
		for (i = ind1; i < ind2 + 1; i++){			
			if (fData[i].fY < fGuidance[cpinc].fY + fMaxRadius && fData[i].fY > fGuidance[cpinc].fY - fMaxRadius){
				hitCntY++;
				Double_t r = pow((fData[i].fX - fGuidance[cpinc].fX)*(fData[i].fX - fGuidance[cpinc].fX) +
					(fData[i].fY - fGuidance[cpinc].fY)*(fData[i].fY - fGuidance[cpinc].fY), 0.5);
				//cout << "r=" << r << " ";
				if (r > fMinRadius && r < fMaxRadius) {
					modf(r / dx, &ir);
					if (ir < nbins && ir > 0 ) fHist[ (int)ir ].push_back(i);
				}
			}
		}
		//cout << " hitCntY = "<< hitCntY<< " ";
		for (i = ind2+1; i < ind3 + 1; i++){
			if ( fData[i].fY < fGuidance[cpinc].fY + fMaxRadius  && fData[i].fY > fGuidance[cpinc].fY - fMaxRadius
			 && !(fData[i].fY < fGuidance[cpinc].fY + fMinRadius/1.4  && fData[i].fY > fGuidance[cpinc].fY - fMinRadius/1.4) ){
				Double_t r = pow((fData[i].fX - fGuidance[cpinc].fX)*(fData[i].fX - fGuidance[cpinc].fX) +
					(fData[i].fY - fGuidance[cpinc].fY)*(fData[i].fY - fGuidance[cpinc].fY), 0.5);
				if (r > fMinRadius && r < fMaxRadius) {
					modf(r / dx, &ir);
					if (ir < nbins && ir > 0 ) fHist[ (int)ir ].push_back(i);
				}
			}
		}

		for (i = ind3+1; i < ind4 + 1; i++){
			if (fData[i].fY < fGuidance[cpinc].fY + fMaxRadius && fData[i].fY > fGuidance[cpinc].fY - fMaxRadius){
				Double_t r = pow((fData[i].fX - fGuidance[cpinc].fX)*(fData[i].fX - fGuidance[cpinc].fX) +
					(fData[i].fY - fGuidance[cpinc].fY)*(fData[i].fY - fGuidance[cpinc].fY), 0.5);
				if (r > fMinRadius && r < fMaxRadius) {
					modf(r / dx, &ir);
					if (ir < nbins && ir > 0 ) fHist[ (int)ir ].push_back(i);
				}
			}
		}
	}
}

void CbmRichRingFinderTrack::FindCenters(Double_t nbins)
{
	Int_t i;
	UInt_t j;
	UInt_t max = 0;
	Int_t ii = 0;
	Double_t dx = (fMaxRadius) / nbins;

	for (j = 0; j < fGuidance.size(); j++){
		max = 0;
		InitHist1D(nbins,j);
		for (i = 0; i < nbins; i++)
			if (fHist[i].size() > max) {
				max = fHist[i].size();
				ii = i;
			}
		Int_t maxsum = (Int_t)fHist[ii].size();
		if (ii+1 < nbins) maxsum += fHist[ii+1].size();
		if (ii-1 > 0) maxsum += fHist[ii-1].size();

		if (maxsum > fCut){
			CbmRichRing tempRing;
			tempRing.SetCenterX(fGuidance[j].fX);
			tempRing.SetCenterY(fGuidance[j].fY);
			double tr = dx * (fHist[ii].size() * ii +
                           fHist[ii-1].size() * (ii-1) +
                           fHist[ii+1].size() * (ii+1)) / maxsum;
			if (ii+1 < nbins ) tempRing.SetRadius(tr);
			else  tempRing.SetRadius( dx * ii );// don't crash the programm

			for (unsigned int ih = 0; ih < fHist[ii].size(); ih++){
				tempRing.AddHit(fData[fHist[ii][ih]].fId);
			}

			if (ii+1 < nbins)
				for (unsigned int ih=0; ih < fHist[ii + 1].size(); ih++){
				tempRing.AddHit(fData[fHist[ii+1][ih]].fId);
			}

			if (ii-1 > 0)
				for (unsigned int ih=0; ih < fHist[ii - 1].size(); ih++){
				tempRing.AddHit(fData[fHist[ii-1][ih]].fId);				
			}
			fFoundRings.push_back(tempRing);
		}

	}
}



// -------------------------------------------------------------------------
ClassImp(CbmRichRingFinderTrack)
