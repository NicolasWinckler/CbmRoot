/**
 * \file CbmLitQaLatexReportElementStudy.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaLatexReportElementStudy.h"
#include "utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

CbmLitQaLatexReportElementStudy::CbmLitQaLatexReportElementStudy()
{

}

CbmLitQaLatexReportElementStudy::~CbmLitQaLatexReportElementStudy()
{

}

string CbmLitQaLatexReportElementStudy::PrintTableBegin(
      const string& caption)
{
   stringstream ss;
   ss << "\\begin{table}[h]" << endl;
   ss << "\\centering";
   if (caption != "") ss << "\\caption{" << caption << "}" << endl;
   ss << "\\begin{tabular}{|";
   for (int i = 0; i < fStudyNames.size() + 1; i++){
      ss << "c|";
   }
   ss << "} \\hline" << endl;
   // names start from the second column
   for (int i = 0; i < fStudyNames.size(); i++) {
      ss << "& $" << fStudyNames[i] << "$";
   }
   ss << "\\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElementStudy::PrintTableEnd()
{
   return "\\end{tabular} \\end{table}";
}

string CbmLitQaLatexReportElementStudy::PrintSubtitle(
      const string& name)
{
   if (name == "") return "";
   stringstream ss;
   ss << "\\section{" << name << "}";
   return ss.str();
}

string CbmLitQaLatexReportElementStudy::PrintEmptyRow(
      int row,
      const string& name)
{
   stringstream ss;
   ss << "\\multicolumn{" << fStudyNames.size() + 1 << "}{|c|}{" << name << "} \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElementStudy::PrintRow(
      int row,
      const string& property,
      const string& name)
{
   stringstream ss;
   ss << name ;
   for (int i = 0; i < fStudyNames.size(); i++) {
      ss << " & " <<  PrintValue(i, property);
   }
   ss << " \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElementStudy::PrintRowEff(
      int row,
      const string& property,
      const string& name)
{
   stringstream ss;
   ss << name ;
   for (int i = 0; i < fStudyNames.size(); i++) {
      string eff = PrintValue(i, property + ".eff");
      string acc = PrintValue(i, property + ".acc");
      string rec = PrintValue(i, property + ".rec");
      ss << " & " << eff << " ("<< rec << "/" << acc << ")";
   }
   ss << " \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElementStudy::PrintImage(
      const string& title,
      const string& file)
{
   stringstream ss;

   ss << "\\begin{figure}[h]" << endl;
   ss << "\\centering" << endl;
   double scale = 0.98/(double)fStudyNames.size();
   for (int iStudy = 0; iStudy < fStudyNames.size(); iStudy ++){
      string fileName = "" + fStudyResults[iStudy] + "/" + file;
      ss << "\\includegraphics[width=" << scale << "\\textwidth]{" << fileName << ".eps}" << endl;
   }
   ss << "\\caption{" << title << "}" << endl;
   ss << "\\end{figure}" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElementStudy::PrintValue(
      int studyId,
      const string& valueName)
{
   stringstream ss;
   ss << fQa[studyId].get(valueName, -1.);

   return ss.str();
}


string CbmLitQaLatexReportElementStudy::PrintHead()
{
   string str = "\\documentclass[a4paper,14pt]{article}";
   str += "\\usepackage[dvips]{graphicx}";
   str += "\\begin{document}";

   return str;
}


string CbmLitQaLatexReportElementStudy::PrintCloseDocument()
{
   return "\\end{document}";
}
