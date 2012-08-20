/**
 * \file CbmStsSimulationQaReport.cxx
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */
#include "CbmStsSimulationQaReport.h"
#include "../base/CbmLitPropertyTree.h"
#include "CbmReportElement.h"
#include "../../../std/utils/CbmLitUtils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
using std::endl;
using boost::assign::list_of;
using lit::NumberToString;
using lit::Split;

CbmStsSimulationQaReport::CbmStsSimulationQaReport()
{

}

CbmStsSimulationQaReport::~CbmStsSimulationQaReport()
{

}

void CbmStsSimulationQaReport::Create(
   ostream& out)
{
   fPT = new CbmLitPropertyTree(fQa);

   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << "Number of events: " << PrintValue("hss_EventNo_StsSimulationQa.entries") << endl;

   out << PrintNofObjects();
   out << PrintImages(".*sts_simulation_qa_.*png");
   out <<  fR->DocumentEnd();

   delete fPT;
}

string CbmStsSimulationQaReport::PrintNofObjects() const
{
   	map<string, Double_t> properties = fPT->GetByPattern<Double_t>("hss_NofObjects_.+mean");
   	map<string, Double_t>::const_iterator it;
   	string str = fR->TableBegin("Number of objects per event", list_of("Name")("Value"));
   	for (it = properties.begin(); it != properties.end(); it++) {
   		string cellName = Split(it->first, '_')[2];
   		str += fR->TableRow(list_of(cellName)(NumberToString<Int_t>(it->second)));
   	}
   	str += fR->TableEnd();
   	return str;
}
