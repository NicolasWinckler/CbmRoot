/**
 * \file CbmLitTrackingQaReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaReport.h"
#include "CbmLitTrackingQaHistCreator.h"
#include "../base/CbmLitPropertyTree.h"
#include "../report/CbmLitReportElement.h"
#include "std/utils/CbmLitUtils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>

using std::vector;
using boost::assign::list_of;
using lit::NumberToString;
using lit::FindAndReplace;
using lit::Split;

CbmLitTrackingQaReport::CbmLitTrackingQaReport():
		fPT(NULL)
{
}

CbmLitTrackingQaReport::~CbmLitTrackingQaReport()
{
}

void CbmLitTrackingQaReport::Create(
   ostream& out)
{
	fPT = new CbmLitPropertyTree(fQa);

   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << "Number of events: " << PrintValue("hen_EventNo_TrackingQa.entries") << endl;

   out << PrintNofObjects();
   out << PrintTrackHits();
   out << PrintTrackingEfficiency(false);
   out << PrintTrackingEfficiency(true);
   out << PrintNofGhosts();
   out << PrintImages(".*tracking_qa_.*png");

   out <<  fR->DocumentEnd();

   delete fPT;
}

string CbmLitTrackingQaReport::PrintNofObjects() const
{
   	map<string, Double_t> properties = fPT->GetByPattern<Double_t>("hno_NofObjects_.+");
   	map<string, Double_t>::const_iterator it;
   	string str = fR->TableBegin("Number of objects per event", list_of("Name")("Value"));
   	for (it = properties.begin(); it != properties.end(); it++) {
   		string cellName = Split(it->first, '_')[2];
   		str += fR->TableRow(list_of(cellName)(NumberToString<Int_t>(it->second)));
   	}
   	str += fR->TableEnd();
   	return str;
}

string CbmLitTrackingQaReport::PrintTrackHits() const
{
    string str = fR->TableBegin("Number of all, true and fake hits in tracks and rings",
   	         list_of("")("all")("true")("fake")("true/all")("fake/all"));
  	map<string, Double_t> properties = fPT->GetByPattern<Double_t>("hth_.+_TrackHits_All.*");
  	map<string, Double_t>::const_iterator it;
  	for (it = properties.begin(); it != properties.end(); it++) {
  		string name = it->first;
  		string cellName = Split(name, '_')[1];
  	    string all = NumberToString<Double_t>(it->second, 2);
  	    string trueh = NumberToString<Double_t>(fQa.get(FindAndReplace(name, "_All", "_True"), -1.), 2);
  	    string fakeh = NumberToString<Double_t>(fQa.get(FindAndReplace(name, "_All", "_Fake"), -1.), 2);
  	    string toa = NumberToString<Double_t>(fQa.get(FindAndReplace(name, "_All", "_TrueOverAll"), -1.), 2);
  	    string foa = NumberToString<Double_t>(fQa.get(FindAndReplace(name, "_All", "_FakeOverAll"), -1.), 2);
  	    str += fR->TableRow(list_of(cellName)(all)(trueh)(fakeh)(toa)(foa));
  	}
  	str += fR->TableEnd();
  	return str;
}

string CbmLitTrackingQaReport::PrintNofGhosts() const
{
	map<string, Double_t> properties = fPT->GetByPattern<Double_t>("hng_NofGhosts_.+");
	map<string, Double_t>::const_iterator it;
	string str = fR->TableBegin("Number of ghosts per event", list_of("Name")("Value"));
	for (it = properties.begin(); it != properties.end(); it++) {
		string cellName = Split(it->first, '_')[2];
		str += fR->TableRow(list_of(cellName)(NumberToString<Int_t>(it->second)));
	}
	str += fR->TableEnd();
	return str;
}

string CbmLitTrackingQaReport::PrintTrackingEfficiency(
		Bool_t includeRich) const
{
	// If includeRich == true than search for tracking efficiency histograms which contain "Rich"
	// otherwise search for tracking efficiency histograms excluding those which contain "Rich"
	string effRegex = (includeRich) ? "hte_.*Rich.*_Eff_p" : "hte_((?!Rich).)*_Eff_p";
    map<string, Double_t> properties = fPT->GetByPattern<Double_t>(effRegex);
	if (properties.size() == 0) return "";

	const vector<string>& cat = (includeRich) ? CbmLitTrackingQaHistCreator::Instance()->GetRingCategories() : CbmLitTrackingQaHistCreator::Instance()->GetTrackCategories();
	Int_t nofCats = cat.size();
	Int_t nofRows = properties.size() / nofCats;

	// Maps category name to cell index
	map<string, Int_t> catToCell;
	for (Int_t iCat = 0; iCat < nofCats; iCat++) { catToCell.insert(make_pair<string, Int_t>(cat[iCat], iCat)); }

	string str = fR->TableBegin("Tracking efficiency", list_of(string("")).range(cat));
  	map<string, Double_t>::const_iterator it = properties.begin();
  	for (Int_t iRow = 0; iRow < nofRows; iRow++) {
  		vector<string> cells(nofCats);
  		string rowName;
  		for (Int_t iCat = 0; iCat < nofCats; iCat++) {
			string effName = it->first;
			string accName = FindAndReplace(effName, "_Eff_", "_Acc_") + ".entries";
			string recName = FindAndReplace(effName, "_Eff_", "_Rec_") + ".entries";
			string eff = NumberToString<Double_t>(fQa.get(effName, -1.));
			string acc = NumberToString<Double_t>(fQa.get(accName, -1.));
			string rec = NumberToString<Double_t>(fQa.get(recName, -1.));
			vector<string> split = Split(effName, '_');
			cells[catToCell[split[3]]] = eff + "(" + rec + "/" + acc + ")";
			it++;
			rowName = split[1] + " (" + split[2] + ")";
  		}
  		str += fR->TableRow(list_of(rowName).range(cells));
	}
  	str += fR->TableEnd();
  	return str;
}
