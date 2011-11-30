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
#include "TH3D.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TPad.h"
#include "TLatex.h"

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

using namespace std;

CbmRichGeoTest::CbmRichGeoTest()
   :FairTask("RichGeoTestQa")
{
	fEventNum = 0;
	fNofDrawnRings = 0;
	fMinNofHits = 7;

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

   // Detector acceptance efficiency vs. pt, y, p
   fhMc3D = new TH3D("fhMc3D", "fhMc3D;P [GeV/c];P_{t} [GeV/c];Rapidity", 100, 0., 10., 30., 0., 3., 40., 0., 4.);
   fHists.push_back(fhMc3D);
   fhAcc3D = new TH3D("fhAcc3D", "fhAcc3D;P [GeV/c];P_{t} [GeV/c];Rapidity", 100, 0., 10., 30., 0., 3., 40., 0., 4.);;
   fHists.push_back(fhAcc3D);

   // numbers in dependence on XY position onto the photodetector
   fhNofHitsXY = new TH2D("fhNofHitsXY", "fhNofHitsXY;X [cm];Y [cm];Yield", 50, -200., 200., 50, -200, 200);
   fHists.push_back(fhNofHitsXY);
   fhNofPointsXY = new TH2D("fhNofPointsXY", "fhNofPointsXY;X [cm];Y [cm];Yield", 50, -200., 200., 50, -200, 200);
   fHists.push_back(fhNofPointsXY);
   fhBoverAXY = new TH2D("fhBoverAXY", "fhBoverAXY;X [cm];Y [cm];Yield", 50, -200., 200., 50, -200, 200);
   fHists.push_back(fhBoverAXY);
   fhBaxisXY = new TH2D("fhBaxisXY", "fhBaxisXY;X [cm];Y [cm];Yield", 50, -200., 200., 50, -200, 200);
   fHists.push_back(fhBaxisXY);
   fhAaxisXY = new TH2D("fhAaxisXY", "fhAaxisXY;X [cm];Y [cm];Yield", 50, -200., 200., 50, -200, 200);
   fHists.push_back(fhAaxisXY);
   fhRadiusXY = new TH2D("fhRadiusXY", "fhRadiusXY;X [cm];Y [cm];Yield", 50, -200., 200., 50, -200, 200);
   fHists.push_back(fhRadiusXY);
   fhCounterXY = new TH2D("fhCounterXY", "fhCounterXY;X [cm];Y [cm];Yield", 50, -200., 200., 50, -200, 200);
   fHists.push_back(fhCounterXY);
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
	FillMcHist();
	RingParameters();
	HitsAndPoints();
}

void CbmRichGeoTest::FillMcHist()
{
   for (Int_t i = 0; i < fMCTracks->GetEntriesFast(); i++){
      CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(i);
      if (!mcTrack) continue;
      Int_t motherId = mcTrack->GetMotherId();
      Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());

      if (pdg != 11 || motherId != -1) continue; // only primary electrons

      fhMc3D->Fill(mcTrack->GetP(), mcTrack->GetPt(), mcTrack->GetRapidity());
   }
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
      Double_t pt = mcTrack->GetPt();
      Double_t rapidity = mcTrack->GetRapidity();

		if (pdg != 11 || motherId != -1) continue; // only primary electrons
      cout << "Nof hits = " << ring->GetNofHits() << endl;
		if (ring->GetNofHits() >= fMinNofHits){
		   cout << "nofHits here " << endl;
		   fhAcc3D->Fill(momentum, pt, rapidity);
		}

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

      Double_t r = ringMc->GetRadius();

      if (ring->GetRadius() > fMinRadius && ring->GetRadius() < fMaxRadius){
         fhNofHitsCircleFit->Fill(ring->GetNofHits());
      }
	   if (fNofDrawnRings < 10 && ring->GetNofHits() <= 7 && ring->GetNofHits() >= 7){
	      DrawRing(ring, xMc, yMc);
	   }

		FitAndFillHistEllipse(0, ring, momentum, x, y);// hits
		FitAndFillHistEllipse(1, ringMc, momentum, xMc, yMc); // points
		FillMcVsHitFitEllipse(ring, ringMc);
	   if (ring->GetAaxis() > fMinAaxis && ring->GetAaxis() < fMaxAaxis
	       &&  ring->GetBaxis() > fMinBaxis && ring->GetAaxis() < fMaxBaxis ){
	      fhNofHitsEllipseFit->Fill(ring->GetNofHits());
	   }
	   Double_t np = xMc.size();
      Double_t xc = ringMc->GetCenterX();
      Double_t yc = ringMc->GetCenterY();
      Double_t a = ringMc->GetAaxis();
      Double_t b = ringMc->GetBaxis();
      Double_t nh = ring->GetNofHits();

	   fhNofHitsXY->Fill(xc, yc, nh);
	   fhNofPointsXY->Fill(xc, yc, np);
	   fhBoverAXY->Fill(xc, yc, a/b);
	   fhBaxisXY->Fill(xc, yc, b);
	   fhAaxisXY->Fill(xc, yc, a);
	   fhRadiusXY->Fill(xc, yc, r);
	   fhCounterXY->Fill(xc, yc);

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

