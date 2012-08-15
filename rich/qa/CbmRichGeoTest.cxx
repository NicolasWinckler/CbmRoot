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
#include "CbmDrawHist.h"
#include "std/utils/CbmLitUtils.h"
#include "CbmRichConverter.h"
#include "CbmReport.h"
#include "CbmStudyReport.h"

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
#include "TStyle.h"
#include "TF1.h"
#include "TLegend.h"

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <string>

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
   fhChi2EllipseVsMom(),
   fhXcYcCircle(),
   fhRadiusVsMom(),
   fhChi2CircleVsMom(),
   fhDRVsMom(),

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

   fh_mc_mom(NULL),
   fh_mc_pt(NULL),
   fh_mc_y(NULL),
   fh_acc_mom(NULL),
   fh_acc_pt(NULL),
   fh_acc_y(NULL),


   fhNofHitsXY(NULL),
   fhNofPointsXY(NULL),
   fhBoverAXY(NULL),
   fhBaxisXY(NULL),
   fhAaxisXY(NULL),
   fhRadiusXY(NULL),
   fhCounterXY(NULL),

   fhHitsXY1Gev(NULL),

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
   int nBinsX = 67;
   double yMin = -300;
   double yMax = 300.;
   int nBinsY = 100;
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
   fhChi2EllipseVsMom.resize(2);
   fhChi2CircleVsMom.resize(2);
   fhDRVsMom.resize(2);
   for (Int_t i = 0; i < 2; i++){
      stringstream ss;
      if (i == 0) ss << "_hits";
      if (i == 1) ss << "_points";
      string t = ss.str();
      fhNofHits[i] = new TH1D(("fhNofHits"+t).c_str(), ("fhNofHits"+t+";Nof hits in ring;Counter").c_str(), 300, 0, 300);
      fHists.push_back(fhNofHits[i]);
      // ellipse fitting parameters
      fhBoverA[i] = new TH1D(("fhBoverA"+t).c_str(), ("fhBoverA"+t+";B/A;Counter").c_str(), 50, 0, 1);
      fHists.push_back(fhBoverA[i]);
      fhXcYcEllipse[i] = new TH2D(("fhXcYcEllipse"+t).c_str(), ("fhXcYcEllipse"+t+";X [cm];Y [cm];Counter").c_str(), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
      fHists.push_back(fhXcYcEllipse[i]);
      fhBaxisVsMom[i] = new TH2D(("fhBaxisVsMom"+t).c_str(), ("fhBaxisVsMom"+t+";P [GeV/c];B axis [cm];Counter").c_str(), 100, 0., 10, 200, 0., 10.);
      fHists.push_back(fhBaxisVsMom[i]);
      fhAaxisVsMom[i] = new TH2D(("fhAaxisVsMom"+t).c_str(), ("fhAaxisVsMom"+t+";P [GeV/c];A axis [cm];Counter").c_str(), 100, 0., 10, 200, 0., 10.);
      fHists.push_back(fhAaxisVsMom[i]);
      fhChi2EllipseVsMom[i] = new TH2D(("fhChi2EllipseVsMom"+t).c_str(), ("fhChi2EllipseVsMom"+t+";P [GeV/c];#Chi^{2};Counter").c_str(), 100, 0., 10., 50, 0., 0.5);
      fHists.push_back(fhChi2EllipseVsMom[i]);
      // circle fitting parameters
      fhXcYcCircle[i] = new TH2D(("fhXcYcCircle"+t).c_str(), ("fhXcYcCircle"+t+";X [cm];Y [cm];Counter").c_str(), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
      fHists.push_back(fhXcYcCircle[i]);
      fhRadiusVsMom[i] = new TH2D(("fhRadiusVsMom"+t).c_str(), ("fhRadiusVsMom"+t+";P [GeV/c];Radius [cm];Counter").c_str(), 100, 0., 10, 200, 0., 10.);
      fHists.push_back(fhRadiusVsMom[i]);
      fhChi2CircleVsMom[i] = new TH2D(("fhChi2CircleVsMom"+t).c_str(), ("fhChi2CircleVsMom"+t+";P [GeV/c];#Chi^{2};Counter").c_str(), 100, 0., 10., 50, 0., .5);
      fHists.push_back(fhChi2CircleVsMom[i]);
      fhDRVsMom[i] = new TH2D(("fhDRVsMom"+t).c_str(), ("fhDRVsMom"+t+";P [GeV/c];dR [cm];Counter").c_str(), 100, 0, 10, 100, -1., 1.);
      fHists.push_back(fhDRVsMom[i]);
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
   fh_mc_mom = new TH1D("fh_mc_mom", "fh_mc_mom;P [GeV/c];Counter", 100, 0., 10.);
   fHists.push_back(fh_mc_mom);
   fh_mc_pt = new TH1D("fh_mc_pt", "fh_mc_pt;P_{t} [GeV/c];Counter", 30, 0., 3.);
   fHists.push_back(fh_mc_pt);
   fh_mc_y = new TH1D("fh_mc_y", "fh_mc_y;Rapidity;Counter", 40, 0., 4.);
   fHists.push_back(fh_mc_y);
   fh_acc_mom = new TH1D("fh_acc_mom", "fh_acc_mom;P [GeV/c];Counter", 100, 0., 10.);
   fHists.push_back(fh_acc_mom);
   fh_acc_pt = new TH1D("fh_acc_pt", "fh_acc_pt;P_{t} [GeV/c];Counter", 30, 0., 3.);
   fHists.push_back(fh_acc_pt);
   fh_acc_y = new TH1D("fh_acc_y", "fh_acc_y;Rapidity;Counter", 40, 0., 4.);
   fHists.push_back(fh_acc_y);

   // Numbers in dependence on XY position onto the photodetector.
   fhNofHitsXY = new TH2D("fhNofHitsXY", "fhNofHitsXY;X [cm];Y [cm];Nof hits in ring", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhNofHitsXY);
   fhNofPointsXY = new TH2D("fhNofPointsXY", "fhNofPointsXY;X [cm];Y [cm];Nof points in ring", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhNofPointsXY);
   fhBoverAXY = new TH2D("fhBoverAXY", "fhBoverAXY;X [cm];Y [cm];B/A", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhBoverAXY);
   fhBaxisXY = new TH2D("fhBaxisXY", "fhBaxisXY;X [cm];Y [cm];B axis [cm]", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhBaxisXY);
   fhAaxisXY = new TH2D("fhAaxisXY", "fhAaxisXY;X [cm];Y [cm];A axis [cm]", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhAaxisXY);
   fhRadiusXY = new TH2D("fhRadiusXY", "fhRadiusXY;X [cm];Y [cm];Radius [cm]", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhRadiusXY);
   fhCounterXY = new TH2D("fhCounterXY", "fhCounterXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhCounterXY);

   fhHitsXY1Gev = new TH2D("fhHitsXY1Gev", "fhHitsXY1Gev;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHists.push_back(fhHitsXY1Gev);

   DrawSummaryPlotsTemp();
}

void CbmRichGeoTest::DrawSummaryPlotsTemp()
{
   TCanvas *c1 = CreateCanvas("chi2_mean_fit_vs_bfield_scale", "chi2_mean_fit_vs_bfield_scale", 600, 600);
   Double_t x[5]={0.015*0.2, 0.015*0.5, 0.015*0.7, 0.015*1., 0.015*1.5};
   Double_t y1[5]={0.02, 0.021, 0.025, 0.029, 0.038};
   TGraph* gr1 = new TGraph(5, x, y1);
   gr1->GetXaxis()->SetTitle("Field integral in RICH [Tm]");
   gr1->GetYaxis()->SetTitle("a_{fit}^{#Chi^{2}.mean}");
   TF1 *f1 = new TF1("f1", "[0]*x+[1]", 0., 210.);
   f1->SetLineColor(kBlack);
   gr1->GetXaxis()->SetRangeUser(10., 210.);
   DrawGraph(gr1, kLinear, kLinear, "AP");
   gr1->SetMarkerColor(4);
   gr1->SetMarkerSize(2.5);
   gr1->SetMarkerStyle(21);
   gr1->Fit(f1, "R");
   gr1->GetXaxis()->SetRangeUser(10., 210.);
   f1->SetLineColor(kBlack);
   double p0 = f1->GetParameter(0);
   double p1 = f1->GetParameter(1);
   stringstream ss;
   ss.precision(3);
   ss << "y="<<lit::NumberToString(p0, 1)<<"*x+"<<lit::NumberToString(p1, 1);
   TLegend* leg = new TLegend(0.15, 0.9, 0.85, 0.99);
   leg->AddEntry(new TH2D(), ss.str().c_str(), "");
   leg->SetFillColor(kWhite);
   leg->SetFillStyle(0);
   leg->SetBorderSize(0);
   leg->Draw();


   TCanvas *c2 = CreateCanvas("dr_rms_fit_vs_bfield_scale", "dr_rms_fit_vs_bfield_scale", 600, 600);
   Double_t y2[5]={0.025, 0.026, 0.033, 0.045, 0.078};
   TGraph* gr2 = new TGraph(5, x, y2);
   gr2->GetXaxis()->SetTitle("Field integral in RICH [Tm]");
   gr2->GetYaxis()->SetTitle("a_{fit}^{dR.RMS}");
   TF1 *f2 = new TF1("f2", "[0]*x+[1]", 0., 210.);
   f2->SetLineColor(kBlack);
   DrawGraph(gr2, kLinear, kLinear, "AP");
   gr2->SetMarkerColor(4);
   gr2->SetMarkerSize(2.5);
   gr2->SetMarkerStyle(21);
   gr2->Fit(f2, "R");
   gr2->GetXaxis()->SetRange(0., 210.);
   f2->SetLineColor(kBlack);
   double p10 = f2->GetParameter(0);
   double p11 = f2->GetParameter(1);
   stringstream ss2;
   ss2 << "y="<<lit::NumberToString(p10, 1)<<"*x+"<<lit::NumberToString(p11, 1);
   TLegend* leg2 = new TLegend(0.15, 0.9, 0.85, 0.99);
   leg2->AddEntry(new TH2D(), ss2.str().c_str(), "");
   leg2->SetFillColor(kWhite);
   leg2->SetFillStyle(0);
   leg2->SetBorderSize(0);
   leg2->Draw();
}

void CbmRichGeoTest::FillMcHist()
{
   for (Int_t i = 0; i < fMCTracks->GetEntriesFast(); i++){
      CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(i);
      if (!mcTrack) continue;
      Int_t motherId = mcTrack->GetMotherId();
      Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());

      if (pdg != 11 || motherId != -1) continue; // only primary electrons

      fh_mc_mom->Fill(mcTrack->GetP());
      fh_mc_pt->Fill(mcTrack->GetPt());
      fh_mc_y->Fill(mcTrack->GetRapidity());
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
	      fh_acc_mom->Fill(momentum);
	      fh_acc_pt->Fill(pt);
	      fh_acc_y->Fill(rapidity);
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
	   fhBoverAXY->Fill(xc, yc, b/a);
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
   fhChi2EllipseVsMom[histIndex]->Fill(momentum, ring->GetChi2()/ring->GetNofHits());
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
   fhChi2CircleVsMom[histIndex]->Fill(momentum, ring->GetChi2()/ring->GetNofHits());

   for (int iH = 0; iH < ring->GetNofHits(); iH++){
      double xh = ring->GetHit(iH).fX;
      double yh = ring->GetHit(iH).fY;
      double dr = radius - sqrt((xcCircle - xh)*(xcCircle - xh) + (ycCircle - yh)*(ycCircle - yh));
      fhDRVsMom[histIndex]->Fill(momentum, dr);
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

      Int_t trackId = point->GetTrackID();
      if (trackId < 0) continue;
      CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(trackId);
      if (NULL == mcTrack) continue;
      CbmMCTrack* mcTrack2 = (CbmMCTrack*)fMCTracks->At(mcTrack->GetMotherId());
      if (NULL == mcTrack2) continue;
      if (mcTrack2->GetP() < 1.){
         fhHitsXY1Gev->Fill(hit->GetX(), hit->GetY());
      }
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
   Int_t nofMc = (Int_t)fh_mc_mom->GetEntries();
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

void CbmRichGeoTest::CreateH2MeanRms(
      TH2D* hist,
      TH1D** meanHist,
      TH1D** rmsHist)
{
   *meanHist = (TH1D*)hist->ProjectionX( (string(hist->GetName()) + "_mean").c_str() )->Clone();
   (*meanHist)->GetYaxis()->SetTitle( ("Mean and RMS. " + string(hist->GetYaxis()->GetTitle()) ).c_str());
   *rmsHist = (TH1D*)hist->ProjectionX((string(hist->GetName()) + "_rms").c_str() )->Clone();
   (*rmsHist)->GetYaxis()->SetTitle( ("RMS. "+ string(hist->GetYaxis()->GetTitle()) ).c_str());
   for (Int_t i = 1; i <= hist->GetXaxis()->GetNbins(); i++){
      stringstream ss;
      ss << string(hist->GetName()) << "_py" << i;
      TH1D* pr = hist->ProjectionY(ss.str().c_str(), i, i);
      if (*meanHist == NULL || pr == NULL) continue;
      (*meanHist)->SetBinContent(i, pr->GetMean());
      (*meanHist)->SetBinError(i, pr->GetRMS());
      (*rmsHist)->SetBinContent(i, pr->GetRMS());
   }
}

void CbmRichGeoTest::DrawH2MeanRms(
      TH2D* hist,
      const string& canvasName)
{
   TH1D* mean, *rms;
   CreateH2MeanRms(hist, &mean, &rms);
   TCanvas *c = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 900, 900);
   c->Divide(2, 2);
   c->cd(1);
   DrawH2(hist);
   c->cd(2);
   TH1D* py = hist->ProjectionY( (string(hist->GetName())+ "_py" ).c_str() );
   py->GetYaxis()->SetTitle("Counter");
   DrawH1(py);
   string txt1 = lit::NumberToString<Double_t>(py->GetMean(), 2) + " / "
         + lit::NumberToString<Double_t>(py->GetRMS(), 2);
   TLatex text;
   text.SetTextAlign(21);
   text.SetTextSize(0.05);
   text.DrawTextNDC(0.5, 0.92, txt1.c_str());
   gPad->SetLogy(true);
   c->cd(3);
   DrawH1(mean);
   c->cd(4);
   DrawH1(rms);
}

void CbmRichGeoTest::FitH1OneOverX(
      TH1D* hist,
      double xMinFit,
      double xMaxFit,
      double xMin,
      double xMax,
      const string& canvasName)
{
   cout << "-I-" << canvasName << endl;
   cout << "bins = " << hist->GetNbinsX() << endl;
   for (int i = 1; i < hist->GetNbinsX() - 1;i++){
      double c0 = hist->GetBinContent(i);
      double c1 = hist->GetBinContent(i+1);
      cout << i << "  " << c0 << endl;
      if (c0 > 0 && c0 <= c1){
         xMinFit = hist->GetBinCenter(i+1);
         cout <<endl<<endl<<endl<< "-I- New xMinFit = " << xMinFit << endl << endl;
      }
      if (c0 > c1) {
         break;
      }
   }
   TCanvas *cDrRmsFit = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 600);
   TF1 *f1 = new TF1("f1", "[0]/x+[1]", xMinFit, xMaxFit);
   f1->SetLineColor(kBlack);
   DrawH1(hist);
   hist->Fit(f1, "R");
   hist->GetXaxis()->SetRangeUser(xMin, xMax);
   f1->SetLineColor(kBlack);

   double p0 = f1->GetParameter(0);
   double p1 = f1->GetParameter(1);
   stringstream ss;
   ss.precision(3);
   ss << "y="<<lit::NumberToString(p0, 1)<<"/P+"<<lit::NumberToString(p1, 1);
   TLegend* leg = new TLegend(0.15, 0.9, 0.85, 0.99);
   leg->AddEntry(new TH2D(), ss.str().c_str(), "");
   leg->SetFillColor(kWhite);
   leg->SetFillStyle(0);
   leg->SetBorderSize(0);
   leg->Draw();
}

void CbmRichGeoTest::DrawHist()
{
   SetDefaultDrawStyle();

   TCanvas *cHitsXY = CreateCanvas("richgeo_hits_xy", "richgeo_hits_xy", 800, 800);
   DrawH2(fhHitsXY);

   TCanvas *cPointsXY = CreateCanvas("richgeo_points_xy", "richgeo_points_xy", 800, 800);
   DrawH2(fhPointsXY);

   for (int i = 0; i < 2; i++){
      stringstream ss;
      if (i == 0) ss << "richgeo_hits_";
      if (i == 1) ss << "richgeo_points_";
      TCanvas *cEllipse = CreateCanvas((ss.str()+"ellipse").c_str(), (ss.str()+"ellipse").c_str(), 800, 400);
      cEllipse->Divide(2, 1);
      cEllipse->cd(1);
      DrawH1(fhBoverA[i]);
      gPad->SetLogy(true);
      cEllipse->cd(2);
      DrawH2(fhXcYcEllipse[i]);

      DrawH2MeanRms(fhChi2EllipseVsMom[i],  ss.str() + "chi2_ellipse_vs_mom");
      DrawH2MeanRms(fhAaxisVsMom[i], ss.str() + "a_vs_mom");
      DrawH2MeanRms(fhBaxisVsMom[i], ss.str() + "b_vs_mom");

      TCanvas *cCircle = CreateCanvas((ss.str()+"circle").c_str(), (ss.str()+"circle").c_str(), 800, 400);
      cCircle->Divide(2,1);
      cCircle->cd(1);
      DrawH1(fhNofHits[i]);
      gPad->SetLogy(true);
      cCircle->cd(2);
      DrawH2(fhXcYcCircle[i]);

      DrawH2MeanRms(fhChi2CircleVsMom[i], ss.str() + "chi2_circle_vs_mom");
      DrawH2MeanRms(fhRadiusVsMom[i], ss.str() + "r_vs_mom");
      DrawH2MeanRms(fhDRVsMom[i], ss.str() + "dr_vs_mom");

      TH1D* meanDr, *rmsDr;
      CreateH2MeanRms(fhDRVsMom[i], &meanDr, &rmsDr);
      FitH1OneOverX(rmsDr, 0.1, 2.9, 0.0, 3.0, ss.str() + "dr_rms_vs_mom_fit");

      TH1D* meanChi2, *rmsChi2;
      CreateH2MeanRms(fhChi2CircleVsMom[i], &meanChi2, &rmsChi2);
      FitH1OneOverX(meanChi2, 0.1, 2.9, 0.0, 3.0, ss.str() + "chi2_circle_mean_vs_mom_fit");
   }

   TCanvas *cDiff2DEllipse = CreateCanvas("richgeo_diff2d_ellipse", "richgeo_diff2d_ellipse", 800, 800);
   cDiff2DEllipse->Divide(2,2);
   cDiff2DEllipse->cd(1);
   DrawH2(fhDiffAaxis);
   cDiff2DEllipse->cd(2);
   DrawH2(fhDiffBaxis);
   cDiff2DEllipse->cd(3);
   DrawH2(fhDiffXcEllipse);
   cDiff2DEllipse->cd(4);
   DrawH2(fhDiffYcEllipse);

   TCanvas *cDiff2DCircle = CreateCanvas("richgeo_diff2d_circle", "richgeo_diff2d_circle", 1200, 400);
   cDiff2DCircle->Divide(3,1);
   cDiff2DCircle->cd(1);
   DrawH2(fhDiffXcCircle);
   cDiff2DCircle->cd(2);
   DrawH2(fhDiffYcCircle);
   cDiff2DCircle->cd(3);
   DrawH2(fhDiffRadius);

   TCanvas *cDiff1DEllipse = CreateCanvas("richgeo_diff1d_ellipse", "richgeo_diff1d_ellipse", 800, 800);
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

   TCanvas *cDiff1DCircle = CreateCanvas("richgeo_diff1d_circle", "richgeo_diff1d_circle", 1200, 400);
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

   TCanvas *cHits = CreateCanvas("richgeo_hits", "richgeo_hits", 1200, 600);
   cHits->Divide(2,1);
   cHits->cd(1);
   DrawH1(fhDiffXhit);
   cHits->cd(2);
   DrawH1(fhDiffYhit);

   TCanvas *cFitEff = CreateCanvas("richgeo_fit_eff", "richgeo_fit_eff", 1200, 400);
   cFitEff->Divide(3,1);
   cFitEff->cd(1);
   DrawH1( list_of((TH1D*)fhNofHitsAll->Clone())((TH1D*)fhNofHitsCircleFit->Clone())((TH1D*)fhNofHitsEllipseFit->Clone()),
         list_of("All")("Circle fit")("Ellipse fit"), kLinear, kLog, true, 0.7, 0.7, 0.99, 0.99);
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

   TCanvas *cAccEff = CreateCanvas("richgeo_acc_eff", "richgeo_acc_eff", 1200, 800);
   cAccEff->Divide(3,2);
   cAccEff->cd(1);
   DrawH1(list_of(fh_mc_mom)(fh_acc_mom), list_of("MC")("ACC"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cAccEff->cd(2);
   DrawH1(list_of(fh_mc_pt)(fh_acc_pt), list_of("MC")("ACC"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cAccEff->cd(3);
   DrawH1(list_of(fh_mc_y)(fh_acc_y), list_of("MC")("ACC"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);

   TH1D* pxEff = DivideH1((TH1D*)fh_acc_mom->Clone(), (TH1D*)fh_mc_mom->Clone(), "pxEff", "", "P [GeV/c]", "Efficiency");
   TH1D* pyEff = DivideH1((TH1D*)fh_acc_pt->Clone(), (TH1D*)fh_mc_pt->Clone(), "pyEff", "", "P_{t} [GeV/c]", "Efficiency");
   TH1D* pzEff = DivideH1((TH1D*)fh_acc_y->Clone(), (TH1D*)fh_mc_y->Clone(), "pzEff", "", "Rapidity", "Efficiency");
   cAccEff->cd(4);
   DrawH1(pxEff);
   TLatex* pxEffTxt = new TLatex(3, 0.5, CalcEfficiency(fh_acc_mom, fh_mc_mom).c_str());
   pxEffTxt->Draw();
   cAccEff->cd(5);
   DrawH1(pyEff);
   cAccEff->cd(6);
   DrawH1(pzEff);

   TCanvas *cAccEffZoom = CreateCanvas("richgeo_acc_eff_zoom", "richgeo_acc_eff_zoom", 1000, 500);
   cAccEffZoom->Divide(2,1);
   cAccEffZoom->cd(1);
   TH1D* fh_mc_mom_clone = (TH1D*)fh_mc_mom->Clone();
   TH1D* fh_acc_mom_clone = (TH1D*)fh_acc_mom->Clone();
   fh_mc_mom_clone->GetXaxis()->SetRangeUser(0., 3.);
   fh_acc_mom_clone->GetXaxis()->SetRangeUser(0., 3.);
   fh_mc_mom_clone->SetMinimum(0.);
   DrawH1(list_of(fh_mc_mom_clone)(fh_acc_mom_clone), list_of("MC")("ACC"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   gPad->SetLogy(false);
   cAccEffZoom->cd(2);
   TH1D* px_eff_clone = (TH1D*) pxEff->Clone();
   px_eff_clone->GetXaxis()->SetRangeUser(0., 3.);
   px_eff_clone->SetMinimum(0.);
   DrawH1(px_eff_clone);

   TCanvas *cNumbersVsXY = CreateCanvas("richgeo_numbers_vs_xy", "richgeo_numbers_vs_xy", 1200, 800);
   cNumbersVsXY->Divide(3,2);
   cNumbersVsXY->cd(1);
   fhNofHitsXY->Divide(fhCounterXY);
   DrawH2(fhNofHitsXY);
   cNumbersVsXY->cd(2);
   fhNofPointsXY->Divide(fhCounterXY);
   DrawH2(fhNofPointsXY);
   cNumbersVsXY->cd(3);
   fhBoverAXY->Divide(fhCounterXY);
   fhBoverAXY->GetZaxis()->SetRangeUser(0.6, 1.0);
   DrawH2(fhBoverAXY);
   cNumbersVsXY->cd(4);
   fhBaxisXY->Divide(fhCounterXY);
   fhBaxisXY->GetZaxis()->SetRangeUser(4., 5.2);
   DrawH2(fhBaxisXY);
   cNumbersVsXY->cd(5);
   fhAaxisXY->Divide(fhCounterXY);
   fhAaxisXY->GetZaxis()->SetRangeUser(4.2, 5.7);
   DrawH2(fhAaxisXY);
   cNumbersVsXY->cd(6);
   fhRadiusXY->Divide(fhCounterXY);
   fhRadiusXY->GetZaxis()->SetRangeUser(4.2, 5.3);
   DrawH2(fhRadiusXY);

   TCanvas *cAccVsMinNofHits = CreateCanvas("richgeo_acc_vs_min_nof_hits", "richgeo_acc_vs_min_nof_hits", 600, 600);
   TH1D* h = CreateAccVsMinNofHitsHist();
   h->GetXaxis()->SetRangeUser(0., 40.0);
   DrawH1(h);

   DrawH2MeanRms(fhRadiusVsNofHits, "richgeo_hits_r_vs_nof_hits");
   DrawH2MeanRms(fhAaxisVsNofHits, "richgeo_hits_a_vs_nof_hits");
   DrawH2MeanRms(fhBaxisVsNofHits, "richgeo_hits_b_vs_nof_hits");

   TCanvas* cHitsXY1Gev = CreateCanvas("richgeo_hits_xy_1gev", "richgeo_hits_xy_1gev", 600,600);
   DrawH2(fhHitsXY1Gev);
}

void CbmRichGeoTest::CreatePTree()
{
   ptree pt;
   pt.put("acc_mean", CalcEfficiency(fh_acc_mom, fh_mc_mom));
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

