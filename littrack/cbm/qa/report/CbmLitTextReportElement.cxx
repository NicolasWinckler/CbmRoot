/**
 * \file CbmLitTextReportElement.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTextReportElement.h"
#include <sstream>
#include <iomanip>

CbmLitTextReportElement::CbmLitTextReportElement():
   fColW(25)
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
   ss << right;
   ss << setfill('_') << setw(colNames.size() * fColW) << "_"<< endl;
   ss << setfill(' ');
   for (Int_t i = 0; i < colNames.size(); i++) {
      ss << FormatCell(colNames[i]); //setw(fColW) << colNames[i];
   }
   ss << endl;
   ss << setfill('_') << setw(colNames.size() * fColW) << "_"<< endl;
   return ss.str();
}

string CbmLitTextReportElement::TableEnd() const
{
   stringstream ss;
   ss << endl;
   return ss.str();
}

string CbmLitTextReportElement::TableEmptyRow(
      Int_t nofCols,
      const string& name) const
{
   stringstream ss;
   ss << setfill('-') << left;
   ss << setw(nofCols * fColW) << name << endl;
   ss << setfill(' ') << left;
   return ss.str();
}

string CbmLitTextReportElement::TableRow(
      const vector<string>& row) const
{
   stringstream ss;
   ss << right;
   for (Int_t i = 0; i < row.size(); i++) {
      ss << FormatCell(row[i]); //setw(fColW) << row[i];
   }
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
      Int_t size,
      const string& title) const
{
   return title;
}

string CbmLitTextReportElement::FormatCell(
      const string& cell) const
{
	if (cell.size() <= fColW) {
		stringstream ss;
		ss << setw(fColW) << cell;
		return ss.str();
	} else {
		string str = cell;
		str.resize(fColW - 3);
		str.insert(fColW - 3, "...");
		return str;
	}
}
