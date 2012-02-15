
/** CbmTrdElectronsQa
 * Author: Semen Lebedev (S.Lebedev@gsi.de)
 **/


#ifndef CBM_TRD_ELECTRONS_TRAIN_ANN
#define CBM_TRD_ELECTRONS_TRAIN_ANN 1


#include "TH1D.h"
#include "TString.h"
#include "TTree.h"
#include "TGraph.h"
#include "TRandom.h"
#include "TMultiLayerPerceptron.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"

#include <fstream>
#include <vector>

using std::vector;

enum IdMethod {kANN = 0, kBDT = 1, kCLUSTERS = 2, kMEDIANA = 3, kLIKELIHOOD=4};


class CbmTrdElectronsTrainAnn
{
public:

	CbmTrdElectronsTrainAnn();

	virtual ~CbmTrdElectronsTrainAnn();
	void Init();
	void InitCumHistos();
	void Run();
	void RunMany();

	void DoTrain();
	void DoTrainTmva();
	void DoTest();
	void DoPreTest();
	TTree* CreateTree();
	TString CreateAnnString();
	TMVA::Factory* CreateFactory(TTree* simu);
	TMVA::Reader* CreateTmvaReader();
	void DrawHistos();

	void Transform();
	void Transform1();
	void Transform2();
	void Transform3();
	void Transform4();
	Double_t FindOptimalCut();
	void CreateCumProbHistos();
	void CreateROCDiagramm();
	Int_t GetNofClusters();
	Double_t Likelihood();
	Double_t FindArea(TH1* h);
	Double_t Eval(Bool_t isEl);
	void ReadFile();
	void SetAnnCut(Double_t annCut){fAnnCut = annCut;}
	void SetIsDoTrain(Bool_t doTrain){fIsDoTrain = doTrain;}
	void SetTransformType(Int_t type){fTransformType = type;}
	void SetIdMethod(IdMethod idMethod){fIdMethod = idMethod;}
	void SetFileNum(Int_t fileNum){fFileNum = fileNum;}
	void SetFileNameEl(TString fileNameEl){fFileNameEl = fileNameEl;}
	void SetFileNamePi(TString fileNamePi){fFileNamePi = fileNamePi;}
	void SetFileNameTestEl(TString fileNameTestEl){fFileNameTestEl = fileNameTestEl;}
	void SetFileNameTestPi(TString fileNameTestPi){fFileNameTestPi = fileNameTestPi;}
	void SetFileNameCumHistos(TString fileNameCumHistos){
		fFileNameCumHistos = fileNameCumHistos;
		InitCumHistos();
	}
	void SetWeightFileDir(TString weightFileDir){fWeightFileDir = weightFileDir;}
	void SetNofHiddenNeurons(Int_t nofHiddenNeurons){fNofHiddenNeurons=nofHiddenNeurons;}
	void SetNofAnnEpochs(Int_t nofAnnEpochs){fNofAnnEpochs=nofAnnEpochs;}
	void SetMaxNofTrainPi(Int_t nofPi){fMaxNofTrainPi = nofPi;}
	void SetMaxNofTrainEl(Int_t nofEl){fMaxNofTrainEl = nofEl;}
	void SetSigmaError(Double_t sigma){fSigmaError = sigma;}
	void FillProbabilityHistos(Bool_t isEl);

	Bool_t FileExists(TString fileName);
private:
	TString fFileNameEl;
	TString fFileNamePi;
	TString fFileNameTestEl;
	TString fFileNameTestPi;
	TString fFileNameCumHistos;
	TString fWeightFileDir;
	Double_t fSigmaError;

	Bool_t fIsDoTrain;
	Int_t fTransformType;

	TH1D* fhCumProbSortPi[12];
	TH1D* fhCumProbSortEl[12];
	TH1D* fhPdfSortPi[12];
	TH1D* fhPdfSortEl[12];

	vector<Float_t> fElossVec;
	vector<Float_t> fInVector;
	vector<vector<Double_t> > fElossPi;
	vector<vector<Double_t> > fElossEl;
	Int_t fFileNum;
	Float_t fXOut;
	Double_t fAnnCut;
	Int_t fNofInputNeurons;
	Int_t fNofHiddenNeurons;
	Int_t fNofLayers;
	Double_t fMaxEval;
	Double_t fMinEval;
	TString fAnnWeightsFile;
	TMultiLayerPerceptron* fNN;
	TMVA::Reader* fReader;
	IdMethod fIdMethod;
	Int_t fNofAnnEpochs;
	Int_t fMaxNofTrainPi;
	Int_t fMaxNofTrainEl;

	TRandom* fRandom;
    std::ofstream  foutResults;

// Histogramms for testing
	TH1D* fhAnnOutputPi;
	TH1D* fhAnnOutputEl;
	TH1D* fhCumProbPi;
	TH1D* fhCumProbEl;

    TH1D* fhElNofClusters;
    TH1D* fhPiNofClusters;
    TH1D* fhElElossMediana;
    TH1D* fhPiElossMediana;

	TH1D* fhPiProbSortEl[12];
	TH1D* fhPiProbSortPi[12];
	TH1D* fhElProbSortEl[12];
	TH1D* fhElProbSortPi[12];
	TH1D* fhMulProbSortEl[12];
	TH1D* fhMulProbSortPi[12];
	TH1D* fhInputEl[12];
	TH1D* fhInputPi[12];
    TGraph* fROCGraph;


    CbmTrdElectronsTrainAnn(const CbmTrdElectronsTrainAnn&);
    CbmTrdElectronsTrainAnn& operator=(const CbmTrdElectronsTrainAnn&);

    ClassDef(CbmTrdElectronsTrainAnn,1);

};

#endif
