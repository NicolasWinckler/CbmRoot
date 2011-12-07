/**
 * \file mvd_reco.C
 * \brief Macro runs littrack MVD tracking.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/

#include <iostream>
using std::cout;
using std::endl;

void mvd_reco(Int_t nEvents = 100)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir = "/data.local1/andrey/events/mvd_urqmd_5jpsi/"; // Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
  	TString mvdRecoFile = dir + "mvd.reco.0000.root"; // Output file with reconstructed tracks and hits

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v11a.digi.par"; // Digi scheme for STS

   TString resultDir = "./test/"; // Directory for output results

   Int_t normStsPoints = 4; // STS normalization for efficiency

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		mvdRecoFile = TString(gSystem->Getenv("MVDRECOFILE"));
		resultDir = TString(gSystem->Getenv("IMAGEDIR"));
		stsDigiFile = TString(gSystem->Getenv("STSDIGI"));
		normStsPoints = TString(gSystem->Getenv("NORMSTSPOINTS")).Atoi();
	}
   parFileList->Add(&stsDigiFile);

   Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(mvdRecoFile);

	if (IsMvd(parFile)) {
		// ----- MVD reconstruction    --------------------------------------------
		CbmMvdDigitizeL* mvdDigi = new CbmMvdDigitizeL("MVD Digitiser", 0, iVerbose);
		run->AddTask(mvdDigi);

		CbmMvdFindHits* mvdHitFinder = new CbmMvdFindHits("MVD Hit Finder", 0, iVerbose);
		run->AddTask(mvdHitFinder);
		// -------------------------------------------------------------------------
	}

	// ----- STS reconstruction   ---------------------------------------------
	FairTask* stsDigitize =	new CbmStsIdealDigitize("STSDigitize", iVerbose);
	run->AddTask(stsDigitize);

	FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
	run->AddTask(stsClusterFinder);

	FairTask* stsFindHits =	new CbmStsIdealFindHits("STSFindHits", iVerbose);
	run->AddTask(stsFindHits);

	FairTask* stsMatchHits = new CbmStsIdealMatchHits("STSMatchHits", iVerbose);
	run->AddTask(stsMatchHits);

	FairTask* kalman = new CbmKF();
	run->AddTask(kalman);
	FairTask* l1 = new CbmL1();
	run->AddTask(l1);
	CbmStsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
	//	CbmStsTrackFinder* trackFinder    = new CbmStsTrackFinderIdeal();
	FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder, false);
	run->AddTask(findTracks);

	FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
	run->AddTask(stsMatchTracks);

	CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
	FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter", trackFitter, iVerbose);
	run->AddTask(fitTracks);
   // ------------------------------------------------------------------------

	CbmLitFindMvdTracks* finder = new CbmLitFindMvdTracks();
	run->AddTask(finder);

	CbmLitFindGlobalTracks* globalFinder = new CbmLitFindGlobalTracks();
	run->AddTask(globalFinder);

   // -----   Primary vertex finding   ---------------------------------------
   CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
   CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
   run->AddTask(findVertex);
   // ------------------------------------------------------------------------

	// -----   Track finding QA check   ------------------------------------
	CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
	trackingQa->SetMinNofPointsSts(normStsPoints);
	trackingQa->SetQuota(0.7);
	trackingQa->SetVerbose(1);
	trackingQa->SetOutputDir(std::string(resultDir));
	run->AddTask(trackingQa);
	// ------------------------------------------------------------------------

	CbmLitFitQa* fitQa = new CbmLitFitQa();
   run->AddTask(fitQa);

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	parIo2->open(parFileList, "in");
	rtdb->setFirstInput(parIo1);
	rtdb->setSecondInput(parIo2);
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
	cout << "Output file is " << mvdRecoFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time "
			<< timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}

