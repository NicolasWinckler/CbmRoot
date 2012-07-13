
#include "CbmTrdElectronsTrainAnn.h"
#include "TMath.h"
#include "TFile.h"
#include "TCut.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TMVA/PDF.h"
#include "TMVA/Config.h"
#include "TLine.h"
#include "TClonesArray.h"

#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTrdPoint.h"
#include "CbmMCTrack.h"

#include "CbmDrawHist.h"

#include <boost/assign/list_of.hpp>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using std::cout;
using std::endl;
using std::stringstream;
using std::string;
using boost::assign::list_of;

CbmTrdElectronsTrainAnn::CbmTrdElectronsTrainAnn() 
  : fWeightFileDir(""),
    fSigmaError(-1.),
    fIsDoTrain(kTRUE),
    fTransformType(-1),
    fXOut(-1.),
    fAnnCut(-1.),
    fNofInputNeurons(-1),
    fNofHiddenNeurons(-1),
    fMaxEval(-1.),
    fMinEval(-1.),
    fAnnWeightsFile(""),
    fNN(NULL),
    fReader(NULL),
    fIdMethod(),
    fNofAnnEpochs(-1),
    fRandom(),
    fROCGraph(NULL)
{
   fEventNum = 0;

   fIsDoTrain = true;
   fAnnCut = 0.9789;
   fIdMethod == kANN;
   fTransformType = 1;
   fAnnWeightsFile = "trd_elid_ann_weights.txt";
   fNofInputNeurons = 12;
   fNofHiddenNeurons = 12;
   fNofAnnEpochs = 250;
   fNofTrdLayers = 12;
   fMaxNofTrainSamples = 1000;

   fEloss.resize(2);
   fhSumEloss.resize(2);
   fhEloss.resize(2);
   fhdEdX.resize(2);
   fhTR.resize(2);
   fhNofTRLayers.resize(2);
   fhNofHits.resize(2);
   string s;
   for (int i = 0; i < 2; i++){
      if (i == 0) s = "El";
      if (i == 1) s = "Pi";
      fhSumEloss[i] = new TH1D(("fhSumEloss"+s).c_str(),"fhSumEloss;Energy loss [GeV];Counters",100, 0, 30e-5);
      fHists.push_back(fhSumEloss[i]);
      fhEloss[i] = new TH1D(("fhEloss"+s).c_str(),"fhEloss;Energy loss [GeV];Counters",100, 0, 30e-6);
      fHists.push_back(fhEloss[i]);
      fhdEdX[i] = new  TH1D(("fhdEdX"+s).c_str(),"fhdEdX;dEdX [GeV];Counters",100, 0, 30e-6);
      fHists.push_back(fhdEdX[i]);
      fhTR[i] = new TH1D(("fhTR"+s).c_str(),"fhTR;Transition radiation [GeV];Counters",100, 0, 30e-6);
      fHists.push_back(fhTR[i]);
      fhNofTRLayers[i] = new TH1D(("fhNofTRLayers"+s).c_str(),"fhNofTRLayers;Nof layers with TR;Counters",13, 0., 13);
      fHists.push_back(fhNofTRLayers[i]);
      fhNofHits[i] = new TH1D(("fhNofHits"+s).c_str(),"fhNofHits;Nof hits;Counters",13, 0., 13.);
      fHists.push_back(fhNofHits[i]);
   }

   Double_t histMax[]={6e-6, 8e-6, 12e-6, 14e-6, 16e-6, 16e-6, 20e-6, 26e-6, 30e-6, 40e-6, 50e-6, 90e-6};
   //Double_t histMax[]={10e-6, 14e-6, 20e-6, 24e-6, 28e-6, 30e-6, 36e-6, 44e-6, 50e-6, 60e-6, 70e-6, 130e-6};
   Int_t nofSortBins = 10000;
   fhElossSort.resize(2);
   fhCumProbSort.resize(2);
   fhElossPdfSort.resize(2);
   for (int j = 0; j < 2; j++){
      fhElossSort[j].resize(12);
      fhCumProbSort[j].resize(12);
      fhElossPdfSort[j].resize(12);
      for (Int_t i = 0; i < 12; i++){
         std::stringstream ss1;
         if (j == 0) ss1 << "fhElossSortEl" << i;
         if (j == 1) ss1 << "fhElossSortPi" << i;
         fhElossSort[j][i] = new TH1D(ss1.str().c_str(), (ss1.str()+";Energy loss, GeV;Counters").c_str(), nofSortBins, 0, histMax[i]);

         std::stringstream ss2;
         if (j == 0) ss2 << "fhCumProbSortEl" << i;
         if (j == 1) ss2 << "fhCumProbSortPi" << i;
         fhCumProbSort[j][i] = new TH1D(ss2.str().c_str(), (ss2.str()+";Energy loss, GeV;Cumulative probability").c_str(), nofSortBins, 0, histMax[i]);
      }
   }

   // Histograms for algorithm testing
   Int_t nofBins = 2000;
   fMaxEval = 1.3;
   fMinEval = -1.3;
   fhAnnOutput.resize(2);
   fhCumProbAnnOutput.resize(2);
   fhAnnInput.resize(2);
   for (Int_t i = 0; i < 2; i++){
      s = (i == 0)? "El": "Pi";
      fhAnnOutput[i] = new TH1D(("fhAnnOutput"+s).c_str(),"fhAnnOutput;ANN output;Counter",nofBins, fMinEval, fMaxEval);
      fhCumProbAnnOutput[i] = new TH1D(("fhCumProbAnnOutput"+s).c_str(),"fhCumProbAnnOutput;ANN output;Cumulative probability",nofBins, fMinEval, fMaxEval);

      fhAnnInput[i].resize(12);
      for (Int_t j = 0; j < 12; j++){
         stringstream ss;
         ss << s << j;
         fhAnnInput[i][j] = new TH1D(("fhAnnInput"+ss.str()).c_str(), "fhAnnInput", 100, -1.1, 1.1);
      }
   }

   fRandom = new TRandom();
}

