#include "CbmRichGeoTest.h"
#include "FairRootManager.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"
#include "CbmMCTrack.h"
#include "CbmRichPoint.h"

#include <iostream>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::map;
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmRichGeoTest::CbmRichGeoTest() :FairTask("RichGeoTestQa")
{
	fEventNum = 0;

	fhNofHitsEl = new TH1D("fhNofHitsEl", "fhNofHitsEl", 45, 0, 45);
	fhAaxisEl = new TH1D("fhAaxisEl", "fhAaxisEl", 50, 0, 10);
	fhBaxisEl = new TH1D("fhBaxisEl", "fhBaxisEl", 50, 0, 10);
	fhBoverAEl = new TH1D("fhBoverAEl", "fhBoverAEl", 50, 0, 1);
	fhBoverAXYEl = new TH2D("fhBoverAXYEl","fhBoverAXYEl", 150, -150, 150, 250, -250, 250);
	fhXYEl = new TH2D("fhXYEl", "fhXYEl",150, -150, 150, 250, -250, 250);
	fhMCEl = new TH1D("fhMCEl", "fhMCEl", 30, 0, 15);
	fhAccEl = new TH1D("fhAccEl", "fhAccEl", 30, 0, 15);
	fhBaxisVsMomEl = new TH2D("fhBaxisVsMomEl", "fhBaxisVsMomEl",30, 0, 15,25, 0, 10 );;
	fhAaxisVsMomEl = new TH2D("fhAaxisVsMomEl", "fhAaxisVsMomEl",30, 0, 15,25, 0, 10 );;
	fhNofPointsEl = new TH1D("fhNofPointsEl", "fhNofPointsEl", 50, 0, 250);;

	fhXYHits = new TH2D("fhXYHits", "fhXYHits",150, -150, 150, 250, -250, 250);
	fhXYPoints = new TH2D("fhXYPoints", "fhXYPoints",150, -150, 150, 250, -250, 250);
	fhXYHitsOneEvent = new TH2D("fhXYHitsOneEvent", "fhXYHitsOneEvent",150, -150, 150, 250, -250, 250);
	fhXYPointsOneEvent = new TH2D("fhXYPointsOneEvent", "fhXYPointsOneEvent",150, -150, 150, 250, -250, 250);
	fhNofHitsEvent= new TH1D("fhNofHitsEvent", "fhNofHitsEvent", 50, 0, 2500);
	fhNofPointsEvent= new TH1D("fhNofPointsEvent", "fhNofPointsEvent", 50, 0, 10000);



}
// -------------------------------------------------------------------------

//------------  standard constructor (with verbosity level)  ---------------------------------
CbmRichGeoTest::CbmRichGeoTest(const char *name, const char *title, Int_t verbose)
  :FairTask(name)
{

}

  // -----   Destructor   ----------------------------------------------------
CbmRichGeoTest::~CbmRichGeoTest()
{


}
// -------------------------------------------------------------------------


// -----   Initialization   -----------------------------------------------

InitStatus CbmRichGeoTest::Init()
{
	cout << "InitStatus CbmRichGeoTest::Init()"<<endl;
	// Get and check FairRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		cout << "-E- CbmRichGeoTest::Init: " << "RootManager not instantised!"
				<< endl;
		return kERROR;
	}

	// Get hit Array
	fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
	if ( !fRichHits) {
		cout << "-W- CbmRichGeoTest::Init: No RichHit array!" << endl;
	}

	// Get RichRing Array
	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if ( !fRichRings) {
		cout << "-E- CbmRichGeoTest::Init: No RichRing array!" << endl;
		return kERROR;
	}

	// Get MC Point array
	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( !fRichPoints) {
		cout << "-E- CbmRichGeoTest::Init: No RichPoint array!" << endl;
		return kERROR;
	}

	// Get MC Point array
	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( !fMCTracks) {
		cout << "-E- CbmRichGeoTest::Init: No MCTrack array!" << endl;
		return kERROR;
	}

	// Get RichRingMatch array
	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if ( !fRichRingMatches) {
		cout << "-E- CbmRichGeoTest::Init: No RichRingMatch array!" << endl;
		return kERROR;
	}

	return kSUCCESS;
}



