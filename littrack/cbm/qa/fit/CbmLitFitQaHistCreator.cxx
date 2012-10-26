/**
 * \file CbmLitFitQaHistCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitFitQaHistCreator.h"
#include "CbmHistManager.h"
#include "TH1F.h"
#include "TH2F.h"
#include <boost/assign/list_of.hpp>
#include <cassert>
using std::make_pair;
using std::pair;
using std::vector;


CbmLitFitQaHistCreator::CbmLitFitQaHistCreator():
    fIsFixedBounds(true),
	fHM(NULL),
	fDet(),
	fPRangeMin(0.),
	fPRangeMax(10.),
	fPRangeBins(20)
{

}

CbmLitFitQaHistCreator::~CbmLitFitQaHistCreator()
{

}

void CbmLitFitQaHistCreator::Create(
      CbmHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   fDet.DetermineSetup();

   CreateResidualAndPullHistograms(kSTS, "Sts");
   CreateResidualAndPullHistograms(kTRD, "Trd");
   CreateResidualAndPullHistograms(kMUCH, "Much");

   // Momentum resolution vs momwntum
   fHM->Add("htf_MomRes_Mom", new TH2F("htf_MomRes_Mom", "htf_MomRes_Mom;P [GeV/c];dP/P [%]", fPRangeBins, fPRangeMin, fPRangeMax, 100, -3., 3.));
   fHM->Add("htf_ChiPrimary", new TH1F("htf_ChiPrimary", "htf_ChiPrimary;#chi_{primary}", 100, 0, 10));
}

void CbmLitFitQaHistCreator::CreateResidualAndPullHistograms(
		DetectorId detId,
		const string& detName)
{
	if (!fDet.GetDet(detId)) return;

	// Parameter names of the state vector (x, y, tx, ty, q/p)
   string parameterNames[] = { "X", "Y", "Tx", "Ty", "Qp" };

   // Axis titles for residual, pull and wrong covariance histograms
   string xTitles[] = {
       "Residual X [cm]", "Residual Y [cm]", "Residual Tx", "Residual Ty", "Residual q/p [(GeV/c)^{-1}]",
       "Pull X", "Pull Y", "Pull Tx", "Pull Ty", "Pull q/p",
       "Number of hits", "Number of hits", "Number of hits", "Number of hits", "Number of hits"
   };

   // Category names: Residual, Pull, Wrong Covariance
   string catNames[] = { "Res", "Pull", "WrongCov" };

   vector<Int_t> bins(15, 200);
   vector<pair<Float_t, Float_t> > bounds;
   if (fIsFixedBounds) {
      bounds = boost::assign::list_of
            (make_pair(-1., 1.)) // X residual
            (make_pair(-1., 1.)) // Y residual
            (make_pair(-.01, .01)) // Tx residual
            (make_pair(-.01, .01)) // Ty residual
            (make_pair(-.1, .1)) // Qp residual
            (make_pair(-5., 5.)) // X pull
            (make_pair(-5., 5.)) // Y pull
            (make_pair(-5., 5.)) // Tx pull
            (make_pair(-5., 5.)) // Ty pull
            (make_pair(-7., 7.)) // Qp pull
            (make_pair(0., 200.)) // X wrong covariance
            (make_pair(0., 200.)) // Y wrong covariance
            (make_pair(0., 200.)) // Tx wrong covariance
            (make_pair(0., 200.)) // Ty wrong covariance
            (make_pair(0., 200.)); // Qp wrong covariance
   } else {
      bounds.assign(15, make_pair(0.,0.));
   }

   // [0] - for the first track parameter, [1] - for the last track parameter
   for (Int_t i = 0; i < 2; i++) {
	  string trackParamName = (i == 0) ? "FirstParam" : "LastParam";
	  // [0] - "Res", [1] - "Pull", [2] - "WrongCov"
	  for (Int_t iCat = 0; iCat < 3; iCat++) {
		  for (Int_t iPar = 0; iPar < 5; iPar++) {
			 string histName = "htf_" + detName + "_" + trackParamName + "_" + catNames[iCat] + "_" + parameterNames[iPar];
			 Int_t histId = iCat * 5 + iPar;
			 fHM->Add(histName, new TH1F(histName.c_str(), string(histName + ";" + xTitles[histId] + ";Counter").c_str(),
				   bins[histId], bounds[histId].first, bounds[histId].second));
		  }
	  }
   }
}
