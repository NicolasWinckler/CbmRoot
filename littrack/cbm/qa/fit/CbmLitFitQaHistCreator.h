/**
 * \file CbmLitFitQaHistCreator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \brief Create histograms for track fit QA.
 * \date 2011
 */

#ifndef CBMLITFITQAHISTCREATOR_H_
#define CBMLITFITQAHISTCREATOR_H_

#include "cbm/base/CbmLitDetectorSetup.h"
class CbmHistManager;

class CbmLitFitQaHistCreator {
public:
	/**
	 * \brief Constructor.
	 */
	CbmLitFitQaHistCreator();

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmLitFitQaHistCreator();

	/**
	* \brief Create histograms for QA.
	* \param[in] histManager CbmLitQaHistManager to be filled with created histograms.
	*/
	void Create(
		 CbmHistManager* histManager);

	/** Setters **/
	void SetPRange(Int_t bins, Int_t min, Int_t max) {
	   fPRangeBins = bins;
	   fPRangeMin = min;
	   fPRangeMax = max;
	}

private:

	void CreateResidualAndPullHistograms(
			DetectorId detId,
			const string& detName);

	Bool_t fIsFixedBounds; // if true than fixed bounds are used for histograms
	CbmHistManager* fHM; // Histogram manager for created histograms
	CbmLitDetectorSetup fDet; // For detector setup determination

	Double_t fPRangeMin; // Min momentum
	Double_t fPRangeMax; // Max momentum
	Int_t fPRangeBins; // Number of bins
};

#endif /* CBMLITFITQAHISTCREATOR_H_ */
