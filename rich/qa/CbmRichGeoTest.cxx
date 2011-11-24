#include "CbmRichGeoTest.h"
#include "FairRootManager.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"
#include "CbmMCTrack.h"
#include "CbmRichPoint.h"
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingFitterEllipseTau.h"
#include "CbmGeoRichPar.h"
#include "FairGeoTransform.h"
#include "FairGeoNode.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmRichHitProducer.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

CbmRichGeoTest::CbmRichGeoTest()
   :FairTask("RichGeoTestQa")
{
	fEventNum = 0;

   fMinAaxis = 3.5;
   fMaxAaxis = 6.5;
   fMinBaxis = 3.1;
   fMaxBaxis = 6.1;
   fMinRadius = 3.7;
   fMaxRadius = 6.1;

	Int_t nofTypes = 2;
   fhNofHits.resize(nofTypes);
   fhAaxis.resize(nofTypes);
   fhBaxis.resize(nofTypes);
   fhBoverA.resize(nofTypes);
   fhXcYcEllipse.resize(nofTypes);
   fhXcYcCircle.resize(nofTypes);
   fhRadius.resize(nofTypes);
   fhBaxisVsMom.resize(nofTypes);
   fhAaxisVsMom.resize(nofTypes);
   fhRadiusVsMom.resize(nofTypes);
	for (Int_t i = 0; i < nofTypes; i++){
	   stringstream ss;
	   if (i == 0) ss << "_Hits";
	   if (i == 1) ss << "_Points";
	   string t = ss.str();
      fhNofHits[i] = new TH1D(("fhNofHits"+t).c_str(), ("fhNofHits"+t).c_str(), 300, 0, 300);
      fHists.push_back(fhNofHits[i]);
      // ellipse fitting parameters
      fhAaxis[i] = new TH1D(("fhAaxis"+t).c_str(), ("fhAaxis"+t).c_str(), 50, 0, 10);
      fHists.push_back(fhAaxis[i]);
      fhBaxis[i] = new TH1D(("fhBaxis"+t).c_str(), ("fhBaxis"+t).c_str(), 50, 0, 10);
      fHists.push_back(fhBaxis[i]);
      fhBoverA[i] = new TH1D(("fhBoverA"+t).c_str(), ("fhBoverA"+t).c_str(), 50, 0, 1);
      fHists.push_back(fhBoverA[i]);
      fhXcYcEllipse[i] = new TH2D(("fhXcYcEllipse"+t).c_str(), ("fhXcYcEllipse"+t).c_str(), 150, -150, 150, 250, -250, 250);
      fHists.push_back(fhXcYcEllipse[i]);
      // circle fitting parameters
      fhXcYcCircle[i] = new TH2D(("fhXcYcCircle"+t).c_str(), ("fhXcYcCircle"+t).c_str(), 150, -150, 150, 250, -250, 250);
      fHists.push_back(fhXcYcCircle[i]);
      fhRadius[i] = new TH1D(("fhRadius"+t).c_str(), ("fhRadius"+t).c_str(), 50, 0, 10);
      fHists.push_back(fhRadius[i]);
      // A, B, radius vs. MC momentum
      fhBaxisVsMom[i] = new TH2D(("fhBaxisVsMom"+t).c_str(), ("fhBaxisVsMom"+t).c_str(), 100, 0., 12, 100, 0., 10.);
      fHists.push_back(fhBaxisVsMom[i]);
      fhAaxisVsMom[i] = new TH2D(("fhAaxisVsMom"+t).c_str(), ("fhAaxisVsMom"+t).c_str(), 100, 0., 12, 100, 0., 10.);
      fHists.push_back(fhAaxisVsMom[i]);
      fhRadiusVsMom[i] = new TH2D(("fhRadiusVsMom"+t).c_str(), ("fhRadiusVsMom"+t).c_str(), 100, 0., 12, 100, 0., 10.);
      fHists.push_back(fhRadiusVsMom[i]);
	}

   // Difference between Mc Points and Hits fit
   fhDiffAaxis = new TH1D("fhDiffAaxis", "fhDiffAaxis", 300, -1.5, 1.5);
   fHists.push_back(fhDiffAaxis);
   fhDiffBaxis = new TH1D("fhDiffBaxis", "fhDiffBaxis", 300, -1.5, 1.5);
   fHists.push_back(fhDiffBaxis);
   fhDiffXcEllipse = new TH1D("fhDiffXcEllipse", "fhDiffXcEllipse", 300, -1.5, 1.5);
   fHists.push_back(fhDiffXcEllipse);
   fhDiffYcEllipse = new TH1D("fhDiffXyEllipse", "fhDiffXyEllipse", 300, -1.5, 1.5);
   fHists.push_back(fhDiffYcEllipse);
   fhDiffXcCircle = new TH1D("fhDiffXcCircle", "fhDiffXcCircle", 300, -1.5, 1.5);
   fHists.push_back(fhDiffXcCircle);
   fhDiffYcCircle = new TH1D("fhDiffYcCircle", "fhDiffYcCircle", 300, -1.5, 1.5);
   fHists.push_back(fhDiffYcCircle);
   fhDiffRadius = new TH1D("fhDiffRadius", "fhDiffRadius", 300, -1.5, 1.5);
   fHists.push_back(fhDiffRadius);

   // Hits and points
   fhDiffXhit = new TH1D("fhDiffXhit", "fhDiffXhit", 200, -1., 1.);
   fHists.push_back(fhDiffXhit);
   fhDiffYhit = new TH1D("fhDiffYhit", "fhDiffYhit", 200, -1., 1.);
   fHists.push_back(fhDiffYhit);

   // fitting efficiency
   fhNofHitsAll = new TH1D("fhNofHitsAll", "fhNofHitsAll", 50, 0., 50.);
   fHists.push_back(fhNofHitsAll);
   fhNofHitsCircleFit = new TH1D("fhNofHitsCircleFit", "fhNofHitsCircleFit", 50, 0., 50.);
   fHists.push_back(fhNofHitsCircleFit);
   fhNofHitsEllipseFit = new TH1D("fhNofHitsEllipseFit", "fhNofHitsEllipseFit", 50, 0., 50.);
   fHists.push_back(fhNofHitsEllipseFit);
   fhNofHitsCircleFitEff = new TH1D("fhNofHitsCircleFitEff", "fhNofHitsCircleFitEff", 50, 0., 50.);
   fHists.push_back(fhNofHitsCircleFitEff);
   fhNofHitsEllipseFitEff = new TH1D("fhNofHitsEllipseFitEff", "fhNofHitsEllipseFitEff", 50, 0., 50.);
   fHists.push_back(fhNofHitsEllipseFitEff);
}

