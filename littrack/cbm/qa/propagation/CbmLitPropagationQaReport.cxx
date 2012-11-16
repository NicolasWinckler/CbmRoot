/**
 * \file CbmLitPropagationQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmLitPropagationQaReport.h"
#include "CbmLitPropagationQaHistCreator.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "TH1.h"
#include "TF1.h"
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

void CbmLitPropagationQaReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   Out() << PrintTable("Propagator: residuals and pulls (sigma)", 0, "sigma");
   Out() << PrintTable("Propagator: residuals and pulls (RMS)", 0, "rms");
   Out() << PrintTable("Propagator: residuals and pulls (mean)", 0, "mean");
   Out() << PrintTable("Fitter: residuals and pulls (sigma)", 1, "sigma");
   Out() << PrintTable("Fitter: residuals and pulls (RMS)", 1, "rms");
   Out() << PrintTable("Fitter: residuals and pulls (mean)", 1, "mean");
   Out() << PrintTable("Smoother: residuals and pulls (sigma)", 2, "sigma");
   Out() << PrintTable("Smoother: residuals and pulls (RMS)", 2, "rms");
   Out() << PrintTable("Smoother: residuals and pulls (mean)", 2, "mean");

//   Out() << PrintImages(".*propagation_qa_.*png");

   Out() <<  R()->DocumentEnd();
}

string CbmLitPropagationQaReport::PrintTable(
      const string& title,
      Int_t algorithmIndex,
      const string& propertyName)
{
   stringstream ss;
   ss << R()->TableBegin(title,
         list_of("Plane")("x")("y")("tx")("ty")("q/p")("x")("y")("tx")("ty")("q/p"));
   for (UInt_t iPlane = 0; iPlane < fNofPlanes; iPlane++) {
      ss << PrintResAndPullRow(ToString<Int_t>(iPlane), algorithmIndex, iPlane, propertyName);
   }
   ss << R()->TableEnd();
   return ss.str();
}

string CbmLitPropagationQaReport::PrintResAndPullRow(
        const string& rowName,
        Int_t algorithmIndex,
        Int_t planeIndex,
        const string& propertyName)
{
   string parNames[] = { "htp_Res_X", "htp_Res_Y", "htp_Res_Tx", "htp_Res_Ty", "htp_Res_Qp",
	         "htp_Pull_X", "htp_Pull_Y", "htp_Pull_Tx", "htp_Pull_Ty", "htp_Pull_Qp" };
   string algoNames[] = { "Propagator", "Fitter", "Smoother" };
   vector<string> parameters(CbmLitPropagationQaHistCreator::NofQaParameters);
   for (UInt_t iParam = 0; iParam < CbmLitPropagationQaHistCreator::NofQaParameters; iParam++) {
      string name = parNames[iParam] + algoNames[algorithmIndex] + ToString<Int_t>(planeIndex) + "." + propertyName;
      if (propertyName == "mean") {
    	  parameters[iParam] = NumberToString<Float_t>(HM()->H1(name)->GetMean(), 2);
      } else if (propertyName == "rms") {
    	  parameters[iParam] = NumberToString<Float_t>(HM()->H1(name)->GetRMS(), 2);
      } else if (propertyName == "sigma") {
    	  TH1* hist = HM()->H1(name);
    	  hist->Fit("gaus");
    	  TF1* fit = hist->GetFunction("gaus");
    	  parameters[iParam] = (NULL != fit) ? fit->GetParameter(2) : 0.;
      }
   }
   return R()->TableRow(list_of(rowName).range(parameters));
}

void CbmLitPropagationQaReport::Draw()
{

}
