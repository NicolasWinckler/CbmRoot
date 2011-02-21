/** prop_ana.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 *
 * Macro runs propagation analysis task.
 **/

void prop_ana(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

//	Double_t trdHitErr = 100; // if == 0 than standard errors are used
	TString dir, imageDir, mcFile, parFile, globalTracksFile, propAnaFile;
	Int_t nofTrdHits, nofMuchHits, nofTofHits, pdg;
	Int_t testFastPropagation;
	if (script != "yes") {
		dir = "/d/cbm02/andrey/test_electrons_fit_norich/";
		mcFile = dir + "mc.0000.root";
		globalTracksFile = dir + "global.tracks.ideal.0000.root";
		parFile = dir + "param.0000.root";
		propAnaFile = dir + "propagation.ana.0000.root";
		imageDir = "./test_electrons_fit/";
		nofTrdHits = 12;
		nofMuchHits = 0;
		nofTofHits = 0;
		pdg = 11;
		testFastPropagation = 1;
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSIDEALFILE"));
		imageDir = TString(gSystem->Getenv("IMAGEDIR"));
		propAnaFile = TString(gSystem->Getenv("PROPANAFILE"));
		nofTrdHits = TString(gSystem->Getenv("NOFTRDHITS")).Atoi();
		nofMuchHits = TString(gSystem->Getenv("NOFMUCHHITS")).Atoi();
		nofTofHits = TString(gSystem->Getenv("NOFTOFHITS")).Atoi();
		pdg = TString(gSystem->Getenv("PDG")).Atoi();
		testFastPropagation = TString(gSystem->Getenv("TESTFASTPROPAGATION")).Atoi();
	}

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalTracksFile);
	run->SetOutputFile(propAnaFile);

//	FairGeane* Geane = new FairGeane(inFile.Data());
//	FairGeane* Geane = new FairGeane(parFile.Data());
//	FairGeane *Geane = new FairGeane();
//	run->AddTask(Geane);

	// -------------------------------------------------------------------------
	CbmLitPropagationAnalysis* propAna = new CbmLitPropagationAnalysis();
	propAna->SetNofPlanes(nofTrdHits + nofMuchHits + nofTofHits);
	propAna->SetNofTrdHits(nofTrdHits);
	propAna->SetNofMuchHits(nofMuchHits);
	propAna->SetNofTofHits(nofTofHits);
	propAna->SetPDGCode(pdg);
	propAna->SetTestFastPropagation(testFastPropagation);
	propAna->SetOutputDir(std::string(imageDir));
	propAna->IsDrawPropagation(true);
	propAna->IsDrawFilter(true);
	propAna->IsDrawSmoother(false);
	propAna->IsCloseCanvas(false);
	propAna->IsFixedBounds(true);
	propAna->SetPlaneNoPhd(nofTrdHits + nofMuchHits - 1);
	run->AddTask(propAna);
	// -------------------------------------------------------------------------
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
	TString stsDigiFile = parDir+ "/sts/sts_standard.digi.par";
	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	parIo2->open(stsDigiFile.Data(),"in");
	rtdb->setFirstInput(parIo1);
	rtdb->setSecondInput(parIo2);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Intialise and run   --------------------------------------------
	run->LoadGeometry();
	run->Init();
//	Geane->SetField(run->GetField());
	run->Run(0,nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << propAnaFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
