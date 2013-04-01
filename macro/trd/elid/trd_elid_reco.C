void trd_elid_reco(Int_t nEvents = 1000)
{
   Int_t trdNFoils = 70;
   Float_t trdDFoils = 0.0014;
   Float_t trdDGap = 0.04;
   Bool_t simpleTR = kTRUE;
   TString radiatorType = "H++";

   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString inFile = "/Users/slebedev/Development/cbm/data/simulations/trd/elid/piel.0000.mc.root";
	TString parFile = "/Users/slebedev/Development/cbm/data/simulations/trd/elid/piel.0000.params.root";
	TString outFile = "/Users/slebedev/Development/cbm/data/simulations/trd/elid/piel.0000.reco.root";

   TList *parFileList = new TList();
   //TObjString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v13c.digi.par"; // TRD digi file
   parFileList->Add(&trdDigiFile);

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

	//CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR, trdNFoils, trdDFoils, trdDGap);
	CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR, radiatorType);
   CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
   run->AddTask(trdHitProd);

	CbmTrdTrackFinder* trdTrackFinder = new CbmTrdTrackFinderIdeal();
	CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRD Track Finder");
	trdFindTracks->UseFinder(trdTrackFinder);
	run->AddTask(trdFindTracks);

	CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(0);
	run->AddTask(trdMatchTracks);

   CbmTrdElectronsTrainAnn* elAnn = new CbmTrdElectronsTrainAnn(10);
	run->AddTask(elAnn);

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

	run->Init();
	run->Run(0, nEvents);
}

