 /*
*  Description : Implementation for concrete RICH ring selection algorithm:
*                reject rings using a trained neural net (input file with weights needed!)
*                store resulting value (0-1) in "SelectionNN":
*                0 = good rings
*                1 = rings to be rejected
*                --> choose a value in between depending on required purity/ efficiency
*
*  Author : Semen Lebedev
*  E-mail : S.Lebedev@gsi.de
*  */

#ifndef CBM_RICH_RING_SELECT_NEURALNET_H
#define CBM_RICH_RING_SELECT_NEURALNET_H

class CbmRichRingLight;
class CbmRichRingSelectImpl;
class NNfunction;
class TMultiLayerPerceptron;

class CbmRichRingSelectNeuralNet
{

	const char* fNeuralNetWeights;

public:
  	CbmRichRingSelectNeuralNet (const char* NNFile);// Standard constructor
	~CbmRichRingSelectNeuralNet();

	virtual void Init();
	void DoSelect(CbmRichRingLight* ring);

	//TMultiLayerPerceptron* fNN;
	NNfunction* fNNfunction;
	CbmRichRingSelectImpl* fSelectImpl;
};

#endif
