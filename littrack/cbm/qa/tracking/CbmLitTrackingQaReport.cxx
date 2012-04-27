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


//   // Electron identification efficiency
//   vector<string> cols5 = list_of("")("efficiency")("pion supp");
//   out << fR->TableBegin("Electron identification efficiency and pion suppression", cols5);
//   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+TRD");
//   out << PrintEfficiencyElId("STS+TRD", "hStsTrd3DElId");
//   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+TRD+TOF");
//   out << PrintEfficiencyElId("STS+TRD", "hStsTrd3DElIdNormStsTrdTof");
//   out << PrintEfficiencyElId("STS+TRD+TOF", "hStsTrdTof3DElId");
//   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+RICH");
//   out << PrintEfficiencyElId("STS+RICH", "hStsRich3DElId");
//   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+RICH+TRD");
//   out << PrintEfficiencyElId("STS+RICH", "hStsRich3DElIdNormStsRichTrd");
//   out << PrintEfficiencyElId("STS+RICH+TRD", "hStsRichTrd3DElId");
//   out << fR->TableEmptyRow(cols5.size()+1, "Normalization STS+RICH+TRD+TOF");
//   out << PrintEfficiencyElId("STS+RICH", "hStsRich3DElIdNormStsRichTrdTof");
//   out << PrintEfficiencyElId("STS+RICH+TRD", "hStsRichTrd3DElIdNormStsRichTrdTof");
//   out << PrintEfficiencyElId("STS+RICH+TRD+TOF", "hStsRichTrdTof3DElId");
//   out << fR->TableEnd();
//
//   // Detector acceptance efficiency
//   out << fR->TableBegin("Detector acceptance for primary electrons", list_of("")("ACC/MC")("REC/MC"));
//   out << PrintDetAccEl("STS", "hStsDetAcc3DEl");
//   out << PrintDetAccEl("STS-RICH","hStsRichDetAcc3DEl");
//   out << PrintDetAccEl("STS-TRD", "hStsTrdDetAcc3DEl");
//   out << PrintDetAccEl("STS-TOF", "hStsTofDetAcc3DEl");
//   out << PrintDetAccEl("STS-RICH-TRD", "hStsRichTrdDetAcc3DEl");
//   out << PrintDetAccEl("STS-RICH-TRD-TOF", "hStsRichTrdTofDetAcc3DEl");
//   out << PrintDetAccEl("STS-TRD-TOF", "hStsTrdTofDetAcc3DEl");
//   out << fR->TableEnd();
//
//   // STS quality numbers
//   out << fR->TableBegin("STS quality numbers", list_of("")("Mean")("RMS"));
//   out << fR->TableRow(list_of("Chi2 to primary vertex")(PrintValue("fhStsChiprim.mean").c_str())(PrintValue("fhStsChiprim.rms").c_str()));
//   out << fR->TableRow(list_of("Momentum resolution")(PrintValue("fhStsMomresVsMom.mean").c_str())(PrintValue("fhStsMomresVsMom.rms").c_str()));
//   out << fR->TableRow(list_of("Tr. len. 100*(MC-Rec)/MC")(PrintValue("fhTrackLength.mean").c_str())(PrintValue("fhTrackLength.rms").c_str()));
//   out << fR->TableEnd();
//
//   // Tracking efficiency vs. polar angle
//   vector<string> cols7 = list_of("")("all")("reference")("primary")("secondary")("electron")("muon");
//   out << fR->TableBegin("Tracking efficiency in dependence on polar angle", cols7);
//   out << fR->TableEmptyRow(cols7.size()+1, "STS");
//   out << setfill(' ') << left << PrintPolarAngle("hStsAngle");
//   out << fR->TableEmptyRow(cols7.size()+1, "TRD(MUCH)");
//   out << setfill(' ') << left << PrintPolarAngle("hRecAngle");
//   out << fR->TableEmptyRow(cols7.size()+1, "TOF");
//   out << setfill(' ') << left << PrintPolarAngle("hTofAngle");
//   out << fR->TableEnd();
//
//   out << fR->Image("Global efficiency all", "tracking_qa_global_efficiency_all");
//   out << fR->Image("Global efficiency signal", "tracking_qa_global_efficiency_signal");
//   out << fR->Image("STS efficiency", "tracking_qa_sts_efficiency");
//   out << fR->Image("TRD(MUCH) efficiency", "tracking_qa_rec_efficiency");
//   out << fR->Image("TOF matching efficiency", "tracking_qa_tof_efficiency");
//   // TODO: Check for RICH detector
//   out << fR->Image("RICH efficiency electrons", "tracking_qa_rich_efficiency_electrons_mom");
//   out << fR->Image("STS+RICH efficiency electrons", "tracking_qa_sts_rich_efficiency_electrons");
//   out << fR->Image("STS+RICH+TRD efficiency electrons", "tracking_qa_sts_rich_trd_efficiency_electrons");
//   out << fR->Image("STS+RICH+TRD+TOF efficiency electrons", "tracking_qa_sts_rich_trd_tof_efficiency_electrons");
//   //out << fR->PrintImage("STS+RICH+TRD+TOF detector acceptance electrons", "tracking_qa_sts_rich_trd_tof_detector_acceptance");
//   out << fR->Image("STS+RICH+TRD+TOF electron identification efficiency electrons", "tracking_qa_sts_rich_trd_tof_electron_identification");
//   out << fR->Image("STS+RICH+TRD+TOF pion suppression", "tracking_qa_sts_rich_trd_tof_pion_suppression");

   out <<  fR->DocumentEnd();

   delete fPT;
}

