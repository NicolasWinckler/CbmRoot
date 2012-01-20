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
using std::endl;
using std::setfill;
using std::left;
using boost::assign::list_of;
using lit::ToString;

CbmLitTrackingQaReport::CbmLitTrackingQaReport(
      LitReportType reportType) : CbmLitSimulationReport(reportType)
{
}

CbmLitTrackingQaReport::~CbmLitTrackingQaReport()
{
}

void CbmLitTrackingQaReport::Create(
   ostream& out,
   boost::property_tree::ptree* qa,
   boost::property_tree::ptree* ideal,
   boost::property_tree::ptree* check)
{
   fQa = qa;
   fIdeal = ideal;
   fCheck = check;

   out.precision(3);
   out << fR->DocumentBegin();

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

   out << fR->Image("Global efficiency all", "rec_qa_global_efficiency_all");
   out << fR->Image("Global efficiency signal", "rec_qa_global_efficiency_signal");
   out << fR->Image("STS efficiency", "rec_qa_sts_efficiency");
   out << fR->Image("TRD(MUCH) efficiency", "rec_qa_rec_efficiency");
   out << fR->Image("TOF matching efficiency", "rec_qa_tof_efficiency");
   // TODO: Check for RICH detector
   out << fR->Image("RICH efficiency electrons", "rec_qa_rich_efficiency_electrons");
   out << fR->Image("STS+RICH efficiency electrons", "rec_qa_sts_rich_efficiency_electrons");
   out << fR->Image("STS+RICH+TRD efficiency electrons", "rec_qa_sts_rich_trd_efficiency_electrons");
   out << fR->Image("STS+RICH+TRD+TOF efficiency electrons", "rec_qa_sts_rich_trd_tof_efficiency_electrons");
   //out << fR->PrintImage("STS+RICH+TRD+TOF detector acceptance electrons", "rec_qa_sts_rich_trd_tof_detector_acceptance");
   out << fR->Image("STS+RICH+TRD+TOF electron identification efficiency electrons", "rec_qa_sts_rich_trd_tof_electron_identification");
   out << fR->Image("STS+RICH+TRD+TOF pion suppression", "rec_qa_sts_rich_trd_tof_pion_suppression");
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
   string st1 = (global != "") ? ToString<Int_t>(fQa->get(global, -1.)) : "-";
   string st2 = (sts != "") ? ToString<Int_t>(fQa->get(sts, -1.)) : "-";
   string st3 = (richRing != "") ? ToString<Int_t>(fQa->get(richRing, -1.)) : "-";
   string st4 = (richProj != "") ? ToString<Int_t>(fQa->get(richProj, -1.)) : "-";
   string st5 = (trd != "") ? ToString<Int_t>(fQa->get(trd, -1.)) : "-";
   string st6 = (much != "") ? ToString<Int_t>(fQa->get(much, -1.)) : "-";
   return fR->TableRow(list_of(st1)(st2)(st3)(st4)(st5)(st6));
}

string CbmLitTrackingQaReport::PrintHits(
        const string& name,
        const string& hist)
{
   float all = fQa->get(hist + ".all", -1.);
   float trueh = fQa->get(hist + ".true", -1.);
   float fakeh = fQa->get(hist + ".fake", -1.);
   float toa = fQa->get(hist + ".trueOverAll", -1.);
   float foa = fQa->get(hist + ".fakeOverAll", -1.);
   string st1 = (all == -1.) ? "-" : ToString<float>(all);
   string st2 = (trueh == -1.) ? "-" : ToString<float>(trueh);
   string st3 = (fakeh == -1.) ? "-" : ToString<float>(fakeh);
   string st4 = (toa == -1.) ? "-" : ToString<float>(100. * toa);
   string st5 = (foa == -1.) ? "-" : ToString<float>(100. * foa);
   return fR->TableRow(list_of(name)(st1)(st2)(st3)(st4)(st5));
}