void CbmRichGeoTest::DrawRing(
      CbmRichRing* ring,
      const vector<Double_t>& x,
      const vector<Double_t>& y)
{
   stringstream ss;
   ss << "Event" << fNofDrawnRings;
   fNofDrawnRings++;
   TCanvas *c = new TCanvas(ss.str().c_str(), ss.str().c_str(), 500, 500);
   c->SetGrid(true, true);
   TH2D* pad = new TH2D("pad", ";X [cm];Y [cm]", 1, -10., 10., 1, -10., 10);
   pad->SetStats(false);
   pad->Draw();

   // find min and max x and y positions of the hits
   // in order to shift drawing
   double xmin = 99999., xmax = -99999., ymin = 99999., ymax = -99999.;
   for (int i = 0; i < ring->GetNofHits(); i++){
      CbmRichHit* hit = (CbmRichHit*)fRichHits->At(ring->GetHit(i));
      double hitX = hit->GetX();
      double hitY = hit->GetY();
      if (xmin > hitX) xmin = hitX;
      if (xmax < hitX) xmax = hitX;
      if (ymin > hitY) ymin = hitY;
      if (ymax < hitY) ymax = hitY;
   }
   double xCur = (xmin + xmax) / 2.;
   double yCur = (ymin + ymax) / 2.;

   //Draw circle and center
   TEllipse* circle = new TEllipse(ring->GetCenterX() - xCur,
         ring->GetCenterY() - yCur, ring->GetRadius());
   circle->SetFillStyle(0);
   circle->SetLineWidth(3);
   circle->Draw();
   TEllipse* center = new TEllipse(ring->GetCenterX() - xCur, ring->GetCenterY() - yCur, 0.25);
   center->Draw();

   // Draw hits
   for (int i = 0; i < ring->GetNofHits(); i++){
      CbmRichHit* hit = (CbmRichHit*)fRichHits->At(ring->GetHit(i));
      TEllipse* hitDr = new TEllipse(hit->GetX() - xCur, hit->GetY() - yCur, 0.25);
      hitDr->SetFillColor(kRed);
      hitDr->Draw();
   }

   // Draw MC Points
   for (int i = 0; i < x.size(); i++){
      TEllipse* pointDr = new TEllipse(x[i] - xCur, y[i] - yCur, 0.1);
      pointDr->SetFillColor(kBlue);
      pointDr->Draw();
   }

   //Draw information
   stringstream ss2;
   ss2 << "(r, n)=(" << setprecision(3) << ring->GetRadius() << ", " << ring->GetNofHits()<<")";
   TLatex* latex = new TLatex(-8., 8., ss2.str().c_str());
   latex->Draw();
}

