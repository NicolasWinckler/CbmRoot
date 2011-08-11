/** CbmLitReconstructionQaHTML.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#include "qa/CbmLitReconstructionQaHTML.h"

#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

#include "utils/CbmLitUtils.h"

#include <boost/property_tree/ptree.hpp>

void CbmLitReconstructionQaHTML::PrintFinalStatistics(
   std::ostream& out,
   boost::property_tree::ptree* pt)
{
   out.precision(3);
   out << "<html><body>" << HtmlHeadString();

   out << "<h1>CbmLitReconstructionQa final statistics</h1>" << std::endl;
   out << "<p>Number of events: " << pt->get("hEventNo", -1) << "</p>" << std::endl;

   // Number of objects statistics
   out << "<h2>Number of objects statistics</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>MVD</th><th>STS</th><th>RICH</th><th>TRD</th>"
         << "<th>MUCH pix</th><th>MUCH st</th><th>TOF</th></tr>" << std::endl;
   out << PrintNofStatisticsToString(0, pt, "Points","hNofMvdPoints", "hNofStsPoints", "hNofRichPoints",
         "hNofTrdPoints", "hNofMuchPoints", "hNofMuchPoints", "hNofTofPoints");
   out << PrintNofStatisticsToString(1, pt, "Digis", "hNofMvdDigis", "hNofStsDigis", "",
         "hNofTrdDigis", "hNofMuchDigis", "", "");
   out << PrintNofStatisticsToString(2, pt, "Clusters", "hNofMvdClusters", "hNofStsClusters", "",
         "hNofTrdClusters", "hNofMuchClusters", "", "");
   out << PrintNofStatisticsToString(3, pt, "Hits","hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");
   out << PrintNofStatisticsToString(4, pt, "Tracks","", "hNofStsTracks", "hNofRichRings",
         "hNofTrdTracks", "hNofMuchTracks", "hNofMuchTracks", "");
   out << "</table>" << std::endl;

   out << "<p>Number of global tracks per event: "<<(int)pt->get("hNofGlobalTracks", -1.) << "</p>" << std::endl;
   out << "<p>Number of track projections in RICH: " <<(int)pt->get("hNofRichProjections", -1.) << "</p>" << std::endl;

   // True and fake hits histograms (nof all, true, fake hits in track/ring)
   out << "<h2>True and fake hits</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>true</th><th>fake</th>"
         << "<th>true/all</th><th>fake/all</th></tr>" << std::endl;
   out << PrintHitsStatisticsToString(0, pt, "hMvdTrackHits", "MVD");
   out << PrintHitsStatisticsToString(1, pt, "hStsTrackHits", "STS");
   out << PrintHitsStatisticsToString(2, pt, "hTrdTrackHits", "TRD");
   out << PrintHitsStatisticsToString(3, pt, "hMuchTrackHits", "MUCH");
   out << PrintHitsStatisticsToString(4, pt, "hRichRingHits", "RICH");
   out << "</table>" << std::endl;

   // Reconstruction efficiency without RICH
   out << "<h2>Reconstruction efficiency w/o RICH</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>reference</th><th>primary</th>"
         << "<th>secondary</th><th>electron</th><th>muon</th></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsToString(pt, "hStsMom") << "</tr>";
   out << "<tr class=\"alt\"><td>TRD(MUCH)</td>" << EventEfficiencyStatisticsToString(pt, "hRecMom") << "</tr>";
   out << "<tr><td>TOF matching</td>" << EventEfficiencyStatisticsToString(pt, "hTofMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+TRD(MUCH)</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsToString(pt, "hStsMomNormHalfGlobal") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+TRD(MUCH)</td>" << EventEfficiencyStatisticsToString(pt, "hHalfGlobalMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+TRD(MUCH)+TOF</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsToString(pt, "hStsMomNormGlobal") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+TRD(MUCH)</td>" << EventEfficiencyStatisticsToString(pt, "hHalfGlobalMomNormGlobal") << "</tr>";
   out << "<tr><td>STS+TRD(MUCH)+TOF</td>" << EventEfficiencyStatisticsToString(pt, "hGlobalMom") << "</tr>";
   out << "</table>" << std::endl;

   // Reconstruction efficiency with RICH
   out << "<h2>Reconstruction efficiency with RICH</h2>";
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>all ref</th><th>electron</th>"
         << "<th>electron ref</th><th>pion</th><th>pion ref</th></tr>" << std::endl;

   out << "<tr><td>RICH</td>" << EventEfficiencyStatisticsRichToString(pt, "hRichMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+RICH</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsRichToString(pt, "hStsMomNormStsRich") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH</td>" << EventEfficiencyStatisticsRichToString(pt, "hStsRichMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+RICH+TRD</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsRichToString(pt, "hStsMomNormStsRichTrd") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH</td>" << EventEfficiencyStatisticsRichToString(pt, "hStsRichMomNormStsRichTrd") << "</tr>";
   out << "<tr><td>STS+RICH+TRD</td>" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdMom") << "</tr>";

   out << "<tr><td colspan=\"7\">Normalization STS+RICH+TRD+TOF</td></tr>" << std::endl;
   out << "<tr><td>STS</td>" << EventEfficiencyStatisticsRichToString(pt, "hStsMomNormStsRichTrdTof") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH</td>" << EventEfficiencyStatisticsRichToString(pt, "hStsRichMomNormStsRichTrdTof") << "</tr>";
   out << "<tr><td>STS+RICH+TRD</td>" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdMomNormStsRichTrdTof") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH+TRD+TOF</td>" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdTofMom") << "</tr>";
   out << "</table>" << std::endl;

   // Print electron identification statistics
   out << "<h2>Electron identification</h2>" << std::endl;
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>Efficiency</th><th>Pion supp.</th></tr>" << std::endl;

   out << "<tr><td colspan=\"3\">Normalization STS+TRD</td></tr>" << std::endl;
   out << "<tr><td>STS+TRD</td>" << EventEfficiencyStatisticsElIdToString(pt, "hStsTrdMomElId") << "</tr>";

   out << "<tr><td colspan=\"3\">Normalization STS+TRD+TOF</td></tr>" << std::endl;
   out << "<tr><td>STS+TRD</td>" << EventEfficiencyStatisticsElIdToString(pt, "hStsTrdMomElIdNormStsTrdTof") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+TRD+TOF</td>" << EventEfficiencyStatisticsElIdToString(pt, "hStsTrdTofMomElId") << "</tr>";

   out << "<tr><td colspan=\"3\">Normalization STS+RICH</td></tr>" << std::endl;
   out << "<tr><td>STS+RICH</td>" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichMomElId") << "</tr>";

   out << "<tr><td colspan=\"3\">Normalization STS+RICH+TRD</td></tr>" << std::endl;
   out << "<tr><td>STS+RICH</td>" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichMomElIdNormStsRichTrd") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH+TRD</td>" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichTrdMomElId") << "</tr>";

   out << "<tr><td colspan=\"3\">Normalization STS+RICH+TRD+TOF</td></tr>" << std::endl;
   out << "<tr><td>STS+RICH</td>" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichMomElIdNormStsRichTrdTof") << "</tr>";
   out << "<tr class=\"alt\"><td>STS+RICH+TRD</td>" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichTrdMomElIdNormStsRichTrdTof") << "</tr>";
   out << "<tr><td>STS+RICH+TRD+TOF</td>" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichTrdTofMomElId") << "</tr>";
   out << "</table>" << std::endl;

   // Detector acceptance efficiency
   out << "<h2>Detector acceptance for primary electrons</h2>" << std::endl;
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>kAcc/MC</th><th>kRec/MC</th></tr>" << std::endl;
   out << EventDetAccElStatisticsToString(0, pt, "STS", "hStsDetAccEl");
   out << EventDetAccElStatisticsToString(1, pt, "STS-RICH","hStsRichDetAccEl");
   out << EventDetAccElStatisticsToString(2, pt, "STS-TRD", "hStsTrdDetAccEl");
   out << EventDetAccElStatisticsToString(3, pt, "STS-TOF", "hStsTofDetAccEl");
   out << EventDetAccElStatisticsToString(4, pt, "STS-RICH-TRD", "hStsRichTrdDetAccEl");
   out << EventDetAccElStatisticsToString(5, pt, "STS-RICH-TRD-TOF", "hStsRichTrdTofDetAccEl");
   out << EventDetAccElStatisticsToString(6, pt, "STS-TRD-TOF", "hStsTrdTofDetAccEl");
   out << "</table>" << std::endl;

   // Ghost statistics
   out << "<h2>Ghosts per event</h2>";
   out << "<ul>";
   out << "<li>STS=" << pt->get("fhStsGhostNh", -1.);
   out << " " << "TRD(MUCH)" << "=" << pt->get("fhRecGhostNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostNh", -1.) << std::endl;
   out << "<li>after STS-RICH matching: STS=" << pt->get("fhStsGhostRichMatchingNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostStsMatchingNh", -1.) << std::endl;
   out << "<li>after STS-RICH matching and el identification: RICH="
         << pt->get("fhRichGhostElIdNh", -1.) << std::endl;
   out << "</ul>";

   // STS quality numbers
   out << "<h2>STS quality numbers</h2>";
   out << "<p>Chi2 to primary vertex: mean = " << pt->get("fhStsChiprim.mean", -1.)
         << " RMS = " << pt->get("fhStsChiprim.rms", -1.) << "</p>" << std::endl;
   out << "<p>Momentum resolution: mean = " << pt->get("fhStsMomresVsMom.mean", -1.)
         << " RMS = " << pt->get("fhStsMomresVsMom.rms", -1.) << "</p>" << std::endl;

   // Tracking efficiency vs. polar angle
   out << "<h2>Tracking efficiency in dependence on polar angle</h2>" << std::endl;
   out << "<table id=\"efficiency\" >";
   out << "<tr><th></th><th>all</th><th>reference</th><th>primary</th>"
         << "<th>secondary</th><th>electron</th><th>muon</th></tr>" << std::endl;
   out << "<tr><td colspan=\"7\">STS</td></tr>" << std::endl;
   out << PolarAngleEfficiencyToString(pt, "hStsAngle");
   out << "<tr><td colspan=\"7\">TRD(MUCH)</td></tr>" << std::endl;
   out << PolarAngleEfficiencyToString(pt, "hRecAngle");
   out << "<tr><td colspan=\"7\">TOF</td></tr>" << std::endl;
   out << PolarAngleEfficiencyToString(pt, "hTofAngle");
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

std::string CbmLitReconstructionQaHTML::PrintNofStatisticsToString(
   int row,
   boost::property_tree::ptree* pt,
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

   if (mvd != "") { ss << "<td>" << (int)pt->get(mvd, -1.) << "</td>"; }
      else { ss << "<td>-</td>"; }
   if (sts != "") { ss << "<td>" << (int)pt->get(sts, -1.) << "</td>"; }
      else { ss << "<td>-</td>"; }
   if (rich != "") { ss << "<td>" << (int)pt->get(rich, -1.) << "</td>"; }
      else { ss << "<td>-</td>"; }
   if (trd != "") { ss << "<td>" << (int)pt->get(trd, -1.) << "</td>"; }
      else { ss << "<td>-</td>"; }
   if (muchP != "") { ss << "<td>" << (int)pt->get(muchP, -1.) << "</td>"; }
      else { ss << "<td>-</td>"; }
   if (muchS != "") { ss << "<td>" << (int)pt->get(muchS, -1.) << "</td>"; }
      else { ss << "<td>-</td>"; }
   if (tof!= "") { ss << "<td>" << (int)pt->get(tof, -1.) << "</td>"; }
      else { ss << "<td>-</td>"; }

   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitReconstructionQaHTML::PrintHitsStatisticsToString(
   int row,
   boost::property_tree::ptree* pt,
   const std::string& hist,
   const std::string& name)
{
   double all = pt->get(hist+".all", -1.);
   double trueh = pt->get(hist+".true", -1.);
   double fakeh = pt->get(hist+".fake", -1.);
   double toa = pt->get(hist+".trueOverAll", -1.);
   double foa = pt->get(hist+".fakeOverAll", -1.);

   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << name << "</td>";
   if (all == -1.) ss << "<td>-</td>"; else ss << "<td>" << all << "</td>";
   if (trueh == -1.) ss << "<td>-</td>"; else ss << "<td>" << trueh << "</td>";
   if (fakeh == -1.) ss << "<td>-</td>"; else ss << "<td>" << fakeh << "</td>";
   if (toa == -1.) ss << "<td>-</td>"; else ss << "<td>" << 100. * toa << "</td>";
   if (foa == -1.) ss << "<td>-</td>"; else ss << "<td>" << 100. * foa << "</td>";

   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitReconstructionQaHTML::EventEfficiencyStatisticsToString(
   boost::property_tree::ptree* pt,
   const std::string& name)
{
   double allRec = pt->get(name + ".all.rec", -1.);
   double allAcc = pt->get(name + ".all.acc", -1.);
   double allEff = pt->get(name + ".all.eff", -1.);
   double refRec = pt->get(name + ".ref.rec", -1.);
   double refAcc = pt->get(name + ".ref.acc", -1.);
   double refEff = pt->get(name + ".ref.eff", -1.);
   double primRec = pt->get(name + ".prim.rec", -1.);
   double primAcc = pt->get(name + ".prim.acc", -1.);
   double primEff = pt->get(name + ".prim.eff", -1.);
   double secRec = pt->get(name + ".sec.rec", -1.);
   double secAcc = pt->get(name + ".sec.acc", -1.);
   double secEff = pt->get(name + ".sec.eff", -1.);
   double muRec = pt->get(name + ".mu.rec", -1.);
   double muAcc = pt->get(name + ".mu.acc", -1.);
   double muEff = pt->get(name + ".mu.eff", -1.);
   double elRec = pt->get(name + ".el.rec", -1.);
   double elAcc = pt->get(name + ".el.acc", -1.);
   double elEff = pt->get(name + ".el.eff", -1.);

   std::stringstream ss;

   ss << "<td>" << allEff << "("<< allRec << "/" << allAcc << ")" << "</td>";
   ss << "<td>" << refEff << "("<< refRec << "/" << refAcc << ")" << "</td>";
   ss << "<td>" << primEff << "(" << primRec << "/" << primAcc << ")" << "</td>";
   ss << "<td>" << secEff << "(" << secRec << "/" << secAcc << ")" << "</td>";
   ss << "<td>" << elEff << "(" << elRec << "/" << elAcc << ")" << "</td>";
   ss << "<td>" << muEff << "(" << muRec << "/" << muAcc << ")" << "</td>";

   return ss.str();
}

std::string CbmLitReconstructionQaHTML::EventEfficiencyStatisticsRichToString(
   boost::property_tree::ptree* pt,
   const std::string& name)
{
   double allRec = pt->get(name + ".richAll.rec", -1.);
   double allAcc = pt->get(name + ".richAll.acc", -1.);
   double allEff = pt->get(name + ".richAll.eff", -1.);
   double allRefRec = pt->get(name + ".richAllRef.rec", -1.);
   double allRefAcc = pt->get(name + ".richAllRef.acc", -1.);
   double allRefEff = pt->get(name + ".richAllRef.eff", -1.);
   double elRec = pt->get(name + ".richEl.rec", -1.);
   double elAcc = pt->get(name + ".richEl.acc", -1.);
   double elEff = pt->get(name + ".richEl.eff", -1.);
   double elRefRec = pt->get(name + ".richElRef.rec", -1.);
   double elRefAcc = pt->get(name + ".richElRef.acc", -1.);
   double elRefEff = pt->get(name + ".richElRef.eff", -1.);
   double piRec = pt->get(name + ".richPi.rec", -1.);
   double piAcc = pt->get(name + ".richPi.acc", -1.);
   double piEff = pt->get(name + ".richPi.eff", -1.);
   double piRefRec = pt->get(name + ".richPiRef.rec", -1.);
   double piRefAcc = pt->get(name + ".richPiRef.acc", -1.);
   double piRefEff = pt->get(name + ".richPiRef.eff", -1.);

   std::stringstream ss;

   ss << "<td>" << allEff << "(" << allRec << "/" << allAcc << ")" << "</td>";
   ss << "<td>" << allRefEff << "(" << allRefRec << "/" << allRefAcc << ")" << "</td>";
   ss << "<td>" << elEff << "(" << elRec << "/" << elAcc << ")" << "</td>";
   ss << "<td>" << elRefEff << "(" << elRefRec << "/" << elRefAcc << ")" << "</td>";
   ss << "<td>" << piEff << "(" << piRec << "/" << piAcc << ")" << "</td>";
   ss << "<td>" << piRefEff << "(" << piRefRec << "/" << piRefAcc << ")" << "</td>";

   return ss.str();
}

std::string CbmLitReconstructionQaHTML::EventEfficiencyStatisticsElIdToString(
   boost::property_tree::ptree* pt,
   const std::string& name)
{
   double elRec = pt->get(name + ".el.rec", -1.);
   double elAcc = pt->get(name + ".el.acc", -1.);
   double elEff = pt->get(name + ".el.eff", -1.);
   double piRec = pt->get(name + ".pi.rec", -1.);
   double piAcc = pt->get(name + ".pi.acc", -1.);
   double piSupp = pt->get(name + ".pi.supp", -1.);

   std::stringstream ss;
   ss << "<td>" << elEff << "("<< elRec << "/" << elAcc << ")" << "</td>";
   ss << "<td>" << piSupp << "("<< piAcc << "/" << piRec << ")" << "</td>";

   return ss.str();
}

std::string CbmLitReconstructionQaHTML::EventDetAccElStatisticsToString(
   int row,
   boost::property_tree::ptree* pt,
   const std::string& effName,
   const std::string& name)
{
   double acc = pt->get(name + ".detAccAcc.acc", -1.);
   double mc = pt->get(name + ".detAccAcc.mc", -1.);
   double effAcc = pt->get(name + ".detAccAcc.eff", -1.);
   double rec = pt->get(name + ".detAccRec.rec", -1.);
   double effRec = pt->get(name + ".detAccRec.eff", -1.);

   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << effName << "</td>";
   ss << "<td>" << effAcc << "(" << acc << "/" << mc << ")" << "</td>";
   ss << "<td>" << effRec << "(" << rec << "/" << mc << ")" << "</td>";
   ss << "</tr>";

   return ss.str();
}

std::string CbmLitReconstructionQaHTML::PolarAngleEfficiencyToString(
   boost::property_tree::ptree* pt,
   const std::string& name)
{
   double maxAngle = pt->get("MaxAngle", -1.);
   double minAngle = pt->get("MinAngle", -1.);
   double nofBinsAngle = pt->get("NofBinsAngle", -1.);
   double step = (maxAngle - minAngle) / nofBinsAngle;

   if (maxAngle == -1. || minAngle == -1. || nofBinsAngle == -1.){
      return "ERROR PolarAngleEfficiencyToString";
   }

   std::stringstream ss;
   for (int i = 0; i < nofBinsAngle; i++) {
      std::string angle0 = lit::ToString<int>(i*step);
      std::string angle1 = lit::ToString<int>(i*step + step);
      if ((i % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
      ss << "<td>(" << angle0 << "-" << angle1 << ")</td>";

      ss << "<td>" << pt->get(name+".all.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".all.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".all.acc."+angle0 +"_" + angle1, -1.) << ")" << "</td>";
      ss << "<td>" << pt->get(name+".ref.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".ref.rec."+angle0 +"_" + angle1, -1.) << "/"
          << pt->get(name+".ref.acc."+angle0 +"_" + angle1, -1.) << ")" << "</td>";
      ss << "<td>" << pt->get(name+".prim.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".prim.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".prim.acc."+angle0 +"_" + angle1, -1.) << ")" << "</td>";
      ss << "<td>" << pt->get(name+".sec.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".sec.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".sec.acc."+angle0 +"_" + angle1, -1.) << ")" << "</td>";
      ss << "<td>" << pt->get(name+".el.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".el.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".el.acc."+angle0 +"_" + angle1, -1.) << ")" << "</td>";
      ss << "<td>" << pt->get(name+".mu.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".mu.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".mu.acc."+angle0 +"_" + angle1, -1.) << ")" << "</td>";

      ss << "</tr>" << std::endl;
   }
   return ss.str();
}

std::string CbmLitReconstructionQaHTML::HtmlHeadString()
{
   std::string str = "<head><style type=\"text/css\">";
   str += "#efficiency";
   str += "{";
   str += "font-family:Verdana, Arial, Helvetica, sans-serif;";
   str += "width:100%;";
   str += "border-collapse:collapse;";
   str += "}";
   str += "#efficiency td, #efficiency th";
   str += "{";
   str += "font-size:1em;";
   str += "border:1px solid #98bf21;";
   str += "padding:3px 7px 2px 7px;";
   str += "}";
   str += "#efficiency th";
   str += "{";
   str += "font-size:1.1em;";
   str += "text-align:left;";
   str += "padding-top:5px;";
   str += "padding-bottom:4px;";
   str += "background-color:#A7C942;";
   str += "color:#ffffff;";
   str += "}";
   str += "#efficiency tr.alt td";
   str += "{";
   str += "color:#000000;";
   str += "background-color:#EAF2D3;";
   str += "}";
   str += "</style></head>";
   return str;
}

