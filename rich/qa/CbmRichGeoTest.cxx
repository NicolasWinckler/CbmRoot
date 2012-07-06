/**
 * \file CbmRichGeoTest.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

#include "CbmRichGeoTest.h"
#include "CbmRichGeoTestStudyReport.h"
#include "FairRootManager.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmTrackMatch.h"
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
#include "cbm/qa/draw/CbmLitDrawHist.h"
#include "std/utils/CbmLitUtils.h"
#include "CbmRichConverter.h"
//#include "cbm/qa/report/CbmReport.h"
//#include "cbm/qa/report/CbmStudyReport.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TPad.h"
#include "TLatex.h"
#include "TSystem.h"

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

#include <boost/assign/list_of.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using boost::assign::list_of;
using boost::property_tree::ptree;

CbmRichGeoTest::CbmRichGeoTest():
   FairTask("RichGeoTestQa"),

   fRichDetectorType("standard"),

   fOutputDir(""),

   fRichHits(NULL),
   fRichRings(NULL),
   fRichPoints(NULL),
   fMCTracks(NULL),
   fRichRingMatches(NULL),

   fDetZOrig(0.),
   fTheta(0.),
   fPhi(0.),
   fSensNodes(NULL),
   fPassNodes(NULL),
   fPar(NULL),

   fCopFit(NULL),
   fTauFit(NULL),

   fEventNum(0),
   fMinNofHits(0),

   fhHitsXY(NULL),
   fhPointsXY(NULL),

   fhNofHits(),
   fhAaxisVsMom(),
   fhBaxisVsMom(),
   fhBoverA(),
   fhXcYcEllipse(),
   fhChi2Ellipse(),
   fhXcYcCircle(),
   fhRadiusVsMom(),
   fhChi2Circle(),
   fhDRCircle(),

   fhRadiusVsNofHits(NULL),
   fhAaxisVsNofHits(NULL),
   fhBaxisVsNofHits(NULL),
   fhDiffAaxis(NULL),
   fhDiffBaxis(NULL),
   fhDiffXcEllipse(NULL),
   fhDiffYcEllipse(NULL),
   fhDiffXcCircle(NULL),
   fhDiffYcCircle(NULL),
   fhDiffRadius(NULL),

   fhDiffXhit(NULL),
   fhDiffYhit(NULL),

   fMinAaxis(0.),
   fMaxAaxis(0.),
   fMinBaxis(0.),
   fMaxBaxis(0.),
   fMinRadius(0.),
   fMaxRadius(0.),

   fhNofHitsAll(NULL),
   fhNofHitsCircleFit(NULL),
   fhNofHitsEllipseFit(NULL),
   fhNofHitsCircleFitEff(NULL),
   fhNofHitsEllipseFitEff(NULL),

   fhMc3D(NULL),
   fhAcc3D(NULL),

   fhNofHitsXY(NULL),
   fhNofPointsXY(NULL),
   fhBoverAXY(NULL),
   fhBaxisXY(NULL),
   fhAaxisXY(NULL),
   fhRadiusXY(NULL),
   fhCounterXY(NULL),

   fHists(),

   fNofDrawnRings(0)

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

   if (fRichDetectorType == "standard") {
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
   }

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   if ( NULL == fRichHits) { Fatal("CbmRichGeoTest::Init","No RichHit array!"); }

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if ( NULL == fRichRings) { Fatal("CbmRichGeoTest::Init","No RichRing array!"); }

	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( NULL == fRichPoints) { Fatal("CbmRichGeoTest::Init","No RichPoint array!"); }

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMCTracks) { Fatal("CbmRichGeoTest::Init","No MCTrack array!"); }

	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if ( NULL == fRichRingMatches) { Fatal("CbmRichGeoTest::Init","No RichRingMatch array!"); }

   fCopFit = new CbmRichRingFitterCOP();
   fTauFit = new CbmRichRingFitterEllipseTau();

   InitHistograms();

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

void CbmRichGeoTest::InitHistograms()
{
   double xMin = -200.;
   double xMax = 200.;
   int nBinsX = 100;
   double yMin = -300;
   double yMax = 300.;
   int nBinsY = 150;
   if (fRichDetectorType == "prototype"){
      xMin = -10.;
      xMax = 10.;
      nBinsX = 50;
      yMin = 25.;
      yMax = 45.;
      nBinsY = 50;
   }

   fhHitsXY = new TH2D("fhHitsXY", "fhHitsXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhHitsXY);
   fhPointsXY = new TH2D("fhPointsXY", "fhPointsXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhPointsXY);

   fhNofHits.resize(2);
   fhBoverA.resize(2);
   fhXcYcEllipse.resize(2);
   fhXcYcCircle.resize(2);
   fhBaxisVsMom.resize(2);
   fhAaxisVsMom.resize(2);
   fhRadiusVsMom.resize(2);
   fhChi2Ellipse.resize(2);
   fhChi2Circle.resize(2);
   fhDRCircle.resize(2);
   for (Int_t i = 0; i < 2; i++){
      stringstream ss;
      if (i == 0) ss << "_Hits";
      if (i == 1) ss << "_Points";
      string t = ss.str();
      fhNofHits[i] = new TH1D(("fhNofHits"+t).c_str(), ("fhNofHits"+t+";Nof hits in ring;Counter").c_str(), 300, 0, 300);
      fHists.push_back(fhNofHits[i]);
      // ellipse fitting parameters
      fhBoverA[i] = new TH1D(("fhBoverA"+t).c_str(), ("fhBoverA"+t+";B/A;Counter").c_str(), 50, 0, 1);
      fHists.push_back(fhBoverA[i]);
      fhXcYcEllipse[i] = new TH2D(("fhXcYcEllipse"+t).c_str(), ("fhXcYcEllipse"+t+";X [cm];Y [cm];Counter").c_str(), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
      fHists.push_back(fhXcYcEllipse[i]);
      fhBaxisVsMom[i] = new TH2D(("fhBaxisVsMom"+t).c_str(), ("fhBaxisVsMom"+t+";P [GeV/c];B axis [cm];Counter").c_str(), 100, 0., 12, 200, 0., 10.);
      fHists.push_back(fhBaxisVsMom[i]);
      fhAaxisVsMom[i] = new TH2D(("fhAaxisVsMom"+t).c_str(), ("fhAaxisVsMom"+t+";P [GeV/c];A axis [cm];Counter").c_str(), 100, 0., 12, 200, 0., 10.);
      fHists.push_back(fhAaxisVsMom[i]);
      fhChi2Ellipse[i] = new TH1D(("fhChi2Ellipse"+t).c_str(), ("fhChi2Ellipse"+t+";#Chi^{2};Counter").c_str(), 50, 0., 0.5);
      fHists.push_back(fhChi2Ellipse[i]);
      // circle fitting parameters
      fhXcYcCircle[i] = new TH2D(("fhXcYcCircle"+t).c_str(), ("fhXcYcCircle"+t+";X [cm];Y [cm];Counter").c_str(), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
      fHists.push_back(fhXcYcCircle[i]);
      fhRadiusVsMom[i] = new TH2D(("fhRadiusVsMom"+t).c_str(), ("fhRadiusVsMom"+t+";P [GeV/c];Radius [cm];Counter").c_str(), 100, 0., 12, 200, 0., 10.);
      fHists.push_back(fhRadiusVsMom[i]);
      fhChi2Circle[i] = new TH1D(("fhChi2Circle"+t).c_str(), ("fhChi2Circle"+t+";#Chi^{2};Counter").c_str(), 50, 0., .5);
      fHists.push_back(fhChi2Circle[i]);
      fhDRCircle[i] = new TH1D(("fhDRCircle"+t).c_str(), ("fhDRCircle"+t+";dR [cm];Counter").c_str(), 100, -1., 1.);
      fHists.push_back(fhDRCircle[i]);
   }
   // Difference between Mc Points and Hits fitting.
   fhDiffAaxis = new TH2D("fhDiffAaxis", "fhDiffAaxis;Nof hits in ring;A_{point}-A_{hit} [cm];Counter", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffAaxis);
   fhDiffBaxis = new TH2D("fhDiffBaxis", "fhDiffBaxis;Nof hits in ring;B_{point}-B_{hit} [cm];Counter", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffBaxis);
   fhDiffXcEllipse = new TH2D("fhDiffXcEllipse", "fhDiffXcEllipse;Nof hits in ring;Xc_{point}-Xc_{hit} [cm];Counter", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffXcEllipse);
   fhDiffYcEllipse = new TH2D("fhDiffYcEllipse", "fhDiffYcEllipse;Nof hits in ring;Yc_{point}-Yc_{hit} [cm];Counter", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffYcEllipse);
   fhDiffXcCircle = new TH2D("fhDiffXcCircle", "fhDiffXcCircle;Nof hits in ring;Xc_{point}-Xc_{hit} [cm];Counter", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffXcCircle);
   fhDiffYcCircle = new TH2D("fhDiffYcCircle", "fhDiffYcCircle;Nof hits in ring;Yc_{point}-Yc_{hit} [cm];Counter", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffYcCircle);
   fhDiffRadius = new TH2D("fhDiffRadius", "fhDiffRadius;Nof hits in ring;Radius_{point}-Radius_{hit} [cm];Counter", 40, 0., 40., 100, -1.5, 1.5);
   fHists.push_back(fhDiffRadius);

   // R, A, B distribution for different number of hits from 0 to 40.
   fhRadiusVsNofHits = new TH2D("fhRadiusVsNofHits", "fhRadiusVsNofHits;Nof hits in ring;Radius [cm];Counter", 40, 0., 40., 100, 0., 10.);
   fhAaxisVsNofHits = new TH2D("fhAaxisVsNofHits", "fhAaxisVsNofHits;Nof hits in ring;A axis [cm];Counter", 40, 0., 40., 100, 0., 10.);
   fhBaxisVsNofHits = new TH2D("fhBaxisVsNofHits", "fhBaxisVsNofHits;Nof hits in ring;B axis [cm];Counter", 40, 0., 40., 100, 0., 10.);

   // Hits and points.
   fhDiffXhit = new TH1D("fhDiffXhit", "fhDiffXhit;Y_{point}-Y_{hit} [cm];Counter", 200, -1., 1.);
   fHists.push_back(fhDiffXhit);
   fhDiffYhit = new TH1D("fhDiffYhit", "fhDiffYhit;Y_{point}-Y_{hit} [cm];Counter", 200, -1., 1.);
   fHists.push_back(fhDiffYhit);

   // Fitting efficiency.
   fhNofHitsAll = new TH1D("fhNofHitsAll", "fhNofHitsAll;Nof hits in ring;Counter", 50, 0., 50.);
   fHists.push_back(fhNofHitsAll);
   fhNofHitsCircleFit = new TH1D("fhNofHitsCircleFit", "fhNofHitsCircleFit;Nof hits in ring;Counter", 50, 0., 50.);
   fHists.push_back(fhNofHitsCircleFit);
   fhNofHitsEllipseFit = new TH1D("fhNofHitsEllipseFit", "fhNofHitsEllipseFit;Nof hits in ring;Counter", 50, 0., 50.);
   fHists.push_back(fhNofHitsEllipseFit);
   fhNofHitsCircleFitEff = new TH1D("fhNofHitsCircleFitEff", "fhNofHitsCircleFitEff;Nof hits in ring;Efficiency [%]", 50, 0., 50.);
   fHists.push_back(fhNofHitsCircleFitEff);
   fhNofHitsEllipseFitEff = new TH1D("fhNofHitsEllipseFitEff", "fhNofHitsEllipseFitEff;Nof hits in ring;Efficiency [%]", 50, 0., 50.);
   fHists.push_back(fhNofHitsEllipseFitEff);

   // Detector acceptance efficiency vs. pt, y, p.
   fhMc3D = new TH3D("fhMc3D", "fhMc3D;P [GeV/c];P_{t} [GeV/c];Rapidity", 50, 0., 10., 30, 0., 3., 40, 0., 4.);
   fHists.push_back(fhMc3D);
   fhAcc3D = new TH3D("fhAcc3D", "fhAcc3D;P [GeV/c];P_{t} [GeV/c];Rapidity", 50, 0., 10., 30, 0., 3., 40, 0., 4.);;
   fHists.push_back(fhAcc3D);

   // Numbers in dependence on XY position onto the photodetector.
   fhNofHitsXY = new TH2D("fhNofHitsXY", "fhNofHitsXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhNofHitsXY);
   fhNofPointsXY = new TH2D("fhNofPointsXY", "fhNofPointsXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhNofPointsXY);
   fhBoverAXY = new TH2D("fhBoverAXY", "fhBoverAXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhBoverAXY);
   fhBaxisXY = new TH2D("fhBaxisXY", "fhBaxisXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhBaxisXY);
   fhAaxisXY = new TH2D("fhAaxisXY", "fhAaxisXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhAaxisXY);
   fhRadiusXY = new TH2D("fhRadiusXY", "fhRadiusXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhRadiusXY);
   fhCounterXY = new TH2D("fhCounterXY", "fhCounterXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhCounterXY);
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
		if (NULL == ring) continue;
		CbmTrackMatch* ringMatch = (CbmTrackMatch*) fRichRingMatches->At(iR);
		if (NULL == ringMatch) continue;

		Int_t mcTrackId = ringMatch->GetMCTrackId();
		if (mcTrackId < 0) continue;
		CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(mcTrackId);
		if (!mcTrack) continue;
		Int_t motherId = mcTrack->GetMotherId();
		Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
		Double_t momentum = mcTrack->GetP();
      Double_t pt = mcTrack->GetPt();
      Double_t rapidity = mcTrack->GetRapidity();

		if (pdg != 11 || motherId != -1) continue; // only primary electrons
		if (ring->GetNofHits() >= fMinNofHits){
		   fhAcc3D->Fill(momentum, pt, rapidity);
		}

		CbmRichRingLight ringPoint;
		int nofRichPoints = fRichPoints->GetEntriesFast();
		for (int iPoint = 0; iPoint < nofRichPoints; iPoint++){
		   CbmRichPoint* richPoint = (CbmRichPoint*) fRichPoints->At(iPoint);
		   if (NULL == richPoint) continue;
		   Int_t trackId = richPoint->GetTrackID();
		   if (trackId < 0) continue;
	      CbmMCTrack* mcTrackRich = (CbmMCTrack*)fMCTracks->At(trackId);
	      if (NULL == mcTrackRich) continue;
	      int motherIdRich = mcTrackRich->GetMotherId();
		   if (motherIdRich == mcTrackId){
		      TVector3 posPoint;
		      richPoint->Position(posPoint);
		      TVector3 detPoint;
		      CbmRichHitProducer::TiltPoint(&posPoint, &detPoint, fPhi, fTheta, fDetZOrig);
		      CbmRichHitLight hit(detPoint.X(), detPoint.Y());
		      ringPoint.AddHit(hit);
		   }
		}
	   fhNofHitsAll->Fill(ring->GetNofHits());

	   CbmRichRingLight ringHit;
	   CbmRichConverter::CopyHitsToRingLight(ring, &ringHit);

      FitAndFillHistCircle(0, &ringHit, momentum); //hits
      FitAndFillHistCircle(1, &ringPoint, momentum); // points
      FillMcVsHitFitCircle(&ringHit, &ringPoint);

      double r = ringPoint.GetRadius();

      if (ringHit.GetRadius() > fMinRadius && ringHit.GetRadius() < fMaxRadius){
         fhNofHitsCircleFit->Fill(ringHit.GetNofHits());
      }
	   if (fNofDrawnRings < 10 && ringHit.GetNofHits() <= 500){
	      DrawRing(&ringHit, &ringPoint);
	   }

		FitAndFillHistEllipse(0, &ringHit, momentum);// hits
		FitAndFillHistEllipse(1, &ringPoint, momentum); // points
		FillMcVsHitFitEllipse(&ringHit, &ringPoint);
	   if (ringHit.GetAaxis() > fMinAaxis && ringHit.GetAaxis() < fMaxAaxis
	       &&  ringHit.GetBaxis() > fMinBaxis && ringHit.GetAaxis() < fMaxBaxis ){
	      fhNofHitsEllipseFit->Fill(ringHit.GetNofHits());
	   }
	   Double_t np = ringPoint.GetNofHits();
      Double_t xc = ringPoint.GetCenterX();
      Double_t yc = ringPoint.GetCenterY();
      Double_t a = ringPoint.GetAaxis();
      Double_t b = ringPoint.GetBaxis();
      Double_t nh = ring->GetNofHits();

	   fhNofHitsXY->Fill(xc, yc, nh);
	   fhNofPointsXY->Fill(xc, yc, np);
	   fhBoverAXY->Fill(xc, yc, a/b);
	   fhBaxisXY->Fill(xc, yc, b);
	   fhAaxisXY->Fill(xc, yc, a);
	   fhRadiusXY->Fill(xc, yc, r);
	   fhCounterXY->Fill(xc, yc);
	}// iR
}

void CbmRichGeoTest::FitAndFillHistEllipse(
      int histIndex,
      CbmRichRingLight* ring,
      double momentum)
{
   fTauFit->DoFit(ring);
   double axisA = ring->GetAaxis();
   double axisB = ring->GetBaxis();
   double xcEllipse = ring->GetCenterX();
   double ycEllipse = ring->GetCenterY();
   int nofHitsRing = ring->GetNofHits();

   fhBoverA[histIndex]->Fill(axisB/axisA);
   fhXcYcEllipse[histIndex]->Fill(xcEllipse, ycEllipse);
   fhNofHits[histIndex]->Fill(nofHitsRing);
   fhBaxisVsMom[histIndex]->Fill(momentum, axisB);
   fhAaxisVsMom[histIndex]->Fill(momentum, axisA);
   fhChi2Ellipse[histIndex]->Fill(ring->GetChi2()/ring->GetNofHits());
   if (histIndex == 0){ // only hit fit
      fhAaxisVsNofHits->Fill(nofHitsRing, axisA);
      fhBaxisVsNofHits->Fill(nofHitsRing, axisB);
   }
}

void CbmRichGeoTest::FitAndFillHistCircle(
      int histIndex,
      CbmRichRingLight* ring,
      double momentum)
{
   fCopFit->DoFit(ring);
   double radius = ring->GetRadius();
   double xcCircle = ring->GetCenterX();
   double ycCircle = ring->GetCenterY();
   int nofHitsRing = ring->GetNofHits();
   fhXcYcCircle[histIndex]->Fill(xcCircle, ycCircle);
   fhRadiusVsMom[histIndex]->Fill(momentum, radius);
   fhChi2Circle[histIndex]->Fill(ring->GetChi2()/ring->GetNofHits());

   for (int iH = 0; iH < ring->GetNofHits(); iH++){
      double xh = ring->GetHit(iH).fX;
      double yh = ring->GetHit(iH).fY;
      double dr = radius - sqrt((xcCircle - xh)*(xcCircle - xh) + (ycCircle - yh)*(ycCircle - yh));
      fhDRCircle[histIndex]->Fill(dr);
   }

   if (histIndex == 0){ // only hit fit
      fhRadiusVsNofHits->Fill(nofHitsRing, radius);
   }
}

void CbmRichGeoTest::FillMcVsHitFitEllipse(
      CbmRichRingLight* ring,
      CbmRichRingLight* ringMc)
{
   fhDiffAaxis->Fill(ring->GetNofHits(), ringMc->GetAaxis() - ring->GetAaxis());
   fhDiffBaxis->Fill(ring->GetNofHits(),ringMc->GetBaxis() - ring->GetBaxis());
   fhDiffXcEllipse->Fill(ring->GetNofHits(),ringMc->GetCenterX() - ring->GetCenterX());
   fhDiffYcEllipse->Fill(ring->GetNofHits(),ringMc->GetCenterY() - ring->GetCenterY());
}

void CbmRichGeoTest::FillMcVsHitFitCircle(
      CbmRichRingLight* ring,
      CbmRichRingLight* ringMc)
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
      fhHitsXY->Fill(hit->GetX(), hit->GetY());
      fhDiffXhit->Fill(hit->GetX() - outPos.X());
      fhDiffYhit->Fill(hit->GetY() - outPos.Y());
   }

   Int_t nofPoints = fRichPoints->GetEntriesFast();
   for (Int_t iP = 0; iP < nofPoints; iP++){
      CbmRichPoint *point = (CbmRichPoint*) fRichPoints->At(iP);
      if ( point == NULL ) continue;
      TVector3 inPos(point->GetX(), point->GetY(), point->GetZ());
      TVector3 outPos;
      CbmRichHitProducer::TiltPoint(&inPos, &outPos, fPhi, fTheta, fDetZOrig);
      fhPointsXY->Fill(outPos.X(), outPos.Y());
   }
}

void CbmRichGeoTest::DrawRing(
      CbmRichRingLight* ringHit,
      CbmRichRingLight* ringPoint)
{
   stringstream ss;
   ss << "Event" << fNofDrawnRings;
   fNofDrawnRings++;
   TCanvas *c = CreateCanvas(ss.str().c_str(), ss.str().c_str(), 500, 500);
   c->SetGrid(true, true);
   TH2D* pad = new TH2D("pad", ";X [cm];Y [cm]", 1, -15., 15., 1, -15., 15);
   pad->SetStats(false);
   pad->Draw();

   // find min and max x and y positions of the hits
   // in order to shift drawing
   double xmin = 99999., xmax = -99999., ymin = 99999., ymax = -99999.;
   for (int i = 0; i < ringHit->GetNofHits(); i++){
      double hitX = ringHit->GetHit(i).fX;
      double hitY = ringHit->GetHit(i).fY;
      if (xmin > hitX) xmin = hitX;
      if (xmax < hitX) xmax = hitX;
      if (ymin > hitY) ymin = hitY;
      if (ymax < hitY) ymax = hitY;
   }
   double xCur = (xmin + xmax) / 2.;
   double yCur = (ymin + ymax) / 2.;

   //Draw circle and center
   TEllipse* circle = new TEllipse(ringHit->GetCenterX() - xCur,
         ringHit->GetCenterY() - yCur, ringHit->GetRadius());
   circle->SetFillStyle(0);
   circle->SetLineWidth(3);
   circle->Draw();
   TEllipse* center = new TEllipse(ringHit->GetCenterX() - xCur, ringHit->GetCenterY() - yCur, .5);
   center->Draw();

   // Draw hits
   for (int i = 0; i < ringHit->GetNofHits(); i++){
      TEllipse* hitDr = new TEllipse(ringHit->GetHit(i).fX - xCur, ringHit->GetHit(i).fY - yCur, .5);
      hitDr->SetFillColor(kRed);
      hitDr->Draw();
   }

   // Draw MC Points
   for (int i = 0; i < ringPoint->GetNofHits(); i++){
      TEllipse* pointDr = new TEllipse(ringPoint->GetHit(i).fX - xCur, ringPoint->GetHit(i).fY - yCur, 0.15);
      pointDr->SetFillColor(kBlue);
      pointDr->Draw();
   }

   //Draw information
   stringstream ss2;
   ss2 << "(r, n)=(" << setprecision(3) << ringHit->GetRadius() << ", " << ringHit->GetNofHits()<<")";
   TLatex* latex = new TLatex(-8., 8., ss2.str().c_str());
   latex->Draw();
}

TH1D* CbmRichGeoTest::CreateAccVsMinNofHitsHist()
{
   Int_t nofMc = (Int_t)fhMc3D->GetEntries();
   TH1D* hist = (TH1D*)fhNofHits[0]->Clone("fhAccVsMinNofHitsHist");
   hist->GetXaxis()->SetTitle("Required min nof hits in ring");
   hist->GetYaxis()->SetTitle("Detector acceptance [%]");
   Double_t sum = 0.;
   for (int i = hist->GetNbinsX(); i > 1 ; i--){
      sum += fhNofHits[0]->GetBinContent(i);
      hist->SetBinContent(i, 100. * sum / nofMc);
   }
   return hist;
}

void CbmRichGeoTest::DrawHist()
{
   TCanvas *cHitsXY = CreateCanvas("rich_geo_hits_xy", "rich_geo_hits_xy", 600, 600);
   DrawH2(fhHitsXY);

   TCanvas *cPointsXY = CreateCanvas("rich_geo_points_xy", "rich_geo_points_xy", 600, 600);
   DrawH2(fhPointsXY);

   for (int i = 0; i < 2; i++){
      stringstream ss;
      if (i == 0) ss << "rich_geo_hits_fit";
      if (i == 1) ss << "rich_geo_points_fit";
      TCanvas *cEllipse = CreateCanvas((ss.str()+"_ellipse").c_str(), (ss.str()+"_ellipse").c_str(), 1200, 600);
      cEllipse->Divide(4, 2);
      cEllipse->cd(1);
      DrawH1(fhBoverA[i]);
      gPad->SetLogy(true);
      cEllipse->cd(2);
      DrawH2(fhXcYcEllipse[i]);
      cEllipse->cd(3);
      DrawH1(fhAaxisVsMom[i]->ProjectionY());
      gPad->SetLogy(true);
      cEllipse->cd(4);
      DrawH1(fhBaxisVsMom[i]->ProjectionY());
      gPad->SetLogy(true);
      cEllipse->cd(5);
      DrawH2(fhBaxisVsMom[i]);
      cEllipse->cd(6);
      DrawH2(fhAaxisVsMom[i]);
      cEllipse->cd(7);
      DrawH1(fhChi2Ellipse[i]);

      TCanvas *cCircle = CreateCanvas((ss.str()+"_circle").c_str(), (ss.str()+"_circle").c_str(), 900, 600);
      cCircle->Divide(3,2);
      cCircle->cd(1);
      DrawH1(fhNofHits[i]);
      gPad->SetLogy(true);
      cCircle->cd(2);
      DrawH2(fhXcYcCircle[i]);
      cCircle->cd(3);
      DrawH1(fhRadiusVsMom[i]->ProjectionY());
      gPad->SetLogy(true);
      cCircle->cd(4);
      DrawH2(fhRadiusVsMom[i]);
      cCircle->cd(5);
      DrawH1(fhChi2Circle[i]);
      cCircle->cd(6);
      DrawH1(fhDRCircle[i]);
   }

   TCanvas *cDiff2DEllipse = CreateCanvas("rich_geo_diff2d_ellipse", "rich_geo_diff2d_ellipse", 600, 600);
   cDiff2DEllipse->Divide(2,2);
   cDiff2DEllipse->cd(1);
   DrawH2(fhDiffAaxis);
   cDiff2DEllipse->cd(2);
   DrawH2(fhDiffBaxis);
   cDiff2DEllipse->cd(3);
   DrawH2(fhDiffXcEllipse);
   cDiff2DEllipse->cd(4);
   DrawH2(fhDiffYcEllipse);

   TCanvas *cDiff2DCircle = CreateCanvas("rich_geo_diff2d_circle", "rich_geo_diff2d_circle", 900, 300);
   cDiff2DCircle->Divide(3,1);
   cDiff2DCircle->cd(1);
   DrawH2(fhDiffXcCircle);
   cDiff2DCircle->cd(2);
   DrawH2(fhDiffYcCircle);
   cDiff2DCircle->cd(3);
   DrawH2(fhDiffRadius);

   TCanvas *cDiff1DEllipse = CreateCanvas("rich_geo_diff1d_ellipse", "rich_geo_diff1d_ellipse", 600, 600);
   cDiff1DEllipse->Divide(2,2);
   cDiff1DEllipse->cd(1);
   DrawH1(fhDiffAaxis->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DEllipse->cd(2);
   DrawH1(fhDiffBaxis->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DEllipse->cd(3);
   DrawH1(fhDiffXcEllipse->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DEllipse->cd(4);
   DrawH1(fhDiffYcEllipse->ProjectionY());
   gPad->SetLogy(true);

   TCanvas *cDiff1DCircle = CreateCanvas("rich_geo_diff1d_circle", "rich_geo_diff1d_circle", 900, 300);
   cDiff1DCircle->Divide(3,1);
   cDiff1DCircle->cd(1);
   DrawH1(fhDiffXcCircle->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DCircle->cd(2);
   DrawH1(fhDiffYcCircle->ProjectionY());
   gPad->SetLogy(true);
   cDiff1DCircle->cd(3);
   DrawH1(fhDiffRadius->ProjectionY());
   gPad->SetLogy(true);

   TCanvas *cHits = CreateCanvas("rich_geo_hits", "rich_geo_hits", 600, 300);
   cHits->Divide(2,1);
   cHits->cd(1);
   DrawH1(fhDiffXhit);
   cHits->cd(2);
   DrawH1(fhDiffYhit);

   TCanvas *cFitEff = CreateCanvas("rich_geo_fit_eff", "rich_geo_fit_eff", 900, 300);
   cFitEff->Divide(3,1);
   cFitEff->cd(1);
   DrawH1( list_of((TH1D*)fhNofHitsAll->Clone())((TH1D*)fhNofHitsCircleFit->Clone())((TH1D*)fhNofHitsEllipseFit->Clone()),
         list_of("All")("Circle fit")("Ellipse fit"), kLitLinear, kLitLog, true, 0.7, 0.7, 0.99, 0.99);
   fhNofHitsCircleFitEff = DivideH1(fhNofHitsCircleFit, fhNofHitsAll, "", "", "Nof hits in ring", "Efficiency");
   fhNofHitsEllipseFitEff = DivideH1(fhNofHitsEllipseFit, fhNofHitsAll, "", "", "Nof hits in ring", "Efficiency");
   cFitEff->cd(2);
   DrawH1(fhNofHitsCircleFitEff);
   TLatex* circleFitEffTxt = new TLatex(15, 0.5, CalcEfficiency(fhNofHitsCircleFit, fhNofHitsAll).c_str());
   circleFitEffTxt->Draw();
   cFitEff->cd(3);
   DrawH1(fhNofHitsEllipseFitEff);
   TLatex* ellipseFitEffTxt = new TLatex(15, 0.5, CalcEfficiency(fhNofHitsEllipseFit, fhNofHitsAll).c_str());
   ellipseFitEffTxt->Draw();

   TCanvas *cAccEff = CreateCanvas("rich_geo_acc_eff", "rich_geo_acc_eff", 900, 600);
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
   DrawH1(list_of((TH1D*)pxMc->Clone())((TH1D*)pxAcc->Clone()), list_of("MC")("ACC"), kLitLinear, kLitLog, true, 0.7, 0.75, 0.99, 0.99);
   cAccEff->cd(2);
   DrawH1(list_of((TH1D*)pyMc->Clone())((TH1D*)pyAcc->Clone()), list_of("MC")("ACC"), kLitLinear, kLitLog, true, 0.7, 0.75, 0.99, 0.99);
   cAccEff->cd(3);
   DrawH1(list_of((TH1D*)pzMc->Clone())((TH1D*)pzAcc->Clone()), list_of("MC")("ACC"), kLitLinear, kLitLog, true, 0.7, 0.75, 0.99, 0.99);

   TH1D* pxEff = DivideH1(pxAcc, pxMc, "pxEff", "", "P [GeV/c]", "Efficiency");
   TH1D* pyEff = DivideH1(pyAcc, pyMc, "pyEff", "", "P_{t} [GeV/c]", "Efficiency");
   TH1D* pzEff = DivideH1(pzAcc, pzMc, "pzEff", "", "Rapidity", "Efficiency");
   cAccEff->cd(4);
   DrawH1(pxEff);
   TLatex* pxEffTxt = new TLatex(3, 0.5, CalcEfficiency(pxAcc, pxMc).c_str());
   pxEffTxt->Draw();
   cAccEff->cd(5);
   DrawH1(pyEff);
   cAccEff->cd(6);
   DrawH1(pzEff);

   TCanvas *cNumbersVsXY = CreateCanvas("rich_geo_numbers_vs_xy", "rich_geo_numbers_vs_xy", 900, 600);
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

   TCanvas *cAccVsMinNofHits = CreateCanvas("rich_geo_acc_vs_min_nof_hits", "rich_geo_acc_vs_min_nof_hits", 600, 600);
   TH1D* h = CreateAccVsMinNofHitsHist();
   DrawH1(h);

   TCanvas *cRABvsNofHits = CreateCanvas("rich_geo_rab_vs_nof_hits", "rich_geo_rab_vs_nof_hits", 900, 300);
   cRABvsNofHits->Divide(3, 1);
   cRABvsNofHits->cd(1);
   DrawH2(fhRadiusVsNofHits);
   cRABvsNofHits->cd(2);
   DrawH2(fhAaxisVsNofHits);
   cRABvsNofHits->cd(3);
   DrawH2(fhBaxisVsNofHits);

   stringstream ss;
   TH1D* prRMean = fhRadiusVsNofHits->ProjectionX("fhRadiusVsNofHits_mean");
   TH1D* prRRms = fhRadiusVsNofHits->ProjectionX("fhRadiusVsNofHits_rms");
   TH1D* prAMean = fhAaxisVsNofHits->ProjectionX("fhAaxisVsNofHits_mean");
   TH1D* prARms = fhAaxisVsNofHits->ProjectionX("fhAaxisVsNofHits_rms");
   TH1D* prBMean = fhBaxisVsNofHits->ProjectionX("fhBaxisVsNofHits_mean");
   TH1D* prBRms = fhBaxisVsNofHits->ProjectionX("fhBaxisVsNofHits_rms");
   for (Int_t i = 1; i <= fhRadiusVsNofHits->GetXaxis()->GetNbins(); i++){
      ss << "_py" << i;
      TH1D* pr = fhRadiusVsNofHits->ProjectionY(ss.str().c_str(), i, i);
      prRMean->SetBinContent(i, pr->GetMean());
      prRRms->SetBinContent(i, pr->GetRMS());
      pr = fhAaxisVsNofHits->ProjectionY(ss.str().c_str(), i, i);
      prAMean->SetBinContent(i, pr->GetMean());
      prARms->SetBinContent(i, pr->GetRMS());
      pr = fhBaxisVsNofHits->ProjectionY(ss.str().c_str(), i, i);
      prBMean->SetBinContent(i, pr->GetMean());
      prBRms->SetBinContent(i, pr->GetRMS());
   }
   TCanvas *cRABMeanRmsvsNofHits = CreateCanvas("rich_geo_rab_mean_rms_vs_nof_hits", "rich_geo_rab_mean_rms_vs_nof_hits", 900, 450);
   cRABMeanRmsvsNofHits->Divide(2, 1);
   cRABMeanRmsvsNofHits->cd(1);
   DrawH1(list_of(prRMean)(prAMean)(prBMean),
         list_of("R")("A")("B"), kLitLinear, kLitLog, true, 0.7, 0.7, 0.99, 0.99);
   cRABMeanRmsvsNofHits->cd(2);
   DrawH1(list_of(prRRms)(prARms)(prBRms),
         list_of("R")("A")("B"), kLitLinear, kLitLog, true, 0.7, 0.7, 0.99, 0.99);
}

void CbmRichGeoTest::CreatePTree()
{
   ptree pt;
   pt.put("acc_mean", CalcEfficiency(fhAcc3D, fhMc3D));
   pt.put("circle_fit_eff_mean", CalcEfficiency(fhNofHitsCircleFit, fhNofHitsAll));
   pt.put("ellipse_fit_eff_mean", CalcEfficiency(fhNofHitsEllipseFit, fhNofHitsAll));
   pt.put("nof_points_in_ring_mean", fhNofHits[1]->GetMean());
   pt.put("nof_points_in_ring_rms", fhNofHits[1]->GetRMS());
   pt.put("nof_hits_in_ring_mean", fhNofHits[0]->GetMean());
   pt.put("nof_hits_in_ring_rms", fhNofHits[0]->GetRMS());
   pt.put("points_fit_a_axis_mean", fhAaxisVsMom[1]->GetMean(2));
   pt.put("points_fit_a_axis_rms", fhAaxisVsMom[1]->GetRMS(2));
   pt.put("points_fit_b_axis_mean", fhBaxisVsMom[1]->GetMean(2));
   pt.put("points_fit_b_axis_rms", fhBaxisVsMom[1]->GetRMS(2));
   pt.put("points_fit_boa_mean", fhBoverA[1]->GetMean());
   pt.put("points_fit_boa_rms", fhBoverA[1]->GetRMS());
   pt.put("points_fit_r_mean", fhRadiusVsMom[1]->GetMean(2));
   pt.put("points_fit_r_rms", fhRadiusVsMom[1]->GetRMS(2));
   pt.put("hits_fit_a_axis_mean", fhAaxisVsMom[0]->GetMean(2));
   pt.put("hits_fit_a_axis_rms", fhAaxisVsMom[0]->GetRMS(2));
   pt.put("hits_fit_b_axis_mean", fhBaxisVsMom[0]->GetMean(2));
   pt.put("hits_fit_b_axis_rms", fhBaxisVsMom[0]->GetRMS(2));
   pt.put("hits_fit_boa_mean", fhBoverA[0]->GetMean());
   pt.put("hits_fit_boa_rms", fhBoverA[0]->GetRMS());
   pt.put("hits_fit_r_mean", fhRadiusVsMom[0]->GetMean(2));
   pt.put("hits_fit_r_rms", fhRadiusVsMom[0]->GetRMS(2));
   pt.put("diff_ellipse_da_mean", fhDiffAaxis->GetMean(2));
   pt.put("diff_ellipse_da_rms", fhDiffAaxis->GetRMS(2));
   pt.put("diff_ellipse_db_mean", fhDiffBaxis->GetMean(2));
   pt.put("diff_ellipse_db_rms", fhDiffBaxis->GetRMS(2));
   pt.put("diff_ellipse_dx_mean", fhDiffXcEllipse->GetMean(2));
   pt.put("diff_ellipse_dx_rms", fhDiffXcEllipse->GetRMS(2));
   pt.put("diff_ellipse_dy_mean", fhDiffYcEllipse->GetMean(2));
   pt.put("diff_ellipse_dy_rms", fhDiffYcEllipse->GetRMS(2));
   pt.put("diff_circle_dr_mean", fhDiffRadius->GetMean(2));
   pt.put("diff_circle_dr_rms", fhDiffRadius->GetRMS(2));
   pt.put("diff_circle_dx_mean", fhDiffXcCircle->GetMean(2));
   pt.put("diff_circle_dx_rms", fhDiffXcCircle->GetRMS(2));
   pt.put("diff_circle_dy_mean", fhDiffYcCircle->GetMean(2));
   pt.put("diff_circle_dy_rms", fhDiffYcCircle->GetRMS(2));

   string qaFile = fOutputDir + "rich_geo_test.json";
   if (fOutputDir != "") {
      gSystem->mkdir(fOutputDir.c_str(), true);
      write_json(qaFile.c_str(), pt);
   }
}

void CbmRichGeoTest::CreateStudyReport(
      const string& title,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames,
      const string& outputDir)
{
   if (outputDir != "") gSystem->mkdir(outputDir.c_str(), true);

   CbmStudyReport* report = new CbmRichGeoTestStudyReport();
   report->SetTitle(title);
   cout << "Report can be found here: " << outputDir << endl;
   ofstream foutHtml(string(outputDir + "rich_geo_test_study.html").c_str());
   ofstream foutLatex(string(outputDir + "rich_geo_test_study.tex").c_str());
   ofstream foutText(string(outputDir + "rich_geo_test_study.txt").c_str());
   report->Create(kHtmlReport, foutHtml, resultDirectories, studyNames);
   report->Create(kLatexReport, foutLatex, resultDirectories, studyNames);
   report->Create(kTextReport, foutText, resultDirectories, studyNames);
   delete report;
}

void CbmRichGeoTest::Finish()
{
   DrawHist();
   CreatePTree();
   for (Int_t i = 0; i < fHists.size(); i++){
      fHists[i]->Write();
   }
   SaveCanvasToImage();
}

string CbmRichGeoTest::CalcEfficiency(
   TH1* histRec,
   TH1* histAcc)
{
   if (histAcc->GetEntries() == 0) { return "0.0"; }
   else {
      Double_t eff = Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
      stringstream ss;
      ss.precision(3);
      ss << eff;
      return ss.str();
   }
}


TH1D* CbmRichGeoTest::DivideH1(
   TH1D* h1,
   TH1D* h2,
   const string& name,
   const string& title,
   const string& axisX,
   const string& axisY)
{
   h1->Sumw2();
   h2->Sumw2();
   TH1D* h3 = new TH1D(name.c_str(), string(title +";"+axisX+";"+ axisY).c_str(),
                       h1->GetNbinsX(), h1->GetXaxis()->GetXmin(),h1->GetXaxis()->GetXmax());
   h3->Divide(h1, h2, 1., 1., "B");
   return h3;
}

TCanvas* CbmRichGeoTest::CreateCanvas(
      const string& name,
      const string& title,
      int width,
      int height)
{
   TCanvas* c = new TCanvas(name.c_str(), title.c_str(), width, height);
   fCanvas.push_back(c);
   return c;
}

void CbmRichGeoTest::SaveCanvasToImage()
{
   for (int i = 0; i < fCanvas.size(); i++)
   {
      lit::SaveCanvasAsImage(fCanvas[i], fOutputDir);
   }
}

ClassImp(CbmRichGeoTest)

