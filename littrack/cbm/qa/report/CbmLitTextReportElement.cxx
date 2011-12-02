/**
 * \file CbmLitTextReportElement.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTextReportElement.h"
#include <sstream>
#include <iomanip>

CbmLitTextReportElement::CbmLitTextReportElement():
   fColW(17)
{
}

CbmLitTextReportElement::~CbmLitTextReportElement()
{

}

string CbmLitTextReportElement::TableBegin(
      const string& caption,
      const vector<string>& colNames) const
{
   stringstream ss;

   ss << caption << endl;
   int w = 17;
   ss << right;
   ss << setfill('_') << setw((colNames.size()+1)*w) << "_"<< endl;
   ss << setfill(' ');
   ss << setw(w) << " "; // first column is empty

   for (int i = 0; i < colNames.size(); i++){
      ss << setw(w) << colNames[i];
   }
   ss << endl;
   ss << setfill('_') << setw((colNames.size()+1)*w) << "_"<< endl;

   return ss.str();
}

string CbmLitTextReportElement::TableEnd() const
{
   stringstream ss;
   ss << endl;
   return ss.str();
}

string CbmLitTextReportElement::TableEmptyRow(
      int nofCols,
      const string& name) const
{
   stringstream ss;
   ss << setfill('-') << left;
   ss << setw(nofCols*fColW) << name << endl;
   ss << setfill(' ') << left;
   return ss.str();
}

string CbmLitTextReportElement::TableRow(
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
   ss << left << setw(fColW) << name1 << right
      << setw(fColW) << name2;
   if (name3 != "") ss << setw(fColW) << name3;
   if (name4 != "") ss << setw(fColW) << name4;
   if (name5 != "") ss << setw(fColW) << name5;
   if (name6 != "") ss << setw(fColW) << name6;
   if (name7 != "") ss << setw(fColW) << name7;
   if (name8 != "") ss << setw(fColW) << name8;

   ss << endl;
   return ss.str();
}

string CbmLitTextReportElement::Image(
      const string& title,
      const string& file) const
{
   return "";
}

string CbmLitTextReportElement::DocumentBegin() const
{
   stringstream ss;
   ss << "------------------------------------------------" << endl;
   return ss.str();
}

string CbmLitTextReportElement::DocumentEnd() const
{
   return "";
}

string CbmLitTextReportElement::Title(
      int size,
      const string& title) const
{
   return title;
}
