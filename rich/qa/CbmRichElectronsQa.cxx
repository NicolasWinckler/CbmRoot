/** CbmRichElectronsQa.cxx
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2008
 * @version 2.0
 **/

#include "CbmRichElectronsQa.h"

#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"
#include "CbmTrackMatch.h"
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"


#include "TString.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include <map>
#include <cmath>

class CbmStsTrack;

using std::cout;
using std::endl;
using std::map;
using std::vector;
using std::fabs;

CbmRichElectronsQa::CbmRichElectronsQa(
      const char *name,
      const char *title,
      Int_t verbose)
  :FairTask(name)
{
	fEventNum = 1;

	fMinNofHitsInRichRing = 5;
	fQuota = 0.6;

	// DIFFERENCE BETWEEN ELECTRONS AND PIONS
	fhAaxisEl= new TH1D("fhAaxisEl", "fhAaxisEl;A axis, cm;Yield", 30,3,8);
	fhAaxisPi= new TH1D("fhAaxisPi", "fhAaxisPi;A axis, cm;Yield", 30,3,8);
	fhBAxisEl= new TH1D("fhBAxisEl", "fhBAxisEl;B axis, cm;Yield", 30,3,8);
	fhBAxisPi= new TH1D("fhBAxisPi", "fhBAxisPi;B axis, cm;Yield", 30,3,8);
	fhAaxisCorEl= new TH1D("fhAaxisCorEl", "fhAaxisCorEl;A axis, cm;Yield", 30,3,8);
	fhAaxisCorPi= new TH1D("fhAaxisCorPi", "fhAaxisCorPi;A axis, cm;Yield", 30,3,8);
	fhBAxisCorEl= new TH1D("fhBAxisCorEl", "fhBAxisCorEl;B axis, cm;Yield", 30,3,8);
	fhBAxisCorPi= new TH1D("fhBAxisCorPi", "fhBAxisCorPi;B axis, cm;Yield", 30,3,8);
	fhDistEl= new TH1D("fhDistEl", "fhDistEl;ring-track distance, cm;Yield", 30,0,3);
	fhDistPi= new TH1D("fhDistPi", "fhDistPi;ring-track distance, cm;Yield", 30,0,3);
	fhNofHitsEl= new TH1D("fhNofHitsEl", "fhNofHitsEl;nof hits;Yield", 30,0,45);
	fhNofHitsPi= new TH1D("fhNofHitsPi", "fhNofHitsPi;nof hits;Yield", 30,0,45);
	fhChi2El= new TH1D("fhChi2El", "fhChi2El;chi2;Yield", 30,0,1.);
	fhChi2Pi= new TH1D("fhChi2Pi", "fhChi2Pi;chi2;Yield", 30,0,1.);
	fhRadPosEl= new TH1D("fhRadPosEl", "fhRadPosEl;radial position, cm;Yield", 30,0,150);
	fhRadPosPi= new TH1D("fhRadPosPi", "fhRadPosPi;radial position, cm;Yield", 30,0,150);
	fhAaxisVsMomEl= new TH2D("fhAaxisVsMomEl", "fhAaxisVsMomEl;momentum, GeV/c;A axis, cm",30,0,15, 30,3,8);
	fhAaxisVsMomPi= new TH2D("fhAaxisVsMomPi", "fhAaxisVsMomPi;momentum, GeV/c;A axis, cm",30,0,15, 30,3,8);
	fhBAxisVsMomEl= new TH2D("fhBAxisVsMomEl", "fhBAxisVsMomEl;momentum, GeV/c;B axis, cm",30,0,15, 30,3,8);
	fhBAxisVsMomPi= new TH2D("fhBAxisVsMomPi", "fhBAxisVsMomPi;momentum, GeV/c;B axis, cm",30,0,15, 30,3,8);
	fhPhiVsRadAngEl= new TH2D("fhPhiVsRadAngEl", "fhPhiVsRadAngEl;phi, rad;rad angle, rad", 50, -2. ,2.,50, 0. , 6.3);
	fhPhiVsRadAngPi= new TH2D("fhPhiVsRadAngPi", "fhPhiVsRadAngPi;phi, rad;rad angle, rad", 50, -2. ,2.,50, 0. , 6.3);

	fOutElandPi.open("ann_el_pi.txt");
}

CbmRichElectronsQa::~CbmRichElectronsQa()
{

}

