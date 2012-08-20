/**
 * \file CbmStsSimulationQaStudyReport.cxx
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */
#include "CbmStsSimulationQaStudyReport.h"
#include "../report/CbmReportElement.h"
#include "../std/utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
using std::endl;
using boost::assign::list_of;
using lit::ToString;

CbmStsSimulationQaStudyReport::CbmStsSimulationQaStudyReport()
{

}

CbmStsSimulationQaStudyReport::~CbmStsSimulationQaStudyReport()
{

}

void CbmStsSimulationQaStudyReport::Create(
      ostream& out)
{
   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);


   out <<  fR->DocumentEnd();
}