CbmRichGeoTest::CbmRichGeoTest(
      const char *name,
      const char *title,
      Int_t verbose)
     :FairTask(name)
{

}

CbmRichGeoTest::~CbmRichGeoTest()
{

}

void CbmRichGeoTest::SetParContainers()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fPar=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));
}

InitStatus CbmRichGeoTest::Init()
{
   cout << "CbmRichGeoTest::Init"<<endl;
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmRichGeoTest::Init","RootManager not instantised!"); }

   fSensNodes = fPar->GetGeoSensitiveNodes();
   fPassNodes = fPar->GetGeoPassiveNodes();

   // get detector position:
   FairGeoNode *det= dynamic_cast<FairGeoNode*> (fSensNodes->FindObject("rich1d#1"));
   if (NULL == det) cout << " -I no RICH Geo Node  found !!!!!  " << endl;

   FairGeoTransform* detTr=det->getLabTransform(); // detector position in labsystem
   FairGeoVector detPosLab=detTr->getTranslation(); // ... in cm
   FairGeoTransform detCen=det->getCenterPosition(); // center in Detector system
   FairGeoVector detPosCen=detCen.getTranslation();

   fDetZOrig = detPosLab.Z() + detPosCen.Z(); // z coordinate of photodetector (Labsystem, cm)
   FairGeoRotation fdetR=detTr->getRotMatrix();
   fTheta = TMath::ASin(fdetR(7)); // tilting angle around x-axis
   fPhi = -1.*TMath::ASin(fdetR(2)); // tilting angle around y-axis

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   if ( !fRichHits) { Fatal("CbmRichGeoTest::Init","No RichHit array!"); }

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if ( NULL == fRichRings) { Fatal("CbmRichGeoTest::Init","No RichRing array!"); }

	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( NULL == fRichPoints) { Fatal("CbmRichGeoTest::Init","No RichPoint array!"); }

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMCTracks) { Fatal("CbmRichGeoTest::Init","No MCTrack array!"); }

	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if ( NULL == fRichRingMatches) { Fatal("CbmRichGeoTest::Init","No RichRingMatch array!"); }

   fCopFit = new CbmRichRingFitterCOP();
   fCopFit->Init();

   fTauFit = new CbmRichRingFitterEllipseTau();
   fTauFit->Init();

	return kSUCCESS;
}

