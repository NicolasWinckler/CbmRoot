/** global_reco_qa.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 * Macro draws histograms produced by CbmLitReconstructionQa class.
 **/

void global_reco_qa(Int_t nEvents = 1000)
{
	// Output directory
	TString dir  = "/d/cbm06/user/andrey/much1_10mu_urqmd/";
	// MC transport file
	TString mcFile = dir + "mc.0000.root";
	// Parameter file
	TString parFile = dir + "param.0000.root";
	// File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
	TString globalHitsFile = dir + "global.hits.0000.root";
	// File with global tracks
	TString globalTracksFile = dir + "global.tracks.branch.0000.root";
	// Output file with histograms
	TString recoQaFile = dir + "reco.qa.branch.0000.root";
	// Output directory for images
	TString imageDir = "./test/much1/branch/";

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();


	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalHitsFile);
	run->AddFriend(globalTracksFile);
	run->SetOutputFile(recoQaFile);
	// ------------------------------------------------------------------------

	// -----   Track finding QA check   ------------------------------------
	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
	reconstructionQa->SetMinNofPointsSts(4);
	reconstructionQa->SetMinNofPointsTrd(9);
	reconstructionQa->SetMinNofPointsMuch(11);
	reconstructionQa->SetMinNofPointsTof(1);
	reconstructionQa->SetQuota(0.7);
	reconstructionQa->SetMinNofHitsTrd(0);
	reconstructionQa->SetMinNofHitsMuch(11);
	reconstructionQa->SetVerbose(1);
	reconstructionQa->SetMomentumRange(0., 25);
	reconstructionQa->SetNofBinsMom(25);
	reconstructionQa->SetOutputDir(std::string(imageDir));
	run->AddTask(reconstructionQa);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Initialize and run   --------------------------------------------
	run->LoadGeometry();
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
