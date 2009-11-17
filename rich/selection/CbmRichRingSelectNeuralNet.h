 /******************************************************************************
*  $Id: CbmRichRingSelectNeuralNet.h,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichRingSelect2DCuts
*  Description : Implementation for concrete RICH ring selection algorithm:
*                reject rings using a trained neural net (input file with weights needed!)
*                store resulting value (0-1) in "SelectionNN":
*                0 = good rings
*                1 = rings to be rejected
*                --> choose a value in between depending on required purity/ efficiency
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichRingSelectNeuralNet.h,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*
*
*******************************************************************************/


#ifndef CBM_RICH_RING_SELECT_NEURALNET_H
#define CBM_RICH_RING_SELECT_NEURALNET_H

#include "TClonesArray.h"
#include "CbmRichRing.h"
/*#include "FairTask.h"
#include "FairRootManager.h"*/
#include "CbmRichRingSelectImpl.h"
#include "CbmRichRingSelect.h"
#include "TMultiLayerPerceptron.h"
#include "TTree.h"

class TMultiLayerPerceptron;

class CbmRichRingSelectNeuralNet : public CbmRichRingSelect {

	  /** Verbosity level **/
  	Int_t fVerbose;
	const char* fNeuralNetWeights;

public:
  	CbmRichRingSelectNeuralNet();// Default constructor
  	CbmRichRingSelectNeuralNet ( Int_t verbose, const char* NNFile);// Standard constructor
	~CbmRichRingSelectNeuralNet();

	virtual void Init();
	void DoSelect(CbmRichRing* ring);

	TMultiLayerPerceptron* fNN;
	CbmRichRingSelectImpl* fSelectImpl;

	ClassDef(CbmRichRingSelectNeuralNet,1);
};

#endif
