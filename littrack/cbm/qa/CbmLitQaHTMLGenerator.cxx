/**
 * \file CbmLitQaHTMLGenerator.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "qa/CbmLitQaHTMLGenerator.h"

#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "utils/CbmLitUtils.h"

#include <boost/property_tree/ptree.hpp>

void CbmLitQaHTMLGenerator::Create(
   std::ostream& out,
   const boost::property_tree::ptree* qa,
   const boost::property_tree::ptree* ideal,
   const boost::property_tree::ptree* check)
{
   fQa = qa;
   fIdeal = ideal;
   fCheck = check;

   int row = 0;

   out.precision(3);
   out << "<html><body>" << HtmlHeadString();

   out << "<h1>CbmLitReconstructionQa final statistics</h1>" << std::endl;
   out << PrintValue("Number of events: ", "hEventNo", "p");

   // Number of objects statistics
   out << "<h2>Number of objects</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>Points</th><th>Digis</th><th>Clusters</th><th>Hits</th><th>Tracks</th></tr>" << std::endl;
   if (fIsMvd) out << PrintRowNofObjects(row++, "MVD", "Mvd");
   if (fIsSts) out << PrintRowNofObjects(row++, "STS", "Sts");
   if (fIsRich) out << PrintRowNofObjects(row++, "RICH", "Rich");
   if (fIsTrd) out << PrintRowNofObjects(row++, "TRD", "Trd");
   if (fIsMuch) out << PrintRowNofObjects(row++, "MUCH", "Much");
   if (fIsTof) out << PrintRowNofObjects(row++, "TOF", "Tof");
   out << PrintRowNofObjects(row++, "Global", "Global");
   out << "</table>" << std::endl;

   // True and fake hits histograms (nof all, true, fake hits in track/ring)
   out << "<h2>True and fake hits</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>true</th><th>fake</th><th>true/all</th><th>fake/all</th></tr>" << std::endl;
   if (fIsMvd) out << PrintRowHits(row++, "MVD", "hMvdTrackHits");
   if (fIsSts) out << PrintRowHits(row++, "STS", "hStsTrackHits");
   if (fIsRich) out << PrintRowHits(row++, "RICH", "hRichRingHits");
   if (fIsTrd) out << PrintRowHits(row++, "TRD", "hTrdTrackHits");
   if (fIsMuch) out << PrintRowHits(row++, "MUCH", "hMuchTrackHits");
   out << "</table>" << std::endl;

   // Reconstruction efficiency without RICH
   out << "<h2>Reconstruction efficiency w/o RICH</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>reference</th><th>primary</th><th>secondary</th><th>electron</th><th>muon</th></tr>" << std::endl;
   out << PrintRowEfficiency(row++, "STS", "hStsMom");
   out << PrintRowEfficiency(row++, "TRD(MUCH)", "hRecMom");
   out << PrintRowEfficiency(row++, "TOF matching", "hTofMom");

   out << "<tr><td colspan=\"7\">Normalization STS+TRD(MUCH)</td></tr>";
   out << PrintRowEfficiency(row++, "STS", "hStsMomNormHalfGlobal");
   out << PrintRowEfficiency(row++, "STS+TRD(MUCH)", "hHalfGlobalMom");

   out << "<tr><td colspan=\"7\">Normalization STS+TRD(MUCH)+TOF</td></tr>";
   out << PrintRowEfficiency(row++, "STS", "hStsMomNormGlobal");
   out << PrintRowEfficiency(row++, "STS+TRD(MUCH)", "hHalfGlobalMomNormGlobal");
   out << PrintRowEfficiency(row++, "STS+TRD(MUCH)+TOF", "hGlobalMom");
   out << "</table>" << std::endl;

   // Reconstruction efficiency with RICH
   if (fIsRich) {
      out << "<h2>Reconstruction efficiency with RICH</h2>";
      out << "<table id=\"efficiency\" >";
      out << "<tr><th></th><th>all</th><th>all ref</th><th>electron</th><th>electron ref</th><th>pion</th><th>pion ref</th></tr>" << std::endl;
      out << PrintRowEfficiencyRich(row++, "RICH", "hRichMom");

      out << "<tr><td colspan=\"7\">Normalization STS+RICH</td></tr>";
      out << PrintRowEfficiencyRich(row++, "STS", "hStsMomNormStsRich");
      out << PrintRowEfficiencyRich(row++, "STS+RICH", "hStsRichMom");

      out << "<tr><td colspan=\"7\">Normalization STS+RICH+TRD</td></tr>";
      out << PrintRowEfficiencyRich(row++, "STS", "hStsMomNormStsRichTrd");
      out << PrintRowEfficiencyRich(row++, "STS+RICH", "hStsRichMomNormStsRichTrd");
      out << PrintRowEfficiencyRich(row++, "STS+RICH+TRD", "hStsRichTrdMom");

      out << "<tr><td colspan=\"7\">Normalization STS+RICH+TRD+TOF</td></tr>";
      out << PrintRowEfficiencyRich(row++, "STS", "hStsMomNormStsRichTrdTof");
      out << PrintRowEfficiencyRich(row++, "STS+RICH", "hStsRichMomNormStsRichTrdTof");
      out << PrintRowEfficiencyRich(row++, "STS+RICH+TRD", "hStsRichTrdMomNormStsRichTrdTof");
      out << PrintRowEfficiencyRich(row++, "STS+RICH+TRD+TOF", "hStsRichTrdTofMom");
      out << "</table>" << std::endl;
   }

   if (fIsElectronSetup) {
      // Print electron identification statistics
      out << "<h2>Electron identification</h2>";
      out << "<table id=\"efficiency\" >";
      out << "<tr><th></th><th>Efficiency</th><th>Pion supp.</th></tr>";
      out << "<tr><td colspan=\"3\">Normalization STS+TRD</td></tr>";
      out << PrintRowEfficiencyElId(row++, "STS+TRD", "hStsTrdMomElId");

      out << "<tr><td colspan=\"3\">Normalization STS+TRD+TOF</td></tr>";
      out << PrintRowEfficiencyElId(row++, "STS+TRD", "hStsTrdMomElIdNormStsTrdTof");
      out << PrintRowEfficiencyElId(row++, "STS+TRD+TOF", "hStsTrdTofMomElId");

      if (fIsRich) {
         out << "<tr><td colspan=\"3\">Normalization STS+RICH</td></tr>";
         out << PrintRowEfficiencyElId(row++, "STS+RICH", "hStsRichMomElId");

         out << "<tr><td colspan=\"3\">Normalization STS+RICH+TRD</td></tr>";
         out << PrintRowEfficiencyElId(row++, "STS+RICH", "hStsRichMomElIdNormStsRichTrd");
         out << PrintRowEfficiencyElId(row++, "STS+RICH+TRD", "hStsRichTrdMomElId");

         out << "<tr><td colspan=\"3\">Normalization STS+RICH+TRD+TOF</td></tr>";
         out << PrintRowEfficiencyElId(row++, "STS+RICH", "hStsRichMomElIdNormStsRichTrdTof");
         out << PrintRowEfficiencyElId(row++, "STS+RICH+TRD", "hStsRichTrdMomElIdNormStsRichTrdTof");
         out << PrintRowEfficiencyElId(row++, "STS+RICH+TRD+TOF", "hStsRichTrdTofMomElId");
      }
      out << "</table>" << std::endl;
   }

   if (fIsElectronSetup) {
      // Detector acceptance efficiency
      out << "<h2>Detector acceptance for signal electrons</h2>";
      out << "<table id=\"efficiency\" >";
      out << "<tr><th></th><th>kAcc/MC</th><th>kRec/MC</th></tr>";
      out << PrintRowDetAccEl(row++, "STS", "hStsDetAccEl");
      out << PrintRowDetAccEl(row++, "STS-RICH","hStsRichDetAccEl");
      out << PrintRowDetAccEl(row++, "STS-TRD", "hStsTrdDetAccEl");
      out << PrintRowDetAccEl(row++, "STS-TOF", "hStsTofDetAccEl");
      out << PrintRowDetAccEl(row++, "STS-RICH-TRD", "hStsRichTrdDetAccEl");
      out << PrintRowDetAccEl(row++, "STS-RICH-TRD-TOF", "hStsRichTrdTofDetAccEl");
      out << PrintRowDetAccEl(row++, "STS-TRD-TOF", "hStsTrdTofDetAccEl");
      out << "</table>" << std::endl;
   }

   // Ghost statistics
   out << "<h2>Ghosts per event</h2>";
   out << "<ul>";
   if (fIsSts) out << "<li>" << PrintValue("STS=", "fhStsGhostNh", "b");
   if (fIsTrd || fIsMuch) out << "<li>" << PrintValue("TRD(MUCH)=", "fhRecGhostNh", "b");
   if (fIsRich) out << "<li>" << PrintValue("RICH=", "fhRichGhostNh", "b");
   if (fIsRich) {
      out << "<li>after STS-RICH matching:";
      out << "<ul>";
      out << "<li>" << PrintValue("STS=", "fhStsGhostRichMatchingNh", "b");
      out << "<li>" << PrintValue("RICH=", "fhRichGhostStsMatchingNh", "b");
      out << "</ul>";
      out << "<li>after STS-RICH matching and el identification: ";
      out << "<ul>";
      out << "<li>" << PrintValue("RICH=", "fhRichGhostElIdNh", "b");
      out << "</ul>";
   }
   out << "</ul>";

   // STS quality numbers
   out << "<h2>STS quality numbers</h2>";
   out << "<p>Chi2 to primary vertex: ";
   out << PrintValue("mean = ", "fhStsChiprim.mean", "b");
   out << PrintValue(" RMS = ", "fhStsChiprim.rms", "b") << "</p>";
   out << "<p>Momentum resolution: ";
   out << PrintValue("mean = ", "fhStsMomresVsMom.mean", "b");
   out << PrintValue(" RMS = ", "fhStsMomresVsMom.rms", "b") << "</p>";

   // Tracking efficiency vs. polar angle
   out << "<h2>Tracking efficiency in dependence on polar angle</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>reference</th><th>primary</th><th>secondary</th><th>electron</th><th>muon</th></tr>" << std::endl;
   if (fIsSts) {
      out << "<tr><td colspan=\"7\">STS</td></tr>";
      out << PrintPolarAngleEfficiency("hStsAngle");
   }
   if (fIsTrd || fIsMuch) {
      out << "<tr><td colspan=\"7\">TRD(MUCH)</td></tr>";
      out << PrintPolarAngleEfficiency("hRecAngle");
   }
   if (fIsTof) {
      out << "<tr><td colspan=\"7\">TOF</td></tr>";
      out << PrintPolarAngleEfficiency("hTofAngle");
   }
   out << "</table>" << std::endl;

   // Images
   out << "<h3>rec_qa_global_efficiency_all</h3><img src=\"rec_qa_global_efficiency_all.png\" alt=\"rec_qa_global_efficiency_all\" />";
   out << "<h3>rec_qa_global_efficiency_signal</h3><img src=\"rec_qa_global_efficiency_signal.png\" alt=\"rec_qa_global_efficiency_signal\" />";
   out << "<h3>rec_qa_sts_efficiency</h3><img src=\"rec_qa_sts_efficiency.png\" alt=\"rec_qa_sts_efficiency\" />";
   out << "<h3>rec_qa_rec_efficiency</h3><img src=\"rec_qa_rec_efficiency.png\" alt=\"rec_qa_rec_efficiency\" />";
   out << "<h3>rec_qa_tof_efficiency</h3><img src=\"rec_qa_tof_efficiency.png\" alt=\"rec_qa_tof_efficiency\" />";
   if (fIsRich) {
      out << "<h3>rec_qa_rich_efficiency_electrons</h3><img src=\"rec_qa_rich_efficiency_electrons.png\" alt=\"rec_qa_rich_efficiency_electrons\" />";
      out << "<h3>rec_qa_sts_rich_efficiency_electrons</h3><img src=\"rec_qa_sts_rich_efficiency_electrons.png\" alt=\"rec_qa_sts_rich_efficiency_electrons\" />";
      out << "<h3>rec_qa_sts_rich_trd_efficiency_electrons</h3><img src=\"rec_qa_sts_rich_trd_efficiency_electrons.png\" alt=\"rec_qa_sts_rich_trd_efficiency_electrons\" />";
      out << "<h3>rec_qa_sts_rich_trd_tof_efficiency_electrons</h3><img src=\"rec_qa_sts_rich_trd_tof_efficiency_electrons.png\" alt=\"rec_qa_sts_rich_trd_tof_efficiency_electrons\" />";
      out << "<h3>rec_qa_sts_rich_trd_tof_detector_acceptance</h3><img src=\"rec_qa_sts_rich_trd_tof_detector_acceptance.png\" alt=\"rec_qa_sts_rich_trd_tof_detector_acceptance\" />";
      out << "<h3>rec_qa_sts_rich_trd_tof_electron_identification</h3><img src=\"rec_qa_sts_rich_trd_tof_electron_identification.png\" alt=\"rec_qa_sts_rich_trd_tof_electron_identification\" />";
      out << "<h3>rec_qa_sts_rich_trd_tof_pion_suppression</h3><img src=\"rec_qa_sts_rich_trd_tof_pion_suppression.png\" alt=\"rec_qa_sts_rich_trd_tof_pion_suppression\" />";
   }
   out <<  "</body></html>";
}

std::string CbmLitQaHTMLGenerator::PrintRowNofObjects(
   int row,
   const std::string& rowName,
   const std::string& detName)
{
   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << rowName << "</td>";
   ss << PrintValue("", "hNof" + detName + "Points", "td");
   if (detName == "Much") {
      ss << PrintValue("", "hNof" + detName + "Digis", "td");
      ss << PrintValue("", "hNof" + detName + "Clusters", "td");
      ss << "<td>" << PrintValue("", "hNofMuchPixelHits", "small")
            << "/" << PrintValue("", "hNofMuchStrawHits", "small")  << "</td>";
   } else {
      ss << PrintValue("", "hNof" + detName + "Digis", "td");
      ss << PrintValue("", "hNof" + detName + "Clusters", "td");
      ss << PrintValue("", "hNof" + detName + "Hits", "td");
   }
   if (detName == "Rich") {
      ss << "<td>" << PrintValue("", "hNofRichRings", "small")
            << "/" << PrintValue("", "hNofRichProjections", "small")  << "</td>";
   } else {
      ss << PrintValue("", "hNof" + detName + "Tracks", "td");
   }
   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGenerator::PrintRowHits(
   int row,
   const std::string& rowName,
   const std::string& histName)
{
   std::string all = PrintValue("", histName + ".all", "td");
   std::string trueh = PrintValue("", histName + ".true", "td");
   std::string fakeh = PrintValue("", histName + ".fake", "td");
   std::string toa = PrintValue("", histName + ".trueOverAll", "td");
   std::string foa = PrintValue("", histName + ".fakeOverAll", "td");

   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << rowName << "</td>";
   ss << all << trueh << fakeh << toa << foa;
   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGenerator::PrintRowEfficiency(
      int row,
      const std::string& rowName,
      const std::string& histName)
{
   std::string allRec = PrintValue("", histName + ".all.rec", "small");
   std::string allAcc = PrintValue("", histName + ".all.acc", "small");
   std::string allEff = PrintValue("", histName + ".all.eff", "b");
   std::string refRec = PrintValue("", histName + ".ref.rec", "small");
   std::string refAcc = PrintValue("", histName + ".ref.acc", "small");
   std::string refEff = PrintValue("", histName + ".ref.eff", "b");
   std::string primRec = PrintValue("", histName + ".prim.rec", "small");
   std::string primAcc = PrintValue("", histName + ".prim.acc", "small");
   std::string primEff = PrintValue("", histName + ".prim.eff", "b");
   std::string secRec = PrintValue("", histName + ".sec.rec", "small");
   std::string secAcc = PrintValue("", histName + ".sec.acc", "small");
   std::string secEff = PrintValue("", histName + ".sec.eff", "b");
   std::string muRec = PrintValue("", histName + ".mu.rec", "small");
   std::string muAcc = PrintValue("", histName + ".mu.acc", "small");
   std::string muEff = PrintValue("", histName + ".mu.eff", "b");
   std::string elRec = PrintValue("", histName + ".el.rec", "small");
   std::string elAcc = PrintValue("", histName + ".el.acc", "small");
   std::string elEff = PrintValue("", histName + ".el.eff", "b");

   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << rowName << "</td>";
   ss << "<td>" << allEff << "("<< allRec << "/" << allAcc << ")" << "</td>";
   ss << "<td>" << refEff << "("<< refRec << "/" << refAcc << ")" << "</td>";
   ss << "<td>" << primEff << "(" << primRec << "/" << primAcc << ")" << "</td>";
   ss << "<td>" << secEff << "(" << secRec << "/" << secAcc << ")" << "</td>";
   ss << "<td>" << elEff << "(" << elRec << "/" << elAcc << ")" << "</td>";
   ss << "<td>" << muEff << "(" << muRec << "/" << muAcc << ")" << "</td>";
   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGenerator::PrintRowEfficiencyRich(
      int row,
      const std::string& rowName,
      const std::string& histName)
{
   std::string allRec = PrintValue("", histName + ".richAll.rec", "small");
   std::string allAcc = PrintValue("", histName + ".richAll.acc", "small");
   std::string allEff = PrintValue("", histName + ".richAll.eff", "b");
   std::string allRefRec = PrintValue("", histName + ".richAllRef.rec", "small");
   std::string allRefAcc = PrintValue("", histName + ".richAllRef.acc", "small");
   std::string allRefEff = PrintValue("", histName + ".richAllRef.eff", "b");
   std::string elRec = PrintValue("", histName + ".richEl.rec", "small");
   std::string elAcc = PrintValue("", histName + ".richEl.acc", "small");
   std::string elEff = PrintValue("", histName + ".richEl.eff", "b");
   std::string elRefRec = PrintValue("", histName + ".richElRef.rec", "small");
   std::string elRefAcc = PrintValue("", histName + ".richElRef.acc", "small");
   std::string elRefEff = PrintValue("", histName + ".richElRef.eff", "b");
   std::string piRec = PrintValue("", histName + ".richPi.rec", "small");
   std::string piAcc = PrintValue("", histName + ".richPi.acc", "small");
   std::string piEff = PrintValue("", histName + ".richPi.eff", "b");
   std::string piRefRec = PrintValue("", histName + ".richPiRef.rec", "small");
   std::string piRefAcc = PrintValue("", histName + ".richPiRef.acc", "small");
   std::string piRefEff = PrintValue("", histName + ".richPiRef.eff", "b");

   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << rowName << "</td>";
   ss << "<td>" << allEff << "(" << allRec << "/" << allAcc << ")" << "</td>";
   ss << "<td>" << allRefEff << "(" << allRefRec << "/" << allRefAcc << ")" << "</td>";
   ss << "<td>" << elEff << "(" << elRec << "/" << elAcc << ")" << "</td>";
   ss << "<td>" << elRefEff << "(" << elRefRec << "/" << elRefAcc << ")" << "</td>";
   ss << "<td>" << piEff << "(" << piRec << "/" << piAcc << ")" << "</td>";
   ss << "<td>" << piRefEff << "(" << piRefRec << "/" << piRefAcc << ")" << "</td>";
   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGenerator::PrintRowEfficiencyElId(
      int row,
      const std::string& rowName,
      const std::string& histName)
{
   std::string elRec = PrintValue("", rowName + ".el.rec", "small");
   std::string elAcc = PrintValue("", rowName + ".el.acc", "small");
   std::string elEff = PrintValue("", rowName + ".el.eff", "b");
   std::string piRec = PrintValue("", rowName + ".pi.rec", "small");
   std::string piAcc = PrintValue("", rowName + ".pi.acc", "small");
   std::string piSupp = PrintValue("", rowName + ".pi.supp", "b");

   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << rowName << "</td>";
   ss << "<td>" << elEff << "("<< elRec << "/" << elAcc << ")" << "</td>";
   ss << "<td>" << piSupp << "("<< piAcc << "/" << piRec << ")" << "</td>";
   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGenerator::PrintRowDetAccEl(
   int row,
   const std::string& rowName,
   const std::string& histName)
{
   std::string acc = PrintValue("", histName + ".detAccAcc.acc", "small");
   std::string mc = PrintValue("", histName + ".detAccAcc.mc", "small");
   std::string effAcc = PrintValue("", histName + ".detAccAcc.eff", "b");
   std::string rec = PrintValue("", histName + ".detAccRec.rec", "small");
   std::string effRec = PrintValue("", histName + ".detAccRec.eff", "b");

   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << rowName << "</td>";
   ss << "<td>" << effAcc << "(" << acc << "/" << mc << ")" << "</td>";
   ss << "<td>" << effRec << "(" << rec << "/" << mc << ")" << "</td>";
   ss << "</tr>";
   return ss.str();
}

std::string CbmLitQaHTMLGenerator::PrintPolarAngleEfficiency(
   const std::string& histName)
{
   float maxAngle = fQa->get("MaxAngle", -1.);
   float minAngle = fQa->get("MinAngle", -1.);
   float nofBinsAngle = fQa->get("NofBinsAngle", -1.);
   float step = (maxAngle - minAngle) / nofBinsAngle;

   if (maxAngle == -1. || minAngle == -1. || nofBinsAngle == -1.){
      return "ERROR PolarAngleEfficiencyToString";
   }

   std::stringstream ss;
   for (int i = 0; i < nofBinsAngle; i++) {
      std::string angle0 = lit::ToString<int>(i*step);
      std::string angle1 = lit::ToString<int>(i*step + step);
      if ((i % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
      ss << "<td>(" << angle0 << "-" << angle1 << ")</td>";

      ss << "<td>" << PrintValue("", histName + ".all.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", histName + ".all.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", histName + ".all.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", histName + ".ref.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", histName + ".ref.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", histName + ".ref.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", histName + ".prim.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", histName + ".prim.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", histName + ".prim.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", histName + ".sec.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", histName + ".sec.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", histName + ".sec.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", histName + ".el.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", histName + ".el.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", histName + ".el.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", histName + ".mu.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", histName + ".mu.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", histName + ".mu.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";

      ss << "</tr>" << std::endl;
   }
   return ss.str();
}

std::string CbmLitQaHTMLGenerator::PrintValue(
      const std::string& valueTitle,
      const std::string& valueName,
      const std::string& tag)
{
   // Print the value with corresponding color
   float check = fCheck->get(valueName, -1.);
   std::string color = (check == -1.) ? fWarningColor :
      (check == 0.) ? fErrorColor : fNormalColor;

   // Hint text
   std::string hint = "";
   if (check == 0. || check == 1.) {
      float min = fIdeal->get(valueName + ".min", -1.);
      float max = fIdeal->get(valueName + ".max", -1.);
      hint = "Limits (" + lit::ToString<float>(min) + ", " + lit::ToString<float>(max) + ")";
   }

   std::stringstream ss;
   std::string value = lit::NumberToString(fQa->get(valueName, -1.), 1);
   ss << "<" << tag << " title=\"" << hint << "\" style=\"background-color:" << color
         << "\">" << valueTitle << value << "</" << tag << ">";
   return ss.str();
}
