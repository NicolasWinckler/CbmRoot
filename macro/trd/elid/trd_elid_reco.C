void trd_elid_reco(Int_t nEvents = 20)
{
   Int_t trdNFoils = 70;
   Float_t trdDFoils = 0.0014;
   Float_t trdDGap = 0.04;
   Bool_t simpleTR = kTRUE;

	TString inFile = "/d/cbm06/user/slebedev/trd/piel.0000.mc.root";
	TString parFile = "/d/cbm06/user/slebedev/trd/piel.0000.params.root";
	TString outFile = "/d/cbm06/user/slebedev/trd/piel.0000.reco.root";

	gDebug = 0;

	// ----  Load libraries   -------------------------------------------------
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
	cbmlibs();

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(inFile);
	// run->AddFile(inFile2);
	run->SetOutputFile(outFile);

	CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR, trdNFoils, trdDFoils, trdDGap);

        CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
        run->AddTask(trdHitProd);

	CbmTrdTrackFinder* trdTrackFinder = new CbmTrdTrackFinderIdeal();
	CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRD Track Finder");
	trdFindTracks->UseFinder(trdTrackFinder);
	run->AddTask(trdFindTracks);

	CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(0);
	run->AddTask(trdMatchTracks);

	CbmTrdElectronsTrainAnn* elAnn = new CbmTrdElectronsTrainAnn();
	run->AddTask(elAnn);


	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();

	run->Init();
	run->Run(0, nEvents);
}

