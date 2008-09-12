#include "CbmLitTrackFitterIter.h"

#include "CbmLitHit.h"
#include "CbmLitTrack.h"
#include "CbmLitFitNode.h"

#include <iostream>

CbmLitTrackFitterIter::CbmLitTrackFitterIter(
		CbmLitTrackFitter* fitter,
		CbmLitTrackFitter* smoother)
{
	fFitter = fitter;
	fSmoother = smoother;
	fNofIterations = 1;
	fChiSqCut = 10;
	fMinNofHits = 2;
}

CbmLitTrackFitterIter::~CbmLitTrackFitterIter()
{
}

LitStatus CbmLitTrackFitterIter::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterIter::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterIter::Fit(
		CbmLitTrack *track,
		Bool_t downstream)
{
//	std::cout << "FIT start. ";
//	std::cout << "nofHits = " << track->GetNofHits() << " ;";
	for (Int_t iter = 0; iter < fNofIterations; iter++) {
//		std::cout << "Iter " << iter << " started ;" << std::endl;
		Bool_t isRefit = false;
		
		if (fFitter->Fit(track) == kLITERROR) {
			return kLITERROR;
		}
	
		if (fSmoother->Fit(track) == kLITERROR) {
			return kLITERROR;
		}
		
		if (iter < fNofIterations -1) {
//			std::cout << "chi2: ";
			for (Int_t i = 0; i < track->GetNofHits(); i++) {
				Double_t chiSq = track->GetFitNode(i)->GetChiSqSmoothed();
//				std::cout << " " << chiSq;
				if (chiSq > fChiSqCut) {
					track->RemoveHit(i);
					isRefit = true;
				}
			}
//			std::cout << std::endl;
		}

//		std::cout << "nofHits = " << track->GetNofHits() << " ; ";
//		std::cout << "Iter " << iter << " finished" << std::endl;
		if (track->GetNofHits() < fMinNofHits) return kLITERROR;
		if (!isRefit) return kLITSUCCESS;
	}
	return kLITSUCCESS;
}

ClassImp(CbmLitTrackFitterIter);
