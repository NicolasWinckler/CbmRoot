
void run_reco_geotest(Int_t nEvents = 10)
{
   TTree::SetMaxTreeSize(90000000000);
   TString script = TString(gSystem->Getenv("SCRIPT"));
   gRandom->SetSeed(10);

   TString inFile = "", parFile = "", outFile ="";
   std::string resultDir = "";

   if (script != "yes") {
      TString outDir = "/d/cbm06/user/slebedev/rich/new_rich_geo/";
      inFile = outDir + "test.mc.0004.root";
      parFile = outDir + "test.params.0004.root";
      outFile = outDir + "test.reco.0004.root";
      resultDir = "results/";
   } else {
      inFile = TString(gSystem->Getenv("MCFILE"));
      outFile = TString(gSystem->Getenv("RECOFILE"));
      parFile = TString(gSystem->Getenv("PARFILE"));
      resultDir = TString(gSystem->Getenv("RICH_GEO_TEST_RESULT_DIR"));
   }

   gDebug = 0;
   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   FairRunAna *run= new FairRunAna();
   run->SetInputFile(inFile);
   run->SetOutputFile(outFile);

   CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
   richHitProd->SetDetectorType(4);
   richHitProd->SetNofNoiseHits(220);
   richHitProd->SetCollectionEfficiency(1.0);
   richHitProd->SetSigmaMirror(0.06);
   run->AddTask(richHitProd);

   CbmRichReconstruction* richReco = new CbmRichReconstruction();
   richReco->SetRunExtrapolation(false);
   richReco->SetRunProjection(false);
   richReco->SetRunTrackAssign(false);
   richReco->SetFinderName("ideal");
   run->AddTask(richReco);

   CbmRichMatchRings* matchRings = new CbmRichMatchRings();
   run->AddTask(matchRings);

   CbmRichGeoTest* geoTest = new CbmRichGeoTest();
   geoTest->SetOutputDir(resultDir);
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
