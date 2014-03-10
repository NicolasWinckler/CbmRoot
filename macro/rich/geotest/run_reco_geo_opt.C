
void run_reco_geo_opt(Int_t nEvents = 2)
{
   TTree::SetMaxTreeSize(90000000000);
   TString script = TString(gSystem->Getenv("SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v12b.root"; // Material budget file for L1 STS tracking
   gRandom->SetSeed(10);

   TString outDir = "/Users/slebedev/Development/cbm/data/simulations/rich/urqmdtest/";
   TString inFile = outDir + "25gev.centr.mc.0000.root";
   TString parFile = outDir + "25gev.centr.param.0000.root";
   TString outFile = outDir + "25gev.centr.reco.0000.root";
   std::string resultDir = "results_urqmd_25gev_centr/";
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file
   parFileList->Add(&stsDigiFile);

   gDebug = 0;
   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   FairRunAna *run= new FairRunAna();
   run->SetInputFile(inFile);
   run->SetOutputFile(outFile);

   Double_t threshold  =  4;
   Double_t noiseWidth =  0.01;
   Int_t    nofBits    = 12;
   Double_t electronsPerAdc    =  10;
   Double_t StripDeadTime = 0.1;
   CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", 0);
   stsDigitize->SetRealisticResponse();
   stsDigitize->SetFrontThreshold (threshold);
   stsDigitize->SetBackThreshold  (threshold);
   stsDigitize->SetFrontNoiseWidth(noiseWidth);
   stsDigitize->SetBackNoiseWidth (noiseWidth);
   stsDigitize->SetFrontNofBits   (nofBits);
   stsDigitize->SetBackNofBits    (nofBits);
   stsDigitize->SetFrontNofElPerAdc(electronsPerAdc);
   stsDigitize->SetBackNofElPerAdc(electronsPerAdc);
   stsDigitize->SetStripDeadTime  (StripDeadTime);
   run->AddTask(stsDigitize);

   FairTask* stsClusterFinder = new CbmStsClusterFinder("CbmStsClusterFinder", 0);
   run->AddTask(stsClusterFinder);

   FairTask* stsFindHits = new CbmStsFindHits(0);
   run->AddTask(stsFindHits);

   CbmStsMatchHits* stsMatchHits = new CbmStsMatchHits(0);
   run->AddTask(stsMatchHits);

   CbmKF* kalman = new CbmKF();
   run->AddTask(kalman);
   CbmL1* l1 = new CbmL1();
   l1->SetMaterialBudgetFileName(stsMatBudgetFileName);
   run->AddTask(l1);

   CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
   Bool_t useMvd = false;
   FairTask* stsFindTracks = new CbmStsFindTracks(0, stsTrackFinder, useMvd);
   run->AddTask(stsFindTracks);

   FairTask* stsMatchTracks = new CbmStsMatchTracks(0);
   run->AddTask(stsMatchTracks);

   CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
   FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, 0);
   run->AddTask(stsFitTracks);

   CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
   run->AddTask(finder);


   CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
   richHitProd->SetDetectorType(4);
   richHitProd->SetNofNoiseHits(220);
   richHitProd->SetCollectionEfficiency(1.0);
   richHitProd->SetSigmaMirror(0.06);
   run->AddTask(richHitProd);

   CbmRichReconstruction* richReco = new CbmRichReconstruction();
   richReco->SetRunExtrapolation(true);
   richReco->SetRunProjection(true);
   richReco->SetRunTrackAssign(false);
   richReco->SetFinderName("ideal");
  // richReco->SetExtrapolationName("ideal");
   run->AddTask(richReco);

   CbmRichMatchRings* matchRings = new CbmRichMatchRings();
   run->AddTask(matchRings);

   CbmRichGeoOpt* richGeoOpt = new CbmRichGeoOpt();
   run->AddTask(richGeoOpt);

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
