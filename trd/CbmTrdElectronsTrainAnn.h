
/** CbmTrdElectronsQa
 * Author: Semen Lebedev (S.Lebedev@gsi.de)
 **/


#ifndef CBM_TRD_ELECTRONS_TRAIN_ANN
#define CBM_TRD_ELECTRONS_TRAIN_ANN 1


#include "TH1D.h"
#include "TString.h"
#include "TTree.h"
#include "TMultiLayerPerceptron.h"

#include <fstream>
#include <vector>

using std::vector;

class CbmTrdElectronsTrainAnn
{
public:

	CbmTrdElectronsTrainAnn();

	~CbmTrdElectronsTrainAnn();
	void Init();
	void Run();
	void DoTrain();
	void DoTest();
	TTree* CreateTreeForAnn();
	TString CreateAnnString();

	void Transform();
	void Transform1();
	void Transform2();
	Double_t FindOptimalCut();
	void CreateCumProbHistos();
	void CreateROCDiagramm();

private:
	TString fFileNameEl;
	TString fFileNamePi;
	TString fFileNameTestEl;
	TString fFileNameTestPi;
	Bool_t fIsDoTrain;

	TH1D* fhCumProbSortPi[12];
	TH1D* fhCumProbSortEl[12];

	vector<Double_t> fElossVec;
	vector<Double_t> fInVector;
	Double_t fXOut;
	Double_t fAnnCut;
	Int_t fNofInputNeurons;
	Int_t fNofHiddenNeurons;
	Int_t fNofLayers;
	Double_t fMaxEval;
	Double_t fMinEval;
	TString fAnnWeightsFile;
	TMultiLayerPerceptron* fNN;

// Histogramms for testing
	TH1D* fhAnnOutputPi;
	TH1D* fhAnnOutputEl;
	TH1D* fhCumProbPi;
	TH1D* fhCumProbEl;


	ClassDef(CbmTrdElectronsTrainAnn,1);

};

#endif
