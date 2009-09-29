
/** CbmTrdElectronsQa
 * Author: Semen Lebedev (S.Lebedev@gsi.de)
 **/


#ifndef CBM_TRD_ELECTRONS_TRAIN_ANN
#define CBM_TRD_ELECTRONS_TRAIN_ANN 1


#include "TH1D.h"
#include "TString.h"
#include "TTree.h"
#include "TMultiLayerPerceptron.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"

#include <fstream>
#include <vector>

using std::vector;

enum IdMethod {kANN = 0, kBDT = 1, kCLUSTERS = 2, kMEDIANA = 3};


class CbmTrdElectronsTrainAnn
{
public:

	CbmTrdElectronsTrainAnn();

	~CbmTrdElectronsTrainAnn();
	void Init();
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
	Double_t FindOptimalCut();
	void CreateCumProbHistos();
	void CreateROCDiagramm();
	Int_t GetNofClusters();
	Double_t Eval(Bool_t isEl);
	void SetAnnCut(Double_t annCut){fAnnCut = annCut;}
	void SetIsDoTrain(Bool_t doTrain){fIsDoTrain = doTrain;}
	void SetTransformType(Int_t type){fTransformType = type;}
	void SetIdMethod(IdMethod idMethod){fIdMethod = idMethod;}

private:
	TString fFileNameEl;
	TString fFileNamePi;
	TString fFileNameTestEl;
	TString fFileNameTestPi;
	Bool_t fIsDoTrain;
	Int_t fTransformType;

	TH1D* fhCumProbSortPi[12];
	TH1D* fhCumProbSortEl[12];

	vector<Float_t> fElossVec;
	vector<Float_t> fInVector;
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

// Histogramms for testing
	TH1D* fhAnnOutputPi;
	TH1D* fhAnnOutputEl;
	TH1D* fhCumProbPi;
	TH1D* fhCumProbEl;

    TH1D* fhElNofClusters;
    TH1D* fhPiNofClusters;
    TH1D* fhElElossMediana;
    TH1D* fhPiElossMediana;
	ClassDef(CbmTrdElectronsTrainAnn,1);

};

#endif
