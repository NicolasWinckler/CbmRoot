/**
 * \file CbmLitQaReconstructionReport.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaReconstructionReport.h"
#include "../CbmLitQaHtmlReportElement.h"
#include "../CbmLitQaLatexReportElement.h"
#include "../CbmLitQaTxtReportElement.h"

#include "utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/assign/list_of.hpp>

using namespace boost::assign;

CbmLitQaReconstructionReport::CbmLitQaReconstructionReport(
      const string& type)
{
   if (type == "latex") fR = new CbmLitQaLatexReportElement();
   if (type == "html") fR = new CbmLitQaHtmlReportElement();
   else if (type == "txt") fR = new CbmLitQaTxtReportElement();
}

CbmLitQaReconstructionReport::~CbmLitQaReconstructionReport()
{
   if (fR != NULL) delete fR;
}

void CbmLitQaReconstructionReport::Create(
   ostream& out)
{
   out.precision(3);
   out << fR->PrintHead();

   out << "Number of events: " << PrintValue("hEventNo") << endl;

   //Number of objects table
   vector<string> cols1 = list_of("MVD")("STS")("RICH")("TRD")("MUCH pix")("MUCH st")("TOF");
   out << fR->PrintTableBegin("Number of objects statistics", cols1);
   out << PrintNofStatistics("Points", "hNofMvdPoints", "hNofStsPoints", "hNofRichPoints",
         "hNofTrdPoints", "hNofMuchPoints", "hNofMuchPoints", "hNofTofPoints");
   out << PrintNofStatistics("Digis", "hNofMvdDigis", "hNofStsDigis", "",
         "hNofTrdDigis", "hNofMuchDigis", "", "");
   out << PrintNofStatistics("Clusters", "hNofMvdClusters", "hNofStsClusters", "",
         "hNofTrdClusters", "hNofMuchClusters", "", "");
   out << PrintNofStatistics("Hits","hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");
   out << PrintNofStatistics("Tracks","", "hNofStsTracks", "hNofRichRings",
         "hNofTrdTracks", "hNofMuchTracks", "hNofMuchTracks", "");
   out << fR->PrintTableEnd();

   out << "Number of global tracks per event: "<<PrintValue("hNofGlobalTracks")<< endl;
   out << "Number of track projections in RICH: " <<PrintValue("hNofRichProjections") << endl;

   //Print hits histos statistics (nof all, true, fake hits in track/ring)
   vector<string> cols2 = list_of("all")("true")("fake")("true/all")("fake/all");
   out << fR->PrintTableBegin("Hits statistics", cols2);
   out << PrintHits("MVD", "hMvdTrackHits");
   out << PrintHits("STS", "hStsTrackHits");
   out << PrintHits("TRD", "hTrdTrackHits");
   out << PrintHits("MUCH", "hMuchTrackHits");
   out << PrintHits("RICH", "hRichRingHits");
   out << fR->PrintTableEnd();

   //Print reconstruction efficiency without RICH
   vector<string> cols3 = list_of("all")("reference")("primary")("secondary")("electron")("muon");
   out << fR->PrintTableBegin("Reconstruction efficiency without RICH", cols3);
   out << PrintEfficiency("STS", "hSts3D");
   out << PrintEfficiency("TRD(MUCH)", "hRec3D");
   out << PrintEfficiency("TOF matching", "hTof3D");
   out << fR->PrintEmptyRow(cols3.size()+1, "Normalization STS+TRD(MUCH)");
   out << PrintEfficiency("STS", "hSts3DNormHalfGlobal");
   string s = "STS+TRD(MUCH)";
   out << PrintEfficiency(s, "hHalfGlobal3D");
   out << fR->PrintEmptyRow(cols3.size()+1, "Normalization STS+TRD(MUCH)+TOF");
   out << PrintEfficiency("STS", "hSts3DNormGlobal");
   out << PrintEfficiency(s, "hHalfGlobal3DNormGlobal");
   string s2 = s + "+TOF";
   out << PrintEfficiency(s2, "hGlobal3D");
   out << fR->PrintTableEnd();

   //Print RICH reconstruction efficiency
   vector<string> cols4 = list_of("all")("all reference")("electron")("electron ref")("pion")("pion ref");
   out << fR->PrintTableBegin("Reconstruction efficiency with RICH", cols4);
   out << PrintEfficiencyRich("RICH", "hRich3D");
   out << fR->PrintEmptyRow(cols4.size()+1, "Normalization STS+RICH");
   out << PrintEfficiencyRich("STS", "hSts3DNormStsRich");
   out << PrintEfficiencyRich("STS+RICH no match", "hStsRichNoMatching3D");
   out << PrintEfficiencyRich("STS+RICH", "hStsRich3D");
   out << fR->PrintEmptyRow(cols4.size()+1, "Normalization STS+RICH+TRD");
   out << PrintEfficiencyRich("STS", "hSts3DNormStsRichTrd");
   out << PrintEfficiencyRich("STS+RICH", "hStsRich3DNormStsRichTrd");
   out << PrintEfficiencyRich("STS+RICH+TRD", "hStsRichTrd3D");
   out << fR->PrintEmptyRow(cols4.size()+1, "Normalization STS+RICH+TRD+TOF");
   out << PrintEfficiencyRich("STS", "hSts3DNormStsRichTrdTof");
   out << PrintEfficiencyRich("STS+RICH", "hStsRich3DNormStsRichTrdTof");
   out << PrintEfficiencyRich("STS+RICH+TRD", "hStsRichTrd3DNormStsRichTrdTof");
   out << PrintEfficiencyRich("STS+RICH+TRD+TOF", "hStsRichTrdTof3D");
   out << fR->PrintTableEnd();

   // print electron identification statistics
   vector<string> cols5 = list_of("efficiency")("pion supp");
   out << fR->PrintTableBegin("Electron identification efficiency and pion suppression", cols5);
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+TRD");
   out << PrintEfficiencyElId("STS+TRD", "hStsTrd3DElId");
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+TRD+TOF");
   out << PrintEfficiencyElId("STS+TRD", "hStsTrd3DElIdNormStsTrdTof");
   out << PrintEfficiencyElId("STS+TRD+TOF", "hStsTrdTof3DElId");
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+RICH");
   out << PrintEfficiencyElId("STS+RICH", "hStsRich3DElId");
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+RICH+TRD");
   out << PrintEfficiencyElId("STS+RICH", "hStsRich3DElIdNormStsRichTrd");
   out << PrintEfficiencyElId("STS+RICH+TRD", "hStsRichTrd3DElId");
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+RICH+TRD+TOF");
   out << PrintEfficiencyElId("STS+RICH", "hStsRich3DElIdNormStsRichTrdTof");
   out << PrintEfficiencyElId("STS+RICH+TRD", "hStsRichTrd3DElIdNormStsRichTrdTof");
   out << PrintEfficiencyElId("STS+RICH+TRD+TOF", "hStsRichTrdTof3DElId");
   out << fR->PrintTableEnd();

   // detector acceptance efficiency
   vector<string> cols6 = list_of("ACC/MC")("REC/MC");
   out << fR->PrintTableBegin("Detector acceptance for primary electrons", cols6);
   out << PrintDetAccEl("STS", "hStsDetAcc3DEl");
   out << PrintDetAccEl("STS-RICH","hStsRichDetAcc3DEl");
   out << PrintDetAccEl("STS-TRD", "hStsTrdDetAcc3DEl");
   out << PrintDetAccEl("STS-TOF", "hStsTofDetAcc3DEl");
   out << PrintDetAccEl("STS-RICH-TRD", "hStsRichTrdDetAcc3DEl");
   out << PrintDetAccEl("STS-RICH-TRD-TOF", "hStsRichTrdTofDetAcc3DEl");
   out << PrintDetAccEl("STS-TRD-TOF", "hStsTrdTofDetAcc3DEl");
   out << fR->PrintTableEnd();

   // ghost statistics
   vector<string> colsGhost = list_of("Number of ghosts");
   out << fR->PrintTableBegin("Number of ghosts per event", colsGhost);
   out << fR->PrintRow("STS", PrintValue("fhStsGhostNh"));
   out << fR->PrintRow("TRD(MUCH)", PrintValue("fhRecGhostNh"));
   out << fR->PrintRow("RICH", PrintValue("fhRichGhostNh"));
   out << fR->PrintEmptyRow(2, "after STS-RICH matching");
   out << fR->PrintRow("STS", PrintValue("fhStsGhostRichMatchingNh"));
   out << fR->PrintRow("RICH", PrintValue("fhRichGhostStsMatchingNh"));
   out << fR->PrintEmptyRow(2, "after STS-RICH matching and el id");
   out << fR->PrintRow("RICH", PrintValue("fhRichGhostElIdNh"));
   out << fR->PrintTableEnd();

   // STS quality numbers
   vector<string> colsStsQa = list_of("Mean")("RMS");
   out << fR->PrintTableBegin("STS quality numbers", colsStsQa);
   out << fR->PrintRow("Chi2 to primary vertex", PrintValue("fhStsChiprim.mean"),
         PrintValue("fhStsChiprim.rms"));
   out << fR->PrintRow("Momentum resolution", PrintValue("fhStsMomresVsMom.mean"),
         PrintValue("fhStsMomresVsMom.rms"));
   out << fR->PrintRow("Tr. len. 100*(MC-Rec)/MC", PrintValue("fhTrackLength.mean"),
         PrintValue("fhTrackLength.rms"));
   out << fR->PrintTableEnd();

   // Tracking efficiency vs. polar angle
   vector<string> cols7 = list_of("all")("reference")("primary")("secondary")("electron")("muon");
   out << fR->PrintTableBegin("Tracking efficiency in dependence on polar angle", cols7);
   out << fR->PrintEmptyRow(cols7.size()+1, "STS");
   out << setfill(' ') << left << PrintPolarAngle("hStsAngle");
   out << fR->PrintEmptyRow(cols7.size()+1, "TRD(MUCH)");
   out << setfill(' ') << left << PrintPolarAngle("hRecAngle");
   out << fR->PrintEmptyRow(cols7.size()+1, "TOF");
   out << setfill(' ') << left << PrintPolarAngle("hTofAngle");
   out << fR->PrintTableEnd();

   out << fR->PrintImage("Global efficiency all", "rec_qa_global_efficiency_all");
   out << fR->PrintImage("Global efficiency signal", "rec_qa_global_efficiency_signal");
   out << fR->PrintImage("STS efficiency", "rec_qa_sts_efficiency");
   out << fR->PrintImage("TRD(MUCH) efficiency", "rec_qa_rec_efficiency");
   out << fR->PrintImage("TOF matching efficiency", "rec_qa_tof_efficiency");
   if (fIsRich){
      out << fR->PrintImage("RICH efficiency electrons", "rec_qa_rich_efficiency_electrons");
      out << fR->PrintImage("STS+RICH efficiency electrons", "rec_qa_sts_rich_efficiency_electrons");
      out << fR->PrintImage("STS+RICH+TRD efficiency electrons", "rec_qa_sts_rich_trd_efficiency_electrons");
      out << fR->PrintImage("STS+RICH+TRD+TOF efficiency electrons", "rec_qa_sts_rich_trd_tof_efficiency_electrons");
      //out << fR->PrintImage("STS+RICH+TRD+TOF detector acceptance electrons", "rec_qa_sts_rich_trd_tof_detector_acceptance");
      out << fR->PrintImage("STS+RICH+TRD+TOF electron identification efficiency electrons", "rec_qa_sts_rich_trd_tof_electron_identification");
      out << fR->PrintImage("STS+RICH+TRD+TOF pion suppression", "rec_qa_sts_rich_trd_tof_pion_suppression");
   }
   out <<  fR->PrintCloseDocument();
}

string CbmLitQaReconstructionReport::PrintNofStatistics(
        const string& name,
        const string& mvd,
        const string& sts,
        const string& rich,
        const string& trd,
        const string& muchP,
        const string& muchS,
        const string& tof)
{
   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6, ss7;

   if (mvd != "") { ss1 << (Int_t)fQa->get(mvd, -1.); }
      else { ss1 << "-"; }
   if (sts != "") { ss2 << (Int_t)fQa->get(sts, -1.); }
      else { ss2 << "-"; }
   if (rich != "") { ss3 << (Int_t)fQa->get(rich, -1.); }
      else { ss3 << "-"; }
   if (trd != "") { ss4 << (Int_t)fQa->get(trd, -1.); }
      else { ss4 << "-"; }
   if (muchP != "") { ss5 << (Int_t)fQa->get(muchP, -1.); }
      else { ss5 << "-"; }
   if (muchS != "") { ss6 << (Int_t)fQa->get(muchS, -1.); }
      else { ss6 << "-"; }
   if (tof!= "") { ss7 << (Int_t)fQa->get(tof, -1.); }
      else { ss7 << "-"; }

   return fR->PrintRow(name, ss1.str(), ss2.str(), ss3.str(), ss4.str(), ss5.str(),
         ss6.str(), ss7.str());
}

string CbmLitQaReconstructionReport::PrintHits(
        const string& name,
        const string& hist)
{
   Double_t all = fQa->get(hist+".all", -1.);
   Double_t trueh = fQa->get(hist+".true", -1.);
   Double_t fakeh = fQa->get(hist+".fake", -1.);
   Double_t toa = fQa->get(hist+".trueOverAll", -1.);
   Double_t foa = fQa->get(hist+".fakeOverAll", -1.);

   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);

   if (all == -1.) ss1 << "-"; else ss1 << all;
   if (trueh == -1.) ss2 << "-"; else ss2 << trueh;
   if (fakeh == -1.) ss3 << "-"; else ss3 << fakeh;
   if (toa == -1.) ss4 << "-"; else ss4 << 100. * toa;
   if (foa == -1.) ss5 << "-"; else ss5 << 100. * foa;

   return fR->PrintRow(name, ss1.str(), ss2.str(), ss3.str(), ss4.str(), ss5.str());
}

string CbmLitQaReconstructionReport::PrintEfficiency(
     const string& name,
     const string& hist)
{
   Double_t allRec = fQa->get(hist+".all.rec", -1.);
   Double_t allAcc = fQa->get(hist+".all.acc", -1.);
   Double_t allEff = fQa->get(hist+".all.eff", -1.);
   Double_t refRec = fQa->get(hist+".ref.rec", -1.);
   Double_t refAcc = fQa->get(hist+".ref.acc", -1.);
   Double_t refEff = fQa->get(hist+".ref.eff", -1.);
   Double_t primRec = fQa->get(hist+".prim.rec", -1.);
   Double_t primAcc = fQa->get(hist+".prim.acc", -1.);
   Double_t primEff = fQa->get(hist+".prim.eff", -1.);
   Double_t secRec = fQa->get(hist+".sec.rec", -1.);
   Double_t secAcc = fQa->get(hist+".sec.acc", -1.);
   Double_t secEff = fQa->get(hist+".sec.eff", -1.);
   Double_t muRec = fQa->get(hist+".mu.rec", -1.);
   Double_t muAcc = fQa->get(hist+".mu.acc", -1.);
   Double_t muEff = fQa->get(hist+".mu.eff", -1.);
   Double_t elRec = fQa->get(hist+".el.rec", -1.);
   Double_t elAcc = fQa->get(hist+".el.acc", -1.);
   Double_t elEff = fQa->get(hist+".el.eff", -1.);

   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
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

   return fR->PrintRow(name, ss1.str(), ss2.str(), ss3.str(), ss4.str(), ss5.str(), ss6.str());
}

string CbmLitQaReconstructionReport::PrintEfficiencyRich(
      const string& name,
      const string& hist)
{
   Double_t allRec = fQa->get(hist+".All.rec", -1.);
   Double_t allAcc = fQa->get(hist+".All.acc", -1.);
   Double_t allEff = fQa->get(hist+".All.eff", -1.);
   Double_t allRefRec = fQa->get(hist+".AllRef.rec", -1.);
   Double_t allRefAcc = fQa->get(hist+".AllRef.acc", -1.);
   Double_t allRefEff = fQa->get(hist+".AllRef.eff", -1.);
   Double_t elRec = fQa->get(hist+".El.rec", -1.);
   Double_t elAcc = fQa->get(hist+".El.acc", -1.);
   Double_t elEff = fQa->get(hist+".El.eff", -1.);
   Double_t elRefRec = fQa->get(hist+".ElRef.rec", -1.);
   Double_t elRefAcc = fQa->get(hist+".ElRef.acc", -1.);
   Double_t elRefEff = fQa->get(hist+".ElRef.eff", -1.);
   Double_t piRec = fQa->get(hist+".Pi.rec", -1.);
   Double_t piAcc = fQa->get(hist+".Pi.acc", -1.);
   Double_t piEff = fQa->get(hist+".Pi.eff", -1.);
   Double_t piRefRec = fQa->get(hist+".PiRef.rec", -1.);
   Double_t piRefAcc = fQa->get(hist+".PiRef.acc", -1.);
   Double_t piRefEff = fQa->get(hist+".PiRef.eff", -1.);

   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   ss6.precision(3);

   ss1 << allEff << "<small>(" << allRec << "/" << allAcc << ")</small>";
   ss2 << allRefEff << "<small>(" << allRefRec << "/" << allRefAcc << ")</small>";
   ss3 << elEff << "<small>(" << elRec << "/" << elAcc << ")</small>";
   ss4 << elRefEff << "<small>(" << elRefRec << "/" << elRefAcc << ")</small>";
   ss5 << piEff << "<small>(" << piRec << "/" << piAcc << ")</small>";
   ss6 << piRefEff << "<small>(" << piRefRec << "/" << piRefAcc << ")</small>";

   return fR->PrintRow(name, ss1.str(), ss2.str(), ss3.str(), ss4.str(), ss5.str(), ss6.str());
}

string CbmLitQaReconstructionReport::PrintEfficiencyElId(
      const string& name,
      const string& hist)
{
   Double_t elRec = fQa->get(hist+".el.rec", -1.);
   Double_t elAcc = fQa->get(hist+".el.acc", -1.);
   Double_t elEff = fQa->get(hist+".el.eff", -1.);
   Double_t piRec = fQa->get(hist+".pi.rec", -1.);
   Double_t piAcc = fQa->get(hist+".pi.acc", -1.);
   Double_t piSupp = fQa->get(hist+".pi.supp", -1.);

   stringstream ss, ss1, ss2;
   ss1.precision(3);
   ss2.precision(3);

   ss1 << elEff << "("<< elRec << "/" << elAcc << ")";
   ss2 << piSupp << "("<< piAcc << "/" << piRec << ")";

   return fR->PrintRow(name, ss1.str(), ss2.str());
}

string CbmLitQaReconstructionReport::PrintDetAccEl(
      const string& name,
      const string& hist)
{
   Double_t acc = fQa->get(hist+".detAccAcc.acc", -1.);
   Double_t mc = fQa->get(hist+".detAccAcc.mc", -1.);
   Double_t effAcc = fQa->get(hist+".detAccAcc.eff", -1.);
   Double_t rec = fQa->get(hist+".detAccRec.rec", -1.);
  // Double_t mc = pt->get(hist+".detAccRec.mc", -1.);
   Double_t effRec = fQa->get(hist+".detAccRec.eff", -1.);

   stringstream ss, ss1, ss2;
   ss.precision(3);
   ss1.precision(3);
   ss2.precision(3);

   ss1 << effAcc << "(" << acc << "/" << mc << ")";
   ss2 << effRec << "(" << rec << "/" << mc << ")";

   return fR->PrintRow(name, ss1.str(), ss2.str());
}

string CbmLitQaReconstructionReport::PrintPolarAngle(
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
      ss7 << i*step;
      string angle0= ss7.str();
      stringstream ss8;
      ss8 << i*step + step;
      string angle1= ss8.str();
      ss0 << "(" << angle0 << "-" << angle1 << ")";

      ss1 << fQa->get(hist+".all.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".all.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".all.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss2 << fQa->get(hist+".ref.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".ref.rec."+angle0 +"_" + angle1, -1.) << "/"
          << fQa->get(hist+".ref.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss3 << fQa->get(hist+".prim.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".prim.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".prim.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss4 << fQa->get(hist+".sec.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".sec.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".sec.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss5 << fQa->get(hist+".el.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".el.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".el.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss6 << fQa->get(hist+".mu.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".mu.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".mu.acc."+angle0 +"_" + angle1, -1.) << ")";

      ss << fR->PrintRow(ss0.str(), ss1.str(), ss2.str(), ss3.str(), ss4.str(),
            ss5.str(), ss6.str()) << endl;;

   }
   return ss.str();
}
