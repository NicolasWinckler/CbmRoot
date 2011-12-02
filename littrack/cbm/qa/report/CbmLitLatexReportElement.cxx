/**
 * \file CbmLitLatexReportElement.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitLatexReportElement.h"

CbmLitLatexReportElement::CbmLitLatexReportElement()
{

}

CbmLitLatexReportElement::~CbmLitLatexReportElement()
{

}

string CbmLitLatexReportElement::TableBegin(
      const string& caption,
      const vector<string>& colNames) const
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

string CbmLitLatexReportElement::TableEnd() const
{
   return "\\end{tabular} \\end{table}";
}

string CbmLitLatexReportElement::TableEmptyRow(
      int nofCols,
      const string& name) const
{
   stringstream ss;
   ss << "\\multicolumn{" << nofCols << "}{|c|}{" << name << "} \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitLatexReportElement::TableRow(
      const vector<string>& row) const
{
   string st;
   for (int i = 0; i < row.size(); i++) {
      st += (i < (row.size() - 1)) ? (row[i] + " & ") : (row[i]);
   }
   st += " \\\\ \\hline \n";
   return st;
}

string CbmLitLatexReportElement::Image(
      const string& title,
      const string& file) const
{
   stringstream ss;

   ss << "\\begin{figure}[h]" << endl;
   ss << "\\centering" << endl;
   ss << "\\includegraphics[width=7cm]{" << file << ".eps}" << endl;
   ss << "\\caption{" << title << "}" << endl;
   ss << "\\end{figure}" << endl;
   return ss.str();
}

string CbmLitLatexReportElement::DocumentBegin() const
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

string CbmLitLatexReportElement::DocumentEnd() const
{
   return "\\end{document}";
}

string CbmLitLatexReportElement::Title(
      int size,
      const string& title) const
{
   string st = "\\";
   for (int i = 0; i < size; i++) st += "sub";
   st += "title{" + title + "}";
   return st;
}
