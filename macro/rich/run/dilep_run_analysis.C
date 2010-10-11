
void dilep_run_analysis(Int_t nEvents = 700)
{
	Int_t iVerbose = 0;

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	gRandom->SetSeed(10);

	TString inFile1 = "", inFile2 = "", parFile = "", outFile ="";

	if (script != "yes") {
		TString inFile1 = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.mc.root";
		TString inFile2 = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root";
		TString parFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.params.root";
		TString outFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.analysis.root";
	} else {
		inFile1 = TString(gSystem->Getenv("MCFILE"));
		inFile2 = TString(gSystem->Getenv("RECOFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		outFile = TString(gSystem->Getenv("DILEPANALYSISFILE"));
	}

    TString stsDigiFile = "sts_standard.digi.par";
    gDebug = 0;

    // load libraries
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
	cbmlibs();
    gSystem->Load("libDielectron");

    // Number of events
    Int_t coutBunch = 100;


    FairRunAna* fRun = new FairRunAna();
    fRun->SetName("TGeant3");
    fRun->SetInputFile(inFile1);
    fRun->SetOutputFile(outFile);
    fRun->AddFriend(inFile2);

    //CbmKF is needed for Extrapolation
     CbmKF* kf = new CbmKF();
     fRun->AddTask(kf);

	TString plutoParticle = TString(gSystem->Getenv("PLUTOPARTICLE"));

     // create and add task
     CbmRichDielectronTask *task = new CbmRichDielectronTask("name", "title");
//    // weight rho0 = Multiplicity * Branching Ratio = 9 * 4.7e-5 for 10 AGeV beam energy
//     if (plutoParticle == "rho0") task->SetWeight(0.000423);
//
//   // weight omega = Multiplicity * Branching Ratio = 19 * 7.07e-5 for 10 AGeV beam energy
//     if (plutoParticle == "omegaepem" ) task->SetWeight(0.0013433);
//
//     // weight omega = Multiplicity * Branching Ratio = 19 * 5.9e-4 for 10 AGeV beam energy
//     if (plutoParticle == "omegadalitz") task->SetWeight(0.01121);
//
//     // weight phi = Multiplicity * Branching Ratio = 0.12 * 3.09e-4 for 10 AGeV beam energy
//     if (plutoParticle == "phi") task->SetWeight(0.00003708);

     // electron ID cuts
//     task->SetRichAnnCut(-0.5);
//     task->SetUseRichAnn(true);
//     task->SetRichMom(5.5);
//     task->SetRichDist(1.);          // if 0. momentum dependent cut will be used
//     task->SetRichAxisAmean(4.95);
//     task->SetRichAxisAsigma(0.30);
//     task->SetRichAxisBmean(4.54);
//     task->SetRichAxisBsigma(0.22);
//     task->SetRichCoeff(3.5);        // +-3.5 * sigma cut
//     task->SetTrdEloss(0.05);
//     task->SetTrdAnn(0.8);
//     task->SetTofM2(0.);             // if 0. momentum dependent cut will be used
//     // di-electron analysis cuts
//     task->SetGcut(0.03);       // Gamma conversion cut (GeV/c^2)
//     task->Set2Dpcut(1.5);       // Sqrt(momentum) of closest neighbour (GeV/c)
//     task->Set2Dtcut(1.5);       // Opening angle of closest neighbour (deg)
//     task->SetPtcut(0.25);       // Transverse momentum of the identified tracks (GeV/c)
//     task->SetTcut(2.);         // Opening angle (deg)
//     task->SetPi0cut(0.2);      // pi0-Dalitz reconstruction
//     task->SetCoutBunch(coutBunch);

     task->SetUseTrd(false);
     fRun->AddTask(task);


    TString stsDigi = gSystem->Getenv("VMCWORKDIR");
    stsDigi += "/parameters/sts/";
    stsDigi += stsDigiFile;
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(parFile.Data());
    parIo2->open(stsDigi.Data(),"in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);

    fRun->Init();
    fRun->Run(0, nEvents);

    task->WriteOutput();

    cout << " Test passed" << endl;
    cout << " All ok " << endl;
    exit(0);
}
