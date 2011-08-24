/**
 * \file CbmLitQaHTMLGeneratorStudy.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaHTMLGeneratorStudy.h"
#include "utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

CbmLitQaHTMLGeneratorStudy::CbmLitQaHTMLGeneratorStudy()
{
   fErrorColor = "#FF0000";
   fWarningColor = "#FFFF00";
   fNormalColor = "#00FF00";
}

CbmLitQaHTMLGeneratorStudy::~CbmLitQaHTMLGeneratorStudy()
{

}

void CbmLitQaHTMLGeneratorStudy::Create(
      const std::vector<std::string>& results,
      const std::vector<std::string>& names)
{
   fNofStudies = results.size();
   fQa.resize(fNofStudies);
   fCheck.resize(fNofStudies);
   fStudyResults = results;
   fStudyNames = names;

   std::string qaIdealFile = std::string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/rec_qa_ideal.json");
   read_json(qaIdealFile.c_str(), fIdeal);

   for (int i = 0; i < fNofStudies; i++) {
      read_json(results[i] + "/rec_qa.json", fQa[i]);
      read_json(results[i] + "/rec_qa_check.json", fCheck[i]);
   }

   std::ofstream foutHtml(std::string("rec_qa_study.html").c_str());
   Create(foutHtml);
}

void CbmLitQaHTMLGeneratorStudy::Create(
   std::ostream& out)
{
   out.precision(3);
   out << "<html><body>" << HtmlHeadString();

   out << "<h1>Study</h1>" << std::endl;

   out << "<h2>Number of objects</h2>";
   out << "<table id=\"efficiency\" >";
   out << PrintTableHeader();

   int row = 0;

   out << PrintRow(row++, "hEventNo", "Number of events");

//   out << PrintRow(row++, "hNofMvdPoints", "MVD points");
//   out << PrintRow(row++, "hNofMvdDigis", "MVD digis");
//   out << PrintRow(row++, "hNofMvdClusters", "MVD clusters");
//   out << PrintRow(row++, "hNofMvdHits", "MVD hits");

   out << PrintRow(row++, "hNofStsPoints", "STS points");
   out << PrintRow(row++, "hNofStsDigis", "STS digis");
   out << PrintRow(row++, "hNofStsClusters", "STS clusters");
   out << PrintRow(row++, "hNofStsHits", "STS hits");
   out << PrintRow(row++, "hNofStsTracks", "STS tracks");

   out << PrintRow(row++, "hNofRichPoints", "RICH points");
   out << PrintRow(row++, "hNofRichHits", "RICH hits");
   out << PrintRow(row++, "hNofRichProjections", "RICH projections");
   out << PrintRow(row++, "hNofRichRings", "RICH rings");

   out << PrintRow(row++, "hNofTrdPoints", "TRD points");
   out << PrintRow(row++, "hNofTrdDigis", "TRD digis");
   out << PrintRow(row++, "hNofTrdClusters", "TRD clusters");
   out << PrintRow(row++, "hNofTrdHits", "TRD hits");
   out << PrintRow(row++, "hNofTrdTracks", "TRD tracks");

//   out << PrintRow(row++, "hNofMuchPoints", "MUCH points");
//   out << PrintRow(row++, "hNofMuchDigis", "MUCH digis");
//   out << PrintRow(row++, "hNofMuchClusters", "MUCH clusters");
//   out << PrintRow(row++, "hNofMuchPixelHits", "MUCH pixel hits");
//   out << PrintRow(row++, "hNofMuchStrawHits", "MUCH straw hits");
//   out << PrintRow(row++, "hNofMuchTracks", "MUCH tracks");

   out << PrintRow(row++, "hNofTofPoints", "TOF points");
//   out << PrintRow(row++, "hNofTofDigis", "TOF digis");
//   out << PrintRow(row++, "hNofTofClusters", "TOF clusters");
   out << PrintRow(row++, "hNofTofHits", "TOF hits");

   out << PrintRow(row++, "hNofGlobalTracks", "Global tracks");

   out << "</table>" << std::endl;

   out << "<h2>Efficiencies</h2>";
   out << "<table id=\"efficiency\" >";
   out << PrintTableHeader();
   row = 0;
   out << PrintRowEff(row++, "hStsMom.all", "STS all");
   out << PrintRowEff(row++, "hStsMom.el", "STS el");
   out << PrintRowEff(row++, "hRecMom.all", "TRD all");
   out << PrintRowEff(row++, "hRecMom.el", "TRD el");
   out << PrintRowEff(row++, "hTofMom.all", "TOF all");
   out << PrintRowEff(row++, "hTofMom.el", "TOF el");

   out << PrintEmptyRow(row++, "Normalization STS+TRD(MUCH)");
   out << PrintRowEff(row++, "hStsMomNormHalfGlobal.all", "STS all");
   out << PrintRowEff(row++, "hStsMomNormHalfGlobal.el", "STS el");
   out << PrintRowEff(row++, "hHalfGlobalMom.all", "STS+TRD all");
   out << PrintRowEff(row++, "hHalfGlobalMom.el", "STS+TRD el");

   out << PrintEmptyRow(row++, "Normalization STS+TRD(MUCH)+TOF");
   out << PrintRowEff(row++, "hStsMomNormGlobal.all", "STS all");
   out << PrintRowEff(row++, "hStsMomNormGlobal.el", "STS el");
   out << PrintRowEff(row++, "hHalfGlobalMomNormGlobal.all", "STS+TRD all");
   out << PrintRowEff(row++, "hHalfGlobalMomNormGlobal.el", "STS+TRD el");
   out << PrintRowEff(row++, "hGlobalMom.all", "STS+TRD+TOF all");
   out << PrintRowEff(row++, "hGlobalMom.all", "STS+TRD+TOF el");

   out << PrintEmptyRow(row++, "Efficiency with RICH");
   out << PrintRowEff(row++, "hRichMom.richEl", "RICH el");
   out << PrintRowEff(row++, "hRichMom.richElRef", "RICH el");

   out << PrintEmptyRow(row++, "Normalization STS+RICH");
   out << PrintRowEff(row++, "hStsMomNormStsRich.richEl", "STS el");
   out << PrintRowEff(row++, "hStsRichMom.richEl", "STS+RICH el");

   out << PrintEmptyRow(row++, "Normalization STS+RICH+TRD");
   out << PrintRowEff(row++, "hStsMomNormStsRichTrd.richEl", "STS el");
   out << PrintRowEff(row++, "hStsRichMomNormStsRichTrd.richEl", "STS+RICH el");
   out << PrintRowEff(row++, "hStsRichTrdMom.richEl", "STS+RICH+TRD el");

   out << PrintEmptyRow(row++, "Normalization STS+RICH+TRD+TOF");
   out << PrintRowEff(row++, "hStsMomNormStsRichTrdTof.richEl", "STS el");
   out << PrintRowEff(row++, "hStsRichMomNormStsRichTrdTof.richEl", "STS+RICH el");
   out << PrintRowEff(row++, "hStsRichTrdMomNormStsRichTrdTof.richEl", "STS+RICH+TRD el");
   out << PrintRowEff(row++, "hStsRichTrdTofMom.richEl", "STS+RICH+TRD+TOF el");

   out << "</table>" << std::endl;

   out << "<h2>Ghosts</h2>";
   out << "<table id=\"efficiency\" >";
   out << PrintTableHeader();
   row = 0;
   out << PrintRow(row++, "fhStsGhostNh", "STS");
   out << PrintRow(row++, "fhRecGhostNh", "TRD(MUCH)");
   out << PrintRow(row++, "fhRichGhostNh", "RICH");
   out << PrintEmptyRow(row++, "after STS-RICH matching");
   out << PrintRow(row++, "fhStsGhostRichMatchingNh", "STS");
   out << PrintRow(row++, "fhRichGhostStsMatchingNh", "RICH");
   out << PrintEmptyRow(row++, "after STS-RICH matching and el identification");
   out << PrintRow(row++, "fhRichGhostElIdNh", "RICH");
   out << "</table>" << std::endl;

   out << PrintImage("STS reconstruction efficiency", "rec_qa_sts_efficiency.png");
   out << PrintImage("TRD reconstruction efficiency", "rec_qa_rec_efficiency.png");
   out << PrintImage("TOF hit matching efficiency", "rec_qa_tof_efficiency.png");
   out << PrintImage("RICH efficiency electrons", "rec_qa_rich_efficiency_electrons.png");
   out << PrintImage("Global reconstruction efficiency", "rec_qa_global_efficiency_all.png");
   out << PrintImage("Global reconstruction efficiency for electrons", "rec_qa_global_efficiency_signal.png");
   out << PrintImage("Global reconstruction efficiency with RICH", "rec_qa_sts_rich_trd_tof_efficiency_electrons.png");

   out <<  "</body></html>";
}

std::string CbmLitQaHTMLGeneratorStudy::PrintTableHeader()
{
   std::stringstream ss;
   ss << "<tr><th></th>";
   for (int i = 0; i < fNofStudies; i++) {
      ss << "<th>" << fStudyNames[i] << "</th>";
   }
   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGeneratorStudy::PrintEmptyRow(
      int row,
      const std::string& name)
{
   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td colspan=\"" << fNofStudies + 1 << "\">" << name << "</td></tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGeneratorStudy::PrintRow(
      int row,
      const std::string& property,
      const std::string& name)
{
   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << name << "</td>";
   for (int i = 0; i < fNofStudies; i++) {
      ss << PrintValue(i, "", property, "td");
   }
   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGeneratorStudy::PrintRowEff(
      int row,
      const std::string& property,
      const std::string& name)
{
   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << name << "</td>";
   for (int i = 0; i < fNofStudies; i++) {
      std::string eff = PrintValue(i, "", property + ".eff", "b");
      std::string acc = PrintValue(i, "", property + ".acc", "small");
      std::string rec = PrintValue(i, "", property + ".rec", "small");
      ss << "<td>" << eff << " ("<< rec << "/" << acc << ")" << "</td>";
   }
   ss << "</tr>" << std::endl;
   return ss.str();
}

std::string CbmLitQaHTMLGeneratorStudy::PrintImage(
      const std::string& title,
      const std::string& file)
{
   std::stringstream ss;
   int ncols = 2;
   ss << "<h2>" << title << "</h2>";
   ss << "<table id=\"efficiency\" >";
   for (int i = 0; i < fNofStudies / ncols; i++) {
      ss << "<tr>";
      for (int j = 0; j < ncols; j++) {
         int iStudy = ncols * i + j;
         if (iStudy < fNofStudies) {
            ss << "<td><h3>" << fStudyNames[iStudy] << "</h3></td>";
         } else {
            ss << "<td>-</td>";
         }
      }
      ss << "</tr>";
      ss << "<tr>";
      for (int j = 0; j < ncols; j++) {
         int iStudy = ncols * i + j;
         if (iStudy < fNofStudies) {
            ss << "<td>" << "<img src=\"" << fStudyResults[iStudy] << "/"
                  << file << "\" alt=\"" << fStudyNames[iStudy] << "\" />" << "</td>";
         } else {
            ss << "<td>-</td>";
         }
      }
      ss << "</tr>";
   }
   ss << "</table>";
   return ss.str();
}

std::string CbmLitQaHTMLGeneratorStudy::PrintValue(
      int studyId,
      const std::string& valueTitle,
      const std::string& valueName,
      const std::string& tag)
{
   // Print the value with corresponding color
   float check = fCheck[studyId].get(valueName, -1.);
   std::string color = (check == -1.) ? fWarningColor :
      (check == 0.) ? fErrorColor : fNormalColor;

   // Hint text
   std::string hint = "";
   if (check == 0. || check == 1.) {
      float min = fIdeal.get(valueName + ".min", -1.);
      float max = fIdeal.get(valueName + ".max", -1.);
      hint = "Limits (" + lit::ToString<float>(min) + ", " + lit::ToString<float>(max) + ")";
   }

   std::stringstream ss;
   ss << "<" << tag << " title=\"" << hint << "\" style=\"background-color:" << color
         << "\">" << valueTitle << fQa[studyId].get(valueName, -1.) << "</" << tag << ">";

   return ss.str();
}

std::string CbmLitQaHTMLGeneratorStudy::HtmlHeadString()
{
   std::string str = "<head><style type=\"text/css\">";
   str += "#efficiency";
   str += "{";
   str += "font-family:Verdana, Arial, Helvetica, sans-serif;";
//   str += "width:100%;";
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
