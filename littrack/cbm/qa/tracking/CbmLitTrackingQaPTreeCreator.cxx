/**
 * \file CbmLitTrackingQaPTreeCreator.h
 * \brief Create property tree for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "qa/tracking/CbmLitTrackingQaPTreeCreator.h"
#include "qa/base/CbmLitHistManager.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"

#include <iostream>
#include <assert.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;

CbmLitTrackingQaPTreeCreator::CbmLitTrackingQaPTreeCreator():
   fHM(NULL)
{

}

CbmLitTrackingQaPTreeCreator::~CbmLitTrackingQaPTreeCreator()
{

}

TH1F* CbmLitTrackingQaPTreeCreator::H1(
      const string& name)
{
   return fHM->H1F(name);
}

TH2F* CbmLitTrackingQaPTreeCreator::H2(
      const string& name)
{
   return fHM->H2F(name);
}

TH3F* CbmLitTrackingQaPTreeCreator::H3(
      const string& name)
{
   return fHM->H3F(name);
}

ptree CbmLitTrackingQaPTreeCreator::Create(
      CbmLitHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   ptree pt;

   pt.put("IsElectronSetup", fIsElectronSetup);
   pt.put("hEventNo", (Int_t)H1("hEventNo")->GetEntries());

   // Print NOF statistics
   NofStatisticsToPTree(&pt,"hNofMvdPoints", "hNofStsPoints", "hNofRichPoints",
         "hNofTrdPoints", "hNofMuchPoints", "hNofMuchPoints", "hNofTofPoints");
   NofStatisticsToPTree(&pt, "hNofMvdDigis", "hNofStsDigis", "",
         "hNofTrdDigis", "hNofMuchDigis", "", "");
   NofStatisticsToPTree(&pt, "hNofMvdClusters", "hNofStsClusters", "",
         "hNofTrdClusters", "hNofMuchClusters", "", "");
   NofStatisticsToPTree(&pt, "hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");
   NofStatisticsToPTree(&pt, "", "hNofStsTracks", "hNofRichRings",
         "hNofTrdTracks", "hNofMuchTracks", "hNofMuchTracks", "");

   pt.put("hNofGlobalTracks", (Int_t)H1("hNofGlobalTracks")->GetMean());
   if (fIsRich) {
      pt.put("hNofRichProjections", (Int_t)H1("hNofRichProjections")->GetMean());
   }

   // print hits histos (nof all, true, fake per track/ring)
   if (fIsMvd) HitsHistosToPTree(&pt, "hMvdTrackHits");
   if (fIsSts) HitsHistosToPTree(&pt, "hStsTrackHits");
   if (fIsTrd) HitsHistosToPTree(&pt, "hTrdTrackHits");
   if (fIsMuch) HitsHistosToPTree(&pt, "hMuchTrackHits");
   if (fIsRich) HitsHistosToPTree(&pt, "hRichRingHits");

   // Print efficiency without RICH
   EventEfficiencyStatisticsToPTree(&pt, "hSts3D");
   EventEfficiencyStatisticsToPTree(&pt, "hRec3D");
   EventEfficiencyStatisticsToPTree(&pt, "hTof3D");
   EventEfficiencyStatisticsToPTree(&pt, "hSts3DNormHalfGlobal");
   EventEfficiencyStatisticsToPTree(&pt, "hHalfGlobal3D");
   EventEfficiencyStatisticsToPTree(&pt, "hSts3DNormGlobal");
   EventEfficiencyStatisticsToPTree(&pt, "hHalfGlobal3DNormGlobal");
   EventEfficiencyStatisticsToPTree(&pt, "hGlobal3D");

   // Print RICH efficiencies
   EventEfficiencyStatisticsRichToPTree(&pt, "hRich3D");
   EventEfficiencyStatisticsRichToPTree(&pt, "hSts3DNormStsRich");
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRich3D");
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichNoMatching3D");
   EventEfficiencyStatisticsRichToPTree(&pt, "hSts3DNormStsRichTrd");
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRich3DNormStsRichTrd");
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichTrd3D");
   EventEfficiencyStatisticsRichToPTree(&pt, "hSts3DNormStsRichTrdTof");
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRich3DNormStsRichTrdTof");
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichTrd3DNormStsRichTrdTof");
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichTrdTof3D");

   // electron identification
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsTrd3DElId");
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsTrd3DElIdNormStsTrdTof");
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsTrdTof3DElId");
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRich3DElId");
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRich3DElIdNormStsRichTrd");
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichTrd3DElId");
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRich3DElIdNormStsRichTrdTof");
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichTrd3DElIdNormStsRichTrdTof");
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichTrdTof3DElId");

   // Detector acceptance efficiency
   if (fIsSts) {
      EventDetAccElStatisticsToPTree(&pt, "hStsDetAcc3DEl", "hStsDetAcc3D_El_Mc",
         "hStsDetAcc3D_El_Acc", "hSts3D_El_Rec");
   }
   if (fIsSts && fIsRich){
      EventDetAccElStatisticsToPTree(&pt, "hStsRichDetAcc3DEl","hStsRichDetAcc3D_El_Mc",
         "hStsRichDetAcc3D_El_Acc", "hStsRich3D_El_Rec");
   }
   if (fIsSts && fIsTrd){
      EventDetAccElStatisticsToPTree(&pt, "hStsTrdDetAcc3DEl", "hStsTrdDetAcc3D_El_Mc",
         "hStsTrdDetAcc3D_El_Acc", "hHalfGlobal3D_El_Rec");
   }
   if (fIsSts && fIsTof){
      EventDetAccElStatisticsToPTree(&pt, "hStsTofDetAcc3DEl", "hStsTofDetAcc3D_El_Mc",
         "hStsTofDetAcc3D_El_Acc", "");
   }
   if (fIsSts && fIsRich && fIsTrd){
      EventDetAccElStatisticsToPTree(&pt, "hStsRichTrdDetAcc3DEl", "hStsRichTrdDetAcc3D_El_Mc",
         "hStsRichTrdDetAcc3D_El_Acc", "hStsRichTrd3D_El_Rec");
   }
   if (fIsSts && fIsRich && fIsTrd && fIsTof){
      EventDetAccElStatisticsToPTree(&pt, "hStsRichTrdTofDetAcc3DEl", "hStsRichTrdTofDetAcc3D_El_Mc",
         "hStsRichTrdTofDetAcc3D_El_Acc", "hStsRichTrdTof3D_El_Rec");
   }
   if (fIsSts && fIsTrd && fIsTof){
      EventDetAccElStatisticsToPTree(&pt, "hStsTrdTofDetAcc3DEl", "hStsTrdTofDetAcc3D_El_Mc",
         "hStsTrdTofDetAcc3D_El_Acc", "hGlobal3D_El_Rec");
   }

   // print ghost statistics
   Double_t nofEvents = (Double_t)H1("hEventNo")->GetEntries();
   pt.put("fhStsGhostNh", H1("hStsGhostNh")->GetEntries() / nofEvents);
   pt.put("fhRecGhostNh", H1("hRecGhostNh")->GetEntries() / nofEvents);
   pt.put("fhRichGhostNh", H1("hRichGhostNh")->GetEntries() / nofEvents);
   pt.put("fhRichGhostStsMatchingNh", H1("hRichGhostStsMatchingNh")->GetEntries() / nofEvents);
   pt.put("fhStsGhostRichMatchingNh", H1("hStsGhostRichMatchingNh")->GetEntries() / nofEvents);
   pt.put("fhRichGhostElIdNh", H1("hRichGhostElIdNh")->GetEntries() / nofEvents);

   // STS Qa
   pt.put("fhStsChiprim.mean", H1("hStsChiprim")->GetMean());
   pt.put("fhStsChiprim.rms", H1("hStsChiprim")->GetRMS());
   pt.put("fhStsMomresVsMom.mean", H2("hStsMomresVsMom")->ProjectionY()->GetMean());
   pt.put("fhStsMomresVsMom.rms", H2("hStsMomresVsMom")->ProjectionY()->GetRMS());
   pt.put("fhTrackLength.mean", H1("hTrackLength")->GetMean());
   pt.put("fhTrackLength.rms", H1("hTrackLength")->GetRMS());

   // Polar angle efficiency
   pt.put("MinAngle", fMinAngle);
   pt.put("MaxAngle", fMaxAngle);
   pt.put("NofBinsAngle", fNofBinsAngle);
   PolarAngleEfficiencyToPTree(&pt, "hStsAngle", "hStsAngle");
   PolarAngleEfficiencyToPTree(&pt, "hRecAngle", "hRecAngle");
   PolarAngleEfficiencyToPTree(&pt, "hTofAngle", "hTofAngle");

   if (fOutputDir != "") write_json(std::string(fOutputDir + "rec_qa.json").c_str(), pt);
   return pt;
}

void CbmLitTrackingQaPTreeCreator::NofStatisticsToPTree(
   ptree* pt,
   const string& mvd,
   const string& sts,
   const string& rich,
   const string& trd,
   const string& muchP,
   const string& muchS,
   const string& tof)
{
   if (mvd != "" && fIsMvd) {
      pt->put(mvd, (Int_t)H1(mvd)->GetMean());
   }
   if (sts != "" && fIsSts) {
      pt->put(sts, (Int_t)H1(sts)->GetMean());
   }
   if (rich != "" && fIsRich) {
      pt->put(rich, (Int_t)H1(rich)->GetMean());
   }
   if (trd != "" && fIsTrd) {
      pt->put(trd, (Int_t)H1(trd)->GetMean());
   }
   if (muchP != "" && fIsMuch) {
      pt->put(muchP, (Int_t)H1(muchP)->GetMean());
   }
   if (muchS != "" && fIsMuch) {
      pt->put(muchS, (Int_t)H1(muchS)->GetMean());
   }
   if (tof != "" && fIsTof) {
      pt->put(tof, (Int_t)H1(tof)->GetMean());
   }
}

void CbmLitTrackingQaPTreeCreator::HitsHistosToPTree(
      ptree* pt,
      const string& name)
{
   pt->put(name+".all", H1(name+"_All")->GetMean());
   pt->put(name+".true", H1(name+"_True")->GetMean());
   pt->put(name+".fake", H1(name+"_Fake")->GetMean());
   pt->put(name+".trueOverAll", H1(name+"_TrueOverAll")->GetMean());
   pt->put(name+".fakeOverAll", H1(name+"_FakeOverAll")->GetMean());
}

void CbmLitTrackingQaPTreeCreator::EventEfficiencyStatisticsToPTree(
      ptree* pt,
      const string& name)
{
   Double_t allRec = H3(name+"_All_Rec")->GetEntries();
   Double_t allAcc = H3(name+"_All_Acc")->GetEntries();
   Double_t allEff = (allAcc != 0.) ? 100. * allRec / allAcc : 0.;

   Double_t refRec = H3(name+"_Ref_Rec")->GetEntries();
   Double_t refAcc = H3(name+"_Ref_Acc")->GetEntries();
   Double_t refEff = (refAcc != 0.) ? 100. * refRec / refAcc : 0.;

   Double_t primRec = H3(name+"_Prim_Rec")->GetEntries();
   Double_t primAcc = H3(name+"_Prim_Acc")->GetEntries();
   Double_t primEff = (primAcc != 0.) ? 100. * primRec / primAcc : 0.;

   Double_t secRec = H3(name+"_Sec_Rec")->GetEntries();
   Double_t secAcc = H3(name+"_Sec_Acc")->GetEntries();
   Double_t secEff = (secAcc != 0.) ? 100. * secRec / secAcc : 0.;

   Double_t muRec = H3(name+"_Mu_Rec")->GetEntries();
   Double_t muAcc = H3(name+"_Mu_Acc")->GetEntries();
   Double_t muEff = (muAcc != 0.) ? 100. * muRec / muAcc : 0;

   Double_t elRec = H3(name+"_El_Rec")->GetEntries();
   Double_t elAcc = H3(name+"_El_Acc")->GetEntries();
   Double_t elEff = (elAcc != 0.) ? 100. * elRec / elAcc : 0;

   Double_t nofEvents = (Double_t)H1("hEventNo")->GetEntries();
   pt->put(name+".all.rec", allRec/nofEvents);
   pt->put(name+".all.acc", allAcc/nofEvents);
   pt->put(name+".all.eff", allEff);
   pt->put(name+".ref.rec", refRec/nofEvents);
   pt->put(name+".ref.acc", refAcc/nofEvents);
   pt->put(name+".ref.eff", refEff);
   pt->put(name+".prim.rec", primRec/nofEvents);
   pt->put(name+".prim.acc", primAcc/nofEvents);
   pt->put(name+".prim.eff", primEff);
   pt->put(name+".sec.rec", secRec/nofEvents);
   pt->put(name+".sec.acc", secAcc/nofEvents);
   pt->put(name+".sec.eff", secEff);
   pt->put(name+".mu.rec", muRec/nofEvents);
   pt->put(name+".mu.acc", muAcc/nofEvents);
   pt->put(name+".mu.eff", muEff);
   pt->put(name+".el.rec", elRec/nofEvents);
   pt->put(name+".el.acc", elAcc/nofEvents);
   pt->put(name+".el.eff", elEff);
}

void CbmLitTrackingQaPTreeCreator::EventEfficiencyStatisticsRichToPTree(
      ptree* pt,
      const std::string& name)
{
   Double_t allRec = H3(name+"_All_Rec")->GetEntries();
   Double_t allAcc = H3(name+"_All_Acc")->GetEntries();
   Double_t allEff = (allAcc != 0.) ? 100. * allRec / allAcc : 0.;

   Double_t allRefRec = H3(name+"_AllRef_Rec")->GetEntries();
   Double_t allRefAcc = H3(name+"_AllRef_Acc")->GetEntries();
   Double_t allRefEff = (allRefAcc != 0.) ? 100. * allRefRec / allRefAcc : 0.;

   Double_t elRec = H3(name+"_El_Rec")->GetEntries();
   Double_t elAcc = H3(name+"_El_Acc")->GetEntries();
   Double_t elEff = (elAcc != 0.) ? 100. * elRec / elAcc : 0.;

   Double_t elRefRec = H3(name+"_ElRef_Rec")->GetEntries();
   Double_t elRefAcc = H3(name+"_ElRef_Acc")->GetEntries();
   Double_t elRefEff = (elRefAcc != 0.) ? 100. * elRefRec / elRefAcc : 0.;

   Double_t piRec = H3(name+"_Pi_Rec")->GetEntries();
   Double_t piAcc = H3(name+"_Pi_Acc")->GetEntries();
   Double_t piEff = (piAcc != 0.) ? 100. * piRec / piAcc : 0.;

   Double_t piRefRec = H3(name+"_PiRef_Rec")->GetEntries();
   Double_t piRefAcc = H3(name+"_PiRef_Acc")->GetEntries();
   Double_t piRefEff = (piRefAcc != 0.) ? 100. * piRefRec / piRefAcc : 0.;

   Double_t nofEvents = (Double_t)H1("hEventNo")->GetEntries();
   pt->put(name+".All.rec", allRec/nofEvents);
   pt->put(name+".All.acc", allAcc/nofEvents);
   pt->put(name+".All.eff", allEff);
   pt->put(name+".AllRef.rec", allRefRec/nofEvents);
   pt->put(name+".AllRef.acc", allRefAcc/nofEvents);
   pt->put(name+".AllRef.eff", allRefEff);
   pt->put(name+".El.rec", elRec/nofEvents);
   pt->put(name+".El.acc", elAcc/nofEvents);
   pt->put(name+".El.eff", elEff);
   pt->put(name+".ElRef.rec", elRefRec/nofEvents);
   pt->put(name+".ElRef.acc", elRefAcc/nofEvents);
   pt->put(name+".ElRef.eff", elRefEff);
   pt->put(name+".Pi.rec", piRec/nofEvents);
   pt->put(name+".Pi.acc", piAcc/nofEvents);
   pt->put(name+".Pi.eff", piEff);
   pt->put(name+".PiRef.rec", piRefRec/nofEvents);
   pt->put(name+".PiRef.acc", piRefAcc/nofEvents);
   pt->put(name+".PiRef.eff", piRefEff);
}

void CbmLitTrackingQaPTreeCreator::EventEfficiencyStatisticsElIdToPTree(
      ptree* pt,
      const string& name)
{
   Double_t elRec = H3(name+"_ElId_Rec")->GetEntries();
   Double_t elAcc = H3(name+"_ElId_Acc")->GetEntries();
   Double_t elEff = (elAcc != 0.) ? 100. * elRec / elAcc : 0.;

   Double_t piRec = H3(name+"_PiSupp_Rec")->GetEntries();
   Double_t piAcc = H3(name+"_PiSupp_Acc")->GetEntries();
   Double_t piSupp = (piRec != 0.) ? piAcc / piRec : 0.;

   Double_t nofEvents = (Double_t)H1("hEventNo")->GetEntries();
   pt->put(name+".el.rec", elRec/nofEvents);
   pt->put(name+".el.acc", elAcc/nofEvents);
   pt->put(name+".el.eff", elEff);
   pt->put(name+".pi.rec", piRec/nofEvents);
   pt->put(name+".pi.acc", piAcc/nofEvents);
   pt->put(name+".pi.supp", piSupp);
}

void CbmLitTrackingQaPTreeCreator::EventDetAccElStatisticsToPTree(
      ptree* pt,
      const string& name,
      const string& hmc,
      const string& hacc,
      const string& hrec)
{
   Double_t mc = H3(hmc)->GetEntries();
   Double_t acc = H3(hacc)->GetEntries();
   Double_t effAcc = (mc != 0.) ? 100. * acc / mc : 0.;

   Double_t rec = (hrec != "") ? H3(hrec)->GetEntries() : 0.;
   Double_t effRec = (hrec != "" && mc != 0.) ? 100. * rec / mc : 0.;

   Double_t nofEvents = (Double_t)H1("hEventNo")->GetEntries();
   pt->put(name+".detAccAcc.acc", acc/nofEvents);
   pt->put(name+".detAccAcc.mc", mc/nofEvents);
   pt->put(name+".detAccAcc.eff", effAcc);
   pt->put(name+".detAccRec.rec", rec/nofEvents);
   pt->put(name+".detAccRec.mc", mc/nofEvents);
   pt->put(name+".detAccRec.eff", effRec);
}

void CbmLitTrackingQaPTreeCreator::PolarAngleEfficiencyToPTree(
      ptree* pt,
      const string& name,
      const string& hist)
{
   Double_t step = (fMaxAngle - fMinAngle) / fNofBinsAngle;
   Double_t eff[6][fNofBinsAngle];
   Double_t rec[6][fNofBinsAngle];
   Double_t acc[6][fNofBinsAngle];
   Int_t kAll = 0, kRef = 1, kPrim = 2, kSec = 3, kEl = 4, kMu = 5;

   for(Int_t i = 0; i < fNofBinsAngle; i++) {
      rec[kAll][i] = H1(hist+"_All_Rec")->GetBinContent(i+1);
      acc[kAll][i] = H1(hist+"_All_Acc")->GetBinContent(i+1);
      if (acc[kAll][i] != 0.) { eff[kAll][i] = 100.*rec[kAll][i] / acc[kAll][i]; }

      rec[kRef][i] = H1(hist+"_Ref_Rec")->GetBinContent(i+1);
      acc[kRef][i] = H1(hist+"_Ref_Acc")->GetBinContent(i+1);
      if (acc[kRef][i] != 0.) { eff[kRef][i] = 100.*rec[kRef][i] / acc[kRef][i]; }

      rec[kPrim][i] = H1(hist+"_Prim_Rec")->GetBinContent(i+1);
      acc[kPrim][i] = H1(hist+"_Prim_Acc")->GetBinContent(i+1);
      if (acc[kPrim][i] != 0.) { eff[kPrim][i] = 100.*rec[kPrim][i] / acc[kPrim][i]; }

      rec[kSec][i] = H1(hist+"_Sec_Rec")->GetBinContent(i+1);
      acc[kSec][i] = H1(hist+"_Sec_Acc")->GetBinContent(i+1);
      if (acc[kSec][i] != 0.) { eff[kSec][i] = 100.*rec[kSec][i] / acc[kSec][i]; }

      rec[kEl][i] = H1(hist+"_El_Rec")->GetBinContent(i+1);
      acc[kEl][i] = H1(hist+"_El_Acc")->GetBinContent(i+1);
      if (acc[kEl][i] != 0.) { eff[kEl][i] = 100.*rec[kEl][i] / acc[kEl][i]; }

      rec[kMu][i] = H1(hist+"_Mu_Rec")->GetBinContent(i+1);
      acc[kMu][i] = H1(hist+"_Mu_Acc")->GetBinContent(i+1);
      if (acc[kMu][i] != 0.) { eff[kMu][i] = 100.*rec[kMu][i] / acc[kMu][i]; }
   }

   Double_t nofEvents = (Double_t)H1("hEventNo")->GetEntries();
   Int_t w = 17;
   std::stringstream ss;
   for (Int_t i = 0; i < fNofBinsAngle; i++) {
      std::stringstream ss1;
      ss1 << i*step;
      std::string angle0= ss1.str();
      std::stringstream ss2;
      ss2 << i*step + step;
      std::string angle1= ss2.str();

      pt->put(name+".all.rec."+angle0 +"_" + angle1, rec[kAll][i]/nofEvents);
      pt->put(name+".all.acc."+angle0 +"_" + angle1, acc[kAll][i]/nofEvents);
      pt->put(name+".all.eff."+angle0 +"_" + angle1, eff[kAll][i]);

      pt->put(name+".ref.rec."+angle0 +"_" + angle1, rec[kRef][i]/nofEvents);
      pt->put(name+".ref.acc."+angle0 +"_" + angle1, acc[kRef][i]/nofEvents);
      pt->put(name+".ref.eff."+angle0 +"_" + angle1, eff[kRef][i]);

      pt->put(name+".prim.rec."+angle0 +"_" + angle1, rec[kPrim][i]/nofEvents);
      pt->put(name+".prim.acc."+angle0 +"_" + angle1, acc[kPrim][i]/nofEvents);
      pt->put(name+".prim.eff."+angle0 +"_" + angle1, eff[kPrim][i]);

      pt->put(name+".sec.rec."+angle0 +"_" + angle1, rec[kSec][i]/nofEvents);
      pt->put(name+".sec.acc."+angle0 +"_" + angle1, acc[kSec][i]/nofEvents);
      pt->put(name+".sec.eff."+angle0 +"_" + angle1, eff[kSec][i]);

      pt->put(name+".el.rec."+angle0 +"_" + angle1, rec[kEl][i]/nofEvents);
      pt->put(name+".el.acc."+angle0 +"_" + angle1, acc[kEl][i]/nofEvents);
      pt->put(name+".el.eff."+angle0 +"_" + angle1, eff[kEl][i]);

      pt->put(name+".mu.rec."+angle0 +"_" + angle1, rec[kMu][i]/nofEvents);
      pt->put(name+".mu.acc."+angle0 +"_" + angle1, acc[kMu][i]/nofEvents);
      pt->put(name+".mu.eff."+angle0 +"_" + angle1, eff[kMu][i]);
   }
}
