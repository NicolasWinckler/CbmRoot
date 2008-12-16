

#include "CbmLitMuchTrackFinderTrigger.h"
#include "CbmLitEnvironment.h"

#include "CbmTrkHit.h"
#include "CbmMuchTrack.h"
#include "CbmLitConverter.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>

// -----------------------------------------------------------------------
CbmLitMuchTrackFinderTrigger::CbmLitMuchTrackFinderTrigger()
{
	fVerbose = 3;
	fEvents = 0;
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
CbmLitMuchTrackFinderTrigger::~CbmLitMuchTrackFinderTrigger()
{
	
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
void CbmLitMuchTrackFinderTrigger::Init()
{
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	   
	env->GetMuchLayout(fNofStations, fNofLayers, fNofLayersPerStation, fLayerZPos);

}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
Int_t CbmLitMuchTrackFinderTrigger::DoFind(TClonesArray* hitArray,
                                          TClonesArray* trackArray)
{
	fHitArray = hitArray;
	
	fHits.clear();
	fHits.resize(fNofLayers);
	
	ArrangeHits();
	CreateTracks();
	CopyToOutput(trackArray);
	
	std::cout << "Number of found tracks: " << fNofTracks << std::endl;
	std::cout << "Number of procesed events:" << fEvents++ << std::endl;
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
void CbmLitMuchTrackFinderTrigger::ArrangeHits()
{

   Int_t nofHits = fHitArray->GetEntriesFast();
   for(Int_t iHit = 0; iHit < nofHits; iHit++) {
   
      CbmTrkHit* hit = (CbmTrkHit*) fHitArray->At(iHit);
      
      if(NULL == hit) continue;
      
      Int_t layer = hit->GetStationNr() - 1;
                 
      if(layer < 0 || layer > fNofLayers) {
         std::cout << "-W- CbmLitMuchTrackFinderTrigger::ArrangeHits: "
                   << "wrong layer number." << std::endl;
         continue;
      }
      
      CbmLitPixelHit* litHit = new CbmLitPixelHit;
      CbmLitConverter::TrkHitToLitHit(hit, iHit, litHit);
      //litHit->MapFromCbmTrkHit(hit, iHit);
      fHits[layer].push_back(litHit);
      //fHitIdMap[hit] = iHit;
   }
     
   if (fVerbose > 1) {
      std::cout << "-I- CbmLitTrackFinder::ArrangeHits: " << std::endl;
      for (Int_t i = 0; i < fNofLayers; i++) {
          std::cout << "  Station " << i << " has " 
                    << fHits[i].size() << " hits" << std::endl;
      }
   }
 
   // sort hits by y coordinate
   //std::sort(fHits[i].begin(), fHits[i].end(), CmpYUp);
 
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------

void CbmLitMuchTrackFinderTrigger::CreateTracks()
{
	std::vector<CbmLitPixelHit*>& hits = fHits[fNofLayers - 1];
	typedef std::vector<CbmLitPixelHit*>::iterator HitIterator;
	
	Int_t nofLastStations = 2;
	Int_t nofLastLayers = 0;
	for (Int_t iStation = fNofStations - nofLastStations;
			iStation < fNofStations; iStation++){
		nofLastLayers += fNofLayersPerStation[iStation];
	}
	
	Int_t startLayer = fNofLayers - nofLastLayers;
	
	for (HitIterator iLastHit = hits.begin(); 
			iLastHit != hits.end(); iLastHit++) {
		
		Double_t tx = TMath::Tan((*iLastHit)->GetX()/(*iLastHit)->GetZ());
		Double_t ty = TMath::Tan((*iLastHit)->GetY()/(*iLastHit)->GetZ());
		
		std::vector<Double_t> nofTrackHits(nofLastLayers);
		
		std::vector<CbmLitPixelHit*> trackHits;
		trackHits.reserve(10);
		trackHits.push_back(*iLastHit);
		
		Int_t nofMissingHits = 0;
		for (Int_t iLayer = startLayer; iLayer < fNofLayers - 1; iLayer++){
			
			Double_t xpred = tx * fLayerZPos[iLayer];
			Double_t ypred = tx * fLayerZPos[iLayer];
			
			Bool_t isMissingHit = true;
			for (HitIterator iHit = fHits[iLayer].begin(); 
			       iHit != fHits[iLayer].end(); iHit++){
				
				if (!IsIn(xpred, ypred, *iHit)) continue;
				isMissingHit = false;
				
				nofTrackHits[iLayer - startLayer]++;
				
				trackHits.push_back(*iHit);										
				
			}
			if(isMissingHit) nofMissingHits++;			
		}
		
		if(nofMissingHits < 1) AddTrack(trackHits);
		
		std::cout << "trackHits.size()=" << trackHits.size() << std::endl;
		
		for (Int_t i = 0; i < nofTrackHits.size(); i++)
			std::cout << nofTrackHits[i] << ", ";
		std::cout << std::endl;
		
	}
	
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
void CbmLitMuchTrackFinderTrigger::AddTrack(std::vector<CbmLitPixelHit*>& trackHits)
{
	CbmLitTrack* track = new CbmLitTrack;
	
	for (Int_t iHit = 0; iHit < trackHits.size(); iHit++) {
		track->AddHit(trackHits[iHit]);		
	}
	
	track->SortHits();
	fFoundTracks.push_back(track);
	
	fNofTracks++;
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
Bool_t CbmLitMuchTrackFinderTrigger::IsIn(Double_t xpred, 
		                                  Double_t ypred,
		                                  CbmLitPixelHit* pHit)
{
    Double_t xhit = pHit->GetX();
	Double_t yhit = pHit->GetY();
	   
	Double_t devX = 15.;
	Double_t devY = 15.;

	return   ( (xpred + devX) >= xhit ) &&
             ( (xpred - devX) <= xhit ) &&
             ( (ypred + devY) >= yhit ) &&
             ( (ypred - devY) <= yhit );
}

// --------------------Copy to output TClonesArray------------------------
void CbmLitMuchTrackFinderTrigger::CopyToOutput(TClonesArray* trackArray)
{
   Int_t nofTracks = trackArray->GetEntriesFast();
      
   for(std::vector<CbmLitTrack*>::iterator iTrack = fFoundTracks.begin();
        iTrack != fFoundTracks.end(); iTrack++) {
	         
	   CbmMuchTrack track;
	   CbmLitConverter::LitTrackToMuchTrack(*iTrack, &track);
	   //(*iTrack)->MapToMuch(&track);
	   new ((*trackArray)[nofTracks]) CbmMuchTrack(track);
	   delete (*iTrack);
	   
	   nofTracks++;	   
   } 

   fFoundTracks.clear();
}
// -----------------------------------------------------------------------


ClassImp(CbmLitMuchTrackFinderTrigger);


