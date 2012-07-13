
void run_reco_geotest(Int_t nEvents = 10)
{
   TTree::SetMaxTreeSize(90000000000);
   TString script = TString(gSystem->Getenv("SCRIPT"));
   gRandom->SetSeed(10);

   TString mcFile = "", parFile = "", recFile = "", outFile ="";
   std::string resultDir = "";
   std::string richDetectorType = ""; // "standard" or "prototype"

   TString outDir = "/d/cbm06/user/slebedev/geotest/field/";
   mcFile = outDir + "mc.0004.root";
   parFile = outDir + "params.0004.root";
   recFile = outDir + "reco.0004.root";
   outFile = outDir + "test.geotest.0004.root";
   resultDir = "results/";
   richDetectorType = "standard";

   gDebug = 0;
   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   FairRunAna *run= new FairRunAna();
   run->SetInputFile(mcFile);
   run->SetOutputFile(outFile);
   run->AddFriend(recFile);

   CbmRichGeoTest* geoTest = new CbmRichGeoTest();
   geoTest->SetOutputDir(resultDir);
   geoTest->SetRichDetectorType(richDetectorType);
   run->AddTask(geoTest);

   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   parIo1->open(parFile.Data());
   rtdb->setFirstInput(parIo1);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();

   run->Init();
   cout << "Starting run" << endl;
   run->Run(0,nEvents);

   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished successfully." << endl;
   cout << "Output file is "    << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
   cout << endl;

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
}