CbmTrdElectronsTrainAnn::~CbmTrdElectronsTrainAnn()
{

}

InitStatus CbmTrdElectronsTrainAnn::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("-E- CbmTrdElectronsQa::Init","RootManager not instantised!"); }

   fMCTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
   if (NULL == fMCTracks) {Fatal("-E- CbmTrdElectronsQa::Init","No MCTrack array!");}

   fTrdPoints  = (TClonesArray*) ioman->GetObject("TrdPoint");
   if (NULL == fTrdPoints) {Fatal("-E- CbmTrdElectronsQa::Init","No TRDPoint array!");}

   fTrdTracks  = (TClonesArray*) ioman->GetObject("TrdTrack");
   if (NULL == fTrdTracks) {Fatal("-E- CbmTrdElectronsQa::Init","No TrdTrack array!");}

   fTrdTrackMatches  = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
   if (NULL == fTrdTrackMatches) {Fatal("-E- CbmTrdElectronsQa::Init","No TrdTrackMatch array!");}

   fTrdHits  = (TClonesArray*) ioman->GetObject("TrdHit");
   if (NULL == fTrdHits) {Fatal("-E- CbmTrdElectronsQa::Init","No TRDHit array!");}

   return kSUCCESS;
}

void CbmTrdElectronsTrainAnn::Exec(
      Option_t* opt)
{
   fEventNum++;
   cout << "CbmTrdElectronsTrainAnn, event " << fEventNum << endl;

   FillElossVector();
   FillElossAnaHist();
   SortElossAndFillHist();

   cout << "Nof electrons = " << fEloss[0].size() << endl;
   cout << "Nof pions = " << fEloss[1].size() << endl;
}

void CbmTrdElectronsTrainAnn::Finish()
{
   FillCumProbHist();
   Run();
   Draw();


   for (int i = 0; i < fHists.size(); i++){
      fHists[i]->Write();
   }
}

