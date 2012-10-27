/**
 * \file CbmAnaDielectronStudyReportAll.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmAnaDielectronStudyReportAll.h"
#include "CbmAnaLmvmNames.h"
#include "CbmReportElement.h"

#include "TSystem.h"

//#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;

CbmAnaDielectronStudyReportAll::CbmAnaDielectronStudyReportAll()
{
}

CbmAnaDielectronStudyReportAll::~CbmAnaDielectronStudyReportAll()
{
}

void CbmAnaDielectronStudyReportAll::Create(
      ostream& out)
{
   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   Bool_t fUseMvd = false;
   for (int step = kElId; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;

      out << fR->TableBegin("LMVM Results " + CbmAnaLmvmNames::fAnaSteps[step], list_of(string("")).range(fStudyNames));
      for (int iF = 0; iF < 3; iF++){
         string signalName = CbmAnaLmvmNames::fSignalNames[iF];
         out << fR->TableEmptyRow(fStudyNames.size() + 1, signalName);
         out << PrintRow(signalName + "_sbg_" + CbmAnaLmvmNames::fAnaSteps[step], "S/BG");
         out << PrintRow(signalName + "_eff_" + CbmAnaLmvmNames::fAnaSteps[step], "Efficiency [%]");
         out << PrintRow(signalName + "_signal_minv_mean_" + CbmAnaLmvmNames::fAnaSteps[step], "Mean [MeV/c2]");
         out << PrintRow(signalName + "_signal_minv_rms_" + CbmAnaLmvmNames::fAnaSteps[step], "Sigma [MeV/c2]");
      }
      out << fR->TableEmptyRow(fStudyNames.size() + 1, "S/BG ranges");
      out << PrintRow("lmvm_s_bg_region_0_0_2_" + CbmAnaLmvmNames::fAnaSteps[step], "Mee: 0.0-0.2 GeV/c2");
      out << PrintRow("lmvm_s_bg_region_0_2_0_6_" + CbmAnaLmvmNames::fAnaSteps[step], "Mee: 0.2-0.6 GeV/c2");
      out << PrintRow("lmvm_s_bg_region_0_6_1_2_" + CbmAnaLmvmNames::fAnaSteps[step], "Mee: 0.6-1.2 GeV/c2");
      out << fR->TableEnd() << endl;
   }
   out <<  fR->DocumentEnd();
}

string CbmAnaDielectronStudyReportAll::PrintRow(
      const string& property,
      const string& name)
{
   vector<string> n(fStudyNames.size(), "");
   for (int i = 0; i < fStudyNames.size(); i++) {
      n[i] = PrintValue(i, property);
   }
   return fR->TableRow(list_of(name).range(n));
}

string CbmAnaDielectronStudyReportAll::PrintValue(
      int studyId,
      const string& valueName)
{
   stringstream ss;
   ss.precision(3);
   ss << fQa[studyId].get(valueName, -1.);
   return ss.str();
}
