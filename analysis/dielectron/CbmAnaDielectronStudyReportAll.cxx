/**
 * \file CbmAnaDielectronStudyReportAll.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmAnaDielectronStudyReportAll.h"
#include "CbmAnaLmvmNames.h"
#include "CbmReportElement.h"

#include "TSystem.h"

#include <sstream>
//#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;
using std::stringstream;

CbmAnaDielectronStudyReportAll::CbmAnaDielectronStudyReportAll()
{
}

CbmAnaDielectronStudyReportAll::~CbmAnaDielectronStudyReportAll()
{
}

void CbmAnaDielectronStudyReportAll::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   Bool_t fUseMvd = false;
   for (int step = kElId; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;

      Out() << R()->TableBegin("LMVM Results " + CbmAnaLmvmNames::fAnaSteps[step], list_of(string("")).range(GetStudyNames()));
      for (int iF = 0; iF < 3; iF++){
         string signalName = CbmAnaLmvmNames::fSignalNames[iF];
         Out() << R()->TableEmptyRow(GetStudyNames().size() + 1, signalName);
         Out() << PrintRow(signalName + "_sbg_" + CbmAnaLmvmNames::fAnaSteps[step], "S/BG");
         Out() << PrintRow(signalName + "_eff_" + CbmAnaLmvmNames::fAnaSteps[step], "Efficiency [%]");
         Out() << PrintRow(signalName + "_signal_minv_mean_" + CbmAnaLmvmNames::fAnaSteps[step], "Mean [MeV/c2]");
         Out() << PrintRow(signalName + "_signal_minv_rms_" + CbmAnaLmvmNames::fAnaSteps[step], "Sigma [MeV/c2]");
      }
      Out() << R()->TableEmptyRow(GetStudyNames().size() + 1, "S/BG ranges");
      Out() << PrintRow("lmvm_s_bg_region_0_0_2_" + CbmAnaLmvmNames::fAnaSteps[step], "Mee: 0.0-0.2 GeV/c2");
      Out() << PrintRow("lmvm_s_bg_region_0_2_0_6_" + CbmAnaLmvmNames::fAnaSteps[step], "Mee: 0.2-0.6 GeV/c2");
      Out() << PrintRow("lmvm_s_bg_region_0_6_1_2_" + CbmAnaLmvmNames::fAnaSteps[step], "Mee: 0.6-1.2 GeV/c2");
      Out() << R()->TableEnd() << endl;
   }
   Out() <<  R()->DocumentEnd();
}

string CbmAnaDielectronStudyReportAll::PrintRow(
      const string& property,
      const string& name)
{
   vector<string> n(GetStudyNames().size(), "");
   for (int i = 0; i < GetStudyNames().size(); i++) {
      n[i] = PrintValue(i, property);
   }
   return R()->TableRow(list_of(name).range(n));
}

string CbmAnaDielectronStudyReportAll::PrintValue(
      int studyId,
      const string& valueName)
{
   stringstream ss;
   ss.precision(3);
 //  ss << fQa[studyId].get(valueName, -1.); // TODO: has to be adopted to new changes
   return ss.str();
}
