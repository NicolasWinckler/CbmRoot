/**
 * \file CbmStsSimulationQaCalculator.cxx
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */

#include "CbmStsSimulationQaCalculator.h"
#include "CbmHistManager.h"
#include "CbmStsPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TH1.h"
#include <cassert>
#include <sstream>

using std::stringstream;

CbmStsSimulationQaCalculator::CbmStsSimulationQaCalculator(
      CbmHistManager* histManager)
{

}

CbmStsSimulationQaCalculator::~CbmStsSimulationQaCalculator()
{

}

void CbmStsSimulationQaCalculator::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	assert(ioman != NULL);
	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
	fNofStations = 8; // TODO: Get number of STS stations from TGeo
}

void CbmStsSimulationQaCalculator::Exec()
{
	Int_t nPoints = fStsPoints->GetEntriesFast();
	Int_t nTracks = fMCTracks->GetEntriesFast();
	if ( nTracks > 10000 ) {
//		fLogger->Warning(MESSAGE_ORIGIN, "Too many MCTracks: %i, skipping event", nTracks);
		return;
	}

	Int_t nPointsOnTrack[10000];
	Int_t hitPattern[10000];
	for (Int_t iTrack = 0; iTrack < 10000; iTrack++) {
		nPointsOnTrack[iTrack] = 0;
		hitPattern[iTrack] = 0;
	}

	for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
		CbmStsPoint* point = (CbmStsPoint*) fStsPoints->At(iPoint);

		// ---> Point properties
		Double_t zPoint = point->GetZIn();
		Int_t iStation = TMath::Nint( (zPoint - 30.) / 10. ); // TODO: Real geometry
		Int_t iTrack = point->GetTrackID();
		CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(iTrack);
		Bool_t isPrimary = kFALSE;
		if ( track->GetMotherId() < 0 ) isPrimary = kTRUE;

		// ---> Update number of points for this track
		nPointsOnTrack[iTrack]++;

		// ---> Update hit pattern for this track
		hitPattern[iTrack] = hitPattern[iTrack] | ( 1 << iStation );

		// ---> Fill number of points per Station
		fHM->H1("hss_NofObjects_Sts_Points_All_Station")->Fill(Double_t(iStation+1));

		// ---> Fill number of points per Station for primary tracks
		if ( isPrimary ) fHM->H1("hss_NofObjects_Sts_Points_Primary_Station")->Fill(Double_t(iStation+1));

		// ---> Fill x-y of points in each station
		Double_t x = point->GetXIn();
		Double_t y = point->GetYIn();
		stringstream ss;
		ss << "hss_XY_Sts_Points_All_Station_" << iStation;
		fHM->H1(ss.str())->Fill(x,y);

	}  // StsPoints


	Int_t nPrimary   = 0;
	Int_t nAccepted  = 0;
	Int_t nReference = 0;
	Int_t nSecondary = 0;
	for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
	CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(iTrack);

	// ---> Track momentum
	Double_t trackP  = track->GetP();
	Double_t trackPt = track->GetPt();


	// ---> Check for primary, acceptance and reference
	Bool_t isPrimary   = kFALSE;
	Bool_t isAccepted  = kFALSE;
	Bool_t isReference = kFALSE;
	if ( track->GetMotherId() < 0 ) isPrimary = kTRUE;
	if ( NofStationsForTrack(hitPattern[iTrack]) > 3) {
	  isAccepted = kTRUE;
	  if ( HasPointInStation(hitPattern[iTrack], fNofStations-1) )
		isReference = kTRUE;
	}


	// ---> Primary tracks
	if ( isPrimary ) {
	  nPrimary++;

	  // ---> Fill number of points on track
	  fHM->H1("hss_NofPointsPerTrack_Sts_Primary")->Fill(Double_t(nPointsOnTrack[iTrack]));

	  // ---> Fill momentum
	  fHM->H1("hss_Sts_All_p")->Fill(trackP);
	  fHM->H1("hss_Sts_All_pt")->Fill(trackPt);

	}   // primary track

	// ---> Accepted primary tracks
	if ( isAccepted ) {
	  nAccepted++;

	  // ---> Fill momentum
	  fHM->H1("hss_Sts_Acc_p")->Fill(trackP);
	  fHM->H1("hss_Sts_Acc_pt")->Fill(trackPt);

	}

	// ---> Reference tracks
	if ( isReference ) {
	  nReference++;

	  // ---> Fill number of points on track
	  fHM->H1("hss_NofPointsPerTrack_Sts_Reference")->Fill(Double_t(nPointsOnTrack[iTrack]));

	  // ---> Fill momentum
	  fHM->H1("hss_Sts_Ref_p")->Fill(trackP);
	  fHM->H1("hss_Sts_Ref_pt")->Fill(trackPt);

	}  // Reference tracks

	// ---> Secondary tracks
	if ( ! isPrimary ) {
	  nSecondary++;

	  // ---> Fill start z position of secondary track
	  Double_t zStart = track->GetStartZ();
	  fHM->H1("hss_StartSecondaries")->Fill(zStart);

	}    // secondary track

	}   // MCTracks
	//fLogger->Info(MESSAGE_ORIGIN, "MCTracks: %i, primary: %i, secondary: %i, StsPoints: %i",
	//  nTracks, nPrimary, nSecondary, nPoints);

	// Increase event counter
	fHM->H1("hss_EventNo_StsSimQa")->Fill(0.5);
	std::cout << "CbmStsSimulationQa::Exec: event=" << fHM->H1("hss_EventNo_StsSimQa")->GetEntries() << std::endl;
}

void CbmStsSimulationQaCalculator::Finish()
{
	Int_t nofEvents = fHM->H1("hen_EventNo_ClusteringQa")->GetEntries();
	fHM->ScaleByPattern("hno_NofObjects_.*_Station", 1. / nofEvents);
	fHM->ShrinkEmptyBinsByPattern("hno_NofObjects_.*_Station");
}

Int_t CbmStsSimulationQaCalculator::NofStationsForTrack(Int_t hitPattern) {
  Int_t nStations = 0;
  for (Int_t iStat = 0; iStat < fNofStations; iStat++)
    if ( HasPointInStation(hitPattern, iStat) ) nStations++;
  return nStations;
}
