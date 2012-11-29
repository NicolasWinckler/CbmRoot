/**
 * \file qa_simulation_report.C
 * \brief Macro for generation of reports from file with histograms.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */

void qa_simulation_report()
{
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   std::string outputDir = "$VMCWORKDIR/macro/littrack/test/";
   std::string fileName = "$VMCWORKDIR/macro/littrack/commit_tests/events_electron/global.reco.0000.root";

   CbmSimulationReport* trackingQaReport = new CbmLitTrackingQaReport();
   trackingQaReport->Create(fileName, outputDir);

   CbmSimulationReport* fitQaReport = new CbmLitFitQaReport();
   fitQaReport->Create(fileName, outputDir);

   CbmSimulationReport* clusteringQaReport = new CbmLitClusteringQaReport();
   clusteringQaReport->Create(fileName, outputDir);
}
