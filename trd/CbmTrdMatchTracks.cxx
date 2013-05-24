/**
 * \file CbmTrdMatchTracks.cxx
 * \author V.Friese <v.friese@gsi.de>
 * \date 01/12/05
 **/
#include "CbmTrdMatchTracks.h"

#include "CbmTrackMatch.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"

#include <map>

CbmTrdMatchTracks::CbmTrdMatchTracks() 
  : FairTask("CbmTrdMatchTracks", 1),
    fTracks(NULL),
    fPoints(NULL),
    fHits(NULL),
    fClusters(NULL),
    fDigiMatches(NULL),
    fTrackMatches(NULL),
    fNofHits(0),
    fNofTrueHits(0),
    fNofWrongHits(0),
    fPercentageTrueHits(0.),
    fPercentageWrongHits(0.),
    fNofMCTracksPerRecoTrack(0),
    fNofTracks(0)
{
}

CbmTrdMatchTracks::~CbmTrdMatchTracks()
{
}

InitStatus CbmTrdMatchTracks::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();

	fHits = (TClonesArray*) ioman->GetObject("TrdHit");
	if (fHits == NULL) LOG(FATAL) << "CbmTrdMatchTracks::Init: No TrdHit array!" << FairLogger::endl;

	fTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
	if (fTracks == NULL) LOG(FATAL) << "CbmTrdMatchTracks::Init: No TrdTrack array!" << FairLogger::endl;

	fPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
	if (fPoints == NULL) LOG(FATAL) << "CbmTrdMatchTracks::Init: No TrdPoint array!" << FairLogger::endl;

	// This two branches exist only if digitization and clustering is used
	fClusters = (TClonesArray*) ioman->GetObject("TrdCluster");
	fDigiMatches = (TClonesArray*) ioman->GetObject("TrdDigiMatch");

	// Create and register CbmTrackMatch array
	fTrackMatches = new TClonesArray("CbmTrackMatch", 100);
	ioman->Register("TrdTrackMatch", "TRD", fTrackMatches, kTRUE);

	return kSUCCESS;
}

void CbmTrdMatchTracks::Exec(
		Option_t* opt)
{
   // If there are no clusters and digis than smearing hit producer was used.
	if (NULL == fClusters || NULL == fDigiMatches) {
		ExecSmearing(opt);
	} else {
	   ExecCluster(opt);
	}
	static Int_t eventNo = 0;
	LOG(INFO) << "CbmTrdMatchTracks::Exec: event=" << eventNo++ << FairLogger::endl;
}

void CbmTrdMatchTracks::ExecSmearing(
		Option_t* opt)
{
	fTrackMatches->Clear();

	// Loop over TrdTracks
	Int_t nofTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
		const CbmTrdTrack* track = static_cast<const CbmTrdTrack*>(fTracks->At(iTrack));
		if (NULL == track) continue;
		std::map<Int_t, Int_t> matchMap;
		Int_t nofHits = track->GetNofHits();
		for (Int_t iHit = 0; iHit < nofHits; iHit++) {
			const CbmTrdHit* hit = static_cast<const CbmTrdHit*>(fHits->At(track->GetHitIndex(iHit)));
			if (NULL == hit) continue;
			const FairMCPoint* point = static_cast<const FairMCPoint*>(fPoints->At(hit->GetRefId()));
			if (NULL == point) continue;
			matchMap[point->GetTrackID()]++;
		}

		// Search for best matching MC track
		Int_t bestMCTrackId = -1;
		Int_t nofMCTracks = 0;
		Int_t nofAllHits = 0;
		Int_t nofTrueHits = 0;
		std::map<Int_t, Int_t>::const_iterator it;
		for (it = matchMap.begin(); it != matchMap.end(); it++) {
			nofMCTracks++;
			nofAllHits += it->second;
			if (it->second > nofTrueHits) {
				bestMCTrackId = it->first;
				nofTrueHits = it->second;
			}
		}
		Int_t nofWrongHits = nofAllHits - nofTrueHits;

		// Create TrdTrackMatch
		new ((*fTrackMatches)[iTrack]) CbmTrackMatch(bestMCTrackId, nofTrueHits, nofWrongHits, 0, nofMCTracks);

		// Some statistics
		fNofHits += nofHits;
		fNofTrueHits += nofTrueHits;
		fNofWrongHits += nofWrongHits;
		fPercentageTrueHits += 100. * (Double_t)nofTrueHits / (Double_t)nofHits;
		fPercentageWrongHits += 100. * (Double_t)nofWrongHits / (Double_t)nofHits;
		fNofMCTracksPerRecoTrack += nofMCTracks;
		fNofTracks++;
	} // Track loop
}