InitStatus CbmRichElectronsQa::Init()
{
	cout << "InitStatus CbmRichElectronsQa::Init()"<<endl;

	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmRichElectronsQa::Init","RootManager not instantised!");}

	fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
	if ( NULL == fRichHits) { Fatal("CbmRichElectronsQa::Init","No RichHit array!");}

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if ( NULL == fRichRings) { Fatal("CbmRichElectronsQa::Init","No RichRing array!");}

	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( NULL == fRichPoints) { Fatal("CbmRichElectronsQa::Init","No RichPoint array!");}

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMCTracks) { Fatal("CbmRichElectronsQa::Init","No MCTrack array!");}

	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if ( NULL == fRichRingMatches) { Fatal("CbmRichElectronsQa::Init","No RichRingMatch array!");}

	fRichProj = (TClonesArray*) ioman->GetObject("RichProjection");
	if ( NULL == fRichProj) { Fatal("CbmRichElectronsQa::Init","No RichProjection array!");}

	fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if ( NULL == fStsTrackMatches) { Fatal("CbmRichElectronsQa::Init","No track match array!");}

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if ( NULL == fGlobalTracks) { Fatal("CbmRichElectronsQa::Init","No global track array!");}

	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if ( NULL == fStsTracks) { Fatal("CbmRichElectronsQa::Init","No  STSTrack array!");}

	return kSUCCESS;
}

void CbmRichElectronsQa::Exec(Option_t* option)
{
   cout << endl <<"----CbmRichElectronsQa---- event # " << fEventNum << endl;
   DiffElandPi();
	fEventNum++;
}

void CbmRichElectronsQa::DiffElandPi()
{
	Int_t nGlTracks = fGlobalTracks->GetEntriesFast();

	for (Int_t iTrack=0; iTrack<nGlTracks; iTrack++) {
		CbmGlobalTrack* gTrack = (CbmGlobalTrack*)fGlobalTracks->At(iTrack);

		Int_t stsIndex = gTrack->GetStsTrackIndex();
		if (stsIndex == -1) continue;
		CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTracks->At(stsIndex);
		if (NULL == stsTrack) continue;
		CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)fStsTrackMatches->At(stsIndex);
		if (NULL == stsTrackMatch) continue;
		Int_t mcIdSts = stsTrackMatch->GetMCTrackId();

		Int_t richIndex = gTrack->GetRichRingIndex();
		if (richIndex == -1) continue;
		CbmRichRing* ring = (CbmRichRing*)fRichRings->At(richIndex);
		if (NULL == ring) continue;
		CbmRichRingMatch* richRingMatch = (CbmRichRingMatch*)fRichRingMatches->At(richIndex);
		if (NULL == richRingMatch) continue;
		Int_t mcIdRich = richRingMatch->GetMCTrackID();

		CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(mcIdSts);
      if (NULL == track) continue;
      Int_t pdg = TMath::Abs(track->GetPdgCode());
      Int_t motherId = track->GetMotherId();
      Double_t momentum = track->GetP();

      Double_t axisA = ring->GetAaxis();
      Double_t axisB = ring->GetBaxis();
      Double_t axisACor = ring->GetAaxisCor();
      Double_t axisBCor= ring->GetBaxisCor();
      Double_t phi = ring->GetPhi();
      Double_t radAngle = ring->GetRadialAngle();
      Double_t chi2 = ring->GetChi2()/ring->GetNDF();
      Double_t radPos = ring->GetRadialPosition();
      Double_t nofHits = ring->GetNofHits();
      Double_t dist = ring->GetDistance();

      if (TMath::IsNaN(axisA) || TMath::IsNaN(axisB)) continue;

      Int_t lFoundHits = richRingMatch->GetNofTrueHits() + richRingMatch->GetNofWrongHits() + richRingMatch->GetNofFakeHits();
      Double_t lPercTrue = 0;
      if (lFoundHits >= 3){
         lPercTrue = (Double_t)richRingMatch->GetNofTrueHits() / (Double_t)lFoundHits;
      }
      Bool_t isTrueFound = (lPercTrue > fQuota);

      // electrons
		if (pdg == 11 && motherId == -1 && isTrueFound &&
		      mcIdSts == mcIdRich && mcIdRich != -1){
			fhAaxisEl->Fill(axisA);
			fhBAxisEl->Fill(axisB);
			fhAaxisCorEl->Fill(axisACor);
			fhBAxisCorEl->Fill(axisBCor);
			fhDistEl->Fill(dist);
			fhNofHitsEl->Fill(nofHits);
			fhChi2El->Fill(chi2);
			fhRadPosEl->Fill(radPos);
			fhAaxisVsMomEl->Fill(momentum, axisA);
			fhBAxisVsMomEl->Fill(momentum, axisB);
			fhPhiVsRadAngEl->Fill(phi, radAngle);
			fOutElandPi << axisA << " " << axisB<< " " << phi <<
				" " << radAngle << " " << chi2 << " " << radPos <<
				" " << nofHits << " " << dist << " " << momentum << " " << 1 << endl;
		}

		// pions
		if ( pdg == 211 &&  mcIdRich != -1){
			fhAaxisPi->Fill(axisA);
			fhBAxisPi->Fill(axisB);
			fhAaxisCorPi->Fill(axisACor);
			fhBAxisCorPi->Fill(axisBCor);
			fhDistPi->Fill(dist);
			fhNofHitsPi->Fill(nofHits);
			fhChi2Pi->Fill(chi2);
			fhRadPosPi->Fill(radPos);
			fhAaxisVsMomPi->Fill(momentum, axisA);
			fhBAxisVsMomPi->Fill(momentum, axisB);
			fhPhiVsRadAngPi->Fill(phi, radAngle);
			fOutElandPi << axisA << " " << axisB<< " " << phi <<
				" " << radAngle << " " << chi2 << " " << radPos <<
				" " << nofHits << " " << dist << " " << momentum << " "<< -1 << endl ;
		}
	}// global tracks
}