void CbmTrdElectronsTrainAnn::FillElossVector()
{
   Int_t nofTrdTracks = fTrdTracks->GetEntries();
   for(Int_t iTrdTrack=0; iTrdTrack < nofTrdTracks; iTrdTrack++){
      CbmTrdTrack* trdtrack = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
      Int_t nHits = trdtrack->GetNofHits();
      //if (nHits < 12) continue;

      CbmTrackMatch* trdmatch = (CbmTrackMatch*) fTrdTrackMatches->At(iTrdTrack);
      Int_t iMC = trdmatch->GetMCTrackId();
      if (iMC < 0 || iMC > fMCTracks->GetEntriesFast())  continue;

      CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(iMC);
      Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
      Double_t momMC = mctrack->GetP();
      Double_t motherId = mctrack->GetMotherId();

      ///only primary tracks generated by BoxGenerator
      if (motherId != -1)  continue;

      int iP = 0; //[0] = electron
      if (pdg == 211) iP = 1; //[1] = pion
      vector<ElossInfo> v;
      fEloss[iP].push_back(v);
      for (Int_t iHit = 0; iHit < trdtrack->GetNofHits(); iHit++) {
         Int_t hitIndex = trdtrack->GetHitIndex(iHit);
         CbmTrdHit* trdhit = (CbmTrdHit*) fTrdHits->At(hitIndex);
         ElossInfo e;
         e.fEloss = trdhit->GetELoss();
         e.fTR = trdhit->GetELossTR();
         e.fdEdX = trdhit->GetELossdEdX();
         fEloss[iP][fEloss[iP].size() - 1].push_back(e);
      } //iHit
   }//iTrdTrack
}

void CbmTrdElectronsTrainAnn::FillElossAnaHist()
{
   // [0]=electron, [1]=pion
   for (int i = 0; i < 2; i++){
      for(int iT = 0; iT < fEloss[i].size(); iT++){
         Int_t nHits = fEloss[i][iT].size();
         double sumEloss = 0.;
         int nofTRLayers = 0;
         for (int iH = 0; iH < nHits; iH++){
            sumEloss += fEloss[i][iT][iH].fEloss;
            fhEloss[i]->Fill(fEloss[i][iT][iH].fEloss);
            fhdEdX[i]->Fill(fEloss[i][iT][iH].fdEdX);
            fhTR[i]->Fill(fEloss[i][iT][iH].fTR);
            if (fEloss[i][iT][iH].fTR >= 1e-9) nofTRLayers++;
         }// iH
         fhNofHits[i]->Fill(nHits);
         fhSumEloss[i]->Fill(sumEloss);
         fhNofTRLayers[i]->Fill(nofTRLayers);
      }//iT
   }
}

void CbmTrdElectronsTrainAnn::SortElossAndFillHist()
{
   vector<double> v;
   v.resize(12);
   for (int i = 0; i < 2; i++){
      for (int iT = 0; iT < fEloss[i].size(); iT++){
         Int_t nofHits = fEloss[i][iT].size();
         for (int iH = 0; iH < nofHits; iH++){
            v[iH] = fEloss[i][iT][iH].fEloss;
         }
         std::sort(v.begin(), v.end());
         for (int iH = 0; iH < nofHits; iH++){
            fhElossSort[i][iH]->Fill(v[iH]);
         }
      }//iT
   }
}

void CbmTrdElectronsTrainAnn::FillCumProbHist()
{
   for (int i = 0; i < 2; i++){
      for (int iL = 0; iL < 12; iL++){
         double cumProb = 0.;
         double nofTr = fhElossSort[i][iL]->GetEntries();
         for (int iBin = 1; iBin <= fhElossSort[i][iL]->GetNbinsX(); iBin++) {
            cumProb += fhElossSort[i][iL]->GetBinContent(iBin);
            fhCumProbSort[i][iL]->SetBinContent(iBin, cumProb/nofTr);
         }
         // fill PDF histograms for sorted energy losses
         fhElossPdfSort[i][iL] = (TH1D*)fhElossSort[i][iL]->Clone();
         fhElossPdfSort[i][iL]->Rebin(20);
         fhElossPdfSort[i][iL]->Scale(1./fhElossPdfSort[i][iL]->Integral());
      }
   }
}

void CbmTrdElectronsTrainAnn::Run()
{
	if (fIdMethod == kBDT || fIdMethod == kANN){
		if (fIsDoTrain) DoTrain();
		DoTest();
	}else if (fIdMethod == kMEDIANA){
		DoTest();
	}else if (fIdMethod == kLIKELIHOOD){
		DoTest();
	}
}

