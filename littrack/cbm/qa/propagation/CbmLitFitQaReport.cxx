/**
 * \file CbmLitFitQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmLitFitQaReport.h"
#include "../report/CbmLitReportElement.h"
#include "std/utils/CbmLitUtils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>

using boost::assign::list_of;
using lit::ToString;

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
   out << PrintResAndPullRow("STS first", "hStsFirst", "sigma");
   out << PrintResAndPullRow("STS last", "hStsLast", "sigma");
   out << PrintResAndPullRow("TRD first", "hTrdFirst", "sigma");
   out << PrintResAndPullRow("TRD last", "hTrdLast", "sigma");
   out << PrintResAndPullRow("MUCH first", "hMuchFirst", "sigma");
   out << PrintResAndPullRow("MUCH last", "hMuchLast", "sigma");
   out << fR->TableEnd();

   out << fR->TableBegin("Residuals and pulls (RMS)",
         list_of("")("x")("y")("tx")("ty")("q/p")("x")("y")("tx")("ty")("q/p"));
   out << PrintResAndPullRow("STS first", "hStsFirst", "rms");
   out << PrintResAndPullRow("STS last", "hStsLast", "rms");
   out << PrintResAndPullRow("TRD first", "hTrdFirst", "rms");
   out << PrintResAndPullRow("TRD last", "hTrdLast", "rms");
   out << PrintResAndPullRow("MUCH first", "hMuchFirst", "rms");
   out << PrintResAndPullRow("MUCH last", "hMuchLast", "rms");
   out << fR->TableEnd();

   out << fR->TableBegin("Residuals and pulls (mean)",
         list_of("")("x")("y")("tx")("ty")("q/p")("x")("y")("tx")("ty")("q/p"));
   out << PrintResAndPullRow("STS first", "hStsFirst", "mean");
   out << PrintResAndPullRow("STS last", "hStsLast", "mean");
   out << PrintResAndPullRow("TRD first", "hTrdFirst", "mean");
   out << PrintResAndPullRow("TRD last", "hTrdLast", "mean");
   out << PrintResAndPullRow("MUCH first", "hMuchFirst", "mean");
   out << PrintResAndPullRow("MUCH last", "hMuchLast", "mean");
   out << fR->TableEnd();

   out << fR->Image("STS first track parameter", "fit_qa_sts_first_param");
   out << fR->Image("STS last track parameter", "fit_qa_sts_last_param");
   out << fR->Image("TRD first track parameter", "fit_qa_trd_first_param");
   out << fR->Image("TRD last track parameter", "fit_qa_trd_last_param");
   out << fR->Image("MUCH first track parameter", "fit_qa_much_first_param");
   out << fR->Image("MUCH last track parameter", "fit_qa_much_last_param");

   out <<  fR->DocumentEnd();
}

string CbmLitFitQaReport::PrintResAndPullRow(
        const string& rowName,
        const string& histName,
        const string& propertyName)
{
   string parNames[] = {
         "ResX", "ResY", "ResTx", "ResTy", "ResQp",
         "PullX", "PullY", "PullTx", "PullTy", "PullQp"
   };
   vector<string> properties(10);
   for (unsigned int iPar = 0; iPar < 10; iPar++) {
      properties[iPar] = ToString<float>(fQa.get(histName + parNames[iPar] + "." + propertyName, -1.));
   }
   return fR->TableRow(list_of(rowName).range(properties));
}
