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

   out.precision(3);
   out << "<html><body>" << HtmlHeadString();

   out << "<h1>CbmLitReconstructionQa final statistics</h1>" << std::endl;
   out << PrintValue("Number of events: ", "hEventNo", "p");

   // Number of objects statistics
   out << "<h2>Number of objects</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>MVD</th><th>STS</th><th>RICH</th><th>TRD</th>"
         << "<th>MUCH pix</th><th>MUCH st</th><th>TOF</th></tr>" << std::endl;
   out << PrintNofStatisticsToString(0, "Points","hNofMvdPoints", "hNofStsPoints", "hNofRichPoints",
         "hNofTrdPoints", "hNofMuchPoints", "hNofMuchPoints", "hNofTofPoints");
   out << PrintNofStatisticsToString(1, "Digis", "hNofMvdDigis", "hNofStsDigis", "",
         "hNofTrdDigis", "hNofMuchDigis", "", "");
   out << PrintNofStatisticsToString(2, "Clusters", "hNofMvdClusters", "hNofStsClusters", "",
         "hNofTrdClusters", "hNofMuchClusters", "", "");
   out << PrintNofStatisticsToString(3, "Hits","hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");
   out << PrintNofStatisticsToString(4, "Tracks","", "hNofStsTracks", "hNofRichRings",
         "hNofTrdTracks", "hNofMuchTracks", "hNofMuchTracks", "");
   out << "</table>" << std::endl;

   out << PrintValue("Number of global tracks per event: ", "hNofGlobalTracks", "p") << std::endl;
   out << PrintValue("Number of track projections in RICH: ", "hNofRichProjections", "p") << std::endl;

   // True and fake hits histograms (nof all, true, fake hits in track/ring)
   out << "<h2>True and fake hits</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>true</th><th>fake</th>"
         << "<th>true/all</th><th>fake/all</th></tr>" << std::endl;
   out << PrintHitsStatisticsToString(0, "hMvdTrackHits", "MVD");
   out << PrintHitsStatisticsToString(1, "hStsTrackHits", "STS");
   out << PrintHitsStatisticsToString(2, "hTrdTrackHits", "TRD");
   out << PrintHitsStatisticsToString(3, "hMuchTrackHits", "MUCH");
   out << PrintHitsStatisticsToString(4, "hRichRingHits", "RICH");
   out << "</table>" << std::endl;

   // Reconstruction efficiency without RICH
   out << "<h2>Reconstruction efficiency w/o RICH</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>reference</th><th>primary</th>"
         << "<th>secondary</th><th>electron</th><th>muon</th></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsToString("hStsMom") << "</tr>";
   out << "<tr class=\"alt\"><td>TRD(MUCH)</td>" << EventEfficiencyStatisticsToString("hRecMom") << "</tr>";
   out << "<tr><td>TOF matching</td>" << EventEfficiencyStatisticsToString("hTofMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+TRD(MUCH)</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsToString("hStsMomNormHalfGlobal") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+TRD(MUCH)</td>" << EventEfficiencyStatisticsToString("hHalfGlobalMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+TRD(MUCH)+TOF</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsToString("hStsMomNormGlobal") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+TRD(MUCH)</td>" << EventEfficiencyStatisticsToString("hHalfGlobalMomNormGlobal") << "</tr>";
   out << "<tr><td>STS+TRD(MUCH)+TOF</td>" << EventEfficiencyStatisticsToString("hGlobalMom") << "</tr>";
   out << "</table>" << std::endl;

   // Reconstruction efficiency with RICH
   out << "<h2>Reconstruction efficiency with RICH</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>all ref</th><th>electron</th>"
         << "<th>electron ref</th><th>pion</th><th>pion ref</th></tr>" << std::endl;

   out << "<tr><td>RICH</td>" << EventEfficiencyStatisticsRichToString("hRichMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+RICH</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsRichToString("hStsMomNormStsRich") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH</td>" << EventEfficiencyStatisticsRichToString("hStsRichMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+RICH+TRD</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsRichToString("hStsMomNormStsRichTrd") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH</td>" << EventEfficiencyStatisticsRichToString("hStsRichMomNormStsRichTrd") << "</tr>";
   out << "<tr><td>STS+RICH+TRD</td>" << EventEfficiencyStatisticsRichToString("hStsRichTrdMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+RICH+TRD+TOF</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsRichToString("hStsMomNormStsRichTrdTof") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH</td>" << EventEfficiencyStatisticsRichToString("hStsRichMomNormStsRichTrdTof") << "</tr>";
   out << "<tr><td>STS+RICH+TRD</td>" << EventEfficiencyStatisticsRichToString("hStsRichTrdMomNormStsRichTrdTof") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH+TRD+TOF</td>" << EventEfficiencyStatisticsRichToString("hStsRichTrdTofMom") << "</tr>";
   out << "</table>" << std::endl;

   // Print electron identification statistics
   out << "<h2>Electron identification</h2>" << std::endl;
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>Efficiency</th><th>Pion supp.</th></tr>" << std::endl;

   out << "<tr><td colspan=\"3\">Normalization STS+TRD</td></tr>" << std::endl;
   out << "<tr><td>STS+TRD</td>" << EventEfficiencyStatisticsElIdToString("hStsTrdMomElId") << "</tr>";

   out << "<tr><td colspan=\"3\">Normalization STS+TRD+TOF</td></tr>" << std::endl;
   out << "<tr><td>STS+TRD</td>" << EventEfficiencyStatisticsElIdToString("hStsTrdMomElIdNormStsTrdTof") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+TRD+TOF</td>" << EventEfficiencyStatisticsElIdToString("hStsTrdTofMomElId") << "</tr>";

   out << "<tr><td colspan=\"3\">Normalization STS+RICH</td></tr>" << std::endl;
   out << "<tr><td>STS+RICH</td>" << EventEfficiencyStatisticsElIdToString("hStsRichMomElId") << "</tr>";

   out << "<tr><td colspan=\"3\">Normalization STS+RICH+TRD</td></tr>" << std::endl;
   out << "<tr><td>STS+RICH</td>" << EventEfficiencyStatisticsElIdToString("hStsRichMomElIdNormStsRichTrd") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH+TRD</td>" << EventEfficiencyStatisticsElIdToString("hStsRichTrdMomElId") << "</tr>";

   out << "<tr><td colspan=\"3\">Normalization STS+RICH+TRD+TOF</td></tr>" << std::endl;
   out << "<tr><td>STS+RICH</td>" << EventEfficiencyStatisticsElIdToString("hStsRichMomElIdNormStsRichTrdTof") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH+TRD</td>" << EventEfficiencyStatisticsElIdToString("hStsRichTrdMomElIdNormStsRichTrdTof") << "</tr>";
   out << "<tr><td>STS+RICH+TRD+TOF</td>" << EventEfficiencyStatisticsElIdToString("hStsRichTrdTofMomElId") << "</tr>";
   out << "</table>" << std::endl;

   // Detector acceptance efficiency
   out << "<h2>Detector acceptance for primary electrons</h2>" << std::endl;
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>kAcc/MC</th><th>kRec/MC</th></tr>" << std::endl;
   out << EventDetAccElStatisticsToString(0, "STS", "hStsDetAccEl");
   out << EventDetAccElStatisticsToString(1, "STS-RICH","hStsRichDetAccEl");
   out << EventDetAccElStatisticsToString(2, "STS-TRD", "hStsTrdDetAccEl");
   out << EventDetAccElStatisticsToString(3, "STS-TOF", "hStsTofDetAccEl");
   out << EventDetAccElStatisticsToString(4, "STS-RICH-TRD", "hStsRichTrdDetAccEl");
   out << EventDetAccElStatisticsToString(5, "STS-RICH-TRD-TOF", "hStsRichTrdTofDetAccEl");
   out << EventDetAccElStatisticsToString(6, "STS-TRD-TOF", "hStsTrdTofDetAccEl");
   out << "</table>" << std::endl;

   // Ghost statistics
   out << "<h2>Ghosts per event</h2>";
   out << "<ul>";
   out << "<li>" << PrintValue("STS=", "fhStsGhostNh", "p");
   out << PrintValue("TRD(MUCH)=", "fhRecGhostNh", "p");
   out << PrintValue("RICH=", "fhRichGhostNh", "p");
   out << "<li>after STS-RICH matching: ";
   out << PrintValue("STS=", "fhStsGhostRichMatchingNh", "p");
   out << PrintValue("RICH=", "fhRichGhostStsMatchingNh", "p");
   out << "<li>after STS-RICH matching and el identification: ";
   out << PrintValue("RICH=", "fhRichGhostElIdNh", "p");
   out << "</ul>";

   // STS quality numbers
   out << "<h2>STS quality numbers</h2>";
   out << "<p>Chi2 to primary vertex: ";
   out << PrintValue("mean = ", "fhStsChiprim.mean", "p");
   out << PrintValue("RMS = ", "fhStsChiprim.rms", "p") << "</p>";
   out << "<p>Momentum resolution: ";
   out << PrintValue("mean = ", "fhStsMomresVsMom.mean", "p");
   out << PrintValue("RMS = ", "fhStsMomresVsMom.rms", "p") << "</p>";

   // Tracking efficiency vs. polar angle
   out << "<h2>Tracking efficiency in dependence on polar angle</h2>" << std::endl;
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>reference</th><th>primary</th>"
         << "<th>secondary</th><th>electron</th><th>muon</th></tr>" << std::endl;
   out << "<tr><td colspan=\"7\">STS</td></tr>" << std::endl;
   out << PolarAngleEfficiencyToString("hStsAngle");
   out << "<tr><td colspan=\"7\">TRD(MUCH)</td></tr>" << std::endl;
   out << PolarAngleEfficiencyToString("hRecAngle");
   out << "<tr><td colspan=\"7\">TOF</td></tr>" << std::endl;
   out << PolarAngleEfficiencyToString("hTofAngle");
   out << "</table>" << std::endl;

   // Images
   out << "<h3>rec_qa_global_efficiency_all</h3><img src=\"rec_qa_global_efficiency_all.png\" alt=\"rec_qa_global_efficiency_all\" />";
   out << "<h3>rec_qa_global_efficiency_signal</h3><img src=\"rec_qa_global_efficiency_signal.png\" alt=\"rec_qa_global_efficiency_signal\" />";
   out << "<h3>rec_qa_sts_efficiency</h3><img src=\"rec_qa_sts_efficiency.png\" alt=\"rec_qa_sts_efficiency\" />";
   out << "<h3>rec_qa_rec_efficiency</h3><img src=\"rec_qa_rec_efficiency.png\" alt=\"rec_qa_rec_efficiency\" />";
   out << "<h3>rec_qa_tof_efficiency</h3><img src=\"rec_qa_tof_efficiency.png\" alt=\"rec_qa_tof_efficiency\" />";
   out << "<h3>rec_qa_rich_efficiency_electrons</h3><img src=\"rec_qa_rich_efficiency_electrons.png\" alt=\"rec_qa_rich_efficiency_electrons\" />";
   out << "<h3>rec_qa_sts_rich_efficiency_electrons</h3><img src=\"rec_qa_sts_rich_efficiency_electrons.png\" alt=\"rec_qa_sts_rich_efficiency_electrons\" />";
   out << "<h3>rec_qa_sts_rich_trd_efficiency_electrons</h3><img src=\"rec_qa_sts_rich_trd_efficiency_electrons.png\" alt=\"rec_qa_sts_rich_trd_efficiency_electrons\" />";
   out << "<h3>rec_qa_sts_rich_trd_tof_efficiency_electrons</h3><img src=\"rec_qa_sts_rich_trd_tof_efficiency_electrons.png\" alt=\"rec_qa_sts_rich_trd_tof_efficiency_electrons\" />";
   out << "<h3>rec_qa_sts_rich_trd_tof_detector_acceptance</h3><img src=\"rec_qa_sts_rich_trd_tof_detector_acceptance.png\" alt=\"rec_qa_sts_rich_trd_tof_detector_acceptance\" />";
   out << "<h3>rec_qa_sts_rich_trd_tof_electron_identification</h3><img src=\"rec_qa_sts_rich_trd_tof_electron_identification.png\" alt=\"rec_qa_sts_rich_trd_tof_electron_identification\" />";
   out << "<h3>rec_qa_sts_rich_trd_tof_pion_suppression</h3><img src=\"rec_qa_sts_rich_trd_tof_pion_suppression.png\" alt=\"rec_qa_sts_rich_trd_tof_pion_suppression\" />";

   out <<  "</body></html>";
}

std::string CbmLitQaHTMLGenerator::PrintNofStatisticsToString(
   int row,
   const std::string& name,
   const std::string& mvd,
   const std::string& sts,
   const std::string& rich,
   const std::string& trd,
   const std::string& muchP,
   const std::string& muchS,
   const std::string& tof)
{
   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << name << "</td>";

   if (mvd != "") { ss << PrintValue("", mvd, "td"); }
      else { ss << "<td>-</td>"; }
   if (sts != "") { ss << PrintValue("", sts, "td"); }
      else { ss << "<td>-</td>"; }
   if (rich != "") { ss << PrintValue("", rich, "td"); }
      else { ss << "<td>-</td>"; }
   if (trd != "") { ss << PrintValue("", trd, "td"); }
      else { ss << "<td>-</td>"; }
   if (muchP != "") { ss << PrintValue("", muchP, "td"); }
      else { ss << "<td>-</td>"; }
   if (muchS != "") { ss << PrintValue("", muchS, "td"); }
      else { ss << "<td>-</td>"; }
   if (tof!= "") { ss << PrintValue("", tof, "td"); }
      else { ss << "<td>-</td>"; }

   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGenerator::PrintHitsStatisticsToString(
   int row,
   const std::string& hist,
   const std::string& name)
{
   std::string all = PrintValue("", hist + ".all", "td");
   std::string trueh = PrintValue("", hist + ".true", "td");
   std::string fakeh = PrintValue("", hist + ".fake", "td");
   std::string toa = PrintValue("", hist + ".trueOverAll", "td");
   std::string foa = PrintValue("", hist + ".fakeOverAll", "td");

   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << name << "</td>";
   ss << all << trueh << fakeh << toa << foa;
   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGenerator::EventEfficiencyStatisticsToString(
   const std::string& name)
{
   std::string allRec = PrintValue("", name + ".all.rec", "small");
   std::string allAcc = PrintValue("", name + ".all.acc", "small");
   std::string allEff = PrintValue("", name + ".all.eff", "b");
   std::string refRec = PrintValue("", name + ".ref.rec", "small");
   std::string refAcc = PrintValue("", name + ".ref.acc", "small");
   std::string refEff = PrintValue("", name + ".ref.eff", "b");
   std::string primRec = PrintValue("", name + ".prim.rec", "small");
   std::string primAcc = PrintValue("", name + ".prim.acc", "small");
   std::string primEff = PrintValue("", name + ".prim.eff", "b");
   std::string secRec = PrintValue("", name + ".sec.rec", "small");
   std::string secAcc = PrintValue("", name + ".sec.acc", "small");
   std::string secEff = PrintValue("", name + ".sec.eff", "b");
   std::string muRec = PrintValue("", name + ".mu.rec", "small");
   std::string muAcc = PrintValue("", name + ".mu.acc", "small");
   std::string muEff = PrintValue("", name + ".mu.eff", "b");
   std::string elRec = PrintValue("", name + ".el.rec", "small");
   std::string elAcc = PrintValue("", name + ".el.acc", "small");
   std::string elEff = PrintValue("", name + ".el.eff", "b");

   std::stringstream ss;

   ss << "<td>" << allEff << "("<< allRec << "/" << allAcc << ")" << "</td>";
   ss << "<td>" << refEff << "("<< refRec << "/" << refAcc << ")" << "</td>";
   ss << "<td>" << primEff << "(" << primRec << "/" << primAcc << ")" << "</td>";
   ss << "<td>" << secEff << "(" << secRec << "/" << secAcc << ")" << "</td>";
   ss << "<td>" << elEff << "(" << elRec << "/" << elAcc << ")" << "</td>";
   ss << "<td>" << muEff << "(" << muRec << "/" << muAcc << ")" << "</td>";

   return ss.str();
}

std::string CbmLitQaHTMLGenerator::EventEfficiencyStatisticsRichToString(
   const std::string& name)
{
   std::string allRec = PrintValue("", name + ".richAll.rec", "small");
   std::string allAcc = PrintValue("", name + ".richAll.acc", "small");
   std::string allEff = PrintValue("", name + ".richAll.eff", "b");
   std::string allRefRec = PrintValue("", name + ".richAllRef.rec", "small");
   std::string allRefAcc = PrintValue("", name + ".richAllRef.acc", "small");
   std::string allRefEff = PrintValue("", name + ".richAllRef.eff", "b");
   std::string elRec = PrintValue("", name + ".richEl.rec", "small");
   std::string elAcc = PrintValue("", name + ".richEl.acc", "small");
   std::string elEff = PrintValue("", name + ".richEl.eff", "b");
   std::string elRefRec = PrintValue("", name + ".richElRef.rec", "small");
   std::string elRefAcc = PrintValue("", name + ".richElRef.acc", "small");
   std::string elRefEff = PrintValue("", name + ".richElRef.eff", "b");
   std::string piRec = PrintValue("", name + ".richPi.rec", "small");
   std::string piAcc = PrintValue("", name + ".richPi.acc", "small");
   std::string piEff = PrintValue("", name + ".richPi.eff", "b");
   std::string piRefRec = PrintValue("", name + ".richPiRef.rec", "small");
   std::string piRefAcc = PrintValue("", name + ".richPiRef.acc", "small");
   std::string piRefEff = PrintValue("", name + ".richPiRef.eff", "b");

   std::stringstream ss;

   ss << "<td>" << allEff << "(" << allRec << "/" << allAcc << ")" << "</td>";
   ss << "<td>" << allRefEff << "(" << allRefRec << "/" << allRefAcc << ")" << "</td>";
   ss << "<td>" << elEff << "(" << elRec << "/" << elAcc << ")" << "</td>";
   ss << "<td>" << elRefEff << "(" << elRefRec << "/" << elRefAcc << ")" << "</td>";
   ss << "<td>" << piEff << "(" << piRec << "/" << piAcc << ")" << "</td>";
   ss << "<td>" << piRefEff << "(" << piRefRec << "/" << piRefAcc << ")" << "</td>";

   return ss.str();
}

std::string CbmLitQaHTMLGenerator::EventEfficiencyStatisticsElIdToString(
   const std::string& name)
{
   std::string elRec = PrintValue("", name + ".el.rec", "small");
   std::string elAcc = PrintValue("", name + ".el.acc", "small");
   std::string elEff = PrintValue("", name + ".el.eff", "b");
   std::string piRec = PrintValue("", name + ".pi.rec", "small");
   std::string piAcc = PrintValue("", name + ".pi.acc", "small");
   std::string piSupp = PrintValue("", name + ".pi.supp", "b");

   std::stringstream ss;
   ss << "<td>" << elEff << "("<< elRec << "/" << elAcc << ")" << "</td>";
   ss << "<td>" << piSupp << "("<< piAcc << "/" << piRec << ")" << "</td>";

   return ss.str();
}

std::string CbmLitQaHTMLGenerator::EventDetAccElStatisticsToString(
   int row,
   const std::string& effName,
   const std::string& name)
{
   std::string acc = PrintValue("", name + ".detAccAcc.acc", "small");
   std::string mc = PrintValue("", name + ".detAccAcc.mc", "small");
   std::string effAcc = PrintValue("", name + ".detAccAcc.eff", "b");
   std::string rec = PrintValue("", name + ".detAccRec.rec", "small");
   std::string effRec = PrintValue("", name + ".detAccRec.eff", "b");

   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << effName << "</td>";
   ss << "<td>" << effAcc << "(" << acc << "/" << mc << ")" << "</td>";
   ss << "<td>" << effRec << "(" << rec << "/" << mc << ")" << "</td>";
   ss << "</tr>";

   return ss.str();
}

std::string CbmLitQaHTMLGenerator::PolarAngleEfficiencyToString(
   const std::string& name)
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

      ss << "<td>" << PrintValue("", name + ".all.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", name + ".all.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", name + ".all.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", name + ".ref.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", name + ".ref.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", name + ".ref.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", name + ".prim.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", name + ".prim.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", name + ".prim.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", name + ".sec.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", name + ".sec.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", name + ".sec.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", name + ".el.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", name + ".el.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", name + ".el.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";
      ss << "<td>" << PrintValue("", name + ".mu.eff." + angle0 + "_" + angle1, "b")
          << "(" << PrintValue("", name + ".mu.rec." + angle0 + "_" + angle1, "small")
          << "/" << PrintValue("", name + ".mu.acc." + angle0 + "_" + angle1, "small") << ")" << "</td>";

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
