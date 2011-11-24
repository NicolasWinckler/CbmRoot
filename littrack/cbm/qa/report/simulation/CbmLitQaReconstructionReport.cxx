/**
 * \file CbmLitQaReconstructionReport.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaReconstructionReport.h"
#include "CbmLitQaHtmlReportElement.h"
#include "CbmLitQaLatexReportElement.h"
#include "CbmLitQaTxtReportElement.h"

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

   out << "Number of events: " << fR->PrintValue("hEventNo") << endl;

   //Number of objects table
   vector<string> cols1 = list_of("MVD")("STS")("RICH")("TRD")("MUCH pix")("MUCH st")("TOF");
   out << fR->PrintTableBegin("Number of objects statistics", cols1);
   out << fR->PrintNofStatistics("Points", "hNofMvdPoints", "hNofStsPoints", "hNofRichPoints",
         "hNofTrdPoints", "hNofMuchPoints", "hNofMuchPoints", "hNofTofPoints");
   out << fR->PrintNofStatistics("Digis", "hNofMvdDigis", "hNofStsDigis", "",
         "hNofTrdDigis", "hNofMuchDigis", "", "");
   out << fR->PrintNofStatistics("Clusters", "hNofMvdClusters", "hNofStsClusters", "",
         "hNofTrdClusters", "hNofMuchClusters", "", "");
   out << fR->PrintNofStatistics("Hits","hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");
   out << fR->PrintNofStatistics("Tracks","", "hNofStsTracks", "hNofRichRings",
         "hNofTrdTracks", "hNofMuchTracks", "hNofMuchTracks", "");
   out << fR->PrintTableEnd();

   out << "Number of global tracks per event: "<<fR->PrintValue("hNofGlobalTracks")<< endl;
   out << "Number of track projections in RICH: " <<fR->PrintValue("hNofRichProjections") << endl;

   //Print hits histos statistics (nof all, true, fake hits in track/ring)
   vector<string> cols2 = list_of("all")("true")("fake")("true/all")("fake/all");
   out << fR->PrintTableBegin("Hits statistics", cols2);
   out << fR->PrintHits("MVD", "hMvdTrackHits");
   out << fR->PrintHits("STS", "hStsTrackHits");
   out << fR->PrintHits("TRD", "hTrdTrackHits");
   out << fR->PrintHits("MUCH", "hMuchTrackHits");
   out << fR->PrintHits("RICH", "hRichRingHits");
   out << fR->PrintTableEnd();

   //Print reconstruction efficiency without RICH
   vector<string> cols3 = list_of("all")("reference")("primary")("secondary")("electron")("muon");
   out << fR->PrintTableBegin("Reconstruction efficiency without RICH", cols3);
   out << fR->PrintEfficiency("STS", "hSts3D");
   out << fR->PrintEfficiency("TRD(MUCH)", "hRec3D");
   out << fR->PrintEfficiency("TOF matching", "hTof3D");
   out << fR->PrintEmptyRow(cols3.size()+1, "Normalization STS+TRD(MUCH)");
   out << fR->PrintEfficiency("STS", "hSts3DNormHalfGlobal");
   string s = "STS+TRD(MUCH)";
   out << fR->PrintEfficiency(s, "hHalfGlobal3D");
   out << fR->PrintEmptyRow(cols3.size()+1, "Normalization STS+TRD(MUCH)+TOF");
   out << fR->PrintEfficiency("STS", "hSts3DNormGlobal");
   out << fR->PrintEfficiency(s, "hHalfGlobal3DNormGlobal");
   string s2 = s + "+TOF";
   out << fR->PrintEfficiency(s2, "hGlobal3D");
   out << fR->PrintTableEnd();

   //Print RICH reconstruction efficiency
   vector<string> cols4 = list_of("all")("all reference")("electron")("electron ref")("pion")("pion ref");
   out << fR->PrintTableBegin("Reconstruction efficiency with RICH", cols4);
   out << fR->PrintEfficiencyRich("RICH", "hRich3D");
   out << fR->PrintEmptyRow(cols4.size()+1, "Normalization STS+RICH");
   out << fR->PrintEfficiencyRich("STS", "hSts3DNormStsRich");
   out << fR->PrintEfficiencyRich("STS+RICH no match", "hStsRichNoMatching3D");
   out << fR->PrintEfficiencyRich("STS+RICH", "hStsRich3D");
   out << fR->PrintEmptyRow(cols4.size()+1, "Normalization STS+RICH+TRD");
   out << fR->PrintEfficiencyRich("STS", "hSts3DNormStsRichTrd");
   out << fR->PrintEfficiencyRich("STS+RICH", "hStsRich3DNormStsRichTrd");
   out << fR->PrintEfficiencyRich("STS+RICH+TRD", "hStsRichTrd3D");
   out << fR->PrintEmptyRow(cols4.size()+1, "Normalization STS+RICH+TRD+TOF");
   out << fR->PrintEfficiencyRich("STS", "hSts3DNormStsRichTrdTof");
   out << fR->PrintEfficiencyRich("STS+RICH", "hStsRich3DNormStsRichTrdTof");
   out << fR->PrintEfficiencyRich("STS+RICH+TRD", "hStsRichTrd3DNormStsRichTrdTof");
   out << fR->PrintEfficiencyRich("STS+RICH+TRD+TOF", "hStsRichTrdTof3D");
   out << fR->PrintTableEnd();

   // print electron identification statistics
   vector<string> cols5 = list_of("efficiency")("pion supp");
   out << fR->PrintTableBegin("Electron identification efficiency and pion suppression", cols5);
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+TRD");
   out << fR->PrintEfficiencyElId("STS+TRD", "hStsTrd3DElId");
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+TRD+TOF");
   out << fR->PrintEfficiencyElId("STS+TRD", "hStsTrd3DElIdNormStsTrdTof");
   out << fR->PrintEfficiencyElId("STS+TRD+TOF", "hStsTrdTof3DElId");
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+RICH");
   out << fR->PrintEfficiencyElId("STS+RICH", "hStsRich3DElId");
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+RICH+TRD");
   out << fR->PrintEfficiencyElId("STS+RICH", "hStsRich3DElIdNormStsRichTrd");
   out << fR->PrintEfficiencyElId("STS+RICH+TRD", "hStsRichTrd3DElId");
   out << fR->PrintEmptyRow(cols5.size()+1, "Normalization STS+RICH+TRD+TOF");
   out << fR->PrintEfficiencyElId("STS+RICH", "hStsRich3DElIdNormStsRichTrdTof");
   out << fR->PrintEfficiencyElId("STS+RICH+TRD", "hStsRichTrd3DElIdNormStsRichTrdTof");
   out << fR->PrintEfficiencyElId("STS+RICH+TRD+TOF", "hStsRichTrdTof3DElId");
   out << fR->PrintTableEnd();

   // detector acceptance efficiency
   vector<string> cols6 = list_of("ACC/MC")("REC/MC");
   out << fR->PrintTableBegin("Detector acceptance for primary electrons", cols6);
   out << fR->PrintDetAccEl("STS", "hStsDetAcc3DEl");
   out << fR->PrintDetAccEl("STS-RICH","hStsRichDetAcc3DEl");
   out << fR->PrintDetAccEl("STS-TRD", "hStsTrdDetAcc3DEl");
   out << fR->PrintDetAccEl("STS-TOF", "hStsTofDetAcc3DEl");
   out << fR->PrintDetAccEl("STS-RICH-TRD", "hStsRichTrdDetAcc3DEl");
   out << fR->PrintDetAccEl("STS-RICH-TRD-TOF", "hStsRichTrdTofDetAcc3DEl");
   out << fR->PrintDetAccEl("STS-TRD-TOF", "hStsTrdTofDetAcc3DEl");
   out << fR->PrintTableEnd();

   // ghost statistics
   vector<string> colsGhost = list_of("Number of ghosts");
   out << fR->PrintTableBegin("Number of ghosts per event", colsGhost);
   out << fR->PrintRow("STS", fR->PrintValue("fhStsGhostNh"));
   out << fR->PrintRow("TRD(MUCH)", fR->PrintValue("fhRecGhostNh"));
   out << fR->PrintRow("RICH", fR->PrintValue("fhRichGhostNh"));
   out << fR->PrintEmptyRow(2, "after STS-RICH matching");
   out << fR->PrintRow("STS", fR->PrintValue("fhStsGhostRichMatchingNh"));
   out << fR->PrintRow("RICH", fR->PrintValue("fhRichGhostStsMatchingNh"));
   out << fR->PrintEmptyRow(2, "after STS-RICH matching and el id");
   out << fR->PrintRow("RICH", fR->PrintValue("fhRichGhostElIdNh"));
   out << fR->PrintTableEnd();

   // STS quality numbers
   vector<string> colsStsQa = list_of("Mean")("RMS");
   out << fR->PrintTableBegin("STS quality numbers", colsStsQa);
   out << fR->PrintRow("Chi2 to primary vertex", fR->PrintValue("fhStsChiprim.mean"),
         fR->PrintValue("fhStsChiprim.rms"));
   out << fR->PrintRow("Momentum resolution", fR->PrintValue("fhStsMomresVsMom.mean"),
         fR->PrintValue("fhStsMomresVsMom.rms"));
   out << fR->PrintRow("Tr. len. 100*(MC-Rec)/MC", fR->PrintValue("fhTrackLength.mean"),
         fR->PrintValue("fhTrackLength.rms"));
   out << fR->PrintTableEnd();

   // Tracking efficiency vs. polar angle
   vector<string> cols7 = list_of("all")("reference")("primary")("secondary")("electron")("muon");
   out << fR->PrintTableBegin("Tracking efficiency in dependence on polar angle", cols7);
   out << fR->PrintEmptyRow(cols7.size()+1, "STS");
   out << setfill(' ') << left << fR->PrintPolarAngle("hStsAngle");
   out << fR->PrintEmptyRow(cols7.size()+1, "TRD(MUCH)");
   out << setfill(' ') << left << fR->PrintPolarAngle("hRecAngle");
   out << fR->PrintEmptyRow(cols7.size()+1, "TOF");
   out << setfill(' ') << left << fR->PrintPolarAngle("hTofAngle");
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