void CbmRichGeoTest::Exec(
      Option_t* option)
{
	fEventNum++;
	cout << "CbmRichGeoTest, event No. " <<  fEventNum << endl;
	RingParameters();
	HitsAndPoints();
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

		if (pdg != 11 || motherId != -1) continue; // only primary electrons

		vector<double> xMc, yMc;
		Int_t nofRichPoints = fRichPoints->GetEntriesFast();
		for (Int_t iPoint = 0; iPoint < nofRichPoints; iPoint++){
		   CbmRichPoint* richPoint = (CbmRichPoint*) fRichPoints->At(iPoint);
		   if (richPoint == NULL) continue;
	      CbmMCTrack* mcTrackRich = (CbmMCTrack*)fMCTracks->At(richPoint->GetTrackID());
	      if (!mcTrackRich) continue;
	      Int_t motherIdRich = mcTrackRich->GetMotherId();
		   if (motherIdRich == mcTrackId){
		      TVector3 posPoint;
		      richPoint->Position(posPoint);
		      TVector3 detPoint;
		      CbmRichHitProducer::TiltPoint(&posPoint, &detPoint, fPhi, fTheta, fDetZOrig);
		      xMc.push_back(detPoint.X());
		      yMc.push_back(detPoint.Y());
		   }
		}
	   fhNofHitsAll->Fill(ring->GetNofHits());

      vector<Double_t> x, y;
		CbmRichRing* ringMc = new CbmRichRing();

      FitAndFillHistCircle(0, ring, momentum, x, y); //hits
      FitAndFillHistCircle(1, ringMc, momentum, xMc, yMc); // points
      FillMcVsHitFitCircle(ring, ringMc);
      if (ring->GetRadius() > fMinRadius && ring->GetRadius() < fMaxRadius){
         fhNofHitsCircleFit->Fill(ring->GetNofHits());
      }

		FitAndFillHistEllipse(0, ring, momentum, x, y);// hits
		FitAndFillHistEllipse(1, ringMc, momentum, xMc, yMc); // points
		FillMcVsHitFitEllipse(ring, ringMc);
	   if (ring->GetAaxis() > fMinAaxis && ring->GetAaxis() < fMaxAaxis
	       &&  ring->GetBaxis() > fMinBaxis && ring->GetAaxis() < fMaxBaxis ){
	      fhNofHitsEllipseFit->Fill(ring->GetNofHits());
	   }
		delete ringMc;
	}// iR
}

void CbmRichGeoTest::FitAndFillHistEllipse(
      Int_t histIndex,
      CbmRichRing* ring,
      Double_t momentum,
      const vector<Double_t>& x,
      const vector<Double_t>& y)
{
   if (x.size() != 0){ fTauFit->DoFit1(ring, x, y);}
   else {fTauFit->DoFit(ring);}
   Double_t axisA = ring->GetAaxis();
   Double_t axisB = ring->GetBaxis();
   Double_t xcEllipse = ring->GetCenterX();
   Double_t ycEllipse = ring->GetCenterY();
   Int_t nofHitsRing = ring->GetNofHits();
   if (x.size() != 0) nofHitsRing = x.size();
   fhBoverA[histIndex]->Fill(axisB/axisA);
   fhXcYcEllipse[histIndex]->Fill(xcEllipse, ycEllipse);
   fhAaxis[histIndex]->Fill(axisA);
   fhBaxis[histIndex]->Fill(axisB);
   fhNofHits[histIndex]->Fill(nofHitsRing);
   fhBaxisVsMom[histIndex]->Fill(momentum, axisB);
   fhAaxisVsMom[histIndex]->Fill(momentum, axisA);
}

void CbmRichGeoTest::FitAndFillHistCircle(
      Int_t histIndex,
      CbmRichRing* ring,
      Double_t momentum,
      const vector<Double_t>& x,
      const vector<Double_t>& y)
{
   if (x.size() != 0){ fCopFit->DoFit1(ring, x, y);}
   else {fCopFit->DoFit(ring);}
   Double_t radius = ring->GetRadius();
   Double_t xcCircle = ring->GetCenterX();
   Double_t ycCircle = ring->GetCenterY();
   fhXcYcCircle[histIndex]->Fill(xcCircle, ycCircle);
   fhRadius[histIndex]->Fill(radius);
   fhRadiusVsMom[histIndex]->Fill(momentum, radius);
}

