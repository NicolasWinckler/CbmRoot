/**
 * \file CbmLitTrackingQaStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaStudyReport.h"
#include "../report/CbmReportElement.h"
#include "../base/CbmLitPropertyTree.h"
#include "../std/utils/CbmLitUtils.h"

#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;
using std::map;
using lit::Split;
using lit::NumberToString;
using lit::FindAndReplace;

string DefaultRowNameFormatter(
		const string& name)
{
	return name;
}

string EventNoRowNameFormatter(
		const string& name)
{
	return Split(name, '_')[1];
}

string NofObjectsRowNameFormatter(
		const string& name)
{
	return Split(name, '_')[2];
}

string TrackHitsRowNameFormatter(
		const string& name)
{
	vector<string> split = Split(name, '_');
	return split[1] + ":" + split[3];
}

string NofGhostsRowNameFormatter(
		const string& name)
{
	return Split(name, '_')[2];
}

CbmLitTrackingQaStudyReport::CbmLitTrackingQaStudyReport():
		fPT()
{
}

CbmLitTrackingQaStudyReport::~CbmLitTrackingQaStudyReport()
{
}

void CbmLitTrackingQaStudyReport::Create(
      ostream& out)
{
   fPT.resize(fQa.size());
   for (Int_t i = 0; i < fQa.size(); i++) fPT[i] = new CbmLitPropertyTree(fQa[i]);

   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << PrintTable("Number of events", "hen_EventNo_TrackingQa.entries", EventNoRowNameFormatter);
   out << PrintTable("Number of objects per event", "hno_NofObjects_.+", NofObjectsRowNameFormatter);
   out << PrintTable("Number of all, true and fake hits in tracks and rings", "hth_.+_TrackHits_.*", TrackHitsRowNameFormatter);
   out << PrintTable("Number of ghosts", "hng_NofGhosts_.+", NofGhostsRowNameFormatter);
   out << PrintEfficiencyTable("Tracking efficiency with RICH", "hte_.*Rich.*_Eff_p");
   out << PrintEfficiencyTable("Tracking efficiency w/o RICH", "hte_((?!Rich).)*_Eff_p");
   out << PrintImages(".*tracking_qa_.*png");
   out <<  fR->DocumentEnd();
}

string CbmLitTrackingQaStudyReport::PrintTable(
		const string& tableName,
		const string& pattern,
		const boost::function<string (const string&)>& rowNameFormatter) const
{
   	Int_t nofStudies = fPT.size();
   	vector<map<string, Double_t> > properties(nofStudies);
   	for (Int_t i = 0; i < nofStudies; i++) {
   		properties[i] = fPT[i]->GetByPattern<Double_t>(pattern);
   	}
   	map<string, Double_t>::const_iterator it;
   	string str = fR->TableBegin(tableName, list_of(string("")).range(fStudyNames));
   	for (it = properties[0].begin(); it != properties[0].end(); it++) {
   		string cellName = rowNameFormatter(it->first);//Split(it->first, '_')[0];
   		vector<string> cells(nofStudies);
   		for (Int_t i = 0; i < nofStudies; i++) {
   			cells[i] = NumberToString<Double_t>(properties[i][it->first]);
   		}
   		str += fR->TableRow(list_of(cellName).range(cells));
   	}
   	str += fR->TableEnd();
   	return str;
}

string CbmLitTrackingQaStudyReport::PrintEfficiencyTable(
		const string& tableName,
		const string& pattern) const
{
   	Int_t nofStudies = fPT.size();
   	vector<map<string, Double_t> > properties(nofStudies);
   	for (Int_t i = 0; i < nofStudies; i++) {
   		properties[i] = fPT[i]->GetByPattern<Double_t>(pattern);
   	}
   	map<string, Double_t>::const_iterator it;
   	string str = fR->TableBegin(tableName, list_of(string("")).range(fStudyNames));
   	for (it = properties[0].begin(); it != properties[0].end(); it++) {
   		vector<string> split = Split(it->first, '_');
   		string cellName = split[1] + "(" + split[2] + "):" + split[3];
   		vector<string> cells(nofStudies);
   		for (Int_t i = 0; i < nofStudies; i++) {
   			string effName = it->first;
			string accName = FindAndReplace(effName, "_Eff_", "_Acc_") + ".entries";
			string recName = FindAndReplace(effName, "_Eff_", "_Rec_") + ".entries";
			string eff = NumberToString<Double_t>(fQa[i].get(effName, -1.));
			string acc = NumberToString<Double_t>(fQa[i].get(accName, -1.));
			string rec = NumberToString<Double_t>(fQa[i].get(recName, -1.));
			cells[i] = eff + "(" + rec + "/" + acc + ")";
   		}
   		str += fR->TableRow(list_of(cellName).range(cells));
   	}
   	str += fR->TableEnd();
   	return str;
}
