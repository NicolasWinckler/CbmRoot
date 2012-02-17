/**
 * \file CbmRichTrainAnnelectrons.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

#include "CbmRichTrainAnnElectrons.h"

#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"
#include "CbmTrackMatch.h"
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "cbm/utils/CbmLitDrawHist.h"

#include "TString.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"
#include "TMultiLayerPerceptron.h"

#include <boost/assign/list_of.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <string>

class CbmStsTrack;

using std::cout;
using std::endl;
using std::vector;
using std::fabs;
using std::string;
using boost::assign::list_of;

CbmRichTrainAnnElectrons::CbmRichTrainAnnElectrons():
   fEventNum(0),

   fRichHits(NULL),
   fRichRings(NULL),
   fRichPoints(NULL),
   fMCTracks(NULL),
   fRichRingMatches(NULL),
   fRichProj(NULL),
   fStsTrackMatches(NULL),
   fGlobalTracks(NULL),
   fStsTracks(NULL),

   fMinNofHitsInRichRing(5),
   fQuota(0.6),
   fMaxNofTrainSamples(1500),

   fNofPiLikeEl(0),
   fNofElLikePi(0),
   fAnnCut(-0.5),

   fhAnnOutput(),
   fhCumProb(),

   fRElIdParams(),

   fhAaxis(),
   fhBaxis(),
   fhAaxisCor(),
   fhBaxisCor(),
   fhDist(),
   fhNofHits(),
   fhChi2(),
   fhRadPos(),
   fhAaxisVsMom(),
   fhBaxisVsMom(),
   fhPhiVsRadAng(),

   fHists()
{
   fhAaxis.resize(2);
   fhBaxis.resize(2);
   fhAaxisCor.resize(2);
   fhBaxisCor.resize(2);
   fhDist.resize(2);
   fhNofHits.resize(2);
   fhChi2.resize(2);
   fhRadPos.resize(2);
   fhAaxisVsMom.resize(2);
   fhBaxisVsMom.resize(2);
   fhPhiVsRadAng.resize(2);
   fhAnnOutput.resize(2);
   fhCumProb.resize(2);
   fRElIdParams.resize(2);
   string ss;
   for (int i = 0; i < 2; i++){
      if (i == 0) ss = "Electron";
      if (i == 1) ss = "Pion";
      // difference between electrons and pions
      fhAaxis[i] = new TH1D(string("fhAaxis"+ss).c_str(), "fhAaxis;A axis [cm];Counter", 30,3,8);
      fHists.push_back(fhAaxis[i]);
      fhBaxis[i] = new TH1D(string("fhBAxis"+ss).c_str(), "fhBAxis;B axis [cm];Counter", 30,3,8);
      fHists.push_back(fhBaxis[i]);
      fhAaxisCor[i] = new TH1D(string("fhAaxisCor"+ss).c_str(), "fhAaxisCor;A axis [cm];Counter", 30,3,8);
      fHists.push_back(fhAaxisCor[i]);
      fhBaxisCor[i] = new TH1D(string("fhBAxisCor"+ss).c_str(), "fhBAxisCor;B axis [cm];Counter", 30,3,8);
      fHists.push_back(fhBaxisCor[i]);
      fhDist[i] = new TH1D(string("fhDist"+ss).c_str(), "fhDist;Ring-track distance [cm];Counter", 30,0,3);
      fHists.push_back(fhDist[i]);
      fhNofHits[i] = new TH1D(string("fhNofHits"+ss).c_str(), "fhNofHits;Number of hits;Counter", 30,0,45);
      fHists.push_back(fhNofHits[i]);
      fhChi2[i] = new TH1D(string("fhChi2"+ss).c_str(), "fhChi2;#Chi^{2};Counter", 50, 0, 0.5);
      fHists.push_back(fhChi2[i]);
      fhRadPos[i] = new TH1D(string("fhRadPos"+ss).c_str(), "fhRadPos;Radial position [cm];Counter", 150, 0., 150.);
      fHists.push_back(fhRadPos[i]);
      fhAaxisVsMom[i] = new TH2D(string("fhAaxisVsMom"+ss).c_str(), "fhAaxisVsMom;P [GeV/c];A axis [cm]",30,0,15, 30,3,8);
      fHists.push_back(fhAaxisVsMom[i]);
      fhBaxisVsMom[i] = new TH2D(string("fhBAxisVsMom"+ss).c_str(), "fhBAxisVsMom;P [GeV/c];B axis [cm]",30,0,15, 30,3,8);
      fHists.push_back(fhBaxisVsMom[i]);
      fhPhiVsRadAng[i] = new TH2D(string("fhPhiVsRadAng"+ss).c_str(), "fhPhiVsRadAng;Phi [rad];Radial angle [rad]", 50, -2. ,2.,50, 0. , 6.3);
      fHists.push_back(fhPhiVsRadAng[i]);
      // ANN outputs
      fhAnnOutput[i] = new TH1D(string("fhAnnOutput"+ss).c_str(),"ANN output;ANN output;Counter",100, -1.2, 1.2);
      fHists.push_back(fhAnnOutput[i]);
      fhCumProb[i] = new TH1D(string("fhCumProb"+ss).c_str(),"ANN output;ANN output;Cumulative probability",100, -1.2, 1.2);
      fHists.push_back(fhCumProb[i]);
   }
}

CbmRichTrainAnnElectrons::~CbmRichTrainAnnElectrons()
{

}

InitStatus CbmRichTrainAnnElectrons::Init()
{
	cout << "InitStatus CbmRichTrainAnnElectrons::Init()"<<endl;

	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmRichTrainAnnElectrons::Init","RootManager not instantised!");}

	fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
	if ( NULL == fRichHits) { Fatal("CbmRichTrainAnnElectrons::Init","No RichHit array!");}

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if ( NULL == fRichRings) { Fatal("CbmRichTrainAnnElectrons::Init","No RichRing array!");}

	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( NULL == fRichPoints) { Fatal("CbmRichTrainAnnElectrons::Init","No RichPoint array!");}

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMCTracks) { Fatal("CbmRichTrainAnnElectrons::Init","No MCTrack array!");}

	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if ( NULL == fRichRingMatches) { Fatal("CbmRichTrainAnnElectrons::Init","No RichRingMatch array!");}

	fRichProj = (TClonesArray*) ioman->GetObject("RichProjection");
	if ( NULL == fRichProj) { Fatal("CbmRichTrainAnnElectrons::Init","No RichProjection array!");}

	fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if ( NULL == fStsTrackMatches) { Fatal("CbmRichTrainAnnElectrons::Init","No track match array!");}

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if ( NULL == fGlobalTracks) { Fatal("CbmRichTrainAnnElectrons::Init","No global track array!");}

	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if ( NULL == fStsTracks) { Fatal("CbmRichTrainAnnElectrons::Init","No  STSTrack array!");}

	return kSUCCESS;
}

void CbmRichTrainAnnElectrons::Exec(Option_t* option)
{
   cout << endl <<"-I- CbmRichTrainAnnElectrons, event " << fEventNum << endl;
   DiffElandPi();
	fEventNum++;
   cout <<"Nof Electrons = " << fRElIdParams[0].size() << endl;
   cout <<"Nof Pions = " << fRElIdParams[1].size() << endl;
}

void CbmRichTrainAnnElectrons::DiffElandPi()
{
	Int_t nGlTracks = fGlobalTracks->GetEntriesFast();

	for (Int_t iTrack=0; iTrack < nGlTracks; iTrack++) {
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

      Double_t axisACor = ring->GetAaxisCor();
      Double_t axisBCor= ring->GetBaxisCor();

      Int_t lFoundHits = richRingMatch->GetNofTrueHits() + richRingMatch->GetNofWrongHits()
            + richRingMatch->GetNofFakeHits();
      Double_t lPercTrue = 0;
      if (lFoundHits >= 3){
         lPercTrue = (Double_t)richRingMatch->GetNofTrueHits() / (Double_t)lFoundHits;
      }
      Bool_t isTrueFound = (lPercTrue > fQuota);

      RingElectronParam p;
      p.fAaxis = ring->GetAaxis();
      p.fBaxis = ring->GetBaxis();
      p.fPhi = ring->GetPhi();
      p.fRadAngle = ring->GetRadialAngle();
      p.fChi2 = ring->GetChi2()/ring->GetNDF();
      p.fRadPos = ring->GetRadialPosition();
      p.fNofHits = ring->GetNofHits();
      p.fDistance = ring->GetDistance();
      p.fMomentum = momentum;

      // electrons
      if (pdg == 11 && motherId == -1 && isTrueFound &&
            mcIdSts == mcIdRich && mcIdRich != -1){
         fhAaxis[0]->Fill(p.fAaxis);
         fhBaxis[0]->Fill(p.fBaxis);
         fhAaxisCor[0]->Fill(axisACor);
         fhBaxisCor[0]->Fill(axisBCor);
         fhDist[0]->Fill(p.fDistance);
         fhNofHits[0]->Fill(p.fNofHits);
         fhChi2[0]->Fill(p.fChi2);
         fhRadPos[0]->Fill(p.fRadPos);
         fhAaxisVsMom[0]->Fill(momentum, p.fAaxis);
         fhBaxisVsMom[0]->Fill(momentum, p.fBaxis);
         fhPhiVsRadAng[0]->Fill(p.fPhi, p.fRadAngle);
         fRElIdParams[0].push_back(p);
      }
      // pions
      if ( pdg == 211 &&  mcIdRich != -1){
         fhAaxis[1]->Fill(p.fAaxis);
         fhBaxis[1]->Fill(p.fBaxis);
         fhAaxisCor[1]->Fill(axisACor);
         fhBaxisCor[1]->Fill(axisBCor);
         fhDist[1]->Fill(p.fDistance);
         fhNofHits[1]->Fill(p.fNofHits);
         fhChi2[1]->Fill(p.fChi2);
         fhRadPos[1]->Fill(p.fRadPos);
         fhAaxisVsMom[1]->Fill(momentum, p.fAaxis);
         fhBaxisVsMom[1]->Fill(momentum, p.fBaxis);
         fhPhiVsRadAng[1]->Fill(p.fPhi, p.fRadAngle);
         fRElIdParams[1].push_back(p);
      }
	}// global tracks
}

void CbmRichTrainAnnElectrons::TrainAndTestAnn()
{
   TTree *simu = new TTree ("MonteCarlo","MontecarloData");
   Double_t x[9];
   Double_t xOut;

   simu->Branch("x0", &x[0],"x0/D");
   simu->Branch("x1", &x[1],"x1/D");
   simu->Branch("x2", &x[2],"x2/D");
   simu->Branch("x3", &x[3],"x3/D");
   simu->Branch("x4", &x[4],"x4/D");
   simu->Branch("x5", &x[5],"x5/D");
   simu->Branch("x6", &x[6],"x6/D");
   simu->Branch("x7", &x[7],"x7/D");
   simu->Branch("x8", &x[8],"x8/D");
   simu->Branch("xOut", &xOut,"xOut/D");

   for (int j = 0; j < 2; j++){
      for (int i = 0; i < fRElIdParams[j].size(); i++){
         x[0] = fRElIdParams[j][i].fAaxis;
         x[1] = fRElIdParams[j][i].fBaxis;
         x[2] = fRElIdParams[j][i].fPhi;
         x[3] = fRElIdParams[j][i].fRadAngle;
         x[4] = fRElIdParams[j][i].fChi2;
         x[5] = fRElIdParams[j][i].fRadPos;
         x[6] = fRElIdParams[j][i].fNofHits;
         x[7] = fRElIdParams[j][i].fDistance;
         x[8] = fRElIdParams[j][i].fMomentum;

         if (j == 0) xOut = 1.;
         if (j == 1) xOut = -1.;
         simu->Fill();
         if (i >= fMaxNofTrainSamples) break;
      }
   }

   TMultiLayerPerceptron network("x0,x1,x2,x3,x4,x5,x6,x7,x8:5:xOut",simu,"Entry$+1");
   //network.LoadWeights("");
   network.Train(200,"text,update=1");
   //network.DumpWeights("NeuralNet_RingSelection_Weights1.txt");
   //network.Export();

   Double_t params[9];

   fNofPiLikeEl = 0;
   fNofElLikePi = 0;

   for (int j = 0; j < 2; j++){
      for (int i = 0; i < fRElIdParams[j].size(); i++){
         params[0] = fRElIdParams[j][i].fAaxis;
         params[1] = fRElIdParams[j][i].fBaxis;
         params[2] = fRElIdParams[j][i].fPhi;
         params[3] = fRElIdParams[j][i].fRadAngle;
         params[4] = fRElIdParams[j][i].fChi2;
         params[5] = fRElIdParams[j][i].fRadPos;
         params[6] = fRElIdParams[j][i].fNofHits;
         params[7] = fRElIdParams[j][i].fDistance;
         params[8] = fRElIdParams[j][i].fMomentum;

        Double_t netEval = network.Evaluate(0,params);

        //if (netEval > maxEval) netEval = maxEval - 0.01;
       // if (netEval < minEval) netEval = minEval + 0.01;

        fhAnnOutput[j]->Fill(netEval);
        if (netEval >= fAnnCut && j == 1) fNofPiLikeEl++;
        if (netEval < fAnnCut && j == 0) fNofElLikePi++;
     }
   }
}

void CbmRichTrainAnnElectrons::Draw()
{
   cout <<"nof electrons = " << fRElIdParams[0].size() << endl;
   cout <<"nof pions = " << fRElIdParams[1].size() << endl;
   cout <<"Pions like electrons = " << fNofPiLikeEl << endl;
   cout <<"Electrons like pions = " << fNofElLikePi << endl;
   cout <<"ANN cut = " << fAnnCut << endl;

   Double_t cumProbFake = 0.;
   Double_t cumProbTrue = 0.;
   Int_t nofFake = (Int_t)fhAnnOutput[1]->GetEntries();
   Int_t nofTrue = (Int_t)fhAnnOutput[0]->GetEntries();
   for (Int_t i = 1; i <= fhAnnOutput[1]->GetNbinsX(); i++){
      cumProbFake += fhAnnOutput[1]->GetBinContent(i);
      fhCumProb[1]->SetBinContent(i, (Double_t)cumProbFake/nofFake);

      cumProbTrue += fhAnnOutput[0]->GetBinContent(i);
      fhCumProb[0]->SetBinContent(i, 1. - (Double_t)cumProbTrue / nofTrue);
   }

   TCanvas* c1 = new TCanvas("ann_select_ann_output", "ann_select_ann_output", 500, 500);
   DrawH1(list_of(fhAnnOutput[0])(fhAnnOutput[1]), list_of("True")("Fake"),
         kLitLinear, kLitLog, true, 0.8, 0.8, 0.99, 0.99);

   TCanvas* c2 = new TCanvas("ann_select_cum_prob", "ann_select_cum_prob", 500, 500);
   DrawH1(list_of(fhCumProb[0])(fhCumProb[1]), list_of("True")("Fake"),
         kLitLinear, kLitLinear, true, 0.8, 0.8, 0.99, 0.99);

   TCanvas* c3 = new TCanvas("ann_select_params", "ann_select_params", 900, 900);
   c3->Divide(3, 3);
   c3->cd(1);
   DrawH1(list_of(fhAaxis[0])(fhAaxis[1]), list_of("True")("Fake"),
            kLitLinear, kLitLog, true, 0.8, 0.8, 0.99, 0.99);
   c3->cd(2);
   DrawH1(list_of(fhBaxis[0])(fhBaxis[1]), list_of("True")("Fake"),
            kLitLinear, kLitLog, true, 0.8, 0.8, 0.99, 0.99);
   c3->cd(3);
   DrawH1(list_of(fhAaxisCor[0])(fhAaxisCor[1]), list_of("True")("Fake"),
            kLitLinear, kLitLog, true, 0.8, 0.8, 0.99, 0.99);
   c3->cd(4);
   DrawH1(list_of(fhBaxisCor[0])(fhBaxisCor[1]), list_of("True")("Fake"),
            kLitLinear, kLitLog, true, 0.8, 0.8, 0.99, 0.99);
   c3->cd(5);
   DrawH1(list_of(fhDist[0])(fhDist[1]), list_of("True")("Fake"),
            kLitLinear, kLitLog, true, 0.8, 0.8, 0.99, 0.99);
   c3->cd(6);
   DrawH1(list_of(fhNofHits[0])(fhNofHits[1]), list_of("True")("Fake"),
            kLitLinear, kLitLog, true, 0.8, 0.8, 0.99, 0.99);
   c3->cd(7);
   DrawH1(list_of(fhChi2[0])(fhChi2[1]), list_of("True")("Fake"),
            kLitLinear, kLitLog, true, 0.8, 0.8, 0.99, 0.99);
   c3->cd(8);
   DrawH1(list_of(fhRadPos[0])(fhRadPos[1]), list_of("True")("Fake"),
            kLitLinear, kLitLog, true, 0.8, 0.8, 0.99, 0.99);

   TCanvas* c4 = new TCanvas("ann_select_params_2d", "ann_select_params_2d", 600, 900);
   c4->Divide(2, 3);
   c4->cd(1);
   DrawH2(fhAaxisVsMom[0]);
   c4->cd(2);
   DrawH2(fhAaxisVsMom[1]);
   c4->cd(3);
   DrawH2(fhBaxisVsMom[0]);
   c4->cd(4);
   DrawH2(fhBaxisVsMom[1]);
   c4->cd(5);
   DrawH2(fhPhiVsRadAng[0]);
   c4->cd(6);
   DrawH2(fhPhiVsRadAng[1]);
}

void CbmRichTrainAnnElectrons::FinishTask()
{
   TrainAndTestAnn();
   Draw();

   for (int i = 0; i < fHists.size(); i++){
      fHists[i]->Scale(1./fHists[i]->Integral());
      fHists[i]->Write();
   }
}

ClassImp(CbmRichTrainAnnElectrons)
