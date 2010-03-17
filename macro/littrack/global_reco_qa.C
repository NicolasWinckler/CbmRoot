/** global_reco_qa.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 * Macro draws histograms produced by CbmLitReconstructionQa class.
 **/

void global_reco_qa(Int_t nEvents = 1000)
{
	// Output directory
	TString dir  = "/d/cbm02/andrey/muchcomp10_10mu_urqmd/";
	// MC transport file
	TString mcFile = dir + "mc.0000.root";
	// Parameter file
	TString parFile = dir + "param.0000.root";
	// File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
	TString globalHitsFile = dir + "global.hits.0000.root";
	// File with global tracks
	TString globalTracksFile = dir + "global.tracks.0000.root";
	// Output file with histograms
	TString recoQaFile = dir + "reco.qa.0000.root";

	TStopwatch timer;
	timer.Start();

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
	reconstructionQa->SetMinNofPointsTrd(10);
	reconstructionQa->SetMinNofPointsMuch(9);
	reconstructionQa->SetMinNofPointsTof(1);
	reconstructionQa->SetQuota(0.7);
	reconstructionQa->SetMinNofHitsTrd(0);
	reconstructionQa->SetMinNofHitsMuch(8);
	reconstructionQa->SetVerbose(1);
	reconstructionQa->SetOutputDir("./test/muchcomp10_10mu_urqmd/");
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
