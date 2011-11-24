/**
 * \file CbmLitQaHtmlReportElementStudy.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaHtmlReportElementStudy.h"
#include "utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

CbmLitQaHtmlReportElementStudy::CbmLitQaHtmlReportElementStudy()
{

}

CbmLitQaHtmlReportElementStudy::~CbmLitQaHtmlReportElementStudy()
{

}

string CbmLitQaHtmlReportElementStudy::PrintTableBegin(
      const string& caption)
{
   stringstream ss;
   ss << "<table id=\"efficiency\" >" << endl;
   ss << "<tr><th></th>";
   for (int i = 0; i < fStudyNames.size(); i++) {
      ss << "<th>" << fStudyNames[i] << "</th>";
   }
   ss << "</tr>" << std::endl;
   return ss.str();
}

string CbmLitQaHtmlReportElementStudy::PrintTableEnd()
{
   return "</table>";
}

string CbmLitQaHtmlReportElementStudy::PrintSubtitle(
      const string& name)
{
   stringstream ss;
   ss << "<h2>" << name << "</h2>";
   return ss.str();
}

string CbmLitQaHtmlReportElementStudy::PrintEmptyRow(
      int row,
      const string& name)
{
   stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td colspan=\"" << fStudyNames.size() + 1 << "\">" << name << "</td></tr>" << endl;
   return ss.str();
}

string CbmLitQaHtmlReportElementStudy::PrintRow(
      int row,
      const string& property,
      const string& name)
{
   stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << name << "</td>";
   for (int i = 0; i < fStudyNames.size(); i++) {
      ss << PrintValue(i, "", property, "td");
   }
   ss << "</tr>" << std::endl;
   return ss.str();
}

string CbmLitQaHtmlReportElementStudy::PrintRowEff(
      int row,
      const string& property,
      const string& name)
{
   stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td>" << name << "</td>";
   for (int i = 0; i < fStudyNames.size(); i++) {
      string eff = PrintValue(i, "", property + ".eff", "b");
      string acc = PrintValue(i, "", property + ".acc", "small");
      string rec = PrintValue(i, "", property + ".rec", "small");
      ss << "<td>" << eff << " ("<< rec << "/" << acc << ")" << "</td>";
   }
   ss << "</tr>" << std::endl;
   return ss.str();
}

string CbmLitQaHtmlReportElementStudy::PrintImage(
      const string& title,
      const string& file)
{
   stringstream ss;
   int ncols = 2;
   ss << "<h2>" << title << "</h2>";
   ss << "<table id=\"efficiency\" >";
   int nRows = ceil((float)fStudyNames.size() / (float)ncols);
   for (int i = 0; i < nRows; i++) {
      ss << "<tr>";
      for (int j = 0; j < ncols; j++) {
         int iStudy = ncols * i + j;
         if (iStudy < fStudyNames.size()) {
            ss << "<td><h3>" << fStudyNames[iStudy] << "</h3></td>";
         } else {
            ss << "<td>-</td>";
         }
      }
      ss << "</tr>";
      ss << "<tr>";
      for (int j = 0; j < ncols; j++) {
         int iStudy = ncols * i + j;
         if (iStudy < fStudyNames.size()) {
            ss << "<td>" << "<img src=\"" << fStudyResults[iStudy] << "/"
                  << file << ".png\" alt=\"" << fStudyNames[iStudy] << "\" />" << "</td>";
         } else {
            ss << "<td>-</td>";
         }
      }
      ss << "</tr>";
   }
   ss << "</table>";
   return ss.str();
}

string CbmLitQaHtmlReportElementStudy::PrintValue(
      int studyId,
      const string& valueTitle,
      const string& valueName,
      const string& tag)
{
   // Print the value with corresponding color
   float check = fCheck[studyId].get(valueName, -1.);
   string color = (check == -1.) ? fWarningColor :
      (check == 0.) ? fErrorColor : fNormalColor;

   // Hint text
   string hint = "";
   if (check == 0. || check == 1.) {
      float min = fIdeal->get(valueName + ".min", -1.);
      float max = fIdeal->get(valueName + ".max", -1.);
      hint = "Limits (" + lit::ToString<float>(min) + ", " + lit::ToString<float>(max) + ")";
   }

   stringstream ss;
   if (fIsUseChecking) {
      ss << "<" << tag << " title=\"" << hint << "\" style=\"background-color:" << color
            << "\">" << valueTitle << fQa[studyId].get(valueName, -1.) << "</" << tag << ">";
   } else {
      ss << "<" << tag << ">" << valueTitle << fQa[studyId].get(valueName, -1.) << "</" << tag << ">";
   }
   return ss.str();
}


string CbmLitQaHtmlReportElementStudy::PrintHead()
{
   string str = "<html><body><head><style type=\"text/css\">";
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


string CbmLitQaHtmlReportElementStudy::PrintCloseDocument()
{
   return "</body></html>";
}