string CbmLitTrackingQaReport::PrintEfficiency(
     const string& name,
     const string& hist)
{
   Double_t allRec = fQa->get(hist + ".all.rec", -1.);
   Double_t allAcc = fQa->get(hist + ".all.acc", -1.);
   Double_t allEff = fQa->get(hist + ".all.eff", -1.);
   Double_t refRec = fQa->get(hist + ".ref.rec", -1.);
   Double_t refAcc = fQa->get(hist + ".ref.acc", -1.);
   Double_t refEff = fQa->get(hist + ".ref.eff", -1.);
   Double_t primRec = fQa->get(hist + ".prim.rec", -1.);
   Double_t primAcc = fQa->get(hist + ".prim.acc", -1.);
   Double_t primEff = fQa->get(hist + ".prim.eff", -1.);
   Double_t secRec = fQa->get(hist + ".sec.rec", -1.);
   Double_t secAcc = fQa->get(hist + ".sec.acc", -1.);
   Double_t secEff = fQa->get(hist + ".sec.eff", -1.);
   Double_t muRec = fQa->get(hist + ".mu.rec", -1.);
   Double_t muAcc = fQa->get(hist + ".mu.acc", -1.);
   Double_t muEff = fQa->get(hist + ".mu.eff", -1.);
   Double_t elRec = fQa->get(hist + ".el.rec", -1.);
   Double_t elAcc = fQa->get(hist + ".el.acc", -1.);
   Double_t elEff = fQa->get(hist + ".el.eff", -1.);

   stringstream ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   ss6.precision(3);

   ss1 << allEff << "("<< allRec << "/" << allAcc << ")";
   ss2 << refEff << "("<< refRec << "/" << refAcc << ")";
   ss3 << primEff << "(" << primRec << "/" << primAcc << ")";
   ss4 << secEff << "(" << secRec << "/" << secAcc << ")";
   ss5 << elEff << "(" << elRec << "/" << elAcc << ")";
   ss6 << muEff << "(" << muRec << "/" << muAcc << ")";

   return fR->TableRow(list_of(name)(ss1.str())(ss2.str())(ss3.str())(ss4.str())(ss5.str())(ss6.str()));
}

string CbmLitTrackingQaReport::PrintEfficiencyRich(
      const string& name,
      const string& hist)
{
   Double_t allRec = fQa->get(hist + ".All.rec", -1.);
   Double_t allAcc = fQa->get(hist + ".All.acc", -1.);
   Double_t allEff = fQa->get(hist + ".All.eff", -1.);
   Double_t allRefRec = fQa->get(hist + ".AllRef.rec", -1.);
   Double_t allRefAcc = fQa->get(hist + ".AllRef.acc", -1.);
   Double_t allRefEff = fQa->get(hist + ".AllRef.eff", -1.);
   Double_t elRec = fQa->get(hist + ".El.rec", -1.);
   Double_t elAcc = fQa->get(hist + ".El.acc", -1.);
   Double_t elEff = fQa->get(hist + ".El.eff", -1.);
   Double_t elRefRec = fQa->get(hist + ".ElRef.rec", -1.);
   Double_t elRefAcc = fQa->get(hist + ".ElRef.acc", -1.);
   Double_t elRefEff = fQa->get(hist + ".ElRef.eff", -1.);
   Double_t piRec = fQa->get(hist + ".Pi.rec", -1.);
   Double_t piAcc = fQa->get(hist + ".Pi.acc", -1.);
   Double_t piEff = fQa->get(hist + ".Pi.eff", -1.);
   Double_t piRefRec = fQa->get(hist + ".PiRef.rec", -1.);
   Double_t piRefAcc = fQa->get(hist + ".PiRef.acc", -1.);
   Double_t piRefEff = fQa->get(hist + ".PiRef.eff", -1.);

   stringstream ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   ss6.precision(3);

   ss1 << allEff << "(" << allRec << "/" << allAcc << ")";
   ss2 << allRefEff << "(" << allRefRec << "/" << allRefAcc << ")";
   ss3 << elEff << "(" << elRec << "/" << elAcc << ")";
   ss4 << elRefEff << "(" << elRefRec << "/" << elRefAcc << ")";
   ss5 << piEff << "(" << piRec << "/" << piAcc << ")";
   ss6 << piRefEff << "(" << piRefRec << "/" << piRefAcc << ")";

   return fR->TableRow(list_of(name)(ss1.str())(ss2.str())(ss3.str())(ss4.str())(ss5.str())(ss6.str()));
}

string CbmLitTrackingQaReport::PrintEfficiencyElId(
      const string& name,
      const string& hist)
{
   Double_t elRec = fQa->get(hist + ".el.rec", -1.);
   Double_t elAcc = fQa->get(hist + ".el.acc", -1.);
   Double_t elEff = fQa->get(hist + ".el.eff", -1.);
   Double_t piRec = fQa->get(hist + ".pi.rec", -1.);
   Double_t piAcc = fQa->get(hist + ".pi.acc", -1.);
   Double_t piSupp = fQa->get(hist + ".pi.supp", -1.);

   stringstream ss1, ss2;
   ss1.precision(3);
   ss2.precision(3);

   ss1 << elEff << "("<< elRec << "/" << elAcc << ")";
   ss2 << piSupp << "("<< piAcc << "/" << piRec << ")";

   return fR->TableRow(list_of(name)(ss1.str())(ss2.str()));
}

