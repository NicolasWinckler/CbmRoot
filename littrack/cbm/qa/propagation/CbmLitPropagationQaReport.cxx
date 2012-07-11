/**
 * \file CbmLitPropagationQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmLitPropagationQaReport.h"
#include "CbmLitPropagationQaHistCreator.h"
#include "CbmReportElement.h"
#include "std/utils/CbmLitUtils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>

using std::stringstream;
using boost::assign::list_of;
using lit::NumberToString;
using lit::ToString;

CbmLitPropagationQaReport::CbmLitPropagationQaReport():
      fNofPlanes(0)
{
}

CbmLitPropagationQaReport::~CbmLitPropagationQaReport()
{
}

void CbmLitPropagationQaReport::Create(
   ostream& out)
{
   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << PrintTable("Propagator: residuals and pulls (sigma)", 0, "sigma");
   out << PrintTable("Propagator: residuals and pulls (RMS)", 0, "rms");
   out << PrintTable("Propagator: residuals and pulls (mean)", 0, "mean");
   out << PrintTable("Fitter: residuals and pulls (sigma)", 1, "sigma");
   out << PrintTable("Fitter: residuals and pulls (RMS)", 1, "rms");
   out << PrintTable("Fitter: residuals and pulls (mean)", 1, "mean");
   out << PrintTable("Smoother: residuals and pulls (sigma)", 2, "sigma");
   out << PrintTable("Smoother: residuals and pulls (RMS)", 2, "rms");
   out << PrintTable("Smoother: residuals and pulls (mean)", 2, "mean");

   out << PrintImages(".*propagation_qa_.*png");

   out <<  fR->DocumentEnd();
}

string CbmLitPropagationQaReport::PrintTable(
      const string& title,
      Int_t algorithmIndex,
      const string& propertyName)
{
   stringstream ss;
   ss << fR->TableBegin(title,
         list_of("Plane")("x")("y")("tx")("ty")("q/p")("x")("y")("tx")("ty")("q/p"));
   for (UInt_t iPlane = 0; iPlane < fNofPlanes; iPlane++) {
      ss << PrintResAndPullRow(ToString<Int_t>(iPlane), algorithmIndex, iPlane, propertyName);
   }
   ss << fR->TableEnd();
   return ss.str();
}

string CbmLitPropagationQaReport::PrintResAndPullRow(
        const string& rowName,
        Int_t algorithmIndex,
        Int_t planeIndex,
        const string& propertyName)
{
   string parNames[] = { "hResX", "hResY", "hResTx", "hResTy", "hResQp",
         "hPullX", "hPullY", "hPullTx", "hPullTy", "hPullQp" };
   string algoNames[] = { "Propagator", "Fitter", "Smoother" };
   vector<string> parameters(CbmLitPropagationQaHistCreator::NofQaParameters);
   for (UInt_t iParam = 0; iParam < CbmLitPropagationQaHistCreator::NofQaParameters; iParam++) {
      string name = parNames[iParam] + algoNames[algorithmIndex] + ToString<Int_t>(planeIndex) + "." + propertyName;
      parameters[iParam] = NumberToString<float>(fQa.get(name, -1.), 2);
   }
   return fR->TableRow(list_of(rowName).range(parameters));
}
