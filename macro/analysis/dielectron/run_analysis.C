/** run_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 1.0
 **/
#include "../../../cbmbase/CbmDetectorList.h";

void run_analysis(Int_t nEvents = 10)
{
   TString script = TString(gSystem->Getenv("SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   //gRandom->SetSeed(10);

	TString dir = "/lustre/cbm/user/ebelolap/aug11/25gev/70field/mvd/rho0/";
	TString mcFile = dir + "/mc.0000.root";
	TString parFile = dir + "/param.0000.root";
	TString recoFile = dir + "/reco.0000.root";
	TString analysisFile = dir + "/ana.0000.root";
	TString energy = "25gev";

	TString plutoParticle = "omegaepem";
	TString useMcMomentum = "no";
	Double_t pionMisidLevel = -1;

	TString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file

   if (script == "yes") {
      mcFile = TString(gSystem->Getenv("MC_FILE"));
      recoFile = TString(gSystem->Getenv("RECO_FILE"));
      parFile = TString(gSystem->Getenv("PAR_FILE"));
      analysisFile = TString(gSystem->Getenv("ANALYSIS_FILE"));
      pionMisidLevel = TString(gSystem->Getenv("PION_MISIDENTIFICATION_LEVEL")).Atof();
      energy = TString(gSystem->Getenv("ENERGY"));
      plutoParticle = TString(gSystem->Getenv("PLUTO_PARTICLE"));
      useMcMomentum = TString(gSystem->Getenv("USE_MC_MOMENTUM"));
      stsDigiFile = TString(gSystem->Getenv("STS_DIGI"));
   }

   // load libraries
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();
   gSystem->Load("libAnalysis");
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

   TStopwatch timer;
   timer.Start();

   FairRunAna* fRun = new FairRunAna();
   fRun->SetName("TGeant3");
   fRun->SetInputFile(mcFile);
   fRun->SetOutputFile(analysisFile);
   fRun->AddFriend(recoFile);

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

   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(stsDigiFile.Data(), "in");
   rtdb->setFirstInput(parIo1);
   rtdb->setSecondInput(parIo2);

   fRun->Init();
   fRun->Run(0, nEvents);

   timer.Stop();
   std::cout << "Macro finished succesfully." << std::endl;
   std::cout << "Output file is " << analysisFile << std::endl;
   std::cout << "Parameter file is " << parFile << std::endl;
   std::cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << std::endl;
   std::cout << " Test passed" << std::endl;
   std::cout << " All ok " << std::endl;
}
