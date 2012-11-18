/** run_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 1.0
 **/
#include "../../../cbmbase/CbmDetectorList.h";

void run_analysis(Int_t nEvents = 10)
{
   Int_t iVerbose = 0;

   TString script = TString(gSystem->Getenv("SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   //gRandom->SetSeed(10);
   Double_t pionMisidLevel = -1;

   TString inFile1 = "", inFile2 = "", parFile = "", outFile ="", energy = "";
   TString plutoParticle = "";
   TString useMcMomentum = "no";

   if (script != "yes") {
      TString DIR="/lustre/cbm/user/ebelolap/aug11/25gev/70field/mvd/rho0/";
      inFile1 = DIR+"/mc.0000.root";
      parFile = DIR+"/param.0000.root";
      inFile2 = DIR+"/reco.ideal.0000.root";
      outFile = DIR+"_test.root";
      energy = "25gev";
   } else {
      inFile1 = TString(gSystem->Getenv("MCFILE"));
      inFile2 = TString(gSystem->Getenv("RECOFILE"));
      parFile = TString(gSystem->Getenv("PARFILE"));
      outFile = TString(gSystem->Getenv("DILEPANALYSISFILE"));
      pionMisidLevel = TString(gSystem->Getenv("PIONMISIDENTIFICATIONLEVEL")).Atof();
      energy = TString(gSystem->Getenv("ENERGY"));
      plutoParticle = TString(gSystem->Getenv("PLUTOPARTICLE"));
      useMcMomentum = TString(gSystem->Getenv("USEMCMOMENTUM"));
   }

   TString stsDigiFile = "sts_v11a.digi.par";
   gDebug = 0;

   // load libraries
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();
   gSystem->Load("libAnalysis");
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

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

   CbmAnaDielectronTask *task = new CbmAnaDielectronTask();
   if (energy == "10gev") {
      // weight rho0 = Multiplicity * Branching Ratio = 9 * 4.7e-5 for 10 AGeV beam energy
      if (plutoParticle == "rho0") task->SetWeight(0.000423);
      // weight omega = Multiplicity * Branching Ratio = 19 * 7.07e-5 for 10 AGeV beam energy
      if (plutoParticle == "omegaepem" ) task->SetWeight(0.0013433);
      // weight omega = Multiplicity * Branching Ratio = 19 * 5.9e-4 for 10 AGeV beam energy
      if (plutoParticle == "omegadalitz") task->SetWeight(0.01121);
      // weight phi = Multipli0city * Branching Ratio = 0.12 * 3.09e-4 for 10 AGeV beam energy
      if (plutoParticle == "phi") task->SetWeight(0.00003708);

   } else if (energy == "25gev") {
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
   }
   task->SetUseMvd(IsMvd(parFile));
   task->SetUseRich(true);
   task->SetUseTrd(IsTrd(parFile));
   task->SetUseTof(true);
   task->SetPionMisidLevel(pionMisidLevel);
   task->SetUseMcMomentum(false);
   if (useMcMomentum == "yes") task->SetUseMcMomentum(true);

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
