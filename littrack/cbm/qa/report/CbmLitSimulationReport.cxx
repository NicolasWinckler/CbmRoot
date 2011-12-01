/**
 * \file CbmLitSimulationReport.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitSimulationReport.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

CbmLitSimulationReport::CbmLitSimulationReport(LitReportType reportType):
   CbmLitReport(reportType)
{

}

CbmLitSimulationReport::~CbmLitSimulationReport()
{

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
