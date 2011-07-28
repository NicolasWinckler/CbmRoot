/** run_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 1.0
 **/

void run_analysis(Int_t nEvents = 200, Int_t cutNum = 1)
{
    Int_t iVerbose = 0;

    TString script = TString(gSystem->Getenv("SCRIPT"));
    TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

    gRandom->SetSeed(10);

    TString inFile1 = "", inFile2 = "", parFile = "", outFile ="";

    if (script != "yes") {
        TString DIR="/lustre/cbm/user/ebelolap/oct10/urqmd_omega/25gev/100_field/real";
        TString inFile1 = DIR+"/mvd.mc.0000.root";
        TString parFile = DIR+"/mvd.param.0000.root";
        TString inFile2 = DIR+"/mvd.reco.0000.root";
        TString outFile = DIR+"_test.root";
        //TString inFile1 = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.mc.root";
        //TString inFile2 = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root";
        //TString parFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.params.root";
        //TString outFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.analysis.root";
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
    gSystem->Load("libAnalysis");
    gSystem->Load("libDielectron");

    // Number of events
    Int_t coutBunch = 100;
    TStopwatch timer;
    timer.Start();

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
  //   CbmDielectronTask *task = new CbmDielectronTask("name", "title");

   CbmAnaDielectronTask *task = new CbmAnaDielectronTask("name", "title");
    // weight for rho0 = 0.001081; omega_ee = 0.0026866; omega_dalitz = 0.02242; phi = 0.00039552; pi0 = 4.38   ------ for 25 GeV

// weight rho0 = Multiplicity * Branching Ratio = 9 * 4.7e-5 for 10 AGeV beam energy
        //if (plutoParticle == "rho0") task->SetWeight(0.000423);
// weight omega = Multiplicity * Branching Ratio = 19 * 7.07e-5 for 10 AGeV beam energy
        //if (plutoParticle == "omegaepem" ) task->SetWeight(0.0013433);
// weight omega = Multiplicity * Branching Ratio = 19 * 5.9e-4 for 10 AGeV beam energy
        //if (plutoParticle == "omegadalitz") task->SetWeight(0.01121);
// weight phi = Multipli0city * Branching Ratio = 0.12 * 3.09e-4 for 10 AGeV beam energy
        //if (plutoParticle == "phi") task->SetWeight(0.00003708);


// weight rho0 = Multiplicity * Branching Ratio = 23 * 4.7e-5 for 25 AGeV beam energy
    if (plutoParticle == "rho0") task->SetWeight(0.001081);

// weight omega = Multiplicity * Branching Ratio = 38 * 7.07e-5 for 25 AGeV beam energy
    if (plutoParticle == "omegaepem" ) task->SetWeight(0.0026866);

// weight omega = Multiplicity * Branching Ratio = 38 * 5.9e-4 for 25 AGeV beam energy
    if (plutoParticle == "omegadalitz") task->SetWeight(0.02242);

// weight phi = Multiplicity * Branching Ratio = 1.28 * 3.09e-4 for 25 AGeV beam energy
    if (plutoParticle == "phi") task->SetWeight(0.00039552);

// weight pi0 = Multiplicity * Branching Ratio = 365 * 1.2e-2  for 25 AGeV beam energy
    if (plutoParticle == "pi0") task->SetWeight(4.38);


     task->SetUseRich(true);
     task->SetUseTrd(true);
     task->SetUseTof(true);
/*
     Double_t chiPr = 0.0;
     Double_t angST = 0.0;
     Double_t angTT = 0.0;

     if (cutNum == 0){
    	 chiPr = 1.5;
    	 angST = 1.5;
    	 angTT = 0.75;

     } else if (cutNum == 1){
    	 chiPr = 2;
		 angST = 1.5;
		 angTT = 0.75;

     } else if (cutNum == 2){
    	 chiPr = 3;
		 angST = 1.5;
		 angTT = 0.75;
     } else if (cutNum == 3){
    	 chiPr = 2;
		 angST = 0.7;
		 angTT = 0.75;
     } else if (cutNum == 4){
    	 chiPr = 3;
		 angST = 1.5;
		 angTT = 0.75;
     } else if (cutNum == 5){
    	 chiPr = 2;
		 angST = 2.5;
		 angTT = 0.75;
     } else if (cutNum == 6){
    	 chiPr = 2;
		 angST = 3.;
		 angTT = 0.75;
     } else if (cutNum == 7){
    	 chiPr = 3;
		 angST = 0.7;
		 angTT = 0.75;
     } else if (cutNum == 8){
    	 chiPr = 2;
		 angST = 0.7;
		 angTT = 2.;
     } else if (cutNum == 9){
    	 chiPr = 2;
		 angST = 0.7;
		 angTT = 3.5;
     } else if (cutNum == 10){

     } else if (cutNum == 11){

     }*/
     // ID cuts
/*     task->SetTrdAnnCut(0.85);
     task->SetRichAnnCut(0.0);
     task->SetUseRichAnn(true);
     // Analysis cuts
     task->SetChiPrimCut(chiPr);
     task->SetPtCut(0.2);
     task->SetAngleCut(1.);
     task->SetGammaCut(0.025);
     task->SetSTCut(angST, 1.5);// ang, pp
     task->SetTTCut(angTT, 4.);// ang, pp
*/
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

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << " Test passed" << endl;
    cout << " All ok " << endl;
    exit(0);
}