void CbmTrdElectronsTrainAnn::Transform()
{
	if (fIdMethod == kBDT || fIdMethod == kANN){
		if (fTransformType == 1){
			Transform1();
		} else if (fTransformType == 2){
			Transform2();
		}
	}
}

void CbmTrdElectronsTrainAnn::Transform1()
{
	Double_t ANNCoef1 = 1.06;
	Double_t ANNCoef2 = 0.57;
	//Double_t ANNCoef1[] = {1.04,1.105,1.154,1.277,1.333,1.394,1.47,1.50,1.54,1.58};
	//Double_t ANNCoef2[] = {0.548,0.567,0.585,0.63,0.645,0.664,0.69,0.705,0.716,0.723};
	for (Int_t i = 0; i < fAnnInput.size(); i++) {
	   fAnnInput[i] = fAnnInput[i]*1e6;
	   fAnnInput[i] = (fAnnInput[i] - ANNCoef1)/ANNCoef2 - 0.225;
	}
	sort(fAnnInput.begin(), fAnnInput.end());
	for (Int_t i = 0; i < fAnnInput.size(); i++){
	   fAnnInput[i] = TMath::LandauI(fAnnInput[i]);
	}
}

void CbmTrdElectronsTrainAnn::Transform2()
{
	sort(fAnnInput.begin(), fAnnInput.end());

	Int_t size = fAnnInput.size();
	for (Int_t j = 0; j < size; j++) {
		Int_t binNumEl = fhElossPdfSort[0][j]->FindBin(fAnnInput[j]);
		if (binNumEl > fhElossPdfSort[0][j]->GetNbinsX())
			binNumEl = fhElossPdfSort[0][j]->GetNbinsX();
		Double_t probEl = fhElossPdfSort[0][j]->GetBinContent(binNumEl);

		Int_t binNumPi = fhElossPdfSort[1][j]->FindBin(fAnnInput[j]);
		if (binNumPi > fhElossPdfSort[1][j]->GetNbinsX())
			binNumPi = fhElossPdfSort[1][j]->GetNbinsX();
		Double_t probPi = fhElossPdfSort[1][j]->GetBinContent(binNumPi);

		if (TMath::IsNaN(probPi/(probEl+probPi))){
		   fAnnInput[j] = 0.;
		}else{
		   fAnnInput[j] = probPi/(probEl+probPi);
		}
	}
}

Double_t CbmTrdElectronsTrainAnn::Likelihood()
{
	Double_t lPi = 1.;
	Double_t lEl = 1.;
   sort(fAnnInput.begin(), fAnnInput.end());
   for (Int_t j = 0; j < fAnnInput.size(); j++) {
      Int_t binNumEl = fhElossPdfSort[0][j]->FindBin(fAnnInput[j]);
		if (binNumEl > fhElossPdfSort[0][j]->GetNbinsX())
			binNumEl = fhElossPdfSort[0][j]->GetNbinsX();
		Double_t probEl = fhElossPdfSort[0][j]->GetBinContent(binNumEl);
		lEl = lEl*probEl;

		Int_t binNumPi = fhElossPdfSort[1][j]->FindBin(fAnnInput[j]);
		if (binNumPi > fhElossPdfSort[1][j]->GetNbinsX())
			binNumPi = fhElossPdfSort[1][j]->GetNbinsX();
		Double_t probPi = fhElossPdfSort[1][j]->GetBinContent(binNumPi);
		lPi = lPi*probPi;
	}
	return lEl/(lEl + lPi);
}

Double_t CbmTrdElectronsTrainAnn::Eval(
      Bool_t isEl)
{
	if (fIdMethod == kBDT){
		return fReader->EvaluateMVA("BDT");
	}else if (fIdMethod == kANN){
		Double_t par[fNofInputNeurons];
		for (UInt_t i = 0; i < fAnnInput.size(); i++) par[i] = fAnnInput[i];
		return fNN->Evaluate(0, par);
	}else if (fIdMethod == kMEDIANA){
		sort(fAnnInput.begin(), fAnnInput.end());
		Double_t eval = (fAnnInput[5] + fAnnInput[6])/2.;
		if (eval > 3.9e-6) return 1.;
		return -1.;
	}else if (fIdMethod == kLIKELIHOOD){
		return Likelihood();
	}
	return -1.;
}


