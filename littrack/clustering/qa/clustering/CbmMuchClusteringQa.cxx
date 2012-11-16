/**
 * \file CbmMuchClusteringQa.cxx
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmMuchClusteringQa.h"
#include "CbmMuchClusteringQaCalculator.h"
#include "CbmMuchClusteringQaHistCreator.h"
#include "CbmMuchClusteringQaPTreeCreator.h"
#include "CbmMuchClusteringQaReport.h"
#include "CbmMuchClusteringQaStudyReport.h"
#include "CbmMuchClusteringQaDraw.h"
#include "CbmHistManager.h"
#include "CbmMuchGeoScheme.h"
#include "../../../cbm/qa/base/CbmLitResultChecker.h"

#include "TSystem.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <fstream>
using std::cout;
using boost::property_tree::ptree;

CbmMuchClusteringQa::CbmMuchClusteringQa():
   fHM(NULL),
   fClusteringQa(NULL),
   fOutputDir("/home/kozlov/cbm/cbmroot_new/cbmroot/littrack/clustering/qa/clustering/res/"),
   fNofEvents()
{

}

CbmMuchClusteringQa::~CbmMuchClusteringQa()
{
   if (fHM) delete fHM;
   if (fClusteringQa) delete fClusteringQa;
}

InitStatus CbmMuchClusteringQa::Init()
{
	std::cout<<"CbmMuchClusteringQa::Init\n";
	fNofEvents = 0;
	CbmMuchGeoScheme* muchGeoScheme = CbmMuchGeoScheme::Instance();
	TString muchDigiFile = string(gSystem->Getenv("VMCWORKDIR")) + "/parameters/much/much_v11a.digi.root";//"../../../parameters/much/much_v11a.digi.root";
	muchGeoScheme->Init(muchDigiFile);
	std::cout<<"-Creating histogram manager\n";
   // Create histogram manager which is used throughout the program
   fHM = new CbmHistManager();

   std::cout<<"-Creating histograms step 1\n";
   // Create histograms
   CbmMuchClusteringQaHistCreator histCreator;
   std::cout<<"-Creating histograms step 2\n";
   histCreator.Create(fHM, muchGeoScheme);

   std::cout<<"-Creating clustering calculator\n";
   fClusteringQa = new CbmMuchClusteringQaCalculator(fHM);
   std::cout<<"-Clustering calculator created\n";
   fClusteringQa->AddMuchGeoScheme(muchGeoScheme);
   fClusteringQa->Init();
   std::cout<<"-Clustering calculator initialized\n";

   return kSUCCESS;
}

void CbmMuchClusteringQa::Exec(
    Option_t* opt)
{
	std::cout<<"CbmMuchClusteringQa::Exec EventNo = "<<fNofEvents<<"\n";
	fClusteringQa->Exec();
	std::cout<<"-Clustering calculator Exec finished\n";
	fNofEvents++;
}

void CbmMuchClusteringQa::Finish()
{
	std::cout<<"CbmMuchClusteringQa::Finish\n";
	fClusteringQa->Finish();
	std::cout<<"-Clustering calculator Finish finished\n";
   fHM->WriteToFile();
   std::cout<<"-fHM->WriteToFile()\n";

   CbmMuchClusteringQaDraw drawQa;
   std::cout<<"-CbmMuchClusteringQaDraw created\n";
   drawQa.Draw(fHM, fOutputDir);
   std::cout<<"-CbmMuchClusteringQaDraw initialized\n";

   string qaFile = fOutputDir + "clustering_qa.json";
   string idealFile = fOutputDir + "clustering_qa_ideal.json";
   string checkFile = fOutputDir + "clustering_qa_check.json";
   std::cout<<"-json checked\n";

   CbmMuchClusteringQaPTreeCreator ptCreator;
   std::cout<<"-ptCreator\n";
   ptree qa = ptCreator.Create(fHM);
   std::cout<<"-ptree initialized\n";
   if (fOutputDir != "") { write_json(qaFile.c_str(), qa); }

   CbmLitResultChecker qaChecker;
   qaChecker.DoCheck(qaFile, idealFile, checkFile);

   std::cout<<"-Creating Report\n";
   CreateSimulationReport("Clustering QA", fOutputDir);
   std::cout<<"-Report created\n";
}

void CbmMuchClusteringQa::CreateSimulationReport(
      const string& title,
      const string& resultDirectory)
{
   CbmSimulationReport* report = new CbmMuchClusteringQaReport();
   report->Create(fHM, resultDirectory);
   delete report;
}

void CbmMuchClusteringQa::CreateStudyReport(
      const string& title,
      const vector<string>& fileNames,
      const vector<string>& studyNames)
{
   CbmStudyReport* report = new CbmMuchClusteringQaStudyReport();
   report->Create(fileNames, studyNames, fOutputDir);
   delete report;
}
ClassImp(CbmMuchClusteringQa);
