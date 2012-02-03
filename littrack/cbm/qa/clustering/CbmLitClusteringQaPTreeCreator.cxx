/**
 * \file CbmLitClusteringQaPTreeCreator.cxx
 * \brief Create property tree for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaPTreeCreator.h"
#include "qa/base/CbmLitHistManager.h"
#include "TH1F.h"

CbmLitClusteringQaPTreeCreator::CbmLitClusteringQaPTreeCreator():
   fHM(NULL)
{

}

CbmLitClusteringQaPTreeCreator::~CbmLitClusteringQaPTreeCreator()
{

}

ptree CbmLitClusteringQaPTreeCreator::Create(
      CbmLitHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   ptree pt;

   pt.put("hEventNo", (Int_t)fHM->H1F("hEventNo")->GetEntries());

   NofStatisticsToPTree(pt, "hNofMvdPoints", "hNofStsPoints", "hNofRichPoints",
         "hNofTrdPoints", "hNofMuchPoints", "hNofMuchPoints", "hNofTofPoints");
   NofStatisticsToPTree(pt, "hNofMvdDigis", "hNofStsDigis", "",
         "hNofTrdDigis", "hNofMuchDigis", "", "");
   NofStatisticsToPTree(pt, "hNofMvdClusters", "hNofStsClusters", "",
         "hNofTrdClusters", "hNofMuchClusters", "", "");
   NofStatisticsToPTree(pt, "hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");

   return pt;
}

void CbmLitClusteringQaPTreeCreator::NofStatisticsToPTree(
   ptree& pt,
   const string& mvd,
   const string& sts,
   const string& rich,
   const string& trd,
   const string& muchP,
   const string& muchS,
   const string& tof) const
{
   if (mvd != "") { pt.put(mvd, (int)fHM->H1F(mvd)->GetMean()); }
   if (sts != "") { pt.put(sts, (int)fHM->H1F(sts)->GetMean()); }
   if (rich != "") { pt.put(rich, (int)fHM->H1F(rich)->GetMean()); }
   if (trd != "") { pt.put(trd, (int)fHM->H1F(trd)->GetMean()); }
   if (muchP != "") { pt.put(muchP, (int)fHM->H1F(muchP)->GetMean()); }
   if (muchS != "") { pt.put(muchS, (int)fHM->H1F(muchS)->GetMean()); }
   if (tof != "") { pt.put(tof, (int)fHM->H1F(tof)->GetMean()); }
}
