/**
 * \file global_reco_qa.C
 * \brief Macro runs CbmLitTrackingQa class.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/

#include <iostream>
using std::cout;
using std::endl;

void global_reco_qa(Int_t nEvents = 2)
{
	TString dir = "/Users/andrey/Development/cbm/d/events/std_electrons/"; // Output directory
	TString resultDir = "./test/"; // Output directory for results
	TString mcFile = dir + "mc.0000.root"; // MC transport file
	TString parFile = dir + "param.0000.root"; // Parameter file
    TString globalHitsFile = dir + "global.hits.0000.root"; // File with reconstructed event
    TString globalTracksFile = dir + "global.tracks.0000.root"; // File with global tracks
	TString recoQaFile = dir + "reco.qa.0000.root"; // Output file with histograms

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run = new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalHitsFile);
	run->AddFriend(globalTracksFile);
	run->SetOutputFile(recoQaFile);
	// ------------------------------------------------------------------------

	CbmKF* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
	run->AddTask(l1);

	// -----   Track finding QA check   ------------------------------------
   CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
   trackingQa->SetUseConsecutivePointsInSts(true);
   trackingQa->SetMinNofPointsSts(4);
   trackingQa->SetMinNofPointsTrd(8);
   trackingQa->SetMinNofPointsMuch(10);
   trackingQa->SetMinNofPointsTof(1);
   trackingQa->SetQuota(0.7);
   trackingQa->SetMinNofHitsTrd(8);
   trackingQa->SetMinNofHitsMuch(10);
   trackingQa->SetVerbose(0);
   trackingQa->SetMinNofHitsRich(7);
   trackingQa->SetQuotaRich(0.6);
   trackingQa->SetOutputDir(std::string(resultDir));
   run->AddTask(trackingQa);
	// ------------------------------------------------------------------------

   CbmLitFitQa* fitQa = new CbmLitFitQa();
   fitQa->SetMvdMinNofHits(0);
   fitQa->SetStsMinNofHits(4);
   fitQa->SetTrdMinNofHits(8);
   fitQa->SetMuchMinNofHits(4);
   fitQa->SetOutputDir(std::string(resultDir));
   //run->AddTask(fitQa);

   CbmLitClusteringQa* clusteringQa = new CbmLitClusteringQa();
   clusteringQa->SetOutputDir(std::string(resultDir));
   //run->AddTask(clusteringQa);

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Initialize and run   --------------------------------------------
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << recoQaFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

}
