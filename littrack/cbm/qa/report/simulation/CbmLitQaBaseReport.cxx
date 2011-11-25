/**
 * \file CbmLitQaBaseReport.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaBaseReport.h"
#include "../std/utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

CbmLitQaBaseReport::CbmLitQaBaseReport():
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

CbmLitQaBaseReport::~CbmLitQaBaseReport()
{

}

void CbmLitQaBaseReport::SetDetectorPresence(
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

void CbmLitQaBaseReport::CreateReport(
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


   fR->SetPtreeQa(qa);
  // fR->SetPtreeIdeal(&ideal);
  // fR->SetPtreeCheck(&check);
  // fR->SetOutputDir(outputDir);

   fR->SetErrorColor("#FF0000");
   fR->SetWarningColor("#FFFF00");
   fR->SetNormalColor("#00FF00");
   fR->SetIsUseChecking(true);

   Create(out);
}
