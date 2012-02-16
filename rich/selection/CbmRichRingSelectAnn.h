/**
* \file CbmRichRingSelectAnn.h
*
* \brief Implementation for concrete RICH ring selection algorithm:
* reject rings using a trained neural net (input file with weights needed!)
* store resulting value (0-1) in "SelectionNN":
* 0 = good rings
* 1 = rings to be rejected
* --> choose a value in between depending on required purity/ efficiency
*
* \author Semen Lebedev
* \date 2008
**/

#ifndef CBM_RICH_RING_SELECT_ANN
#define CBM_RICH_RING_SELECT_ANN

#define NN_FUNCTION
//#define ANN_FILE

#include <string>

class CbmRichRingLight;
class CbmRichRingSelectImpl;
class NNfunction;
class TMultiLayerPerceptron;

/**
* \class CbmRichRingSelectAnn
*
* \brief Implementation for concrete RICH ring selection algorithm:
* reject rings using a trained neural net (input file with weights needed!)
* store resulting value (0-1) in "SelectionNN":
* 0 = good rings
* 1 = rings to be rejected
* --> choose a value in between depending on required purity/ efficiency
*
* \author Semen Lebedev
* \date 2008
**/
class CbmRichRingSelectAnn
{
private:
	std::string fNeuralNetWeights;
	TMultiLayerPerceptron* fNN;
	NNfunction* fNNfunction;
	CbmRichRingSelectImpl* fSelectImpl;

public:
	/**
	 * \brief Standard constructor.
	 */
  	CbmRichRingSelectAnn(
  	      const std::string& annFile);

   /**
    * \brief Destructor.
    */
	virtual ~CbmRichRingSelectAnn();

	/**
	 * \brief Initialize ANN.
	 */
	virtual void Init();

	/**
	 * \Perform selection.
	 * \param[in,out] ring Found and fitted ring.
	 */
	void DoSelect(
	      CbmRichRingLight* ring);

private:
   /**
    * \brief Copy constructor.
    */
   CbmRichRingSelectAnn(const CbmRichRingSelectAnn&);

   /**
    * \brief Assignment operator.
    */
   CbmRichRingSelectAnn& operator=(const CbmRichRingSelectAnn&);
};

#endif
