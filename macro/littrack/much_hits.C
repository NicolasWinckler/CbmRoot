void much_hits(Int_t nEvents = 10000)
{
  Int_t iVerbose = 0;

  TString dir = "/d/cbm02/andrey/events/muchstraw/large/10mu/mu/";
   
  TString inFile = dir + "mc.root";
  TString parFile = dir + "params.root";  
  TString outFile = dir + "much.hits.root";
  
   //TString muchDigiFile = "$VMCWORKDIR/much/parameters/much_digi.400x800mic.par";
   //TString muchDigiFile = "/u/andrey/cbm/svnfeb08/cbmroot/much/parameters/much_large.digi.par";
    //TString muchDigiFile = "/u/andrey/cbm/svnfeb08/cbmroot/much/parameters/much_compact.2.norot.digi.par";
  TString muchDigiFile = "/u/andrey/cbm/svnfeb08/cbmroot/parameters/much/much_standard.digi.par";
   
   TStopwatch timer;
   timer.Start();
   
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();
   
   // -----   Reconstruction run   -------------------------------------------
   CbmRunAna *run= new CbmRunAna();
   run->SetInputFile(inFile);
   run->SetOutputFile(outFile);
   // ------------------------------------------------------------------------
     
   
   // =========================================================================
   // ===                      MUCH local reconstruction                    ===
   // =========================================================================
   
   // ---  MuCh digitizer ----------------------------------------------------
   CbmMuchDigitize* muchDigitize = new CbmMuchDigitize("MuchDigitize", iVerbose);
   muchDigitize->SetUseAvalanche(0); // Not account for avalanches
   run->AddTask(muchDigitize);
   // ------------------------------------------------------------------------
   
   // ---  MuCh hit finder ---------------------------------------------------
   CbmMuchFindHits* muchFindHits = new CbmMuchFindHits("MuchFindHits", iVerbose);
   muchFindHits->SetUseClustering(0);
   run->AddTask(muchFindHits);
   
   // -------------------------------------------------------------------------
   // ===                 End of MUCH local reconstruction                  ===
   // =========================================================================
      

   // -----  Parameter database   --------------------------------------------
   CbmRuntimeDb* rtdb = run->GetRuntimeDb();
   CbmParRootFileIo* parIo1 = new CbmParRootFileIo();
   CbmParAsciiFileIo* parIo2 = new CbmParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(muchDigiFile.Data(),"in");
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
