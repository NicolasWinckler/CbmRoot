/**
 * \file CbmStsSimulationQaPTreeCreator.cxx
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */
#include "CbmStsSimulationQaPTreeCreator.h"
#include "CbmHistManager.h"
#include "TH1.h"

CbmStsSimulationQaPTreeCreator::CbmStsSimulationQaPTreeCreator():
   fHM(NULL)
{

}

CbmStsSimulationQaPTreeCreator::~CbmStsSimulationQaPTreeCreator()
{

}

ptree CbmStsSimulationQaPTreeCreator::Create(
      CbmHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   ptree pt;

   pt.put("hen_EventNo_ClusteringQa.entries", (Int_t)fHM->H1("hss_EventNo_StsSimulationQa")->GetEntries());

   NofObjectsToPTree(pt);

   return pt;
}

void CbmStsSimulationQaPTreeCreator::NofObjectsToPTree(
   ptree& pt)
{
	vector<TH1*> histos = fHM->H1Vector("hss_NofObjects_.+");
	Int_t nofHistos = histos.size();
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string name = string(histos[iHist]->GetName()) + ".mean";
		pt.put(name, histos[iHist]->GetMean());
	}
}