string CbmLitTrackingQaReport::PrintNofObjects()
{
   	map<string, Double_t> properties = fPT->GetByPattern<Double_t>("hno_NofObjects_.+");
   	map<string, Double_t>::const_iterator it;
   	string str = fR->TableBegin("Number of objects per event", list_of("Name")("Value"));
   	for (it = properties.begin(); it != properties.end(); it++) {
   		str += fR->TableRow(list_of(it->first)(NumberToString<Int_t>(it->second)));
   	}
   	str += fR->TableEnd();
   	return str;
}

string CbmLitTrackingQaReport::PrintTrackHits()
{
    string str = fR->TableBegin("Number of all, true and fake hits in tracks and rings",
   	         list_of("")("all")("true")("fake")("true/all")("fake/all"));
  	map<string, Double_t> properties = fPT->GetByPattern<Double_t>("hth_.+_TrackHits_All.*");
  	map<string, Double_t>::const_iterator it;
  	for (it = properties.begin(); it != properties.end(); it++) {
  		string name = it->first;
  	    string all = NumberToString<Double_t>(it->second, 2);
  	    string trueh = NumberToString<Double_t>(fQa.get(FindAndReplace(name, "_All", "_True"), -1.), 2);
  	    string fakeh = NumberToString<Double_t>(fQa.get(FindAndReplace(name, "_All", "_Fake"), -1.), 2);
  	    string toa = NumberToString<Double_t>(fQa.get(FindAndReplace(name, "_All", "_TrueOverAll"), -1.), 2);
  	    string foa = NumberToString<Double_t>(fQa.get(FindAndReplace(name, "_All", "_FakeOverAll"), -1.), 2);
  	    str += fR->TableRow(list_of(name)(all)(trueh)(fakeh)(toa)(foa));
  	}
  	str += fR->TableEnd();
  	return str;
}

string CbmLitTrackingQaReport::PrintNofGhosts()
{
	map<string, Double_t> properties = fPT->GetByPattern<Double_t>("hng_NofGhosts_.+");
	map<string, Double_t>::const_iterator it;
	string str = fR->TableBegin("Number of ghosts per event", list_of("Name")("Value"));
	for (it = properties.begin(); it != properties.end(); it++) {
		str += fR->TableRow(list_of(it->first)(NumberToString<Int_t>(it->second)));
	}
	str += fR->TableEnd();
	return str;
}

string CbmLitTrackingQaReport::PrintTrackingEfficiency(
		Bool_t includeRich)
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

