// -------------------------------------------------------------------------
// -----                  CbmLitTrdTrackFinderS source file            -----
// -----                  Created 06/07/06  by A. Lebedev              -----
// -------------------------------------------------------------------------
 
 
#include "CbmLitTrdTrackFinderS.h"

#include "CbmLitTrack.h"
#include "CbmLitPixelHit.h"
#include "CbmLitLineTrackExtrapolator.h"
#include "CbmLitKalmanFilter.h"
#include "CbmLitTrackFitterImp.h"
#include "CbmLitTrackPropagatorImp.h"
#include "CbmLitEnvironment.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitTrackSelectionTrd.h"
#include "CbmLitTrackSelectionEmpty.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitToolFactory.h"

#include "CbmTrkHit.h"
#include "CbmTrdTrack.h"

#include "TClonesArray.h"

CbmLitTrdTrackFinderS::CbmLitTrdTrackFinderS()
{
}

CbmLitTrdTrackFinderS::~CbmLitTrdTrackFinderS()
{
}


void CbmLitTrdTrackFinderS::Init()
{
   fPropagator = new CbmLitTrackPropagatorImp(new CbmLitLineTrackExtrapolator());
   fPropagator->Initialize();
	   
   fFilter = new CbmLitKalmanFilter();
   fFilter->Initialize();
	   
   fFitter = new CbmLitTrackFitterImp(fPropagator, fFilter);
   fFitter->Initialize();
   fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();
   
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fSeedSelection = factory->CreateTrackSelection("Momentum");
   
   //fTrackSelectionStation = new CbmLitTrackSelectionEmpty();    
   fStationGroupSelection = new CbmLitTrackSelectionTrd();
   fStationGroupSelection->Initialize();
	      
   fFinalSelection = new CbmLitTrackSelectionTrd();
   fFinalSelection->Initialize();
   
   fFinalPreSelection = factory->CreateTrackSelection("Empty");
	   
   fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();
   fVerbose = 3;
   fNofIter = 1; 
   fBeginStationGroup = 0; 
   fEndStationGroup = fLayout.GetNofStationGroups() - 1;
   fPDG = 211;
}

void CbmLitTrdTrackFinderS::SetIterationParameters(Int_t iter)
{
	if (iter == 0) {
		fMaxNofMissingHitsInStationGroup = 1;
		fMaxNofMissingHits = 1;
		fSigmaCoef = 3.5; 
	} else 
	if (iter == 1) {
		fMaxNofMissingHitsInStationGroup = 1;
		fMaxNofMissingHits = 2;
		fSigmaCoef = 5.; 
	}	
}


Int_t CbmLitTrdTrackFinderS::DoFind(
		TClonesArray* hitArray,
        TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;
	
	CreateHits(hitArray, hits);
	CreateTrackSeeds(hits, trackSeeds);
	
	CbmLitTrackFinderBranch::DoFind(hits, trackSeeds, foundTracks);
	
	CopyToOutput(foundTracks, trackArray);
	
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	hits.clear();
	trackSeeds.clear();
	
	return trackArray->GetEntriesFast();
}

void CbmLitTrdTrackFinderS::CreateTrackSeeds(
		const HitPtrVector& hits,
		TrackPtrVector& trackSeeds)
{

}

void CbmLitTrdTrackFinderS::CreateHits(
		TClonesArray* hitArray,
		HitPtrVector& hits)
{
	
	Int_t nofHits = hitArray->GetEntriesFast();

	for(Int_t iHit = 0; iHit < nofHits; iHit++) {
		
		CbmTrkHit* hit = (CbmTrkHit*) hitArray->At(iHit);
	    if(NULL == hit) continue;
	      
	    CbmLitPixelHit* litHit = new CbmLitPixelHit;
	    CbmLitConverter::TrkHitToLitPixelHit(hit, iHit, litHit);
	    hits.push_back(litHit);
	}	   
}

