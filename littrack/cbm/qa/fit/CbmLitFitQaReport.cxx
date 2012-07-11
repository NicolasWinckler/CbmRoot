/**
 * \file CbmLitFitQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmLitFitQaReport.h"
#include "CbmReportElement.h"
#include "std/utils/CbmLitUtils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>

using boost::assign::list_of;
using lit::NumberToString;

CbmLitFitQaReport::CbmLitFitQaReport()
{
}

CbmLitFitQaReport::~CbmLitFitQaReport()
{
}

void CbmLitFitQaReport::Create(
   ostream& out)
{
   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << fR->TableBegin("Residuals and pulls (standard deviation)",
         list_of("")("x")("y")("tx")("ty")("q/p")("x")("y")("tx")("ty")("q/p"));
   out << PrintResAndPullRow("STS first", "htf_Sts_FirstParam_", "sigma");
   out << PrintResAndPullRow("STS last", "htf_Sts_LastParam_", "sigma");
   out << PrintResAndPullRow("TRD first", "htf_Trd_FirstParam_", "sigma");
   out << PrintResAndPullRow("TRD last", "htf_Trd_LastParam_", "sigma");
   out << PrintResAndPullRow("MUCH first", "htf_Much_FirstParam_", "sigma");
   out << PrintResAndPullRow("MUCH last", "htf_Much_LastParam", "sigma");
   out << fR->TableEnd();

   out << fR->TableBegin("Residuals and pulls (RMS)",
         list_of("")("x")("y")("tx")("ty")("q/p")("x")("y")("tx")("ty")("q/p"));
   out << PrintResAndPullRow("STS first", "htf_Sts_FirstParam_", "rms");
   out << PrintResAndPullRow("STS last", "htf_Sts_LastParam_", "rms");
   out << PrintResAndPullRow("TRD first", "htf_Trd_FirstParam_", "rms");
   out << PrintResAndPullRow("TRD last", "htf_Trd_LastParam_", "rms");
   out << PrintResAndPullRow("MUCH first", "htf_Much_FirstParam_", "rms");
   out << PrintResAndPullRow("MUCH last", "htf_Much_LastParam_", "rms");
   out << fR->TableEnd();

   out << fR->TableBegin("Residuals and pulls (mean)",
         list_of("")("x")("y")("tx")("ty")("q/p")("x")("y")("tx")("ty")("q/p"));
   out << PrintResAndPullRow("STS first", "htf_Sts_FirstParam_", "mean");
   out << PrintResAndPullRow("STS last", "htf_Sts_LastParam_", "mean");
   out << PrintResAndPullRow("TRD first", "htf_Trd_FirstParam_", "mean");
   out << PrintResAndPullRow("TRD last", "htf_Trd_LastParam_", "mean");
   out << PrintResAndPullRow("MUCH first", "htf_Much_FirstParam_", "mean");
   out << PrintResAndPullRow("MUCH last", "htf_Much_LastParam_", "mean");
   out << fR->TableEnd();

   out << PrintImages(".*fit_qa_.*png");

   out <<  fR->DocumentEnd();
}

string CbmLitFitQaReport::PrintResAndPullRow(
        const string& rowName,
        const string& histName,
        const string& propertyName)
{
   string parameterNames[] = {
		   "Res_X", "Res_Y", "Res_Tx", "Res_Ty", "Res_Qp",
           "Pull_X", "Pull_Y", "Pull_Tx", "Pull_Ty", "Pull_Qp"
   };
   vector<string> properties(10);
   for (UInt_t iPar = 0; iPar < 10; iPar++) {
      string name = histName + parameterNames[iPar] + "." + propertyName;
      properties[iPar] = NumberToString<float>(fQa.get(name, -1.), 2);
   }
   return fR->TableRow(list_of(rowName).range(properties));
}
