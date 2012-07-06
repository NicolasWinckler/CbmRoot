/**
 * \file CbmLitFitQaDraw.h
 * \brief Draw histograms for track fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITFITQADRAW_H_
#define CBMLITFITQADRAW_H_

#include <string>
#include "TObject.h"
class CbmHistManager;
using std::string;

class CbmLitFitQaDraw {
public:
	/**
	 * \Constructor.
	 */
	CbmLitFitQaDraw();

	/**
	 * \Destructor.
	 */
	virtual ~CbmLitFitQaDraw();

	/**
	* \brief Draw histograms.
	* \param[in] histManager Pointer to histogram manager.
	* \param[in] outputDir Output directory for images. If not specified images are not saved.
	*/
	void Draw(
		 CbmHistManager* histManager,
		 const string& outputDir = "");

private:
	void DrawResidualAndPullHistograms(
	      const string& detName);

	CbmHistManager* fHM; // Pointer to histogram manager
	string fOutputDir; // Output directory for images
};

#endif /* CBMLITFITQADRAW_H_ */