void CbmLitTrdTrackFinderS::CopyToOutput(
		TrackPtrVector& tracks,
		TClonesArray* trackArray)
{
	Int_t trackNo = trackArray->GetEntriesFast();
	for(TrackPtrIterator iTrack = tracks.begin();iTrack != tracks.end(); iTrack++) {
	   CbmTrdTrack track;
	   CbmLitConverter::LitTrackToTrdTrack(*iTrack, &track);
       new ((*trackArray)[trackNo++]) CbmTrdTrack(track);
       delete (*iTrack);
	}
	tracks.clear();
}


/*
void CbmLitTrdTrackFinderS::SetIterPar(Double_t SigmaCoef, 
                             Double_t Mom, 
                             Int_t Flag, 
                             Int_t beginStation, 
                             Int_t endStation,
                             Int_t maxNofMissingHitsInStation)
{

   fSigmaCoef = SigmaCoef;
   fMom = Mom;
   
   if (Flag == 1) {
      fSigmaX[0] = 0.;
      fSigmaX[1] = 1.95;
      fSigmaX[2] = 1.928;
      fSigmaX[3] = 0.3682;
   
      fSigmaY[0] = 0.;
      fSigmaY[1] = 1.461;
      fSigmaY[2] = 0.1332;
      fSigmaY[3] = 1.454; 
   }
   
   if (Flag == 2) {
     // fSigmaCoef = 5; 
     // fMom = 0.5; 
      //for Sec
      fSigmaX[0] = 0.;
      fSigmaX[1] = 2.116;
      fSigmaX[2] = 2.061;
      fSigmaX[3] = 1.178;
   
      fSigmaY[0] = 0.;
      fSigmaY[1] = 1.734;
      fSigmaY[2] = 1.134;
      fSigmaY[3] = 2.155;
   }
}
// -----------------------------------------------------------------------

// ----------------Create track candidates on the 1st station-------------
void CbmLitTrdTrackFinderS::CreateTrdTracks()
{

   Int_t IndMin[4];
   Int_t HitCnt[4];
   Double_t Xpred[4];
   Double_t Ypred[4];

   CbmLitTrack* trdTrack = new CbmLitTrack();
   CbmTrkHit* pHits[fNofLayersPerStation[0]];  
   
   for (UInt_t iHit0 = 0; iHit0 < fHits[0].size(); iHit0++) { //1
   
     pHits[0] = fHits[0][iHit0];

     Double_t x0 = pHits[0]->GetX();
     Double_t y0 = pHits[0]->GetY();
     Double_t z0 = pHits[0]->GetZ();
     Double_t z1 = fHits[1][0]->GetZ();

      Xpred[1] = (x0/z0) * z1;
      Ypred[1] = (y0/z0) * z1;
      
      MinMaxIndex(Xpred[1], IndMin[1], HitCnt[1], 1); 
      for ( Int_t iHit1 = IndMin[1]; iHit1 < IndMin[1] + HitCnt[1]; iHit1++){//2
         
         if ( !IsIn(Ypred[1], fHits[1][iHit1], 1) ) continue;
         
         pHits[1] = fHits[1][iHit1];
            
         Double_t x1 = fHits[1][iHit1]->GetX();
         Double_t z2 = fHits[2][0]->GetZ();
            
         Xpred[2] = (x1/z1) * z2;
         Ypred[2] = (y0/z0) * z2;
            
         MinMaxIndex(Ypred[2], IndMin[2], HitCnt[2], 2); 
         for (Int_t iHit2 = IndMin[2]; iHit2 < IndMin[2] + HitCnt[2]; iHit2++){ //3
            if ( !IsIn(Xpred[2], fHits[2][iHit2], 2) ) continue;
       
            pHits[2] = fHits[2][iHit2];

            // for 3x3  geometry
            if (fNofLayersPerStation[0] == 3) {
               AddTrackCandidate1(trdTrack, pHits);
               continue;
            }
  
            Double_t z3 = fHits[3][0]->GetZ();

            Xpred[3] = (x1/z1) * z3;
            Ypred[3] = (y0/z0) * z3;

            MinMaxIndex(Xpred[3], IndMin[3], HitCnt[3], 3); 

            for (Int_t iHit3 = IndMin[3]; iHit3 < IndMin[3] + HitCnt[3]; iHit3++){ //4
            
	            if ( !IsIn(Ypred[3], fHits[3][iHit3], 3) ) continue;
         
               pHits[3] = fHits[3][iHit3];

               AddTrackCandidate1(trdTrack, pHits);
	  
            } //4              
         } //3        
      } //2
   } //1

   delete trdTrack;
  


//
//   for (int iHit0 = 0; iHit0 < fHits[0].size(); iHit0++) {
//
//   	CbmTrdHit* TrdHit = fHits[0][iHit0];
//
//	if(!TrdHit) {
//	  cout << "-W- CbmLitTrdTrackFinderS::CreateTrdTracks:"
//	       << " Wrong TRD Hit!!!!" << endl;
//	}
//
//	int refId =  TrdHit->GetRefIndex();
//	CbmTrdPoint* TrdPoint = (CbmTrdPoint*) fArrayTrdPoint->At(refId);
//	if(!TrdPoint) {
//	  cout << "-W- CbmLitTrdTrackFinderS::CreateTrdTracks:"
//	       << " Wrong TRD Point!!!!" << endl;
//	}
//
//	CbmTrackParam* par = new CbmTrackParam();
//
//
//	par->SetX(TrdPoint->GetX());
//	par->SetY(TrdPoint->GetY());
//	par->SetTx(TrdPoint->GetPx() / TrdPoint->GetPz());
//	par->SetTy(TrdPoint->GetPy() / TrdPoint->GetPz());
//   	double p = TMath::Sqrt(TrdPoint->GetPx() * TrdPoint->GetPx() +
//			       TrdPoint->GetPy() * TrdPoint->GetPy() +
//			       TrdPoint->GetPz() * TrdPoint->GetPz());
//
//	if (p < .25) continue ;
//
//	par->SetQp(1. / p);
//
//	par->SetZ(TrdPoint->GetZ());//503.
//	
//	par->SetCovariance(0, 0, fHits[0][iHit0]->GetDx() *
//				 fHits[0][iHit0]->GetDx() * 1e-8);
//	par->SetCovariance(1, 1, fHits[0][iHit0]->GetDy() *
//			         fHits[0][iHit0]->GetDy() * 1e-8);
//	
//	CbmTrdTrack *TrdTrack = new CbmTrdTrack();
//
//	TrdTrack->SetParamFirst(*par);
//	TrdTrack->SetParamLast(*par);
//	TrdTrack->SetChi2(0.0);
//
//	fTrdTracks.push_back(TrdTrack);
//
//	delete par;
//   }

   
   if (fVerbose > 1) {
      std::cout << "-I- CbmLitTrdTrackFinderS::CreateTrdTracks() : " << std::endl
           << "fTrdTracks.size() = " << fTracks.size() << std::endl;
   }
}
// -----------------------------------------------------------------------

// ------------------Defines min and max index of hits -------------------
void CbmLitTrdTrackFinderS::MinMaxIndex(Double_t pred, 
                                        Int_t &IndMin,
                                        Int_t &HitCnt, 
                                        Int_t layer)
{

   CbmTrkHit* hit = new CbmTrdHit();
   std::vector<CbmTrkHit*>::iterator it_min;
   std::vector<CbmTrkHit*>::iterator it_max;
   
   if (fMaxErrY[layer] != 0) {
      hit->SetY(pred - fSigmaCoef * fSigmaY[layer]);
      it_min = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                                 hit, CmpYUp);
      
      hit->SetY(pred + fSigmaCoef * fSigmaY[layer]);
      it_max = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                                 hit, CmpYUp) - 1 ;
   } else {
      hit->SetX(pred - fSigmaCoef * fSigmaX[layer]);
      it_min = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                                 hit, CmpXUp);
      
      hit->SetX(pred + fSigmaCoef * fSigmaX[layer]);
      it_max = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                                 hit, CmpXUp) - 1 ;
   }
   
   IndMin = it_min - fHits[layer].begin();
   
   HitCnt = it_max - it_min + 1;
   
   delete hit;
}
// -----------------------------------------------------------------------

// -------------Checks if the hit in the area near the track--------------
Bool_t CbmLitTrdTrackFinderS::IsIn(Double_t pred, CbmTrkHit *pHit, Int_t layer)
{
   if (pHit->GetDx() < pHit->GetDy()) 
      return pHit->GetY() < pred + fSigmaCoef * fSigmaY[layer] &&
               pHit->GetY() > pred - fSigmaCoef * fSigmaY[layer];
   else     
      return pHit->GetX() < pred + fSigmaCoef * fSigmaX[layer] &&
               pHit->GetX() > pred - fSigmaCoef * fSigmaX[layer];
}
// -----------------------------------------------------------------------


// -------------------Adds track candidate--------------------------------
void CbmLitTrdTrackFinderS::AddTrackCandidate1(CbmLitTrack* pTrack, 
                                               CbmTrkHit* pHits[])
{
   CbmTrackParam* par = new CbmTrackParam();

   Double_t x0 = pHits[0]->GetX();
   Double_t x1 = pHits[1]->GetX();
   Double_t x2 = pHits[2]->GetX();
   Double_t x3;
   Double_t y0 = pHits[0]->GetY();
   Double_t y2 = pHits[2]->GetY();
   Double_t z0 = pHits[0]->GetZ();
   Double_t z1 = pHits[1]->GetZ();
   Double_t z2 = pHits[2]->GetZ();
   Double_t z3;

   Double_t a ,b;
   
   if (fNofLayersPerStation[0] == 4) {
      x3 = pHits[3]->GetX();
      z3 = pHits[3]->GetZ();
   
      a = (x3 - x1)/(z3 - z1);
      b = x1 - a * z1;
      
   } else 
   if (fNofLayersPerStation[0] == 3) {
   
      Double_t s0, sz, sz2, sy, szy;
      Double_t t2, t4, t7;
      Double_t wv = 1., w0 = 0.01, w1 = 1., w2 = 0.01;

      Double_t zv = 0.;
      Double_t xv = 0.;
      
      s0 = wv + w0 + w1 + w2;
      sz = wv * zv + w0 * z0 + w1 * z1 + w2 * z2;
      sz2 = wv * zv * zv + w0 * z0 * z0 + w1 * z1 * z1 + w2 * z2 * z2;
      sy = wv * xv + w0 * x0 + w1 * x1 + w2 * x2;
      szy = wv * zv * xv + w0 * z0 * x0 + w1 * z1 * x1 + w2 * z2 * x2;
      t2 = sz * sz;
      t4 = 0.10e1 / (s0 * sz2 - t2);
      t7 = sz * t4;

      b = sz2 * t4 * sy - t7 * szy;
      a = -t7 * sy + s0 * t4 * szy;   
      
   }
     
   
   par->SetX(a*z0+b);//x0
   par->SetY(y0);
   par->SetTx(a);//(x3 - x1)/(z3 - z1));
   par->SetTy((y2 - y0)/(z2 - z0));

   par->SetQp(1./fMom);
            
   par->SetZ(z0); //503.
   
   par->SetCovariance(0, 0, 100.);
   par->SetCovariance(1, 1, 100.);
   par->SetCovariance(0, 0, 1.);
   par->SetCovariance(1, 1, 1.);
   par->SetCovariance(0, 0, 0.1);
  

   pTrack->SetParamLast(par);
   pTrack->SetParamFirst(par);
   pTrack->SetChi2(0.0);

   AddTrackCandidate(pTrack, pHits, 0);

   delete par;
}

*/

ClassImp(CbmLitTrdTrackFinderS);
