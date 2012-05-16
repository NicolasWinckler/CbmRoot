/**
 * \file CbmLitTrackingQaPTreeCreator.h
 * \brief Create property tree for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "qa/tracking/CbmLitTrackingQaPTreeCreator.h"
#include "qa/base/CbmLitHistManager.h"
#include "std/utils/CbmLitUtils.h"

#include "TH1.h"

#include <iostream>
#include <cassert>
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;
using lit::FindAndReplace;

CbmLitTrackingQaPTreeCreator::CbmLitTrackingQaPTreeCreator():
   fHM(NULL)
{

}

CbmLitTrackingQaPTreeCreator::~CbmLitTrackingQaPTreeCreator()
{

}

ptree CbmLitTrackingQaPTreeCreator::Create(
      CbmLitHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   ptree pt;

   pt.put("hen_EventNo_TrackingQa.entries", (Int_t)fHM->H1("hen_EventNo_TrackingQa")->GetEntries());

   NofObjectsToPTree(pt);
   TrackHitsToPTree(pt);
   TrackingEfficiencyToPTree(pt);
   NofGhostsToPTree(pt);


//   // electron identification
//   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsTrd3DElId");
//   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsTrd3DElIdNormStsTrdTof");
//   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsTrdTof3DElId");
//   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRich3DElId");
//   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRich3DElIdNormStsRichTrd");
//   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichTrd3DElId");
//   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRich3DElIdNormStsRichTrdTof");
//   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichTrd3DElIdNormStsRichTrdTof");
//   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichTrdTof3DElId");
//
//   // Detector acceptance efficiency
//   EventDetAccElStatisticsToPTree(&pt, "hStsDetAcc3DEl", "hStsDetAcc3D_El_Mc", "hStsDetAcc3D_El_Acc", "hStsNormSts3D_El_Rec");
//   EventDetAccElStatisticsToPTree(&pt, "hStsRichDetAcc3DEl","hStsRichDetAcc3D_El_Mc", "hStsRichDetAcc3D_El_Acc", "hStsRich3D_El_Rec");
//   EventDetAccElStatisticsToPTree(&pt, "hStsTrdDetAcc3DEl", "hStsTrdDetAcc3D_El_Mc", "hStsTrdDetAcc3D_El_Acc", "hHalfGlobal3D_El_Rec");
//   EventDetAccElStatisticsToPTree(&pt, "hStsTofDetAcc3DEl", "hStsTofDetAcc3D_El_Mc", "hStsTofDetAcc3D_El_Acc", "");
//   EventDetAccElStatisticsToPTree(&pt, "hStsRichTrdDetAcc3DEl", "hStsRichTrdDetAcc3D_El_Mc", "hStsRichTrdDetAcc3D_El_Acc", "hStsRichTrd3D_El_Rec");
//   EventDetAccElStatisticsToPTree(&pt, "hStsRichTrdTofDetAcc3DEl", "hStsRichTrdTofDetAcc3D_El_Mc", "hStsRichTrdTofDetAcc3D_El_Acc", "hStsRichTrdTof3D_El_Rec");
//   EventDetAccElStatisticsToPTree(&pt, "hStsTrdTofDetAcc3DEl", "hStsTrdTofDetAcc3D_El_Mc", "hStsTrdTofDetAcc3D_El_Acc", "hGlobal3D_El_Rec");
//
//   // STS Qa
//   pt.put("fhStsChiprim.mean", fHM->H1F("hStsChiprim")->GetMean());
//   pt.put("fhStsChiprim.rms", fHM->H1F("hStsChiprim")->GetRMS());
//   pt.put("fhStsMomresVsMom.mean", fHM->H2F("hStsMomresVsMom")->ProjectionY()->GetMean());
//   pt.put("fhStsMomresVsMom.rms", fHM->H2F("hStsMomresVsMom")->ProjectionY()->GetRMS());
//   pt.put("fhTrackLength.mean", fHM->H1F("hTrackLength")->GetMean());
//   pt.put("fhTrackLength.rms", fHM->H1F("hTrackLength")->GetRMS());
//
//   // Polar angle efficiency
//   PolarAngleEfficiencyToPTree(&pt, "hStsAngle", "hStsAngle");
//   PolarAngleEfficiencyToPTree(&pt, "hRecAngle", "hRecAngle");
//   PolarAngleEfficiencyToPTree(&pt, "hTofAngle", "hTofAngle");

   return pt;
}

void CbmLitTrackingQaPTreeCreator::NofObjectsToPTree(
   ptree& pt)
{
	vector<TH1*> histos = fHM->H1Vector("hno_NofObjects_.+");
	Int_t nofHistos = histos.size();
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string name = string(histos[iHist]->GetName()) + ".mean";
		pt.put(name, histos[iHist]->GetMean());
	}
}

void CbmLitTrackingQaPTreeCreator::TrackHitsToPTree(
      ptree& pt)
{
	vector<TH1*> histos = fHM->H1Vector("hth_.+_TrackHits_.+");
	Int_t nofHistos = histos.size();
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string name = string(histos[iHist]->GetName()) + ".mean";
		pt.put(name, histos[iHist]->GetMean());
	}
}

void CbmLitTrackingQaPTreeCreator::TrackingEfficiencyToPTree(
		ptree& pt)
{
	Int_t nofEvents = fHM->H1("hen_EventNo_TrackingQa")->GetEntries();
	vector<TH1*> histos = fHM->H1Vector("hte_.+_Eff_.+");
	Int_t nofHistos = histos.size();
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string effHistName = histos[iHist]->GetName();
		string accHistName = FindAndReplace(effHistName, "_Eff_", "_Acc_");
		string recHistName = FindAndReplace(effHistName, "_Eff_", "_Rec_");
		Double_t acc = fHM->H1(accHistName)->GetEntries() / nofEvents;
		Double_t rec = fHM->H1(recHistName)->GetEntries() / nofEvents;
		Double_t eff = (acc != 0.) ? 100. * rec / acc : 0.;
		pt.put(accHistName + ".entries", acc);
		pt.put(recHistName + ".entries", rec);
		pt.put(effHistName, eff);
	}
}

void CbmLitTrackingQaPTreeCreator::NofGhostsToPTree(
		ptree& pt)
{
	Double_t nofEvents = fHM->H1("hen_EventNo_TrackingQa")->GetEntries();
	vector<TH1*> histos = fHM->H1Vector("hng_NofGhosts_.+");
	Int_t nofHistos = histos.size();
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string name = string(histos[iHist]->GetName()) + ".entries";
		pt.put(name, histos[iHist]->GetEntries() / nofEvents);
	}
}

//void CbmLitTrackingQaPTreeCreator::EventEfficiencyStatisticsElIdToPTree(
//      ptree* pt,
//      const string& name)
//{
//   Double_t elRec = fHM->H3F(name + "_ElId_Rec")->GetEntries();
//   Double_t elAcc = fHM->H3F(name + "_ElId_Acc")->GetEntries();
//   Double_t elEff = (elAcc != 0.) ? 100. * elRec / elAcc : 0.;
//
//   Double_t piRec = fHM->H3F(name + "_PiSupp_Rec")->GetEntries();
//   Double_t piAcc = fHM->H3F(name + "_PiSupp_Acc")->GetEntries();
//   Double_t piSupp = (piRec != 0.) ? piAcc / piRec : 0.;
//
//   Double_t nofEvents = (Double_t)fHM->H1F("hEventNo")->GetEntries();
//   pt->put(name + ".el.rec", elRec/nofEvents);
//   pt->put(name + ".el.acc", elAcc/nofEvents);
//   pt->put(name + ".el.eff", elEff);
//   pt->put(name + ".pi.rec", piRec/nofEvents);
//   pt->put(name + ".pi.acc", piAcc/nofEvents);
//   pt->put(name + ".pi.supp", piSupp);
//}
//
//void CbmLitTrackingQaPTreeCreator::EventDetAccElStatisticsToPTree(
//      ptree* pt,
//      const string& name,
//      const string& hmc,
//      const string& hacc,
//      const string& hrec)
//{
//   if (!(fHM->Exists(hmc) && fHM->Exists(hacc) && fHM->Exists(hrec))) return;
//
//   Double_t mc = fHM->H3F(hmc)->GetEntries();
//   Double_t acc = fHM->H3F(hacc)->GetEntries();
//   Double_t effAcc = (mc != 0.) ? 100. * acc / mc : 0.;
//
//   Double_t rec = (hrec != "") ? fHM->H3F(hrec)->GetEntries() : 0.;
//   Double_t effRec = (hrec != "" && mc != 0.) ? 100. * rec / mc : 0.;
//
//   Double_t nofEvents = (Double_t)fHM->H1F("hEventNo")->GetEntries();
//   pt->put(name + ".detAccAcc.acc", acc/nofEvents);
//   pt->put(name + ".detAccAcc.mc", mc/nofEvents);
//   pt->put(name + ".detAccAcc.eff", effAcc);
//   pt->put(name + ".detAccRec.rec", rec/nofEvents);
//   pt->put(name + ".detAccRec.mc", mc/nofEvents);
//   pt->put(name + ".detAccRec.eff", effRec);
//}
//
//void CbmLitTrackingQaPTreeCreator::PolarAngleEfficiencyToPTree(
//      ptree* pt,
//      const string& name,
//      const string& hist)
//{
//   float minAngle = fHM->H1F(hist + "_All_Rec")->GetXaxis()->GetXmin();
//   float maxAngle = fHM->H1F(hist + "_All_Rec")->GetXaxis()->GetXmax();
//   int nofBinsAngle = fHM->H1F(hist + "_All_Rec")->GetNbinsX();
//
//   pt->put("MinAngle", minAngle);
//   pt->put("MaxAngle", maxAngle);
//   pt->put("NofBinsAngle", nofBinsAngle);
//
//   Double_t step = (maxAngle - minAngle) / nofBinsAngle;
//   Double_t eff[6][nofBinsAngle];
//   Double_t rec[6][nofBinsAngle];
//   Double_t acc[6][nofBinsAngle];
//
//   Int_t kAll = 0, kRef = 1, kPrim = 2, kSec = 3, kEl = 4, kMu = 5;
//
//   for(Int_t i = 0; i < nofBinsAngle; i++) {
//      rec[kAll][i] = fHM->H1F(hist+"_All_Rec")->GetBinContent(i+1);
//      acc[kAll][i] = fHM->H1F(hist+"_All_Acc")->GetBinContent(i+1);
//      if (acc[kAll][i] != 0.) { eff[kAll][i] = 100.*rec[kAll][i] / acc[kAll][i]; }
//
//      rec[kRef][i] = fHM->H1F(hist+"_Ref_Rec")->GetBinContent(i+1);
//      acc[kRef][i] = fHM->H1F(hist+"_Ref_Acc")->GetBinContent(i+1);
//      if (acc[kRef][i] != 0.) { eff[kRef][i] = 100.*rec[kRef][i] / acc[kRef][i]; }
//
//      rec[kPrim][i] = fHM->H1F(hist+"_Prim_Rec")->GetBinContent(i+1);
//      acc[kPrim][i] = fHM->H1F(hist+"_Prim_Acc")->GetBinContent(i+1);
//      if (acc[kPrim][i] != 0.) { eff[kPrim][i] = 100.*rec[kPrim][i] / acc[kPrim][i]; }
//
//      rec[kSec][i] = fHM->H1F(hist+"_Sec_Rec")->GetBinContent(i+1);
//      acc[kSec][i] = fHM->H1F(hist+"_Sec_Acc")->GetBinContent(i+1);
//      if (acc[kSec][i] != 0.) { eff[kSec][i] = 100.*rec[kSec][i] / acc[kSec][i]; }
//
//      rec[kEl][i] = fHM->H1F(hist+"_El_Rec")->GetBinContent(i+1);
//      acc[kEl][i] = fHM->H1F(hist+"_El_Acc")->GetBinContent(i+1);
//      if (acc[kEl][i] != 0.) { eff[kEl][i] = 100.*rec[kEl][i] / acc[kEl][i]; }
//
//      rec[kMu][i] = fHM->H1F(hist+"_Mu_Rec")->GetBinContent(i+1);
//      acc[kMu][i] = fHM->H1F(hist+"_Mu_Acc")->GetBinContent(i+1);
//      if (acc[kMu][i] != 0.) { eff[kMu][i] = 100.*rec[kMu][i] / acc[kMu][i]; }
//   }
//
//   Double_t nofEvents = (Double_t)fHM->H1F("hEventNo")->GetEntries();
//   std::stringstream ss;
//   for (Int_t i = 0; i < nofBinsAngle; i++) {
//      std::stringstream ss1;
//      ss1 << i*step;
//      std::string angle0= ss1.str();
//      std::stringstream ss2;
//      ss2 << i*step + step;
//      std::string angle1= ss2.str();
//
//      pt->put(name + ".all.rec." + angle0 + "_" + angle1, rec[kAll][i] / nofEvents);
//      pt->put(name + ".all.acc." + angle0 + "_" + angle1, acc[kAll][i] / nofEvents);
//      pt->put(name + ".all.eff." + angle0 + "_" + angle1, eff[kAll][i]);
//
//      pt->put(name + ".ref.rec." + angle0 + "_" + angle1, rec[kRef][i] / nofEvents);
//      pt->put(name + ".ref.acc." + angle0 + "_" + angle1, acc[kRef][i] / nofEvents);
//      pt->put(name + ".ref.eff." + angle0 + "_" + angle1, eff[kRef][i]);
//
//      pt->put(name + ".prim.rec." + angle0 + "_" + angle1, rec[kPrim][i] / nofEvents);
//      pt->put(name + ".prim.acc." + angle0 + "_" + angle1, acc[kPrim][i] / nofEvents);
//      pt->put(name + ".prim.eff." + angle0 + "_" + angle1, eff[kPrim][i]);
//
//      pt->put(name + ".sec.rec." + angle0 + "_" + angle1, rec[kSec][i] / nofEvents);
//      pt->put(name + ".sec.acc." + angle0 + "_" + angle1, acc[kSec][i] / nofEvents);
//      pt->put(name + ".sec.eff." + angle0 + "_" + angle1, eff[kSec][i]);
//
//      pt->put(name + ".el.rec." + angle0 + "_" + angle1, rec[kEl][i] / nofEvents);
//      pt->put(name + ".el.acc." + angle0 + "_" + angle1, acc[kEl][i] / nofEvents);
//      pt->put(name + ".el.eff." + angle0 + "_" + angle1, eff[kEl][i]);
//
//      pt->put(name + ".mu.rec." + angle0 + "_" + angle1, rec[kMu][i] / nofEvents);
//      pt->put(name + ".mu.acc." + angle0 + "_" + angle1, acc[kMu][i] / nofEvents);
//      pt->put(name + ".mu.eff." + angle0 + "_" + angle1, eff[kMu][i]);
//   }
//}