void CbmTrdElectronsTrainAnn::DoTrain()
{
   fAnnInput.clear();
   fAnnInput.resize(fNofTrdLayers);

	TTree* simu = CreateTree();
   for (Int_t i = 0; i < 2; i++) {
      for (Int_t iT = 0; iT < fEloss[i].size() - 2; iT++) {
         for (int iH = 0; iH < fNofTrdLayers; iH++){
            fAnnInput[iH] = fEloss[i][iT][iH].fEloss;
         }
         fXOut = (i == 0)? 1.: -1.;
         Transform();
         simu->Fill();
         if (iT >= fMaxNofTrainSamples) break;
      }//iT
   }//i

   if (fIdMethod == kANN){
      if (NULL != fNN) delete fNN;
      TString mlpString = CreateAnnString();
      cout << "-I- create ANN: "<< mlpString << endl;
      cout << "-I- number of training epochs = " << fNofAnnEpochs << endl;
      fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
      fNN->Train(fNofAnnEpochs, "+text,update=10");
      fNN->DumpWeights((fWeightFileDir+"/"+fAnnWeightsFile).c_str());
   } else if (fIdMethod == kBDT) {
      TMVA::Factory* factory = CreateFactory(simu);
      (TMVA::gConfig().GetIONames()).fWeightFileDir = fWeightFileDir;
      TCut mycuts = "";
      TCut mycutb = "";
      factory->PrepareTrainingAndTestTree(mycuts, mycutb,"SplitMode=Random:NormMode=NumEvents:!V");
      //factory->BookMethod(TMVA::Types::kTMlpANN, "TMlpANN","!H:!V:NCycles=50:HiddenLayers=N+1");
      stringstream ss;
      ss << "nTrain_Signal=" << fMaxNofTrainSamples - 500 <<":nTrain_Background="
            << fMaxNofTrainSamples - 500 <<":nTest_Signal=0:nTest_Background=0";
      factory->PrepareTrainingAndTestTree("", ss.str().c_str());
      factory->BookMethod(TMVA::Types::kBDT, "BDT",
            "!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=4.5");
      factory->TrainAllMethods();
      //factory->TestAllMethods();
      //factory->EvaluateAllMethods();
   }
}

void CbmTrdElectronsTrainAnn::DoPreTest()
{
	cout << "-I- Start pretesting " << endl;
	if (fIdMethod == kBDT){
		cout << "-I- IdMethod = kBDT " << endl;
	}else if (fIdMethod == kANN){
		cout << "-I- IdMethod = kANN " << endl;
	}else if (fIdMethod == kMEDIANA){
		cout << "-I- IdMethod = kMEDIANA " << endl;
	}else if (fIdMethod == kLIKELIHOOD){
		cout << "-I- IdMethod = kLIKELIHOOD " << endl;
	}

	fAnnInput.clear();
	fAnnInput.resize(fNofTrdLayers);

	if (fIdMethod == kBDT){
		fReader = CreateTmvaReader();
		fReader->BookMVA("BDT", fWeightFileDir+"/TMVAnalysis_BDT.weights.xml");
	} else if (fIdMethod == kANN){
		if (fNN != NULL) delete fNN;
		TTree* simu = CreateTree();
		TString mlpString = CreateAnnString();
		fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
		fNN->LoadWeights((fWeightFileDir+"/"+fAnnWeightsFile).c_str());
	}

	for (Int_t i = 0; i < 2; i++){
      for (Int_t iT = 0; iT < fEloss[i].size(); iT++) {
         if (iT < fMaxNofTrainSamples) continue;// exclude training samples
         if (iT%50000 == 0) cout << "-I- read Electron number: "<< iT << endl;

         //change electron hit to pion hit and vice versa
         //int randPi = rand()%fElossPi.size();
         //for (int i1 = 0; i1 < 5; i1++){
         //     fElossEl[iE][i1] = fElossPi[randPi][i1];
         // }
         for (int iH = 0; iH < fNofTrdLayers; iH++){
            if (fSigmaError !=0.) fEloss[i][iT][iH].fEloss += fRandom->Gaus(0., fSigmaError);
            fAnnInput[iH] = fEloss[i][iT][iH].fEloss;
         }

         Transform();
         Bool_t isEl = (i == 0)?true:false;
         FillAnnInputHist(isEl);
         Double_t nnEval = Eval(isEl);
         if (nnEval > fMaxEval) nnEval = fMaxEval - 0.01;
         if (nnEval < fMinEval) nnEval = fMinEval + 0.01;
         fhAnnOutput[i]->Fill(nnEval);
      }
	}
	CreateCumProbHistos();
	CreateROCDiagramm();
}

