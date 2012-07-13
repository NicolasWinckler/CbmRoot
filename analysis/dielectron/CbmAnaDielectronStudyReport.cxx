/**
 * \file CbmAnaDielectronStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmAnaDielectronStudyReport.h"
#include "CbmAnaLmvmNames.h"
#include "CbmReportElement.h"

#include "TSystem.h"

//#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;

CbmAnaDielectronStudyReport::CbmAnaDielectronStudyReport()
{
}

CbmAnaDielectronStudyReport::~CbmAnaDielectronStudyReport()
{
}

void CbmAnaDielectronStudyReport::Create(
      ostream& out)
{
   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << fR->TableBegin("LMVM Results", list_of(string("")).range(fStudyNames));
   for (int step = kReco; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      out << fR->TableEmptyRow(fStudyNames.size() + 1, CbmAnaLmvmNames::fAnaSteps[step]);
      out << PrintRow("sbg_"+CbmAnaLmvmNames::fAnaSteps[step], "S/BG");
      out << PrintRow("eff_"+CbmAnaLmvmNames::fAnaSteps[step], "Efficiency [%]");
      out << PrintRow("signal_minv_mean_"+CbmAnaLmvmNames::fAnaSteps[step], "Mean [MeV/c2]");
      out << PrintRow("signal_minv_rms_"+CbmAnaLmvmNames::fAnaSteps[step], "Sigma [MeV/c2]");
   }
   out << fR->TableEnd() << endl;

   out <<  fR->DocumentEnd();
}

string CbmAnaDielectronStudyReport::PrintRow(
      const string& property,
      const string& name)
{
   vector<string> n(fStudyNames.size(), "");
   for (int i = 0; i < fStudyNames.size(); i++) {
      n[i] = PrintValue(i, property);
   }
   return fR->TableRow(list_of(name).range(n));
}

string CbmAnaDielectronStudyReport::PrintValue(
      int studyId,
      const string& valueName)
{
   stringstream ss;
   ss.precision(3);
   ss << fQa[studyId].get(valueName, -1.);
   return ss.str();
}
