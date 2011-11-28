
void run_reco_geotest(Int_t nEvents = 10000)
{
   Int_t iVerbose = 0;

   TString inFile = "/d/cbm02/slebedev/rich/JUL09/geotest/mc.0000.root";
   TString parFile = "/d/cbm02/slebedev/rich/JUL09/geotest/params.0000.root";
   TString outFile = "/d/cbm02/slebedev/rich/JUL09/geotest/reco.0000.root";

   TString stsDigiFile = "sts_standard.digi.par";

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

   Double_t richPmtRad = 0.4; // PMT radius [cm]
   Double_t richPmtDist = 0.; // Distance between PMTs [cm]
   Int_t richDetType = 4; // Detector type Hamamatsu H8500-03
   Int_t richNoise = 220; // Number of noise points per event
   Double_t collectionEff = 1.; //collection efficiency of the photodetector
   Double_t richSMirror = 0.00001; // Sigma for additional point smearing due to light scattering in mirror
   CbmRichHitProducer* richHitProd = new CbmRichHitProducer(richPmtRad, richPmtDist,
         richDetType, richNoise, iVerbose, collectionEff, richSMirror);
   run->AddTask(richHitProd);

   CbmRichRingFinderIdeal* richFinder = new CbmRichRingFinderIdeal(iVerbose);
   CbmRichFindRings* richFindRings = new CbmRichFindRings();
   richFindRings->UseFinder(richFinder);
   run->AddTask(richFindRings);

   CbmRichMatchRings* matchRings = new CbmRichMatchRings(iVerbose);
   run->AddTask(matchRings);

   CbmRichGeoTest* geoTest = new CbmRichGeoTest();
   run->AddTask(geoTest);

   // -----  Parameter database   --------------------------------------------
   TString stsDigi = gSystem->Getenv("VMCWORKDIR");
   stsDigi += "/parameters/sts/";
   stsDigi += stsDigiFile;
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(stsDigi.Data(),"in");
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
