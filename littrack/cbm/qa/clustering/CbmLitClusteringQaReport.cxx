/**
 * \file CbmLitClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaReport.h"
#include "../base/CbmLitPropertyTree.h"
#include "CbmReportElement.h"
#include "../../../std/utils/CbmLitUtils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
using std::endl;
using boost::assign::list_of;
using lit::NumberToString;
using lit::Split;

CbmLitClusteringQaReport::CbmLitClusteringQaReport()
{

}

CbmLitClusteringQaReport::~CbmLitClusteringQaReport()
{

}

void CbmLitClusteringQaReport::Create(
   ostream& out)
{
   fPT = new CbmLitPropertyTree(fQa);

   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << "Number of events: " << PrintValue("hen_EventNo_ClusteringQa.entries") << endl;

   out << PrintNofObjects();
   out << PrintImages(".*clustering_qa_.*png");
   out <<  fR->DocumentEnd();

   delete fPT;
}

string CbmLitClusteringQaReport::PrintNofObjects() const
{
   	map<string, Double_t> properties = fPT->GetByPattern<Double_t>("hno_NofObjects_.+mean");
   	map<string, Double_t>::const_iterator it;
   	string str = fR->TableBegin("Number of objects per event", list_of("Name")("Value"));
   	for (it = properties.begin(); it != properties.end(); it++) {
   		string cellName = Split(it->first, '_')[2];
   		str += fR->TableRow(list_of(cellName)(NumberToString<Int_t>(it->second)));
   	}
   	str += fR->TableEnd();
   	return str;
}