void CbmRichGeoTest::DrawHist()
{
   //SetStyles();
   for (int i = 0; i < 2; i++){
      stringstream ss;
      if (i == 0) ss << "rich_geo_hits_fit";
      if (i == 1) ss << "rich_geo_points_fit";
      TCanvas *cEllipse = new TCanvas((ss.str()+"_ellipse").c_str(), (ss.str()+"_ellipse").c_str(), 900, 600);
      cEllipse->Divide(3, 2);
      cEllipse->cd(1);
      DrawHist1D(fhBoverA[i]);
      gPad->SetLogy(true);
      cEllipse->cd(2);
      DrawHist2D(fhXcYcEllipse[i]);
      cEllipse->cd(3);
      DrawHist1D(fhAaxisVsMom[i]->ProjectionY());
      gPad->SetLogy(true);
      cEllipse->cd(4);
      DrawHist1D(fhBaxisVsMom[i]->ProjectionY());
      gPad->SetLogy(true);
      cEllipse->cd(5);
      DrawHist2D(fhBaxisVsMom[i]);
      cEllipse->cd(6);
      DrawHist2D(fhAaxisVsMom[i]);

      TCanvas *cCircle = new TCanvas((ss.str()+"_circle").c_str(), (ss.str()+"_circle").c_str(), 600, 600);
      cCircle->Divide(2,2);
      cCircle->cd(1);
      DrawHist1D(fhNofHits[i]);
      gPad->SetLogy(true);
      cCircle->cd(2);
      DrawHist2D(fhXcYcCircle[i]);
      cCircle->cd(3);
      DrawHist1D(fhRadiusVsMom[i]->ProjectionY());
      gPad->SetLogy(true);
      cCircle->cd(4);
      DrawHist2D(fhRadiusVsMom[i]);
   }

   TCanvas *cDiff2DEllipse = new TCanvas("rich_geo_diff2d_ellipse", "rich_geo_diff2d_ellipse", 600, 600);
   cDiff2DEllipse->Divide(2,2);
   cDiff2DEllipse->cd(1);
   DrawHist2D(fhDiffAaxis);
   cDiff2DEllipse->cd(2);
   DrawHist2D(fhDiffBaxis);
   cDiff2DEllipse->cd(3);
   DrawHist2D(fhDiffXcEllipse);
   cDiff2DEllipse->cd(4);
   DrawHist2D(fhDiffYcEllipse);

   TCanvas *cDiff2DCircle = new TCanvas("rich_geo_diff2d_circle", "rich_geo_diff2d_circle", 900, 300);
   cDiff2DCircle->Divide(3,1);
   cDiff2DCircle->cd(1);
   DrawHist2D(fhDiffXcCircle);
   cDiff2DCircle->cd(2);
   DrawHist2D(fhDiffYcCircle);
   cDiff2DCircle->cd(3);
   DrawHist2D(fhDiffRadius);

   TCanvas *cDiff1DEllipse = new TCanvas("rich_geo_diff1d_ellipse", "rich_geo_diff1d_ellipse", 600, 600);
   cDiff1DEllipse->Divide(2,2);
   cDiff1DEllipse->cd(1);
   DrawHist1D(fhDiffAaxis->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DEllipse->cd(2);
   DrawHist1D(fhDiffBaxis->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DEllipse->cd(3);
   DrawHist1D(fhDiffXcEllipse->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DEllipse->cd(4);
   DrawHist1D(fhDiffYcEllipse->ProjectionY());
   gPad->SetLogy(true);

   TCanvas *cDiff1DCircle = new TCanvas("rich_geo_diff1d_circle", "rich_geo_diff1d_circle", 900, 300);
   cDiff1DCircle->Divide(3,1);
   cDiff1DCircle->cd(1);
   DrawHist1D(fhDiffXcCircle->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DCircle->cd(2);
   DrawHist1D(fhDiffYcCircle->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DCircle->cd(3);
   DrawHist1D(fhDiffRadius->ProjectionY());
   gPad->SetLogy(true);

   TCanvas *cHits = new TCanvas("rich_geo_hits", "rich_geo_hits", 600, 300);
   cHits->Divide(2,1);
   cHits->cd(1);
   DrawHist1D(fhDiffXhit);
   cHits->cd(2);
   DrawHist1D(fhDiffYhit);

   TCanvas *cFitEff = new TCanvas("rich_geo_fit_eff", "rich_geo_fit_eff", 900, 300);
   cFitEff->Divide(3,1);
   cFitEff->cd(1);
   DrawHist1D( (TH1D*)fhNofHitsAll->Clone(), (TH1D*)fhNofHitsCircleFit->Clone(), (TH1D*)fhNofHitsEllipseFit->Clone(),
         NULL, "All", "Circle fit", "Ellipse fit", "", kLitLinear, kLitLog, true, 0.7, 0.7, 0.99, 0.99);
   fhNofHitsCircleFitEff = Divide1DHists(fhNofHitsCircleFit, fhNofHitsAll, "", "", "Nof hits in ring", "Efficiency");
   fhNofHitsEllipseFitEff = Divide1DHists(fhNofHitsEllipseFit, fhNofHitsAll, "", "", "Nof hits in ring", "Efficiency");
   cFitEff->cd(2);
   DrawHist1D(fhNofHitsCircleFitEff);
   cFitEff->cd(3);
   DrawHist1D(fhNofHitsEllipseFitEff);

   TCanvas *cAccEff = new TCanvas("rich_geo_acc_eff", "rich_geo_acc_eff", 900, 600);
   cAccEff->Divide(3,2);
   TH1D* pxMc = fhMc3D->ProjectionX();
   pxMc->SetName((string(fhMc3D->GetName())+"_px").c_str());
   TH1D* pyMc = fhMc3D->ProjectionY();
   pyMc->SetName((string(fhMc3D->GetName())+"_py").c_str());
   TH1D* pzMc = fhMc3D->ProjectionZ();
   pzMc->SetName((string(fhMc3D->GetName())+"_pz").c_str());
   TH1D* pxAcc = fhAcc3D->ProjectionX();
   pxAcc->SetName((string(fhAcc3D->GetName())+"_px").c_str());
   TH1D* pyAcc = fhAcc3D->ProjectionY();
   pyAcc->SetName((string(fhAcc3D->GetName())+"_py").c_str());
   TH1D* pzAcc = fhAcc3D->ProjectionZ();
   pzAcc->SetName((string(fhAcc3D->GetName())+"_pz").c_str());
   cAccEff->cd(1);
   DrawHist1D((TH1D*)pxMc->Clone(), (TH1D*)pxAcc->Clone(), NULL, NULL, "MC", "ACC", "", "", kLitLinear, kLitLog, true, 0.7, 0.7, 0.99, 0.99);
   cAccEff->cd(2);
   DrawHist1D((TH1D*)pyMc->Clone(), (TH1D*)pyAcc->Clone(), NULL, NULL, "MC", "ACC", "", "", kLitLinear, kLitLog, true, 0.7, 0.7, 0.99, 0.99);
   cAccEff->cd(3);
   DrawHist1D((TH1D*)pzMc->Clone(), (TH1D*)pzAcc->Clone(), NULL, NULL, "MC", "ACC", "", "", kLitLinear, kLitLog, true, 0.7, 0.7, 0.99, 0.99);

   TH1D* pxEff = Divide1DHists(pxAcc, pxMc, "pxEff", "", "P [GeV/c]", "Efficiency");
   TH1D* pyEff = Divide1DHists(pyAcc, pyMc, "pyEff", "", "P_{t} [GeV/c]", "Efficiency");
   TH1D* pzEff = Divide1DHists(pzAcc, pzMc, "pzEff", "", "Rapidity", "Efficiency");
   cAccEff->cd(4);
   DrawHist1D(pxEff);
   cAccEff->cd(5);
   DrawHist1D(pyEff);
   cAccEff->cd(6);
   DrawHist1D(pzEff);

   TCanvas *cNumbersVsXY = new TCanvas("rich_geo_numbers_vs_xy", "rich_geo_numbers_vs_xy", 900, 600);
   cNumbersVsXY->Divide(3,2);
   cNumbersVsXY->cd(1);
   fhNofHitsXY->Divide(fhCounterXY);
   fhNofHitsXY->Draw("COLZ");
   cNumbersVsXY->cd(2);
   fhNofPointsXY->Divide(fhCounterXY);
   fhNofPointsXY->Draw("COLZ");
   cNumbersVsXY->cd(3);
   fhBoverAXY->Divide(fhCounterXY);
   fhBoverAXY->Draw("COLZ");
   cNumbersVsXY->cd(4);
   fhBaxisXY->Divide(fhCounterXY);
   fhBaxisXY->Draw("COLZ");
   cNumbersVsXY->cd(5);
   fhAaxisXY->Divide(fhCounterXY);
   fhAaxisXY->Draw("COLZ");
   cNumbersVsXY->cd(6);
   fhRadiusXY->Divide(fhCounterXY);
   fhRadiusXY->Draw("COLZ");
}

void CbmRichGeoTest::PrintStatisctics()
{
   cout << endl;
   cout << "-I- CbmRichGeoTest final statistics:" << endl;
   cout << "Detector Acceptance = " << CalcEfficiency(fhAcc3D, fhMc3D) << endl;
   cout << "Circle fit efficiency = " << CalcEfficiency(fhNofHitsCircleFit, fhNofHitsAll) << endl;
   cout << "Ellipse fit efficiency = " << CalcEfficiency(fhNofHitsEllipseFit, fhNofHitsAll) << endl;
   cout << endl;

   cout << "Nof Points per ring, mean = " << fhNofHits[1]->GetMean() << " rms = " << fhNofHits[1]->GetRMS() << endl;
   cout << "Nof Hits per ring, mean = " << fhNofHits[0]->GetMean() << " rms = " << fhNofHits[0]->GetRMS() << endl;


   cout << endl << "McPoint fitting" << endl;
   cout << "Ellipse fitting" << endl;
   cout << "A, mean = " << fhAaxisVsMom[1]->GetMean(2) << " rms = " << fhAaxisVsMom[1]->GetRMS(2) << endl;
   cout << "B, mean = " << fhBaxisVsMom[1]->GetMean(2) << " rms = " << fhBaxisVsMom[1]->GetRMS(2) << endl;
   cout << "B/A, mean = " << fhBoverA[1]->GetMean() << " rms = " << fhBoverA[1]->GetRMS() << endl;
   cout << "Circle fitting" << endl;
   cout << "R, mean = " << fhRadiusVsMom[1]->GetMean(2) << " rms = " << fhRadiusVsMom[1]->GetRMS(2) << endl;

   cout << endl << "Hit fitting" << endl;
   cout << "Ellipse fitting" << endl;
   cout << "A, mean = " << fhAaxisVsMom[0]->GetMean(2) << " rms = " << fhAaxisVsMom[0]->GetRMS(2) << endl;
   cout << "B, mean = " << fhBaxisVsMom[0]->GetMean(2) << " rms = " << fhBaxisVsMom[0]->GetRMS(2) << endl;
   cout << "B/A, mean = " << fhBoverA[0]->GetMean() << " rms = " << fhBoverA[0]->GetRMS() << endl;
   cout << "Circle fitting" << endl;
   cout << "R, mean = " << fhRadiusVsMom[0]->GetMean(2) << " rms = " << fhRadiusVsMom[0]->GetRMS(2) << endl;

   cout << endl << "McPoint fit vs. hit fit" << endl;
   cout << "Ellipse fitting" << endl;
   cout << "dA, mean = " << fhDiffAaxis->GetMean(2) << " rms = " << fhDiffAaxis->GetRMS(2) << endl;
   cout << "dB, mean = " << fhDiffBaxis->GetMean(2) << " rms = " << fhDiffBaxis->GetRMS(2) << endl;
   cout << "dX, mean = " << fhDiffXcEllipse->GetMean(2) << " rms = " << fhDiffXcEllipse->GetRMS(2) << endl;
   cout << "dY, mean = " << fhDiffYcEllipse->GetMean(2) << " rms = " << fhDiffYcEllipse->GetRMS(2) << endl;
   cout << "Circle fitting" << endl;
   cout << "dR, mean = " << fhDiffRadius->GetMean(2) << " rms = " << fhDiffRadius->GetRMS(2) << endl;
   cout << "dX, mean = " << fhDiffXcCircle->GetMean(2) << " rms = " << fhDiffXcCircle->GetRMS(2) << endl;
   cout << "dY, mean = " << fhDiffYcCircle->GetMean(2) << " rms = " << fhDiffYcCircle->GetRMS(2) << endl;
}

void CbmRichGeoTest::Finish()
{
   DrawHist();
   PrintStatisctics();
   for (Int_t i = 0; i < fHists.size(); i++){
      fHists[i]->Write();
   }
}

ClassImp(CbmRichGeoTest)

