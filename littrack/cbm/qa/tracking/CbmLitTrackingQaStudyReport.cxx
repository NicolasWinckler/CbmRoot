/**
 * \file CbmLitTrackingQaStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaStudyReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "../std/utils/CbmLitUtils.h"
#include "TH1.h"
#include <vector>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;
using std::vector;
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
		CbmStudyReport()
{
}

CbmLitTrackingQaStudyReport::~CbmLitTrackingQaStudyReport()
{
}

void CbmLitTrackingQaStudyReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   Out() << PrintTable("Number of events", "hen_EventNo_TrackingQa.entries", EventNoRowNameFormatter);
   Out() << PrintTable("Number of objects per event", "hno_NofObjects_.+", NofObjectsRowNameFormatter);
   Out() << PrintTable("Number of all, true and fake hits in tracks and rings", "hth_.+_TrackHits_.*", TrackHitsRowNameFormatter);
   Out() << PrintTable("Number of ghosts", "hng_NofGhosts_.+", NofGhostsRowNameFormatter);
   Out() << PrintEfficiencyTable("Tracking efficiency with RICH", "hte_.*Rich.*_Eff_p");
   Out() << PrintEfficiencyTable("Tracking efficiency w/o RICH", "hte_((?!Rich).)*_Eff_p");
//   Out() << PrintImages(".*tracking_qa_.*png");
   Out() <<  R()->DocumentEnd();
}

string CbmLitTrackingQaStudyReport::PrintTable(
		const string& tableName,
		const string& pattern,
		string (*rowNameFormatter)(const string&)) const
{
   	Int_t nofStudies = HM().size();
   	vector<vector<TH1*> > histos(nofStudies);
   	for (Int_t i = 0; i < nofStudies; i++) {
   		histos[i] = HM(i)->H1Vector(pattern);
	}
   	string str = R()->TableBegin(tableName, list_of(string("")).range(GetStudyNames()));
   	for (Int_t iHist = 0; iHist < histos[0].size(); iHist++) {
		string cellName = rowNameFormatter(histos[0][iHist]->GetName());
		vector<string> cells(nofStudies);
		for (Int_t i = 0; i < nofStudies; i++) {
			cells[i] = NumberToString<Double_t>(histos[i][iHist]->GetMean()) + " FIX";
		}
		str += R()->TableRow(list_of(cellName).range(cells));
	}
   	str += R()->TableEnd();
   	return str;
}

string CbmLitTrackingQaStudyReport::PrintEfficiencyTable(
		const string& tableName,
		const string& pattern) const
{
   	Int_t nofStudies = HM().size();
   	vector<vector<TH1*> > histos(nofStudies);// = fHM->H1Vector(effRegex);
   	for (Int_t i = 0; i < nofStudies; i++) {
   		histos[i] = HM(i)->H1Vector(pattern);
	}
   	string str = R()->TableBegin(tableName, list_of(string("")).range(GetStudyNames()));

   	for (Int_t iHist = 0; iHist != histos[0].size(); iHist++) {
   		vector<string> split = Split(histos[0][iHist]->GetName(), '_');
   		string cellName = split[1] + "(" + split[2] + "):" + split[3];
   		vector<string> cells(nofStudies);
   		for (Int_t i = 0; i < nofStudies; i++) {
   			Int_t nofEvents = HM(i)->H1("hen_EventNo_TrackingQa")->GetEntries();
   			string effName = histos[0][iHist]->GetName();
			string accName = FindAndReplace(effName, "_Eff_", "_Acc_");
			string recName = FindAndReplace(effName, "_Eff_", "_Rec_");
			Double_t acc = HM(i)->H1(accName)->GetEntries() / nofEvents;
			Double_t rec = HM(i)->H1(recName)->GetEntries() / nofEvents;
			Double_t eff = (acc != 0.) ? 100. * rec / acc : 0.;
			string accStr = NumberToString<Double_t>(acc);
			string recStr = NumberToString<Double_t>(rec);
			string effStr = NumberToString<Double_t>(eff);
			cells[i] = effStr + "(" + recStr + "/" + accStr + ")";
   		}
   		str += R()->TableRow(list_of(cellName).range(cells));
   	}
   	str += R()->TableEnd();
   	return str;
}

void CbmLitTrackingQaStudyReport::Draw()
{

}

ClassImp(CbmLitTrackingQaStudyReport)