void CbmTrdMatchTracks::ExecCluster(
		Option_t* opt) {
	fTrackMatches->Clear();

	Int_t nofTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) { // Loop over tracks
		// std::map stores MC track id to number of contributions of this MC track
	   std::map<Int_t, Int_t> matchMap;

		CbmTrdTrack* pTrack = (CbmTrdTrack*) fTracks->At(iTrack);
		if (pTrack == NULL) continue;

		Int_t nofHits = pTrack->GetNofHits();
		for (Int_t iHit = 0; iHit < nofHits; iHit++) { // Loop over hits
		   // std::set stores MC track indices contributed to a certain hit
		   std::set<Int_t> mcIdHit;
			Int_t index = pTrack->GetHitIndex(iHit);
			CbmTrdHit* hit = (CbmTrdHit*) fHits->At(index);
			if (hit == NULL) continue;

			Int_t clusterId = hit->GetRefId();
         CbmTrdCluster* cluster = (CbmTrdCluster*) fClusters->At(clusterId);
			if (cluster == NULL) continue;
			for (Int_t iDigi = 0; iDigi < cluster->GetNofDigis(); iDigi++){
             Int_t digiId = cluster->GetDigi(iDigi);
			    CbmTrdDigiMatch* digiMatch = (CbmTrdDigiMatch*) fDigiMatches->At(digiId);
			    if (digiMatch == NULL) continue;
			    for (Int_t iPoint = 0; iPoint < digiMatch->GetNofRefs(); iPoint++) {
               Int_t pointIndex = digiMatch->GetRefIndex(iPoint);
               if (pointIndex < 0) { // Fake or background hit
                  mcIdHit.insert(-1);
                  continue;
               }
               FairMCPoint* point = (FairMCPoint*) fPoints->At(pointIndex);
               if (point == NULL) continue;
               mcIdHit.insert(point->GetTrackID());
			    }
			} // loop over cluster

			for (std::set<Int_t>::iterator it = mcIdHit.begin(); it != mcIdHit.end(); it++) {
			   matchMap[*it]++;
			}

		} // Loop over hits

		Int_t nofTrueHits = 0;
		Int_t bestMcTrackId = -1;
		Int_t nPoints = 0;
		for (std::map<Int_t, Int_t>::iterator it = matchMap.begin(); it != matchMap.end(); it++) {
			if (it->first != -1 && it->second >= nofTrueHits) {
				bestMcTrackId = it->first;
				nofTrueHits = it->second;
			}
			nPoints += it->second;
		}

		Int_t nofWrongHits = nofHits - nofTrueHits;
		Int_t nofMCTracks = matchMap.size() - 1;

		new ((*fTrackMatches)[iTrack]) CbmTrackMatch(bestMcTrackId, nofTrueHits, nofWrongHits, 0, nofMCTracks);

		fNofHits += nofHits;
		fNofTrueHits += nofTrueHits;
		fNofWrongHits += nofWrongHits;
      fPercentageTrueHits += 100. * (Double_t)nofTrueHits / (Double_t)nofHits;
      fPercentageWrongHits += 100. * (Double_t)nofWrongHits / (Double_t)nofHits;
      fNofMCTracksPerRecoTrack += nofMCTracks;
      fNofTracks++;
	} // Loop over tracks
}

void CbmTrdMatchTracks::Finish()
{
   PrintStatistics();
}

void CbmTrdMatchTracks::PrintStatistics()
{
   Double_t nofHits = (fNofTracks != 0) ? (Double_t)fNofHits / (Double_t)fNofTracks : 0;
   Double_t nofTrueHits = (fNofTracks != 0) ? (Double_t)fNofTrueHits / (Double_t)fNofTracks : 0;
   Double_t nofWrongHits = (fNofTracks != 0) ? (Double_t)fNofWrongHits / (Double_t)fNofTracks : 0;
   Double_t percentageTrueHits = (fNofTracks != 0) ? (Double_t)fPercentageTrueHits / (Double_t)fNofTracks : 0;
   Double_t percentageWrongHits = (fNofTracks != 0) ? (Double_t)fPercentageWrongHits / (Double_t)fNofTracks : 0;
   Double_t nofMCTracksPerRecoTrack = (fNofTracks != 0) ? (Double_t)fNofMCTracksPerRecoTrack / (Double_t)fNofTracks : 0;
   LOG(INFO) << "CbmTrdMatchTracks::PrintStatistics: nofHits=" << nofHits
         << " nofTrueHits=" << nofTrueHits << " nofWrongHits=" << nofWrongHits
         << " percentageTrueHits=" << percentageTrueHits
         << " percentageWrongHits=" << percentageWrongHits
         << " nofMCTracksPerRecoTrack=" << nofMCTracksPerRecoTrack << FairLogger::endl;
}

ClassImp( CbmTrdMatchTracks)
