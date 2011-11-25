/**
 * \file CbmRichGeoTest.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

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
#include "cbm/utils/CbmLitDrawHist.h"

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
   fhBoverA.resize(nofTypes);
   fhXcYcEllipse.resize(nofTypes);
   fhXcYcCircle.resize(nofTypes);
   fhBaxisVsMom.resize(nofTypes);
   fhAaxisVsMom.resize(nofTypes);
   fhRadiusVsMom.resize(nofTypes);
	for (Int_t i = 0; i < nofTypes; i++){
	   stringstream ss;
	   if (i == 0) ss << "_Hits";
	   if (i == 1) ss << "_Points";
	   string t = ss.str();
      fhNofHits[i] = new TH1D(("fhNofHits"+t).c_str(), ("fhNofHits"+t+";Nof hits in ring;Yield").c_str(), 300, 0, 300);
      fHists.push_back(fhNofHits[i]);
      // ellipse fitting parameters
      fhBoverA[i] = new TH1D(("fhBoverA"+t).c_str(), ("fhBoverA"+t+";B/A;Yield").c_str(), 50, 0, 1);
      fHists.push_back(fhBoverA[i]);
      fhXcYcEllipse[i] = new TH2D(("fhXcYcEllipse"+t).c_str(), ("fhXcYcEllipse"+t+";X [cm];Y [cm];Yield").c_str(), 150, -150, 150, 250, -250, 250);
      fHists.push_back(fhXcYcEllipse[i]);
      fhBaxisVsMom[i] = new TH2D(("fhBaxisVsMom"+t).c_str(), ("fhBaxisVsMom"+t+";P [GeV/c];B axis [cm];Yield").c_str(), 100, 0., 12, 100, 0., 10.);
      fHists.push_back(fhBaxisVsMom[i]);
      fhAaxisVsMom[i] = new TH2D(("fhAaxisVsMom"+t).c_str(), ("fhAaxisVsMom"+t+";P [GeV/c];A axis [cm];Yield").c_str(), 100, 0., 12, 100, 0., 10.);
      fHists.push_back(fhAaxisVsMom[i]);
      // circle fitting parameters
      fhXcYcCircle[i] = new TH2D(("fhXcYcCircle"+t).c_str(), ("fhXcYcCircle"+t+";X [cm];Y [cm];Yield").c_str(), 150, -150, 150, 250, -250, 250);
      fHists.push_back(fhXcYcCircle[i]);
      fhRadiusVsMom[i] = new TH2D(("fhRadiusVsMom"+t).c_str(), ("fhRadiusVsMom"+t+";P [GeV/c];Radius [cm];Yield").c_str(), 100, 0., 12, 100, 0., 10.);
      fHists.push_back(fhRadiusVsMom[i]);
	}

   // Difference between Mc Points and Hits fit
   fhDiffAaxis = new TH2D("fhDiffAaxis", "fhDiffAaxis;Nof hits in ring;A_{point}-A_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffAaxis);
   fhDiffBaxis = new TH2D("fhDiffBaxis", "fhDiffBaxis;Nof hits in ring;B_{point}-B_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffBaxis);
   fhDiffXcEllipse = new TH2D("fhDiffXcEllipse", "fhDiffXcEllipse;Nof hits in ring;Xc_{point}-Xc_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffXcEllipse);
   fhDiffYcEllipse = new TH2D("fhDiffYcEllipse", "fhDiffYcEllipse;Nof hits in ring;Yc_{point}-Yc_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffYcEllipse);
   fhDiffXcCircle = new TH2D("fhDiffXcCircle", "fhDiffXcCircle;Nof hits in ring;Xc_{point}-Xc_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffXcCircle);
   fhDiffYcCircle = new TH2D("fhDiffYcCircle", "fhDiffYcCircle;Nof hits in ring;Yc_{point}-Yc_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffYcCircle);
   fhDiffRadius = new TH2D("fhDiffRadius", "fhDiffRadius;Nof hits in ring;Radius_{point}-Radius_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffRadius);

   // Hits and points
   fhDiffXhit = new TH1D("fhDiffXhit", "fhDiffXhit;Y_{point}-Y_{hit} [cm];Yield", 200, -1., 1.);
   fHists.push_back(fhDiffXhit);
   fhDiffYhit = new TH1D("fhDiffYhit", "fhDiffYhit;Y_{point}-Y_{hit} [cm];Yield", 200, -1., 1.);
   fHists.push_back(fhDiffYhit);

   // fitting efficiency
   fhNofHitsAll = new TH1D("fhNofHitsAll", "fhNofHitsAll;Nof hits in ring;Yield", 50, 0., 50.);
   fHists.push_back(fhNofHitsAll);
   fhNofHitsCircleFit = new TH1D("fhNofHitsCircleFit", "fhNofHitsCircleFit;Nof hits in ring;Yield", 50, 0., 50.);
   fHists.push_back(fhNofHitsCircleFit);
   fhNofHitsEllipseFit = new TH1D("fhNofHitsEllipseFit", "fhNofHitsEllipseFit;Nof hits in ring;Yield", 50, 0., 50.);
   fHists.push_back(fhNofHitsEllipseFit);
   fhNofHitsCircleFitEff = new TH1D("fhNofHitsCircleFitEff", "fhNofHitsCircleFitEff;Nof hits in ring;Efficiency [%]", 50, 0., 50.);
   fHists.push_back(fhNofHitsCircleFitEff);
   fhNofHitsEllipseFitEff = new TH1D("fhNofHitsEllipseFitEff", "fhNofHitsEllipseFitEff;Nof hits in ring;Efficiency [%]", 50, 0., 50.);
   fHists.push_back(fhNofHitsEllipseFitEff);
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
   fhRadiusVsMom[histIndex]->Fill(momentum, radius);
}

void CbmRichGeoTest::FillMcVsHitFitEllipse(
      CbmRichRing* ring,
      CbmRichRing* ringMc)
{
   fhDiffAaxis->Fill(ring->GetNofHits(), ringMc->GetAaxis() - ring->GetAaxis());
   fhDiffBaxis->Fill(ring->GetNofHits(),ringMc->GetBaxis() - ring->GetBaxis());
   fhDiffXcEllipse->Fill(ring->GetNofHits(),ringMc->GetCenterX() - ring->GetCenterX());
   fhDiffYcEllipse->Fill(ring->GetNofHits(),ringMc->GetCenterY() - ring->GetCenterY());
}

void CbmRichGeoTest::FillMcVsHitFitCircle(
      CbmRichRing* ring,
      CbmRichRing* ringMc)
{
   fhDiffXcCircle->Fill(ring->GetNofHits(),ringMc->GetCenterX() - ring->GetCenterX());
   fhDiffYcCircle->Fill(ring->GetNofHits(),ringMc->GetCenterY() - ring->GetCenterY());
   fhDiffRadius->Fill(ring->GetNofHits(),ringMc->GetRadius() - ring->GetRadius());
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
      if (i == 0) ss << "hits fitting";
      if (i == 1) ss << "points fitting";
      TCanvas *c = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1000, 750);
      c->Divide(4, 3);
      c->cd(1);
      DrawHist1D(fhBoverA[i]);
      gPad->SetLogy(true);
      c->cd(2);
      DrawHist2D(fhXcYcEllipse[i]);
      c->cd(3);
      DrawHist1D(fhAaxisVsMom[i]->ProjectionY());
      gPad->SetLogy(true);
      c->cd(4);
      DrawHist1D(fhBaxisVsMom[i]->ProjectionY());
      gPad->SetLogy(true);
      c->cd(5);
      DrawHist1D(fhNofHits[i]);
      gPad->SetLogy(true);
      c->cd(6);
      DrawHist2D(fhXcYcCircle[i]);
      c->cd(7);
      DrawHist1D(fhRadiusVsMom[i]->ProjectionY());
      gPad->SetLogy(true);
      c->cd(8);
      DrawHist2D(fhBaxisVsMom[i]);
      c->cd(9);
      DrawHist2D(fhAaxisVsMom[i]);
      c->cd(10);
      DrawHist2D(fhRadiusVsMom[i]);
   }

   TCanvas *cDiff2D = new TCanvas("cDiff2D", "cDiff2D", 1000, 500);
   cDiff2D->Divide(4,2);
   cDiff2D->cd(1);
   DrawHist2D(fhDiffAaxis);
   cDiff2D->cd(2);
   DrawHist2D(fhDiffBaxis);
   cDiff2D->cd(3);
   DrawHist2D(fhDiffXcEllipse);
   cDiff2D->cd(4);
   DrawHist2D(fhDiffYcEllipse);
   cDiff2D->cd(5);
   DrawHist2D(fhDiffXcCircle);
   cDiff2D->cd(6);
   DrawHist2D(fhDiffYcCircle);
   cDiff2D->cd(7);
   DrawHist2D(fhDiffRadius);

   TCanvas *cDiff1D = new TCanvas("cDiff1D", "cDiff1D", 1000, 500);
   cDiff1D->Divide(4,2);
   cDiff1D->cd(1);
   DrawHist1D(fhDiffAaxis->ProjectionY());
   gPad->SetLogy(true);
   cDiff1D->cd(2);
   DrawHist1D(fhDiffBaxis->ProjectionY());
   gPad->SetLogy(true);
   cDiff1D->cd(3);
   DrawHist1D(fhDiffXcEllipse->ProjectionY());
   gPad->SetLogy(true);
   cDiff1D->cd(4);
   DrawHist1D(fhDiffYcEllipse->ProjectionY());
   gPad->SetLogy(true);
   cDiff1D->cd(5);
   DrawHist1D(fhDiffXcCircle->ProjectionY());
   gPad->SetLogy(true);
   cDiff1D->cd(6);
   DrawHist1D(fhDiffYcCircle->ProjectionY());
   gPad->SetLogy(true);
   cDiff1D->cd(7);
   DrawHist1D(fhDiffRadius->ProjectionY());
   gPad->SetLogy(true);

   TCanvas *cHits = new TCanvas("cHits", "cHits", 1000, 500);
   cHits->Divide(2,1);
   cHits->cd(1);
   DrawHist1D(fhDiffXhit);
   cHits->cd(2);
   DrawHist1D(fhDiffYhit);

   TCanvas *cFitEff = new TCanvas("cFitEff", "cFitEff", 900, 600);
   cFitEff->Divide(3,2);
   cFitEff->cd(1);
   DrawHist1D(fhNofHitsAll);
   cFitEff->cd(2);
   DrawHist1D(fhNofHitsCircleFit);
   cFitEff->cd(3);
   DrawHist1D(fhNofHitsEllipseFit);
   cFitEff->cd(4);
   fhNofHitsCircleFitEff->Divide(fhNofHitsCircleFit, fhNofHitsAll);
   DrawHist1D(fhNofHitsCircleFitEff);
   cFitEff->cd(5);
   fhNofHitsEllipseFitEff->Divide(fhNofHitsEllipseFit, fhNofHitsAll);
   DrawHist1D(fhNofHitsEllipseFitEff);
}

void CbmRichGeoTest::Finish()
{
   Draw();
   for (Int_t i = 0; i < fHists.size(); i++){
      fHists[i]->Write();
   }
}

ClassImp(CbmRichGeoTest)

