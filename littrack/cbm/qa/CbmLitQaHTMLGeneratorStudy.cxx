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

}

CbmLitQaHTMLGeneratorStudy::~CbmLitQaHTMLGeneratorStudy()
{

}

void CbmLitQaHTMLGeneratorStudy::Create(
      const std::string& title,
      const std::vector<std::string>& results,
      const std::vector<std::string>& names)
{
   fTitle = title;
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

   out << "<h1>" << fTitle << "</h1>" << std::endl;

   out << "<h2>Number of objects</h2>";
   out << "<table id=\"efficiency\" >";
   out << PrintTableHeader();

   int row = 0;

   out << PrintRow(row++, "hEventNo", "Number of events");

   if (fIsMvd) {
      out << PrintRow(row++, "hNofMvdPoints", "MVD points");
      out << PrintRow(row++, "hNofMvdDigis", "MVD digis");
      out << PrintRow(row++, "hNofMvdClusters", "MVD clusters");
      out << PrintRow(row++, "hNofMvdHits", "MVD hits");
   }

   if (fIsSts) {
      out << PrintRow(row++, "hNofStsPoints", "STS points");
      out << PrintRow(row++, "hNofStsDigis", "STS digis");
      out << PrintRow(row++, "hNofStsClusters", "STS clusters");
      out << PrintRow(row++, "hNofStsHits", "STS hits");
      out << PrintRow(row++, "hNofStsTracks", "STS tracks");
   }

   if (fIsRich) {
      out << PrintRow(row++, "hNofRichPoints", "RICH points");
      out << PrintRow(row++, "hNofRichHits", "RICH hits");
      out << PrintRow(row++, "hNofRichProjections", "RICH projections");
      out << PrintRow(row++, "hNofRichRings", "RICH rings");
   }

   if (fIsTrd) {
      out << PrintRow(row++, "hNofTrdPoints", "TRD points");
      out << PrintRow(row++, "hNofTrdDigis", "TRD digis");
      out << PrintRow(row++, "hNofTrdClusters", "TRD clusters");
      out << PrintRow(row++, "hNofTrdHits", "TRD hits");
      out << PrintRow(row++, "hNofTrdTracks", "TRD tracks");
   }

   if (fIsMuch) {
      out << PrintRow(row++, "hNofMuchPoints", "MUCH points");
      out << PrintRow(row++, "hNofMuchDigis", "MUCH digis");
      out << PrintRow(row++, "hNofMuchClusters", "MUCH clusters");
      out << PrintRow(row++, "hNofMuchPixelHits", "MUCH pixel hits");
      out << PrintRow(row++, "hNofMuchStrawHits", "MUCH straw hits");
      out << PrintRow(row++, "hNofMuchTracks", "MUCH tracks");
   }

   if (fIsTof) {
      out << PrintRow(row++, "hNofTofPoints", "TOF points");
      //out << PrintRow(row++, "hNofTofDigis", "TOF digis");
      //out << PrintRow(row++, "hNofTofClusters", "TOF clusters");
      out << PrintRow(row++, "hNofTofHits", "TOF hits");
   }

   out << PrintRow(row++, "hNofGlobalTracks", "Global tracks");
   out << "</table>" << std::endl;

   std::string signal = "el";
   if (!fIsElectronSetup) signal = "mu";
   std::string recDet = (fIsTrd) ? "TRD" : (fIsMuch) ? "MUCH" :
         (fIsMuch && fIsTrd) ? "MUCH+TRD" : "";

   out << "<h2>Reconstruction efficiency</h2>";
   out << "<table id=\"efficiency\" >";
   out << PrintTableHeader();
   row = 0;
   out << PrintRowEff(row++, "hStsMom.all", "STS all");
   out << PrintRowEff(row++, "hStsMom." + signal, "STS " + signal);
   out << PrintRowEff(row++, "hRecMom.all", recDet + " all");
   out << PrintRowEff(row++, "hRecMom." + signal, recDet + " " + signal);
   out << PrintRowEff(row++, "hTofMom.all", "TOF all");
   out << PrintRowEff(row++, "hTofMom." + signal, "TOF " + signal);

   out << PrintEmptyRow(row++, "Normalization STS+" + recDet);
   out << PrintRowEff(row++, "hStsMomNormHalfGlobal.all", "STS all");
   out << PrintRowEff(row++, "hStsMomNormHalfGlobal." + signal, "STS " + signal);
   out << PrintRowEff(row++, "hHalfGlobalMom.all", "STS+" + recDet + " all");
   out << PrintRowEff(row++, "hHalfGlobalMom." + signal, "STS+" + recDet + " " + signal);

   out << PrintEmptyRow(row++, "Normalization STS+" + recDet + "+TOF");
   out << PrintRowEff(row++, "hStsMomNormGlobal.all", "STS all");
   out << PrintRowEff(row++, "hStsMomNormGlobal." + signal, "STS " + signal);
   out << PrintRowEff(row++, "hHalfGlobalMomNormGlobal.all", "STS+" + recDet + " all");
   out << PrintRowEff(row++, "hHalfGlobalMomNormGlobal." + signal, "STS+" + recDet + " " + signal);
   out << PrintRowEff(row++, "hGlobalMom.all", "STS+" + recDet + "+TOF all");
   out << PrintRowEff(row++, "hGlobalMom.all", "STS+" + recDet + "+TOF " + signal);

   if (fIsRich) {
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
   }
   out << "</table>" << std::endl;

   out << "<h2>Ghost rate</h2>";
   out << "<table id=\"efficiency\" >";
   out << PrintTableHeader();
   row = 0;
   out << PrintRow(row++, "fhStsGhostNh", "STS");
   out << PrintRow(row++, "fhRecGhostNh", recDet);
   if (fIsRich) {
      out << PrintRow(row++, "fhRichGhostNh", "RICH");
      out << PrintEmptyRow(row++, "after STS-RICH matching");
      out << PrintRow(row++, "fhStsGhostRichMatchingNh", "STS");
      out << PrintRow(row++, "fhRichGhostStsMatchingNh", "RICH");
      out << PrintEmptyRow(row++, "after STS-RICH matching and el identification");
      out << PrintRow(row++, "fhRichGhostElIdNh", "RICH");
   }
   out << "</table>" << std::endl;

   if (fIsSts) out << PrintImage("STS reconstruction efficiency", "rec_qa_sts_efficiency.png");
   if (fIsTrd || fIsMuch) out << PrintImage(recDet + " reconstruction efficiency", "rec_qa_rec_efficiency.png");
   if (fIsTof) out << PrintImage("TOF hit matching efficiency", "rec_qa_tof_efficiency.png");
   if (fIsRich) out << PrintImage("RICH efficiency electrons", "rec_qa_rich_efficiency_electrons.png");
   out << PrintImage("Global reconstruction efficiency", "rec_qa_global_efficiency_all.png");
   out << PrintImage("Global reconstruction efficiency for signal", "rec_qa_global_efficiency_signal.png");
   if (fIsRich) out << PrintImage("Global reconstruction efficiency with RICH", "rec_qa_sts_rich_trd_tof_efficiency_electrons.png");

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
   int nRows = ceil((float)fNofStudies / (float)ncols);
   for (int i = 0; i < nRows; i++) {
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
   if (fIsUseChecking) {
      ss << "<" << tag << " title=\"" << hint << "\" style=\"background-color:" << color
            << "\">" << valueTitle << fQa[studyId].get(valueName, -1.) << "</" << tag << ">";
   } else {
      ss << "<" << tag << ">" << valueTitle << fQa[studyId].get(valueName, -1.) << "</" << tag << ">";
   }
   return ss.str();
}
