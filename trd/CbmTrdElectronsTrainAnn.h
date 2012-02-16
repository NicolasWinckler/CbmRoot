
/** CbmTrdElectronsTrainAnn
 * Author: Semen Lebedev (S.Lebedev@gsi.de)
 **/


#ifndef CBM_TRD_ELECTRONS_TRAIN_ANN
#define CBM_TRD_ELECTRONS_TRAIN_ANN

#include "FairTask.h"

#include "TMultiLayerPerceptron.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

class TH1D;
class TTree;
class TGraph;
class TRandom;
//class TMultiLayerPerceptron;

enum IdMethod {kANN = 0, kBDT = 1, kMEDIANA = 2, kLIKELIHOOD = 3};

/**
 * \class ElossInfo
 *
 * \brief Represents information about energy losses in one layer.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/
class ElossInfo
{
public:
   double fEloss;
   double fdEdX;
   double fTR;
};


class CbmTrdElectronsTrainAnn : public FairTask
{
public:

   /**
    * \brief Default constructor
    */
   CbmTrdElectronsTrainAnn();

   /**
    * \brief Destructor
    */
   virtual ~CbmTrdElectronsTrainAnn();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
         Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

private:

   /**
    * \brief Fill vector with energy loss information.
    */
   void FillElossVector();

   /**
    * \brief Fill histograms with energy loss information.
    */
   void FillElossAnaHist();

   /**
    * \brief Sort energy losses and fill histograms.
    */
   void SortElossAndFillHist();

   /**
    * \brief Fill cumulative probability histograms.
    */
   void FillCumProbHist();

   /**
    * \brief Draw results.
    */
	void Draw();

	void Run();

	void DoTrain();
	void DoTest();
	void DoPreTest();

	TTree* CreateTree();
	string CreateAnnString();
	TMVA::Factory* CreateFactory(TTree* simu);
	TMVA::Reader* CreateTmvaReader();

	void Transform();
	void Transform1();
	void Transform2();

	Double_t FindOptimalCut();
	void CreateCumProbHistos();
	void CreateROCDiagramm();
	Double_t Likelihood();
	Double_t Eval(Bool_t isEl);

	void SetAnnCut(Double_t annCut){fAnnCut = annCut;}
	void SetIsDoTrain(Bool_t doTrain){fIsDoTrain = doTrain;}
	void SetTransformType(Int_t type){fTransformType = type;}
	void SetIdMethod(IdMethod idMethod){fIdMethod = idMethod;}
	void SetWeightFileDir(const string& weightFileDir){fWeightFileDir = weightFileDir;}
	void SetNofHiddenNeurons(Int_t nofHiddenNeurons){fNofHiddenNeurons = nofHiddenNeurons;}
	void SetNofAnnEpochs(Int_t nofAnnEpochs){fNofAnnEpochs = nofAnnEpochs;}
	void SetMaxNofTrainSamples(Int_t nofTr){fMaxNofTrainSamples = nofTr;}
	void SetSigmaError(Double_t sigma){fSigmaError = sigma;}

	void FillAnnInputHist(
	      Bool_t isEl);

	Bool_t FileExists(const string& fileName);


   TClonesArray* fMCTracks;
   TClonesArray* fTrdPoints;
   TClonesArray* fTrdTracks;
   TClonesArray* fTrdTrackMatches;
   TClonesArray* fTrdHits;

   // store energy loss information from TrdTrack
   // 1st index -> [0] = electrons, [1] = pions
   // 2nd index -> track index
   // 3rd index -> hit index in track
   vector< vector< vector<ElossInfo> > > fEloss;

   vector<TH1*> fHists; //store all pointers to histograms

   // [0] = electrons, [1] = pions
   vector<TH1D*> fhSumEloss; // sum of energy losses in all layers
   vector<TH1D*> fhEloss; // energy losses in one layer
   vector<TH1D*> fhdEdX; // dEdX in one layer
   vector<TH1D*> fhTR; // TR in one layer
   vector<TH1D*> fhNofTRLayers; // Number of layers which have TR
   vector<TH1D*> fhNofHits; // Number of TRD hits

   // store sorted energy losses and cumulative probability for each TRD layer
   // 1st index -> [0] = electrons, [1] = pions
   // 2nd index -> layer number
   vector<vector<TH1D*> > fhElossSort;
   vector<vector<TH1D*> > fhCumProbSort;
	vector<vector<TH1D*> > fhElossPdfSort;

   Int_t fEventNum; // event number

	string fWeightFileDir;
	Double_t fSigmaError;

	Bool_t fIsDoTrain;
	Int_t fTransformType;

	vector<Float_t> fAnnInput; // input vector for ANN
	Float_t fXOut; // output value from ANN

	Double_t fAnnCut;
	Int_t fNofInputNeurons;
	Int_t fNofHiddenNeurons;
	Int_t fNofTrdLayers;

	Double_t fMaxEval;
	Double_t fMinEval;

	string fAnnWeightsFile;
	TMultiLayerPerceptron* fNN;
	TMVA::Reader* fReader;
	IdMethod fIdMethod;
	Int_t fNofAnnEpochs;

	Int_t fMaxNofTrainSamples;

	TRandom* fRandom;

   // Histograms for testing
	// [0] = electron, [1] = pion
	vector<TH1D*> fhAnnOutput; // ANN output
	vector<TH1D*> fhCumProbAnnOutput; // Cumulative probabilities for ANN output
   vector< vector<TH1D*> > fhAnnInput; // Input data for ANN for each input

   TGraph* fROCGraph;

    CbmTrdElectronsTrainAnn(const CbmTrdElectronsTrainAnn&);
    CbmTrdElectronsTrainAnn& operator=(const CbmTrdElectronsTrainAnn&);

    ClassDef(CbmTrdElectronsTrainAnn,1);
};

#endif