void CbmTrdElectronsTrainAnn::DoTest()
{
   fAnnInput.clear();
   fAnnInput.resize(fNofTrdLayers);

	DoPreTest();
	fAnnCut = FindOptimalCut();
	cout << " optimal cut = " << fAnnCut << " for 90% electron efficiency" << endl;
	cout << "-I- Start testing " << endl;
	fAnnInput.clear();
	fAnnInput.resize(fNofTrdLayers);

	if (fIdMethod == kBDT){
		fReader = CreateTmvaReader();
		//reader->BookMVA("TMlpANN", "weights/TMVAnalysis_TMlpANN.weights.txt");
		fReader->BookMVA("BDT", fWeightFileDir+"/TMVAnalysis_BDT.weights.xml");
	} else if (fIdMethod == kANN){
		if (fNN != NULL) delete fNN;
		TTree* simu = CreateTree();
		TString mlpString = CreateAnnString();
		fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
		fNN->LoadWeights((fWeightFileDir+"/"+fAnnWeightsFile).c_str());
	}

	Int_t nofPiLikeEl = 0;
	Int_t nofElLikePi = 0;
	Int_t nofElTest = 0;
	Int_t nofPiTest = 0;

	for (Int_t i = 0; i < 2; i++){
      for (Int_t iT = 0; iT < fEloss[i].size(); iT++) {
         if (iT < fMaxNofTrainSamples) continue;//exclude training samples
         if (iT%50000 == 0) cout << "-I- read Electron number: "<< iT<< endl;

         for (Int_t iH = 0; iH < fNofTrdLayers; iH++){
            fAnnInput[i] = fEloss[i][iT][iH].fEloss;
         }
         Transform();
         Bool_t isEl = (i == 0)?true:false;
         Double_t nnEval = Eval(isEl);
         if (nnEval > fMaxEval) nnEval = fMaxEval - 0.01;
         if (nnEval < fMinEval) nnEval = fMinEval + 0.01;
         if (i == 0){
            nofElTest++;
            if (nnEval < fAnnCut) nofElLikePi++;
         } else if (i == 1){
            nofPiTest++;
            if (nnEval > fAnnCut) nofPiLikeEl++;
         }
      }
	}

	cout << "Testing results:" << endl;
	cout << "cut = " << fAnnCut << endl;
	cout << "nof El = " << nofElTest << endl;
	cout << "nof Pi = " << nofPiTest << endl;
	cout << "nof Pi identified as El = " << nofPiLikeEl<<endl;
	cout << "nof El identified as Pi = " << nofElLikePi<<endl;
	cout << "pion suppression = "<< nofPiTest<< "/"<< nofPiLikeEl<< " = ";
	if (nofPiLikeEl != 0){
		cout<< (Double_t) nofPiTest / nofPiLikeEl << endl;
	}else {
		cout<< " no misidentified pi" << endl;
	}
	cout << "electron efficiency loss in % = " << nofElLikePi << "/"
			<< nofElTest << " = " << (Double_t) nofElLikePi/nofElTest * 100. << endl;
}

void CbmTrdElectronsTrainAnn::CreateCumProbHistos()
{
	for (Int_t i = 0; i < 2; i++){
	   Double_t cumProb = 0.;
	   Double_t nofTr = fhAnnOutput[i]->GetEntries();
      for (Int_t iH = 1; iH <= fhAnnOutput[i]->GetNbinsX(); iH++){
         cumProb += fhAnnOutput[i]->GetBinContent(iH);
         Double_t pr = (i == 0)? 1. - cumProb / nofTr:cumProb/nofTr;
         fhCumProbAnnOutput[i]->SetBinContent(iH, pr);
      }
	}
}