void CbmRichGeoTest::Exec(Option_t* option)
{
	fEventNum++;
	cout << "CbmRichGeoTest, event No. " <<  fEventNum << endl;
	RingParameters();
	EventPoints();
	MCElectrons();
}

void CbmRichGeoTest::RingParameters()
{
	Int_t nofRings = fRichRings->GetEntriesFast();
	for (Int_t iR = 0; iR < nofRings; iR++){
		CbmRichRing *ring = (CbmRichRing*) fRichRings->At(iR);
		if (!ring) continue;
		CbmRichRingMatch* ringMatch = (CbmRichRingMatch*) fRichRingMatches->At(iR);
		if (!ringMatch) continue;

		Int_t mcTrackId = ringMatch->GetMCTrackID();
		CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(mcTrackId);
		if (!mcTrack) continue;
		Int_t motherId = mcTrack->GetMotherId();
		Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
		Double_t momentum = mcTrack->GetP();

		if (pdg != 11 || motherId != -1) continue;//only primary electrons



		Double_t axisA = ring->GetAaxis();
		Double_t axisB = ring->GetBaxis();
		Double_t xc = ring->GetCenterX();
		Double_t yc = ring->GetCenterY();
		Int_t nofHitsRing = ring->GetNofHits();

		if (nofHitsRing >= 5) fhAccEl->Fill(momentum);

		if (axisA > 10 || axisA < 1) continue;
		if (axisB > 10 || axisB < 1) continue;


		fhBoverAXYEl->Fill(xc, yc, axisB/axisA);
		fhBoverAEl->Fill(axisB/axisA);
		fhXYEl->Fill(xc, yc);
		fhAaxisEl->Fill(axisA);
		fhBaxisEl->Fill(axisB);
		fhNofHitsEl->Fill(nofHitsRing);
		fhBaxisVsMomEl->Fill(momentum, axisB );
		fhAaxisVsMomEl->Fill(momentum, axisA );
	}

}

void CbmRichGeoTest::EventPoints()
{
	Int_t oneEventNum = 2;
	Int_t nofPoints = fRichPoints->GetEntriesFast();
	for (Int_t iP = 0; iP < nofPoints; iP++){
		CbmRichPoint* point = (CbmRichPoint*)fRichPoints->At(iP);
		if (!point)continue;
		fhXYPoints->Fill(point->GetX(), point->GetY());
		if (oneEventNum == fEventNum) fhXYPointsOneEvent->Fill(point->GetX(), point->GetY());
	}

	Int_t nofHits = fRichHits->GetEntriesFast();
	for (Int_t iH = 0; iH < nofHits; iH++){
		CbmRichHit* hit = (CbmRichHit*)fRichHits->At(iH);
		if (!hit)continue;
		fhXYHits->Fill(hit->GetX(), hit->GetY());
		if (oneEventNum == fEventNum) fhXYHitsOneEvent->Fill(hit->GetX(), hit->GetY());
	}
	fhNofHitsEvent->Fill(nofHits);
	fhNofPointsEvent->Fill(nofPoints);
}

void CbmRichGeoTest::MCElectrons()
{
	Int_t nofMCTracks = fMCTracks->GetEntriesFast();
	for (Int_t iT = 0; iT < nofMCTracks; iT++){
		CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(iT);
		if (!mcTrack) continue;
		Int_t motherId = mcTrack->GetMotherId();
		Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
		if (pdg != 11 || motherId != -1) continue;//only primary electrons

		fhMCEl->Fill(mcTrack->GetP());
		fhNofPointsEl->Fill(mcTrack->GetNPoints(kRICH));
	}

}

// -----   Finish Task   ---------------------------------------------------
void CbmRichGeoTest::Finish()
{
	fhAaxisEl->Write();
	fhBaxisEl->Write();
	fhBoverAEl->Write();
   	fhBoverAXYEl->Write();
	fhXYEl->Write();
	fhXYHits->Write();
	fhXYPoints->Write();
	fhXYHitsOneEvent->Write();
	fhXYPointsOneEvent->Write();
	fhNofHitsEvent->Write();
	fhNofPointsEvent->Write();
	fhNofHitsEl->Write();
	fhMCEl->Write();
	fhAccEl->Write();
	fhBaxisVsMomEl->Write();
	fhAaxisVsMomEl->Write();
	fhNofPointsEl->Write();
}

// -------------------------------------------------------------------------

ClassImp(CbmRichGeoTest)

