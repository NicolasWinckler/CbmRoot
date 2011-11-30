/**
 * \file CbmLitSimulationReport.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitSimulationReport.h"
#include "../std/utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

CbmLitSimulationReport::CbmLitSimulationReport():
   fR(NULL),
   fIsElectronSetup(true),
   fIsMvd(true),
   fIsSts(true),
   fIsRich(true),
   fIsTrd(true),
   fIsMuch(true),
   fIsTof(true),
   fTitle("")
{

}

CbmLitSimulationReport::~CbmLitSimulationReport()
{

}

void CbmLitSimulationReport::SetDetectorPresence(
         DetectorId detId,
         bool isDet)
{
   switch(detId) {
      case kMVD: fIsMvd = isDet; break;
      case kSTS: fIsSts = isDet; break;
      case kRICH: fIsRich = isDet; break;
      case kTRD: fIsTrd = isDet; break;
      case kMUCH: fIsMuch = isDet; break;
      case kTOF: fIsTof = isDet; break;
      default: break;
   }
}

void CbmLitSimulationReport::CreateReport(
      ostream& out,
      boost::property_tree::ptree* qa)
{
   //fTitle = title;
  // boost::property_tree::ptree qa;
   //boost::property_tree::ptree check;

   //boost::property_tree::ptree ideal;
   //string qaIdealFile = string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/rec_qa_ideal.json");
   //read_json(qaIdealFile.c_str(), ideal);

   //read_json(outputDir + "/rec_qa.json", qa);
   //read_json(outputDir + "/rec_qa_check.json", check);


   SetPtreeQa(qa);
  // SetPtreeIdeal(&ideal);
  // SetPtreeCheck(&check);
  // SetOutputDir(outputDir);

   SetErrorColor("#FF0000");
   SetWarningColor("#FFFF00");
   SetNormalColor("#00FF00");
   SetIsUseChecking(true);

   Create(out);
}
