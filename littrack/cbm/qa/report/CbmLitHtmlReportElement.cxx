/**
 * \file CbmLitHtmlReportElement.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitHtmlReportElement.h"

CbmLitHtmlReportElement::CbmLitHtmlReportElement()
{
}

CbmLitHtmlReportElement::~CbmLitHtmlReportElement()
{

}

string CbmLitHtmlReportElement::TableBegin(
      const string& caption,
      const vector<string>& colNames) const
{
   stringstream ss;
   ss << "<h2>" << caption << "</h2>";
   ss << "<table id=\"efficiency\" >" << endl;
   ss << "<tr><th></th>";
   for (int i = 0; i < colNames.size(); i++) {
      ss << "<th>" << colNames[i] << "</th>";
   }
   ss << "</tr>" << endl;
   return ss.str();
}

string CbmLitHtmlReportElement::TableEnd() const
{
   return "</table>";
}

string CbmLitHtmlReportElement::TableEmptyRow(
      int nofCols,
      const string& name) const
{
   stringstream ss;
   ss << "<td colspan=\"" << nofCols << "\">" << name << "</td></tr>" << endl;
   return ss.str();
}

string CbmLitHtmlReportElement::TableRow(
      const string& name1,
      const string& name2,
      const string& name3,
      const string& name4,
      const string& name5,
      const string& name6,
      const string& name7,
      const string& name8) const
{
   stringstream ss;
   ss << "<tr>";
   ss << "<td>" << name1 << "</td>";
   ss << "<td>" << name2 << "</td>";
   if (name3 != "") ss << "<td>" << name3 << "</td>";
   if (name4 != "") ss << "<td>" << name4 << "</td>";
   if (name5 != "") ss << "<td>" << name5 << "</td>";
   if (name6 != "") ss << "<td>" << name6 << "</td>";
   if (name7 != "") ss << "<td>" << name7 << "</td>";
   if (name8 != "") ss << "<td>" << name8 << "</td>";

   ss << "</tr>" << endl;
   return ss.str();
}

string CbmLitHtmlReportElement::Image(
      const string& title,
      const string& file) const
{
   stringstream ss;
   ss << "<h3>" << title << "</h3>";
   ss << "<img src=\"" << file << ".png\" alt=\"" << title << "\" />";

   return ss.str();
}

string CbmLitHtmlReportElement::DocumentBegin() const
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

string CbmLitHtmlReportElement::DocumentEnd() const
{
   return "</body></html>";
}