string CbmLitTrackingQaReport::PrintDetAccEl(
      const string& name,
      const string& hist)
{
   Double_t acc = fQa->get(hist+".detAccAcc.acc", -1.);
   Double_t mc = fQa->get(hist+".detAccAcc.mc", -1.);
   Double_t effAcc = fQa->get(hist+".detAccAcc.eff", -1.);
   Double_t rec = fQa->get(hist+".detAccRec.rec", -1.);
  // Double_t mc = pt->get(hist+".detAccRec.mc", -1.);
   Double_t effRec = fQa->get(hist+".detAccRec.eff", -1.);

   stringstream ss1, ss2;
   ss1.precision(3);
   ss2.precision(3);

   ss1 << effAcc << "(" << acc << "/" << mc << ")";
   ss2 << effRec << "(" << rec << "/" << mc << ")";

   return fR->TableRow(list_of(name)(ss1.str())(ss2.str()));
}

string CbmLitTrackingQaReport::PrintPolarAngle(
      const string& hist)
{
   Double_t maxAngle = fQa->get("MaxAngle", -1.);
   Double_t minAngle = fQa->get("MinAngle", -1.);
   Double_t nofBinsAngle = fQa->get("NofBinsAngle", -1.);
   Double_t step = (maxAngle - minAngle) / nofBinsAngle;

   if (maxAngle == -1. || minAngle == -1. || nofBinsAngle == -1.){
      return "ERROR PolarAngleEfficiencyToString";
   }

   stringstream ss;
   for (Int_t i = 0; i < nofBinsAngle; i++) {
      stringstream ss0, ss1, ss2, ss3, ss4, ss5, ss6;
      ss0.precision(3);
      ss1.precision(3);
      ss2.precision(3);
      ss3.precision(3);
      ss4.precision(3);
      ss5.precision(3);
      ss6.precision(3);

      stringstream ss7;
      ss7 << i * step;
      string angle0 = ss7.str();
      stringstream ss8;
      ss8 << i * step + step;
      string angle1 = ss8.str();
      ss0 << "(" << angle0 << "-" << angle1 << ")";

      ss1 << fQa->get(hist + ".all.eff." + angle0 + "_" + angle1, -1.)
          << "(" << fQa->get(hist + ".all.rec." + angle0 + "_" + angle1, -1.)
          << "/" << fQa->get(hist + ".all.acc." + angle0 + "_" + angle1, -1.) << ")";
      ss2 << fQa->get(hist + ".ref.eff." + angle0 + "_" + angle1, -1.)
          << "(" << fQa->get(hist + ".ref.rec." + angle0 + "_" + angle1, -1.) << "/"
          << fQa->get(hist + ".ref.acc." + angle0 + "_" + angle1, -1.) << ")";
      ss3 << fQa->get(hist + ".prim.eff." + angle0 + "_" + angle1, -1.)
          << "(" << fQa->get(hist + ".prim.rec." + angle0 + "_" + angle1, -1.)
          << "/" << fQa->get(hist + ".prim.acc." + angle0 + "_" + angle1, -1.) << ")";
      ss4 << fQa->get(hist + ".sec.eff." + angle0 + "_" + angle1, -1.)
          << "(" << fQa->get(hist + ".sec.rec."+angle0 + "_" + angle1, -1.)
          << "/" << fQa->get(hist + ".sec.acc."+angle0 + "_" + angle1, -1.) << ")";
      ss5 << fQa->get(hist + ".el.eff." + angle0 + "_" + angle1, -1.)
          << "(" << fQa->get(hist + ".el.rec." + angle0 + "_" + angle1, -1.)
          << "/" << fQa->get(hist + ".el.acc." + angle0 + "_" + angle1, -1.) << ")";
      ss6 << fQa->get(hist + ".mu.eff." + angle0 + "_" + angle1, -1.)
          << "(" << fQa->get(hist + ".mu.rec."+ angle0 + "_" + angle1, -1.)
          << "/" << fQa->get(hist + ".mu.acc."+ angle0 + "_" + angle1, -1.) << ")";
      ss << fR->TableRow(list_of(ss0.str())(ss1.str())(ss2.str())(ss3.str())(ss4.str())(ss5.str())(ss6.str()));
   }
   return ss.str();
}