void CbmTrdElectronsTrainAnn::CreateROCDiagramm()
{
	Int_t nBins = fhCumProbAnnOutput[0]->GetNbinsX();
	Double_t x[nBins], y[nBins];

	for (Int_t i = 1; i <= nBins; i++) {
		Double_t cpi = 100. * fhCumProbAnnOutput[1]->GetBinContent(i);
		if (cpi == 100.) cpi = 100. - 0.0001;
		y[i - 1] = 100. / (100. - cpi);
		x[i - 1] = 100. * fhCumProbAnnOutput[0]->GetBinContent(i);
	}
	fROCGraph = new TGraph(nBins, x, y);
	fROCGraph->SetTitle("ROC diagram");
	fROCGraph->GetXaxis()->SetTitle("Efficiency");
	fROCGraph->GetYaxis()->SetTitle("Pion suppression");
}

Double_t CbmTrdElectronsTrainAnn::FindOptimalCut()
{
	Double_t optimalCut = -1;
	for (Int_t i = 1; i <= fhCumProbAnnOutput[0]->GetNbinsX(); i++) {
		if (fhCumProbAnnOutput[0]->GetBinContent(i) <= 0.9) {
			optimalCut = fhCumProbAnnOutput[0]->GetBinCenter(i);
			return optimalCut;
		}
	}
}

TTree* CbmTrdElectronsTrainAnn::CreateTree()
{
   fAnnInput.clear();
   fAnnInput.resize(fNofInputNeurons);
	TTree *simu = new TTree("MonteCarlo", "MontecarloData");
	char txt1[100], txt2[100];
	for (Int_t i = 0; i < fNofInputNeurons; i++){
		sprintf(txt1, "x%d",i);
		sprintf(txt2, "x%d/F", i);
		simu->Branch(txt1, &fAnnInput[i], txt2);
	}
	simu->Branch("xOut", &fXOut, "xOut/F");

	return simu;
}

string CbmTrdElectronsTrainAnn::CreateAnnString()
{
	string st = "";
	char txt[50];
	for (Int_t i = 0; i < fNofInputNeurons - 1; i++){
		sprintf(txt, "x%d",i);
		st = st + txt + ",";

	}
	sprintf(txt, "x%d",fNofInputNeurons - 1);
	st = st + txt+ "";
	sprintf(txt, "%d",fNofHiddenNeurons);
	st = st+ ":"+txt+":xOut";
	return st;
}

TMVA::Factory* CbmTrdElectronsTrainAnn::CreateFactory(TTree* simu)
{
	TFile* outputFile = TFile::Open("TMVA.root", "RECREATE");

	TMVA::Factory *factory = new TMVA::Factory("TMVAnalysis", outputFile);

	TCut piCut = "xOut<0";
	TCut elCut = "xOut>0";

	factory->SetInputTrees(simu, elCut, piCut);
	char txt1[100];
	for (Int_t i = 0; i < fNofInputNeurons; i++){
		sprintf(txt1, "x%d",i);
		factory->AddVariable(txt1, 'F');
	}

	return factory;
}

TMVA::Reader* CbmTrdElectronsTrainAnn::CreateTmvaReader()
{
	if (!fReader) delete fReader;

	fAnnInput.clear();
	fAnnInput.resize(fNofInputNeurons);

	TMVA::Reader* reader = new TMVA::Reader();

	char txt1[100];
	for (Int_t i = 0; i < fNofInputNeurons; i++){
		sprintf(txt1, "x%d",i);
		reader->AddVariable(txt1, &fAnnInput[i]);
	}
	return reader;
}

void CbmTrdElectronsTrainAnn::FillAnnInputHist(
      Bool_t isEl)
{
	for (Int_t j = 0; j < fAnnInput.size(); j++) {
		if (isEl) {
			fhAnnInput[0][j]->Fill(fAnnInput[j]);
		}else {
			fhAnnInput[1][j]->Fill(fAnnInput[j]);
		}
	}
}