void CbmRichElectronsQa::FinishTask()
{
// difference between electrons and pions
	fhAaxisEl->Scale(1./fhAaxisEl->Integral());
	fhAaxisPi->Scale(1./fhAaxisPi->Integral());
	fhBAxisEl->Scale(1./fhBAxisEl->Integral());
	fhBAxisPi->Scale(1./fhBAxisPi->Integral());
	fhAaxisCorEl->Scale(1./fhAaxisCorEl->Integral());
	fhAaxisCorPi->Scale(1./fhAaxisCorPi->Integral());
	fhBAxisCorEl->Scale(1./fhBAxisCorEl->Integral());
	fhBAxisCorPi->Scale(1./fhBAxisCorPi->Integral());
	fhDistEl->Scale(1./fhDistEl->Integral());
	fhDistPi->Scale(1./fhDistPi->Integral());
	fhNofHitsEl->Scale(1./fhNofHitsEl->Integral());
	fhNofHitsPi->Scale(1./fhNofHitsPi->Integral());
	fhChi2El->Scale(1./fhChi2El->Integral());
	fhChi2Pi->Scale(1./fhChi2Pi->Integral());
	fhRadPosEl->Scale(1./fhRadPosEl->Integral());
	fhRadPosPi->Scale(1./fhRadPosPi->Integral());
	fhAaxisVsMomEl->Scale(1./fhAaxisVsMomEl->Integral());
	fhAaxisVsMomPi->Scale(1./fhAaxisVsMomPi->Integral());
	fhBAxisVsMomEl->Scale(1./fhBAxisVsMomEl->Integral());
	fhBAxisVsMomPi->Scale(1./fhBAxisVsMomPi->Integral());
	fhPhiVsRadAngEl->Scale(1./fhPhiVsRadAngEl->Integral());
	fhPhiVsRadAngPi->Scale(1./fhPhiVsRadAngPi->Integral());

	fhAaxisEl->Write();
	fhAaxisPi->Write();
	fhBAxisEl->Write();
	fhBAxisPi->Write();
	fhAaxisCorEl->Write();
	fhAaxisCorPi->Write();
	fhBAxisCorEl->Write();
	fhBAxisCorPi->Write();
	fhDistEl->Write();
	fhDistPi->Write();
	fhNofHitsEl->Write();
	fhNofHitsPi->Write();
	fhChi2El->Write();
	fhChi2Pi->Write();
	fhRadPosEl->Write();
	fhRadPosPi->Write();
	fhAaxisVsMomEl->Write();
	fhAaxisVsMomPi->Write();
	fhBAxisVsMomEl->Write();
	fhBAxisVsMomPi->Write();
	fhPhiVsRadAngEl->Write();
	fhPhiVsRadAngPi->Write();
}

ClassImp(CbmRichElectronsQa)
