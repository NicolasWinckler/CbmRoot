void run_reco(Int_t nEvents = 20, Float_t posReso = 1., Float_t thetaReso = 1.)
{
   Int_t iVerbose = 0;
   gRandom->SetSeed(10);

   TString dir = "/d/cbm06/user/slebedev/rich_prot";
   TString inFile = dir + "/mc.root";
   TString parFile = dir + "/params.root";
   TString outFile = dir + "/reco";
   if(0 != posReso) {
      char strPosReso[100];
      sprintf(strPosReso, "%1.0f", posReso);
      outFile = outFile + "." + TString(strPosReso) + "mm";
   }
   if(0 != thetaReso) {
      char strThetaReso[100];
      sprintf(strThetaReso, "%1.0f", thetaReso);
      outFile = outFile + "." + TString(strThetaReso) + "mrad";
   }
   outFile = outFile + ".reco.root";

   gDebug = 0;
   TStopwatch timer;
   timer.Start();
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();

   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   FairRunAna *run = new FairRunAna();
   run->SetInputFile(inFile);
   run->SetOutputFile(outFile);

   CbmKF *kalman = new CbmKF();
   run->AddTask(kalman);

   // =========================================================================
   // ===                        RICH reconstruction                        ===
   // =========================================================================
   CbmRichProtHitProducer* richHitProd  = new CbmRichProtHitProducer();
   richHitProd->SetDetectorType(4);
   richHitProd->SetNofNoiseHits(5);
   richHitProd->SetCollectionEfficiency(1.0);
   richHitProd->SetSigmaMirror(0.00001);
   run->AddTask(richHitProd);

   CbmRichProtPrepareExtrapolation *richPrepare = new CbmRichProtPrepareExtrapolation(iVerbose);
   richPrepare->SetReso(posReso/10., thetaReso/1000.);
   run->AddTask(richPrepare);

   CbmRichReconstruction* richReco = new CbmRichReconstruction();
   richReco->SetZTrackExtrapolation(50.);
   richReco->SetMinNofStsHits(0);
   richReco->SetProjectionName("prototype");
   richReco->SetRunTrackAssign(false);
   run->AddTask(richReco);

   CbmRichMatchRings* matchRings = new CbmRichMatchRings();
   run->AddTask(matchRings);

   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   parIo1->open(parFile.Data());
   rtdb->setFirstInput(parIo1);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();
   // ------------------------------------------------------------------------


   // -----   Intialise and run   --------------------------------------------
   //run->LoadGeometry();
   run->Init();
   cout << "Starting run" << endl;
   run->Run(0, nEvents);
   // ------------------------------------------------------------------------


   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is " << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
   cout << endl;
   // ------------------------------------------------------------------------


   cout << " Test passed" << endl;
   cout << " All ok " << endl;
}


