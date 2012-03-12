/**
 * \file CbmLitTrackingQaReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaReport.h"
#include "../report/CbmLitReportElement.h"
#include "../../../std/utils/CbmLitUtils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
#include <iomanip>

using std::vector;
using boost::assign::list_of;
using lit::NumberToString;

CbmLitTrackingQaReport::CbmLitTrackingQaReport()
{
}

CbmLitTrackingQaReport::~CbmLitTrackingQaReport()
{
}

void CbmLitTrackingQaReport::Create(
   ostream& out)
{
   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << "Number of events: " << PrintValue("hEventNo") << endl;

   // Number of tracks and rings table
   out << fR->TableBegin("Number of reconstructed tracks and rings per event",
         list_of("Global track")("STS track")("RICH ring")("RICH proj")("TRD track")("MUCH track"));
   out << PrintNofStatistics("hNofGlobalTracks", "hNofStsTracks", "hNofRichRings",
         "hNofRichProjections", "hNofTrdTracks", "hNofMuchTracks");
   out << fR->TableEnd();

   // Number of all, true and fake hits in tracks and rings
   out << fR->TableBegin("Number of all, true and fake hits in tracks and rings",
         list_of("")("all")("true")("fake")("true/all")("fake/all"));
   out << PrintHits("MVD", "hMvdTrackHits");
   out << PrintHits("STS", "hStsTrackHits");
   out << PrintHits("TRD", "hTrdTrackHits");
   out << PrintHits("MUCH", "hMuchTrackHits");
   out << PrintHits("RICH", "hRichRingHits");
   out << fR->TableEnd();

   // Reconstruction efficiency without RICH
   vector<string> cols3 = list_of("")("all")("reference")("primary")("secondary")("electron")("muon");
   out << fR->TableBegin("Reconstruction efficiency without RICH", cols3);
   out << PrintEfficiency("STS", "hSts3D");
   out << PrintEfficiency("TRD(MUCH)", "hRec3D");
   out << PrintEfficiency("TOF matching", "hTof3D");
   out << fR->TableEmptyRow(cols3.size()+1, "Normalization STS+TRD(MUCH)");
   out << PrintEfficiency("STS", "hSts3DNormHalfGlobal");
   string s = "STS+TRD(MUCH)";
   out << PrintEfficiency(s, "hHalfGlobal3D");
   out << fR->TableEmptyRow(cols3.size()+1, "Normalization STS+TRD(MUCH)+TOF");
   out << PrintEfficiency("STS", "hSts3DNormGlobal");
   out << PrintEfficiency(s, "hHalfGlobal3DNormGlobal");
   string s2 = s + "+TOF";
   out << PrintEfficiency(s2, "hGlobal3D");
   out << fR->TableEnd();

   // Reconstruction efficiency with RICH
   vector<string> cols4 = list_of("")("all")("all reference")("electron")("electron ref")("pion")("pion ref");
   out << fR->TableBegin("Reconstruction efficiency with RICH", cols4);
   out << PrintEfficiencyRich("RICH", "hRich3D");
   out << fR->TableEmptyRow(cols4.size()+1, "Normalization STS+RICH");
   out << PrintEfficiencyRich("STS", "hSts3DNormStsRich");
   out << PrintEfficiencyRich("STS+RICH no match", "hStsRichNoMatching3D");
   out << PrintEfficiencyRich("STS+RICH", "hStsRich3D");
   out << fR->TableEmptyRow(cols4.size()+1, "Normalization STS+RICH+TRD");
   out << PrintEfficiencyRich("STS", "hSts3DNormStsRichTrd");
   out << PrintEfficiencyRich("STS+RICH", "hStsRich3DNormStsRichTrd");
   out << PrintEfficiencyRich("STS+RICH+TRD", "hStsRichTrd3D");
   out << fR->TableEmptyRow(cols4.size()+1, "Normalization STS+RICH+TRD+TOF");
   out << PrintEfficiencyRich("STS", "hSts3DNormStsRichTrdTof");
   out << PrintEfficiencyRich("STS+RICH", "hStsRich3DNormStsRichTrdTof");
   out << PrintEfficiencyRich("STS+RICH+TRD", "hStsRichTrd3DNormStsRichTrdTof");
   out << PrintEfficiencyRich("STS+RICH+TRD+TOF", "hStsRichTrdTof3D");
   out << fR->TableEnd();

   // Electron identification efficiency
   vector<string> cols5 = list_of("")("efficiency")("pion supp");
   out << fR->TableBegin("Electron identification efficiency and pion suppression", cols5);
   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+TRD");
   out << PrintEfficiencyElId("STS+TRD", "hStsTrd3DElId");
   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+TRD+TOF");
   out << PrintEfficiencyElId("STS+TRD", "hStsTrd3DElIdNormStsTrdTof");
   out << PrintEfficiencyElId("STS+TRD+TOF", "hStsTrdTof3DElId");
   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+RICH");
   out << PrintEfficiencyElId("STS+RICH", "hStsRich3DElId");
   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+RICH+TRD");
   out << PrintEfficiencyElId("STS+RICH", "hStsRich3DElIdNormStsRichTrd");
   out << PrintEfficiencyElId("STS+RICH+TRD", "hStsRichTrd3DElId");
   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+RICH+TRD+TOF");
   out << PrintEfficiencyElId("STS+RICH", "hStsRich3DElIdNormStsRichTrdTof");
   out << PrintEfficiencyElId("STS+RICH+TRD", "hStsRichTrd3DElIdNormStsRichTrdTof");
   out << PrintEfficiencyElId("STS+RICH+TRD+TOF", "hStsRichTrdTof3DElId");
   out << fR->TableEnd();

   // Detector acceptance efficiency
   out << fR->TableBegin("Detector acceptance for primary electrons", list_of("")("ACC/MC")("REC/MC"));
   out << PrintDetAccEl("STS", "hStsDetAcc3DEl");
   out << PrintDetAccEl("STS-RICH","hStsRichDetAcc3DEl");
   out << PrintDetAccEl("STS-TRD", "hStsTrdDetAcc3DEl");
   out << PrintDetAccEl("STS-TOF", "hStsTofDetAcc3DEl");
   out << PrintDetAccEl("STS-RICH-TRD", "hStsRichTrdDetAcc3DEl");
   out << PrintDetAccEl("STS-RICH-TRD-TOF", "hStsRichTrdTofDetAcc3DEl");
   out << PrintDetAccEl("STS-TRD-TOF", "hStsTrdTofDetAcc3DEl");
   out << fR->TableEnd();

   // Ghost statistics
   out << fR->TableBegin("Number of ghosts per event", list_of("")("Number of ghosts"));
   out << fR->TableRow(list_of("STS")(PrintValue("fhStsGhostNh").c_str()));
   out << fR->TableRow(list_of("TRD(MUCH)")(PrintValue("fhRecGhostNh").c_str()));
   out << fR->TableRow(list_of("RICH")(PrintValue("fhRichGhostNh").c_str()));
   out << fR->TableEmptyRow(2, "after STS-RICH matching");
   out << fR->TableRow(list_of("STS")(PrintValue("fhStsGhostRichMatchingNh").c_str()));
   out << fR->TableRow(list_of("RICH")(PrintValue("fhRichGhostStsMatchingNh").c_str()));
   out << fR->TableEmptyRow(2, "after STS-RICH matching and el id");
   out << fR->TableRow(list_of("RICH")(PrintValue("fhRichGhostElIdNh").c_str()));
   out << fR->TableEnd();

   // STS quality numbers
   out << fR->TableBegin("STS quality numbers", list_of("")("Mean")("RMS"));
   out << fR->TableRow(list_of("Chi2 to primary vertex")(PrintValue("fhStsChiprim.mean").c_str())(PrintValue("fhStsChiprim.rms").c_str()));
   out << fR->TableRow(list_of("Momentum resolution")(PrintValue("fhStsMomresVsMom.mean").c_str())(PrintValue("fhStsMomresVsMom.rms").c_str()));
   out << fR->TableRow(list_of("Tr. len. 100*(MC-Rec)/MC")(PrintValue("fhTrackLength.mean").c_str())(PrintValue("fhTrackLength.rms").c_str()));
   out << fR->TableEnd();

   // Tracking efficiency vs. polar angle
   vector<string> cols7 = list_of("")("all")("reference")("primary")("secondary")("electron")("muon");
   out << fR->TableBegin("Tracking efficiency in dependence on polar angle", cols7);
   out << fR->TableEmptyRow(cols7.size()+1, "STS");
   out << setfill(' ') << left << PrintPolarAngle("hStsAngle");
   out << fR->TableEmptyRow(cols7.size()+1, "TRD(MUCH)");
   out << setfill(' ') << left << PrintPolarAngle("hRecAngle");
   out << fR->TableEmptyRow(cols7.size()+1, "TOF");
   out << setfill(' ') << left << PrintPolarAngle("hTofAngle");
   out << fR->TableEnd();

   out << fR->Image("Global efficiency all", "tracking_qa_global_efficiency_all");
   out << fR->Image("Global efficiency signal", "tracking_qa_global_efficiency_signal");
   out << fR->Image("STS efficiency", "tracking_qa_sts_efficiency");
   out << fR->Image("TRD(MUCH) efficiency", "tracking_qa_rec_efficiency");
   out << fR->Image("TOF matching efficiency", "tracking_qa_tof_efficiency");
   // TODO: Check for RICH detector
   out << fR->Image("RICH efficiency electrons", "tracking_qa_rich_efficiency_electrons_mom");
   out << fR->Image("STS+RICH efficiency electrons", "tracking_qa_sts_rich_efficiency_electrons");
   out << fR->Image("STS+RICH+TRD efficiency electrons", "tracking_qa_sts_rich_trd_efficiency_electrons");
   out << fR->Image("STS+RICH+TRD+TOF efficiency electrons", "tracking_qa_sts_rich_trd_tof_efficiency_electrons");
   //out << fR->PrintImage("STS+RICH+TRD+TOF detector acceptance electrons", "tracking_qa_sts_rich_trd_tof_detector_acceptance");
   out << fR->Image("STS+RICH+TRD+TOF electron identification efficiency electrons", "tracking_qa_sts_rich_trd_tof_electron_identification");
   out << fR->Image("STS+RICH+TRD+TOF pion suppression", "tracking_qa_sts_rich_trd_tof_pion_suppression");
   out <<  fR->DocumentEnd();
}

string CbmLitTrackingQaReport::PrintNofStatistics(
        const string& global,
        const string& sts,
        const string& richRing,
        const string& richProj,
        const string& trd,
        const string& much)
{
   string str1 = (global != "") ? NumberToString<Int_t>(fQa.get(global, -1.)) : "-";
   string str2 = (sts != "") ? NumberToString<Int_t>(fQa.get(sts, -1.)) : "-";
   string str3 = (richRing != "") ? NumberToString<Int_t>(fQa.get(richRing, -1.)) : "-";
   string str4 = (richProj != "") ? NumberToString<Int_t>(fQa.get(richProj, -1.)) : "-";
   string str5 = (trd != "") ? NumberToString<Int_t>(fQa.get(trd, -1.)) : "-";
   string str6 = (much != "") ? NumberToString<Int_t>(fQa.get(much, -1.)) : "-";
   return fR->TableRow(list_of(str1)(str2)(str3)(str4)(str5)(str6));
}

string CbmLitTrackingQaReport::PrintHits(
        const string& name,
        const string& hist)
{
   float all = fQa.get(hist + ".all", -1.);
   float trueh = fQa.get(hist + ".true", -1.);
   float fakeh = fQa.get(hist + ".fake", -1.);
   float toa = fQa.get(hist + ".trueOverAll", -1.);
   float foa = fQa.get(hist + ".fakeOverAll", -1.);
   string str1 = (all == -1.) ? "-" : NumberToString<float>(all, 2);
   string str2 = (trueh == -1.) ? "-" : NumberToString<float>(trueh, 2);
   string str3 = (fakeh == -1.) ? "-" : NumberToString<float>(fakeh, 2);
   string str4 = (toa == -1.) ? "-" : NumberToString<float>(100. * toa, 1);
   string str5 = (foa == -1.) ? "-" : NumberToString<float>(100. * foa, 1);
   return fR->TableRow(list_of(name)(str1)(str2)(str3)(str4)(str5));
}

string CbmLitTrackingQaReport::PrintEfficiency(
     const string& name,
     const string& hist)
{
   string allRec = NumberToString<Double_t>(fQa.get(hist + ".all.rec", -1.));
   string allAcc = NumberToString<Double_t>(fQa.get(hist + ".all.acc", -1.));
   string allEff = NumberToString<Double_t>(fQa.get(hist + ".all.eff", -1.));
   string refRec = NumberToString<Double_t>(fQa.get(hist + ".ref.rec", -1.));
   string refAcc = NumberToString<Double_t>(fQa.get(hist + ".ref.acc", -1.));
   string refEff = NumberToString<Double_t>(fQa.get(hist + ".ref.eff", -1.));
   string primRec = NumberToString<Double_t>(fQa.get(hist + ".prim.rec", -1.));
   string primAcc = NumberToString<Double_t>(fQa.get(hist + ".prim.acc", -1.));
   string primEff = NumberToString<Double_t>(fQa.get(hist + ".prim.eff", -1.));
   string secRec = NumberToString<Double_t>(fQa.get(hist + ".sec.rec", -1.));
   string secAcc = NumberToString<Double_t>(fQa.get(hist + ".sec.acc", -1.));
   string secEff = NumberToString<Double_t>(fQa.get(hist + ".sec.eff", -1.));
   string muRec = NumberToString<Double_t>(fQa.get(hist + ".mu.rec", -1.));
   string muAcc = NumberToString<Double_t>(fQa.get(hist + ".mu.acc", -1.));
   string muEff = NumberToString<Double_t>(fQa.get(hist + ".mu.eff", -1.));
   string elRec = NumberToString<Double_t>(fQa.get(hist + ".el.rec", -1.));
   string elAcc = NumberToString<Double_t>(fQa.get(hist + ".el.acc", -1.));
   string elEff = NumberToString<Double_t>(fQa.get(hist + ".el.eff", -1.));
   string str1 = allEff + "(" + allRec + "/" + allAcc + ")";
   string str2 = refEff + "(" + refRec + "/" + refAcc + ")";
   string str3 = primEff + "(" + primRec + "/" + primAcc + ")";
   string str4 = secEff + "(" + secRec + "/" + secAcc + ")";
   string str5 = elEff + "(" + elRec + "/" + elAcc + ")";
   string str6 = muEff + "(" + muRec + "/" + muAcc + ")";
   return fR->TableRow(list_of(name)(str1)(str2)(str3)(str4)(str5)(str6));
}

string CbmLitTrackingQaReport::PrintEfficiencyRich(
      const string& name,
      const string& hist)
{
   string allRec = NumberToString<Double_t>(fQa.get(hist + ".All.rec", -1.));
   string allAcc = NumberToString<Double_t>(fQa.get(hist + ".All.acc", -1.));
   string allEff = NumberToString<Double_t>(fQa.get(hist + ".All.eff", -1.));
   string allRefRec = NumberToString<Double_t>(fQa.get(hist + ".AllRef.rec", -1.));
   string allRefAcc = NumberToString<Double_t>(fQa.get(hist + ".AllRef.acc", -1.));
   string allRefEff = NumberToString<Double_t>(fQa.get(hist + ".AllRef.eff", -1.));
   string elRec = NumberToString<Double_t>(fQa.get(hist + ".El.rec", -1.));
   string elAcc = NumberToString<Double_t>(fQa.get(hist + ".El.acc", -1.));
   string elEff = NumberToString<Double_t>(fQa.get(hist + ".El.eff", -1.));
   string elRefRec = NumberToString<Double_t>(fQa.get(hist + ".ElRef.rec", -1.));
   string elRefAcc = NumberToString<Double_t>(fQa.get(hist + ".ElRef.acc", -1.));
   string elRefEff = NumberToString<Double_t>(fQa.get(hist + ".ElRef.eff", -1.));
   string piRec = NumberToString<Double_t>(fQa.get(hist + ".Pi.rec", -1.));
   string piAcc = NumberToString<Double_t>(fQa.get(hist + ".Pi.acc", -1.));
   string piEff = NumberToString<Double_t>(fQa.get(hist + ".Pi.eff", -1.));
   string piRefRec = NumberToString<Double_t>(fQa.get(hist + ".PiRef.rec", -1.));
   string piRefAcc = NumberToString<Double_t>(fQa.get(hist + ".PiRef.acc", -1.));
   string piRefEff = NumberToString<Double_t>(fQa.get(hist + ".PiRef.eff", -1.));
   string str1 = allEff + "(" + allRec + "/" + allAcc + ")";
   string str2 = allRefEff + "(" + allRefRec + "/" + allRefAcc + ")";
   string str3 = elEff + "(" + elRec + "/" + elAcc + ")";
   string str4 = elRefEff + "(" + elRefRec + "/" + elRefAcc + ")";
   string str5 = piEff + "(" + piRec + "/" + piAcc + ")";
   string str6 = piRefEff + "(" + piRefRec + "/" + piRefAcc + ")";
   return fR->TableRow(list_of(name)(str1)(str2)(str3)(str4)(str5)(str6));
}

string CbmLitTrackingQaReport::PrintEfficiencyElId(
      const string& name,
      const string& hist)
{
   string elRec = NumberToString<Double_t>(fQa.get(hist + ".el.rec", -1.));
   string elAcc = NumberToString<Double_t>(fQa.get(hist + ".el.acc", -1.));
   string elEff = NumberToString<Double_t>(fQa.get(hist + ".el.eff", -1.));
   string piRec = NumberToString<Double_t>(fQa.get(hist + ".pi.rec", -1.));
   string piAcc = NumberToString<Double_t>(fQa.get(hist + ".pi.acc", -1.));
   string piSupp = NumberToString<Double_t>(fQa.get(hist + ".pi.supp", -1.));
   string str1 = elEff + "(" + elRec + "/" + elAcc + ")";
   string str2 = piSupp + "(" + piAcc + "/" + piRec + ")";
   return fR->TableRow(list_of(name)(str1)(str2));
}

string CbmLitTrackingQaReport::PrintDetAccEl(
      const string& name,
      const string& hist)
{
   string acc = NumberToString<Double_t>(fQa.get(hist + ".detAccAcc.acc", -1.));
   string mc = NumberToString<Double_t>(fQa.get(hist + ".detAccAcc.mc", -1.));
   string effAcc = NumberToString<Double_t>(fQa.get(hist + ".detAccAcc.eff", -1.));
   string rec = NumberToString<Double_t>(fQa.get(hist + ".detAccRec.rec", -1.));
   string effRec = NumberToString<Double_t>(fQa.get(hist + ".detAccRec.eff", -1.));
   string str1 = effAcc + "(" + acc + "/" + mc + ")";
   string str2 = effRec + "(" + rec + "/" + mc + ")";
   return fR->TableRow(list_of(name)(str1)(str2));
}

string CbmLitTrackingQaReport::PrintPolarAngle(
      const string& hist)
{
   Double_t maxAngle = fQa.get("MaxAngle", -1.);
   Double_t minAngle = fQa.get("MinAngle", -1.);
   Double_t nofBinsAngle = fQa.get("NofBinsAngle", -1.);
   Double_t step = (maxAngle - minAngle) / nofBinsAngle;

   assert(maxAngle != -1. && minAngle != -1. && nofBinsAngle != -1.);

   string str;
   for (Int_t iAngle = 0; iAngle < nofBinsAngle; iAngle++) {
      string angle0 = lit::ToString<Double_t>(iAngle * step);
      string angle1 = lit::ToString<Double_t>(iAngle * step + step);
      string str0 = "(" + angle0 + "-" + angle1 + ")";

      string str1 = NumberToString<Double_t>(fQa.get(hist + ".all.eff." + angle0 + "_" + angle1, -1.))
          + "(" + NumberToString<Double_t>(fQa.get(hist + ".all.rec." + angle0 + "_" + angle1, -1.))
          + "/" + NumberToString<Double_t>(fQa.get(hist + ".all.acc." + angle0 + "_" + angle1, -1.)) + ")";
      string str2 = NumberToString<Double_t>(fQa.get(hist + ".ref.eff." + angle0 + "_" + angle1, -1.))
          + "(" + NumberToString<Double_t>(fQa.get(hist + ".ref.rec." + angle0 + "_" + angle1, -1.))
          + "/" + NumberToString<Double_t>(fQa.get(hist + ".ref.acc." + angle0 + "_" + angle1, -1.)) + ")";
      string str3 = NumberToString<Double_t>(fQa.get(hist + ".prim.eff." + angle0 + "_" + angle1, -1.))
          + "(" + NumberToString<Double_t>(fQa.get(hist + ".prim.rec." + angle0 + "_" + angle1, -1.))
          + "/" + NumberToString<Double_t>(fQa.get(hist + ".prim.acc." + angle0 + "_" + angle1, -1.)) + ")";
      string str4 = NumberToString<Double_t>(fQa.get(hist + ".sec.eff." + angle0 + "_" + angle1, -1.))
          + "(" + NumberToString<Double_t>(fQa.get(hist + ".sec.rec." + angle0 + "_" + angle1, -1.))
          + "/" + NumberToString<Double_t>(fQa.get(hist + ".sec.acc." + angle0 + "_" + angle1, -1.)) + ")";
      string str5 = NumberToString<Double_t>(fQa.get(hist + ".el.eff." + angle0 + "_" + angle1, -1.))
          + "(" + NumberToString<Double_t>(fQa.get(hist + ".el.rec." + angle0 + "_" + angle1, -1.))
          + "/" + NumberToString<Double_t>(fQa.get(hist + ".el.acc." + angle0 + "_" + angle1, -1.)) + ")";
      string str6 = NumberToString<Double_t>(fQa.get(hist + ".mu.eff." + angle0 + "_" + angle1, -1.))
          + "(" + NumberToString<Double_t>(fQa.get(hist + ".mu.rec." + angle0 + "_" + angle1, -1.))
          + "/" + NumberToString<Double_t>(fQa.get(hist + ".mu.acc." + angle0 + "_" + angle1, -1.)) + ")";
      str += fR->TableRow(list_of(str0)(str1)(str2)(str3)(str4)(str5)(str6));
   }
   return str;
}