void CbmTrdElectronsTrainAnn::Draw()
{
    //SetStyles();
   TCanvas* cEloss = new TCanvas("trd_elid_eloss", "trd_elid_eloss", 900, 600);
   cEloss->Divide(3, 2);
   cEloss->cd(1);
   DrawH1(list_of(fhSumEloss[0])(fhSumEloss[1]), list_of("El")("Pi"),
         kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cEloss->cd(2);
   DrawH1(list_of(fhEloss[0])(fhEloss[1]), list_of("El")("Pi"),
         kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cEloss->cd(3);
   DrawH1(list_of(fhdEdX[0])(fhdEdX[1]), list_of("El")("Pi"),
         kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cEloss->cd(4);
   DrawH1(list_of(fhTR[0])(fhTR[1]), list_of("El")("Pi"),
         kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cEloss->cd(5);
   DrawH1(list_of(fhNofTRLayers[0])(fhNofTRLayers[1]), list_of("El")("Pi"),
         kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cEloss->cd(6);
   DrawH1(list_of(fhNofHits[0])(fhNofHits[1]), list_of("El")("Pi"),
         kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);

   TCanvas* cElossSort = new TCanvas("trd_elid_eloss_sort", "trd_elid_eloss_sort", 1200, 900);
   cElossSort->Divide(4, 3);
   for (int iL = 0; iL < 12; iL++){
      cElossSort->cd(iL+1);
      DrawH1(list_of(fhElossSort[0][iL])(fhElossSort[1][iL]), list_of("El")("Pi"),
            kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   }

   TCanvas* cCumProbSort = new TCanvas("trd_elid_eloss_cum_prob", "trd_elid_eloss_cum_prob", 1200, 900);
   cCumProbSort->Divide(4, 3);
   for (int iL = 0; iL < 12; iL++){
      cCumProbSort->cd(iL+1);
      DrawH1(list_of(fhCumProbSort[0][iL])(fhCumProbSort[1][iL]), list_of("El")("Pi"),
            kLinear, kLinear, true, 0.8, 0.8, 0.99, 0.99);
   }

	TCanvas* c1_1 = new TCanvas("trd_elid_classifier_output","trd_elid_classifier_output", 500,500);
   DrawH1(list_of(fhAnnOutput[0])(fhAnnOutput[1]), list_of("El")("Pi"), kLinear, kLog,
         true, 0.8, 0.8, 0.99, 0.99);
	fhAnnOutput[0]->Scale(1./fhAnnOutput[0]->Integral());
	fhAnnOutput[1]->Scale(1./fhAnnOutput[1]->Integral());

	TCanvas* c1_2 = new TCanvas("trd_elid_cum_prob","trd_elid_cum_prob", 500,500);
   DrawH1(list_of(fhCumProbAnnOutput[0])(fhCumProbAnnOutput[1]), list_of("El")("Pi"),
         kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);

	TCanvas* c1_3 = new TCanvas("trd_elid_roc","trd_elid_roc", 500,500);

   // DrawGraph(fROCGraph, "electron efficiency,%", "pion suppression",
   // 		kBlack, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
	// 	   LIT_MARKER_STYLE1, false, true, "ACP");
	gPad->SetLogy();
	fROCGraph->GetXaxis()->SetTitle("Efficiency [%]");
	fROCGraph->GetYaxis()->SetTitle("Pion suppression");
	fROCGraph->SetLineWidth(3);
   fROCGraph->Draw("AL");

	TCanvas* c9 = new TCanvas("trd_elid_ann_input", "trd_elid_ann_input", 10, 10, 800, 800);
	c9->Divide(4, 3);
	for (int i = 0; i < 12; i++) {
		c9->cd(i + 1);
      DrawH1(list_of(fhAnnInput[0][i])(fhAnnInput[1][i]), list_of("El")("Pi"),
            kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
	}

	TCanvas* c10 = new TCanvas("c10", "c10", 10, 10, 800, 800);
	c10->Divide(4, 3);
	for (int i = 0; i < 12; i++) {
		c10->cd(i + 1);
      DrawH1(list_of(fhElossPdfSort[0][i])(fhElossPdfSort[1][i]), list_of("El")("Pi"),
            kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
	}
}

Bool_t CbmTrdElectronsTrainAnn::FileExists(
      const string& fileName)
{
	bool flag = false;
	fstream fin;
	fin.open(fileName.c_str(), std::ios::in);
	if (fin.is_open()) {
		flag = true;
	}
	fin.close();
	return flag;
}

ClassImp(CbmTrdElectronsTrainAnn)
