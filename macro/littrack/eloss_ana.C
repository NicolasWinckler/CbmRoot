void eloss_ana(Int_t nEvents = 200000)
{
  TString engine = "geant4/";
  TString dir = "/d/cbm02/andrey/events/eloss/" + engine + "10gev10X0iron/";
  TString inFile = dir + "eloss.sim.root";  
  TString outFile = dir + "eloss.ana.root";  

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();

   
   FairRunAna *run= new FairRunAna();
   run->SetInputFile(inFile);
   run->SetOutputFile(outFile);


   CbmLitElossAna* elossAna = new CbmLitElossAna();
   run->AddTask(elossAna);

   run->LoadGeometry();
   run->Init();
   run->Run(0,nEvents);

   
   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is "    << outFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
   cout << endl;
   // ------------------------------------------------------------------------
   
   cout << " Test passed" << endl;
   cout << " All ok " << endl;
   exit(0);
}
