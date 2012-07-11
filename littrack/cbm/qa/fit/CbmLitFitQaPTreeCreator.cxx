/**
 * \file CbmLitFitQaPTreeCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */

#include "CbmLitFitQaPTreeCreator.h"
#include "CbmHistManager.h"
#include "TH1.h"
#include "TF1.h"
#include <cassert>

CbmLitFitQaPTreeCreator::CbmLitFitQaPTreeCreator():
	fHM(NULL)
{

}

CbmLitFitQaPTreeCreator::~CbmLitFitQaPTreeCreator()
{

}

ptree CbmLitFitQaPTreeCreator::Create(
      CbmHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;
   ptree pt;
   ResidualAndPullToPtree(pt);
   WrongCovToPtree(pt);
   return pt;
}

void CbmLitFitQaPTreeCreator::ResidualAndPullToPtree(
      ptree& pt) const
{
	vector<TH1*> histos = fHM->H1Vector("htf_.+_.+_(Res|Pull)_.*");
	Int_t nofHistos = histos.size();
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		TH1* hist = histos[iHist];
		hist->Fit("gaus");
		TF1* fit = hist->GetFunction("gaus");
		Double_t sigma = (NULL != fit) ? fit->GetParameter(2) : 0.;
		string histName = hist->GetName();
		pt.put(histName + ".mean", hist->GetMean());
		pt.put(histName + ".rms", hist->GetRMS());
		pt.put(histName + ".sigma", sigma);
	}
}

void CbmLitFitQaPTreeCreator::WrongCovToPtree(
      ptree& pt) const
{
	vector<TH1*> histos = fHM->H1Vector("htf_.+_.+_WrongCov_.*");
	Int_t nofHistos = histos.size();
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string name = string(histos[iHist]->GetName()) + ".entries";
		pt.put(name, histos[iHist]->GetEntries());
	}
}
