/** CbmMuchTrackFinderIdeal
 * @author A.Lebedev <Andrey.Lebedev@gsi.de>
 * @since 2007
 **/
#include "CbmMuchTrackFinderIdeal.h"

#include "CbmMuchPixelHit.h"
#include "CbmMuchTrack.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigiMatch.h"

#include "CbmDetectorList.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "TClonesArray.h"

#include <iostream>
#include <map>
#include <cmath>

CbmMuchTrackFinderIdeal::CbmMuchTrackFinderIdeal():
	fVerbose(1),
	fEvents(0),
	fMcTracks(NULL),
	fMcPoints(NULL),
	fMuchPixelHits(NULL),
	fMuchTracks(NULL),
	fMuchDigiMatches(NULL),
	fClusters(NULL)
{
}

CbmMuchTrackFinderIdeal::~CbmMuchTrackFinderIdeal()
{
}

void CbmMuchTrackFinderIdeal::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (ioman == NULL) Fatal("Init", "No FairRootManager!");

	fMcTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
	if (fMcTracks == NULL) Fatal("Init", "No MCTrack array!");

	fMcPoints  = (TClonesArray*) ioman->GetObject("MuchPoint");
	if (fMcPoints == NULL) Fatal("Init", "No MuchPoint array!");

	fMuchDigiMatches  = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
	if (fMuchDigiMatches == NULL) Fatal("Init", "No MuchDigiMatches array!");

	fClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
	if (fClusters == NULL) Fatal("Init","No MuchCluster array!");
}

Int_t CbmMuchTrackFinderIdeal::DoFind(
		TClonesArray* hitArray,
        TClonesArray* trackArray)
{
	fMuchPixelHits = hitArray;
	fMuchTracks = trackArray;

	// MCTrack index to MuchTrack index
	std::map<Int_t, Int_t> muchTrackMap;
	Int_t nofMuchTracks = 0;
	Int_t nofMcTracks = fMcTracks->GetEntriesFast();
	for (Int_t iMcTrack=0; iMcTrack < nofMcTracks; iMcTrack++) {
		CbmMCTrack*  pMcTrack = (CbmMCTrack*) fMcTracks->At(iMcTrack);
		if (pMcTrack == NULL) continue;
		if (std::abs(pMcTrack->GetPdgCode()) != 13 ) continue;
		if (pMcTrack->GetMotherId() != -1) continue;

		new((*fMuchTracks)[nofMuchTracks]) CbmMuchTrack();
		muchTrackMap[iMcTrack] = nofMuchTracks;
		nofMuchTracks++;
	}

	Int_t nofMuchHits = fMuchPixelHits->GetEntriesFast();
	for (Int_t iHit = 0; iHit < nofMuchHits; iHit++) {
		CbmMuchPixelHit* pHit = (CbmMuchPixelHit*) fMuchPixelHits->At(iHit);
		if (!pHit) continue;

		Int_t clusterId = pHit->GetRefId();
		CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(clusterId);
		for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++){
			Int_t digiIndex = cluster->GetDigiIndex(iDigi);

			CbmMuchDigiMatch* pDigiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(digiIndex);
			if (!pDigiMatch) continue;

			for (Int_t iDigi = 0; iDigi < pDigiMatch->GetNPoints(); iDigi++){
				Int_t pointIndex = pDigiMatch->GetRefIndex(iDigi);
				if (pointIndex < 0) continue;

				FairMCPoint* pMcPoint = (FairMCPoint*) (fMcPoints->At(pointIndex));
				if (pMcPoint == NULL) continue;

				Int_t mcTrackIndex = pMcPoint->GetTrackID();
				if (mcTrackIndex < 0) continue;

				CbmMCTrack*  pMcTrack = (CbmMCTrack*) fMcTracks->At(mcTrackIndex);
				if (pMcTrack == NULL) continue;

				if (muchTrackMap.find(mcTrackIndex) == muchTrackMap.end()) continue;

				Int_t muchTrackIndex = muchTrackMap[mcTrackIndex];
				CbmMuchTrack* pMuchTrack = (CbmMuchTrack*) fMuchTracks->At(muchTrackIndex);
				pMuchTrack->AddHitIndex(iHit);

				if (pMuchTrack->GetNHits() == 1){
				 SetTrackParam(pMcTrack, pMcPoint, pMuchTrack);
				}
			} // loop over digis
		} // loop over clusters
	} // loop over hits

	std::cout << "Event: " << fEvents++ << std::endl;
	return nofMuchTracks;
}

void CbmMuchTrackFinderIdeal::SetTrackParam(
		const CbmMCTrack* mcTrack,
		const FairMCPoint* mcPoint,
		CbmMuchTrack* track)
{
	FairTrackParam par;
	par.SetX(mcPoint->GetX());
    par.SetY(mcPoint->GetY());
    par.SetTx(mcPoint->GetPx()/mcPoint->GetPz());
    par.SetTy(mcPoint->GetPy()/mcPoint->GetPz());
    if (mcTrack->GetPdgCode() == 13) par.SetQp(-1./mcTrack->GetP());
    else
    if (mcTrack->GetPdgCode() == -13) par.SetQp(1./mcTrack->GetP());
    par.SetZ(mcPoint->GetZ());

    Double_t cov[15];
    cov[0] = 1e-9; cov[1] = 1e-9;  cov[2] = 1e-9; cov[3] = 1e-9; cov[4] = 1e-9;
                   cov[5] = 1e-9;  cov[6] = 1e-9; cov[7] = 1e-9; cov[8] = 1e-9;
                                   cov[9] = 1e-9; cov[10]= 1e-9; cov[11]= 1e-9;
                                                  cov[12]= 1e-9; cov[13]= 1e-9;
                                                                 cov[14]= 1e-9;
    par.SetCovMatrix(cov);
    track->SetMuchTrack(&par);
}

ClassImp(CbmMuchTrackFinderIdeal)
