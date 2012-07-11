/**
 * \file CbmLitPropagationQaHistCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmLitPropagationQaHistCreator.h"
#include "CbmHistManager.h"
#include "std/utils/CbmLitUtils.h"
#include "TH1F.h"
#include <boost/tuple/tuple.hpp>
#include <boost/assign/list_of.hpp>
#include <cassert>
#include <vector>

using lit::ToString;
using std::vector;
using boost::tuples::tuple;
using boost::tuples::make_tuple;
using boost::tuples::get;
using boost::assign::list_of;

CbmLitPropagationQaHistCreator::CbmLitPropagationQaHistCreator():
      fFixedBounds(true),
      fNofPlanes(0)
{

}

CbmLitPropagationQaHistCreator::~CbmLitPropagationQaHistCreator()
{

}

string CbmLitPropagationQaHistCreator::HistName(
      Int_t parameterIndex,
      Int_t algorithmIndex,
      Int_t planeIndex)
{
   assert(parameterIndex < 10 && algorithmIndex < 3);
   string parameters[] = { "hResX", "hResY", "hResTx", "hResTy", "hResQp",
         "hPullX", "hPullY", "hPullTx", "hPullTy", "hPullQp" };
   string algorithms[] = { "Propagator", "Fitter", "Smoother" };
   return parameters[parameterIndex] + algorithms[algorithmIndex] + ToString<Int_t>(planeIndex);
}

void CbmLitPropagationQaHistCreator::Create(
      CbmHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   vector<tuple<Int_t, Double_t, Double_t> > bb;
   if (fFixedBounds) {
      bb = list_of
            (make_tuple(200, -10., 10.)) // X residual
            (make_tuple(200, -10., 10.)) // Y residual
            (make_tuple(200, -.15, .15)) // Tx residual
            (make_tuple(200, -.15, .15)) // Ty residual
            (make_tuple(200, -.1, .1)) // Qp residual
            (make_tuple(200, -5., 5.)) // X pull
            (make_tuple(200, -5., 5.)) // Y pull
            (make_tuple(200, -5., 5.)) // Tx pull
            (make_tuple(200, -5., 5.)) // Ty pull
            (make_tuple(200, -7., 7.)); // Qp pull
   } else {
      bb.assign(NofQaParameters, make_tuple(200, 0., 0.));
   }

   string axisTitles[] = { "Residual X [cm]", "Residual Y [cm]", "Residual Tx", "Residual Ty", "Residual q/p [(GeV/c)^{-1}]",
            "Pull X", "Pull Y", "Pull Tx", "Pull Ty", "Pull q/p"
   };

   for (Int_t iAlgorithm = 0; iAlgorithm < 3; iAlgorithm++) {
      for (Int_t iPlane = 0; iPlane < fNofPlanes; iPlane++) {
         for (Int_t iParam = 0; iParam < NofQaParameters; iParam++) {
            string histName = HistName(iParam, iAlgorithm, iPlane);
            TH1F* hist = new TH1F(histName.c_str(), string(histName + ";" + axisTitles[iParam] + ";Counter").c_str(),
                                  get<0>(bb[iParam]), get<1>(bb[iParam]), get<2>(bb[iParam]));
            fHM->Add(histName, hist);
            std::cout << "CbmLitPropagationQaHistCreator::Create: plane=" << iPlane << " name=" << histName.c_str() << std::endl;
         }
      }
   }
}