void CbmRichGeoTest::FillMcVsHitFitEllipse(
      CbmRichRing* ring,
      CbmRichRing* ringMc)
{
   fhDiffAaxis->Fill(ringMc->GetAaxis() - ring->GetAaxis());
   fhDiffBaxis->Fill(ringMc->GetBaxis() - ring->GetBaxis());
   fhDiffXcEllipse->Fill(ringMc->GetCenterX() - ring->GetCenterX());
   fhDiffYcEllipse->Fill(ringMc->GetCenterY() - ring->GetCenterY());
}

void CbmRichGeoTest::FillMcVsHitFitCircle(
      CbmRichRing* ring,
      CbmRichRing* ringMc)
{
   fhDiffXcCircle->Fill(ringMc->GetCenterX() - ring->GetCenterX());
   fhDiffYcCircle->Fill(ringMc->GetCenterY() - ring->GetCenterY());
   fhDiffRadius->Fill(ringMc->GetRadius() - ring->GetRadius());
}

void CbmRichGeoTest::HitsAndPoints()
{
   Int_t nofHits = fRichHits->GetEntriesFast();
   for (Int_t iH = 0; iH < nofHits; iH++){
      CbmRichHit *hit = (CbmRichHit*) fRichHits->At(iH);
      if ( hit == NULL ) continue;
      Int_t pointInd =  hit->GetRefId();
      if (pointInd < 0) continue;
      CbmRichPoint *point = (CbmRichPoint*) fRichPoints->At(pointInd);
      if ( point == NULL ) continue;
      TVector3 inPos(point->GetX(), point->GetY(), point->GetZ());
      TVector3 outPos;
      CbmRichHitProducer::TiltPoint(&inPos, &outPos, fPhi, fTheta, fDetZOrig);
      fhDiffXhit->Fill(hit->GetX() - outPos.X());
      fhDiffYhit->Fill(hit->GetY() - outPos.Y());
   }
}


void CbmRichGeoTest::Draw()
{
   for (int i = 0; i < 2; i++){
      stringstream ss;
      ss << "canvas" << i ;
      TCanvas *c = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1000, 750);
      c->Divide(4, 3);
      c->cd(1);
      fhBoverA[i]->Draw();
      c->cd(2);
      fhXcYcEllipse[i]->Draw("COLZ");
      c->cd(3);
      fhAaxis[i]->Draw();
      c->cd(4);
      fhBaxis[i]->Draw();
      c->cd(5);
      fhNofHits[i]->Draw();
      c->cd(6);
      fhXcYcCircle[i]->Draw("COLZ");
      c->cd(7);
      fhRadius[i]->Draw();
      c->cd(8);
      fhBaxisVsMom[i]->Draw("COLZ");
      c->cd(9);
      fhAaxisVsMom[i]->Draw("COLZ");
      c->cd(10);
      fhRadiusVsMom[i]->Draw("COLZ");
   }

   TCanvas *cDiff = new TCanvas("cDiff", "cDiff", 1000, 500);
   cDiff->Divide(4,2);
   cDiff->cd(1);
   fhDiffAaxis->Draw();
   cDiff->cd(2);
   fhDiffBaxis->Draw();
   cDiff->cd(3);
   fhDiffXcEllipse->Draw();
   cDiff->cd(4);
   fhDiffYcEllipse->Draw();
   cDiff->cd(5);
   fhDiffXcCircle->Draw();
   cDiff->cd(6);
   fhDiffYcCircle->Draw();
   cDiff->cd(7);
   fhDiffRadius->Draw();

   TCanvas *cHits = new TCanvas("cHits", "cHits", 1000, 500);
   cHits->Divide(2,1);
   cHits->cd(1);
   fhDiffXhit->Draw();
   cHits->cd(2);
   fhDiffYhit->Draw();

   TCanvas *cFitEff = new TCanvas("cFitEff", "cFitEff", 900, 600);
   cFitEff->Divide(3,2);
   cFitEff->cd(1);
   fhNofHitsAll->Draw();
   cFitEff->cd(2);
   fhNofHitsCircleFit->Draw();
   cFitEff->cd(3);
   fhNofHitsEllipseFit->Draw();
   cFitEff->cd(4);
   fhNofHitsCircleFitEff->Divide(fhNofHitsCircleFit, fhNofHitsAll);
   fhNofHitsCircleFitEff->Draw();
   cFitEff->cd(5);
   fhNofHitsEllipseFitEff->Divide(fhNofHitsEllipseFit, fhNofHitsAll);
   fhNofHitsEllipseFitEff->Draw();
}

void CbmRichGeoTest::Finish()
{
   Draw();
   for (Int_t i = 0; i < fHists.size(); i++){
      fHists[i]->Write();
   }
}

ClassImp(CbmRichGeoTest)

