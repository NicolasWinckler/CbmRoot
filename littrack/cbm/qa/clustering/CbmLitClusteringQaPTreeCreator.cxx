/**
 * \file CbmLitClusteringQaPTreeCreator.cxx
 * \brief Create property tree for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaPTreeCreator.h"
#include "qa/base/CbmHistManager.h"
#include "TH1.h"

CbmLitClusteringQaPTreeCreator::CbmLitClusteringQaPTreeCreator():
   fHM(NULL)
{

}

CbmLitClusteringQaPTreeCreator::~CbmLitClusteringQaPTreeCreator()
{

}

ptree CbmLitClusteringQaPTreeCreator::Create(
      CbmHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   ptree pt;

   pt.put("hen_EventNo_ClusteringQa.entries", (Int_t)fHM->H1("hen_EventNo_ClusteringQa")->GetEntries());

   NofObjectsToPTree(pt);

   return pt;
}

void CbmLitClusteringQaPTreeCreator::NofObjectsToPTree(
   ptree& pt)
{
	vector<TH1*> histos = fHM->H1Vector("hno_NofObjects_.+");
	Int_t nofHistos = histos.size();
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string name = string(histos[iHist]->GetName()) + ".mean";
		pt.put(name, histos[iHist]->GetMean());
	}
}