//string CbmLitTrackingQaReport::PrintEfficiencyElId(
//      const string& name,
//      const string& hist)
//{
//   string elRec = NumberToString<Double_t>(fQa.get(hist + ".el.rec", -1.));
//   string elAcc = NumberToString<Double_t>(fQa.get(hist + ".el.acc", -1.));
//   string elEff = NumberToString<Double_t>(fQa.get(hist + ".el.eff", -1.));
//   string piRec = NumberToString<Double_t>(fQa.get(hist + ".pi.rec", -1.));
//   string piAcc = NumberToString<Double_t>(fQa.get(hist + ".pi.acc", -1.));
//   string piSupp = NumberToString<Double_t>(fQa.get(hist + ".pi.supp", -1.));
//   string str1 = elEff + "(" + elRec + "/" + elAcc + ")";
//   string str2 = piSupp + "(" + piAcc + "/" + piRec + ")";
//   return fR->TableRow(list_of(name)(str1)(str2));
//}
//
//string CbmLitTrackingQaReport::PrintDetAccEl(
//      const string& name,
//      const string& hist)
//{
//   string acc = NumberToString<Double_t>(fQa.get(hist + ".detAccAcc.acc", -1.));
//   string mc = NumberToString<Double_t>(fQa.get(hist + ".detAccAcc.mc", -1.));
//   string effAcc = NumberToString<Double_t>(fQa.get(hist + ".detAccAcc.eff", -1.));
//   string rec = NumberToString<Double_t>(fQa.get(hist + ".detAccRec.rec", -1.));
//   string effRec = NumberToString<Double_t>(fQa.get(hist + ".detAccRec.eff", -1.));
//   string str1 = effAcc + "(" + acc + "/" + mc + ")";
//   string str2 = effRec + "(" + rec + "/" + mc + ")";
//   return fR->TableRow(list_of(name)(str1)(str2));
//}
//
//string CbmLitTrackingQaReport::PrintPolarAngle(
//      const string& hist)
//{
//   Double_t maxAngle = fQa.get("MaxAngle", -1.);
//   Double_t minAngle = fQa.get("MinAngle", -1.);
//   Double_t nofBinsAngle = fQa.get("NofBinsAngle", -1.);
//   Double_t step = (maxAngle - minAngle) / nofBinsAngle;
//
//   assert(maxAngle != -1. && minAngle != -1. && nofBinsAngle != -1.);
//
//   string str;
//   for (Int_t iAngle = 0; iAngle < nofBinsAngle; iAngle++) {
//      string angle0 = lit::ToString<Double_t>(iAngle * step);
//      string angle1 = lit::ToString<Double_t>(iAngle * step + step);
//      string str0 = "(" + angle0 + "-" + angle1 + ")";
//
//      string str1 = NumberToString<Double_t>(fQa.get(hist + ".all.eff." + angle0 + "_" + angle1, -1.))
//          + "(" + NumberToString<Double_t>(fQa.get(hist + ".all.rec." + angle0 + "_" + angle1, -1.))
//          + "/" + NumberToString<Double_t>(fQa.get(hist + ".all.acc." + angle0 + "_" + angle1, -1.)) + ")";
//      string str2 = NumberToString<Double_t>(fQa.get(hist + ".ref.eff." + angle0 + "_" + angle1, -1.))
//          + "(" + NumberToString<Double_t>(fQa.get(hist + ".ref.rec." + angle0 + "_" + angle1, -1.))
//          + "/" + NumberToString<Double_t>(fQa.get(hist + ".ref.acc." + angle0 + "_" + angle1, -1.)) + ")";
//      string str3 = NumberToString<Double_t>(fQa.get(hist + ".prim.eff." + angle0 + "_" + angle1, -1.))
//          + "(" + NumberToString<Double_t>(fQa.get(hist + ".prim.rec." + angle0 + "_" + angle1, -1.))
//          + "/" + NumberToString<Double_t>(fQa.get(hist + ".prim.acc." + angle0 + "_" + angle1, -1.)) + ")";
//      string str4 = NumberToString<Double_t>(fQa.get(hist + ".sec.eff." + angle0 + "_" + angle1, -1.))
//          + "(" + NumberToString<Double_t>(fQa.get(hist + ".sec.rec." + angle0 + "_" + angle1, -1.))
//          + "/" + NumberToString<Double_t>(fQa.get(hist + ".sec.acc." + angle0 + "_" + angle1, -1.)) + ")";
//      string str5 = NumberToString<Double_t>(fQa.get(hist + ".el.eff." + angle0 + "_" + angle1, -1.))
//          + "(" + NumberToString<Double_t>(fQa.get(hist + ".el.rec." + angle0 + "_" + angle1, -1.))
//          + "/" + NumberToString<Double_t>(fQa.get(hist + ".el.acc." + angle0 + "_" + angle1, -1.)) + ")";
//      string str6 = NumberToString<Double_t>(fQa.get(hist + ".mu.eff." + angle0 + "_" + angle1, -1.))
//          + "(" + NumberToString<Double_t>(fQa.get(hist + ".mu.rec." + angle0 + "_" + angle1, -1.))
//          + "/" + NumberToString<Double_t>(fQa.get(hist + ".mu.acc." + angle0 + "_" + angle1, -1.)) + ")";
//      str += fR->TableRow(list_of(str0)(str1)(str2)(str3)(str4)(str5)(str6));
//   }
//   return str;
//}
