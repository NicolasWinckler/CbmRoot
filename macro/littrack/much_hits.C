void much_hits(Int_t nEvents = 1000)
{
  Int_t iVerbose = 0;

  TString dir = "/home/d/andrey/events/strawmuch/standard/10mu/mu_urqmd/";
  TString mcFile = dir + "mc.root";
  TString parFile = dir + "params.root";
  TString outFile = dir + "much.hits.root";
//  TString digiFile = dir + "much.digi.root";

  TString muchDigiFile = TString(gSystem->Getenv("VMCWORKDIR")) + "/parameters/much/much_standard.digi.par";

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();

   // -----   Reconstruction run   -------------------------------------------
   FairRunAna *run= new FairRunAna();
   run->SetInputFile(mcFile);
   run->SetOutputFile(outFile);
   // ------------------------------------------------------------------------


   // =========================================================================
   // ===                      MUCH local reconstruction                    ===
   // =========================================================================

   // ---  MuCh digitizer ----------------------------------------------------
   CbmMuchDigitize* muchDigitize = new CbmMuchDigitize("MuchDigitize", iVerbose);
 //  CbmMuchDigitize* muchDigitize = new CbmMuchDigitize("MuchDigitize", digiFile.Data(), iVerbose);
 //  muchDigitize->SetUseAvalanche(0); // 0 - Not account for avalanches; 1 - Account for avalanches
   //muchDigitize->SetMeanNoise(0);
   run->AddTask(muchDigitize);
   // ------------------------------------------------------------------------

   // ---  MuCh hit finder ---------------------------------------------------
   CbmMuchFindHits* muchFindHits = new CbmMuchFindHits("MuchFindHits", iVerbose);
//   CbmMuchFindHits* muchFindHits = new CbmMuchFindHits("MuchFindHits", digiFile.Data(), iVerbose);
//   muchFindHits->SetUseClustering(0);
   run->AddTask(muchFindHits);

   // -------------------------------------------------------------------------
   // ===                 End of MUCH local reconstruction                  ===
   // =========================================================================


   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(muchDigiFile.Data());
   rtdb->setFirstInput(parIo1);
   rtdb->setSecondInput(parIo2);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();
   // ------------------------------------------------------------------------

   // -----   Intialise and run   --------------------------------------------
   run->LoadGeometry();
   run->Init();
   run->Run(0,nEvents);
   // ------------------------------------------------------------------------


   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is "    << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
   cout << endl;
   // ------------------------------------------------------------------------

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
   exit(0);
}
