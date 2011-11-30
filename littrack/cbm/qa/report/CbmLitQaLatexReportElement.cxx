/**
 * \file CbmLitQaLatexReportElement.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaLatexReportElement.h"
#include "utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

CbmLitQaLatexReportElement::CbmLitQaLatexReportElement()
{

}

CbmLitQaLatexReportElement::~CbmLitQaLatexReportElement()
{

}

string CbmLitQaLatexReportElement::PrintTableBegin(
      const string& caption,
      const vector<string>& colNames)
{
   stringstream ss;
   ss << "\\begin{table}[h]" << endl;
   ss << "\\centering";
   if (caption != "") ss << "\\caption{" << caption << "}" << endl;
   ss << "\\begin{tabular}{|";
   for (int i = 0; i < colNames.size() + 1; i++){
      ss << "c|";
   }
   ss << "} \\hline" << endl;
   // names start from the second column
   for (int i = 0; i < colNames.size(); i++) {
      ss << "& " << colNames[i];
   }
   ss << "\\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintTableEnd()
{
   return "\\end{tabular} \\end{table}";
}

string CbmLitQaLatexReportElement::PrintEmptyRow(
      int nofCols,
      const string& name)
{
   stringstream ss;
   ss << "\\multicolumn{" << nofCols << "}{|c|}{" << name << "} \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintRow(
      const string& name1,
      const string& name2,
      const string& name3,
      const string& name4,
      const string& name5,
      const string& name6,
      const string& name7,
      const string& name8)
{
   stringstream ss;
   ss << name1 << " & " << name2;
   if (name3 != "") ss << " & " << name3;
   if (name4 != "") ss << " & " << name4;
   if (name5 != "") ss << " & " << name5;
   if (name6 != "") ss << " & " << name6;
   if (name7 != "") ss << " & " << name7;
   if (name8 != "") ss << " & " << name8;
   ss << " \\\\ \\hline";
   ss << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintImage(
      const string& title,
      const string& file)
{
   stringstream ss;

   ss << "\\begin{figure}[h]" << endl;
   ss << "\\centering" << endl;
   ss << "\\includegraphics[width=7cm]{" << file << ".eps}" << endl;
   ss << "\\caption{" << title << "}" << endl;
   ss << "\\end{figure}" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintHead()
{
   string str = "\\documentclass[a4paper,14pt]{article}";
   str += "\\usepackage{amssymb}";
   str += "\\setlength\\oddsidemargin{-2cm}";
   str += "\\setlength\\evensidemargin{-2cm}";
   str += "\\setlength\\textwidth{17cm}";
   str += "\\setlength\\topmargin{0cm}";
   str += "\\usepackage[dvips]{graphicx}";
   str += "\\begin{document}";

   return str;
}


string CbmLitQaLatexReportElement::PrintCloseDocument()
{
   return "\\end{document}";
}
