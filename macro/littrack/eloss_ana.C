

void eloss_ana(Int_t nEvents = 10000)
{
  TString engine = "geant3/";
  TString dir = "/d/cbm02/andrey/events/eloss/" + engine + "1-10gev_20cm/";
  TString inFile = dir + "eloss.sim.root";  
  TString outFile = dir + "eloss.ana.root";  

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gSystem->Load("libGeoBase");
   gSystem->Load("libParBase");
   gSystem->Load("libBase");
   gSystem->Load("libCbmBase");
   gSystem->Load("libField");
   gSystem->Load("libGen");
   gSystem->Load("libPassive"); 
   gSystem->Load("libTrkBase");
   gSystem->Load("libGeane");
   gSystem->Load("libMvd");
   gSystem->Load("libSts");
   gSystem->Load("libRich");
   gSystem->Load("libTrd");
   gSystem->Load("libTof");
   gSystem->Load("libEcal");
   gSystem->Load("libGlobal");
   gSystem->Load("libKF");
   gSystem->Load("libL1");   
   gSystem->Load("libMuch"); 
   gSystem->Load("libLittrack");

   
   CbmRunAna *run= new CbmRunAna();
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
