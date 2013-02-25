#include <iostream>
using std::cout;
using std::endl;

void radlength_sim(Int_t nEvents = 100000)
{
   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

   TString dir  = "data";
   TString mcFile = dir + "/radlength.mc.0000.root";
   TString parFile = dir + "/radlength.param.0000.root";

   TString caveGeom = "cave.geo";
   TString stsGeom = "";//"sts/sts_v12b_12344444.geo.root";
   TString trdGeom = "trd/trd_v13b.root";

   if (script == "yes") {
      mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
      parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));

      caveGeom = TString(gSystem->Getenv("LIT_CAVE_GEOM"));
      //stsGeom = TString(gSystem->Getenv("LIT_STS_GEOM"));
      trdGeom = TString(gSystem->Getenv("LIT_TRD_GEOM"));
   }

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   FairRunSim* run = new FairRunSim();
   run->SetName("TGeant3");              // Transport engine
   run->SetOutputFile(mcFile);          // Output file
   FairRuntimeDb* rtdb = run->GetRuntimeDb();

   run->SetMaterials("media.geo");       // Materials

   // -----   Create detectors and passive volumes   -------------------------
   if (caveGeom != "") {
      FairModule* cave = new CbmCave("CAVE");
      cave->SetGeometryFileName(caveGeom);
      run->AddModule(cave);
   }

   if (stsGeom != "") {
      FairDetector* sts = new CbmSts("STS", kTRUE);
      sts->SetGeometryFileName(stsGeom);
      run->AddModule(sts);
   }

   if (trdGeom != "") {
      FairDetector* trd = new CbmTrd("TRD",kTRUE );
      trd->SetGeometryFileName(trdGeom);
      run->AddModule(trd);
   }

   // -----   Create PrimaryGenerator   --------------------------------------
   FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
   run->SetGenerator(primGen);

   const int RMax = 550; // Maximum radius of the station

 /*  FairBoxGenerator* box = new FairBoxGenerator(0, 1);
   box->SetBoxXYZ(-RMax, -RMax, RMax, RMax, 0.);
   box->SetPRange(0.1, 10);
   box->SetThetaRange(0., 0.);
   box->SetPhiRange(0., 0.);
   primGen->AddGenerator(box);*/

   FairBoxGenerator* box = new FairBoxGenerator(0, 1);
   box->SetPRange(0.1, 10);
   box->SetPhiRange(0., 360.);
   box->SetThetaRange(0., 35.);
   primGen->AddGenerator(box);

   run->SetStoreTraj(kFALSE);
   run->SetRadLenRegister(kTRUE);
   // ------------------------------------------------------------------------

   run->Init();

   // -----   Runtime database   ---------------------------------------------
   Bool_t kParameterMerged = kTRUE;
   FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
   parOut->open(parFile.Data());
   rtdb->setOutput(parOut);
   rtdb->saveOutput();
   rtdb->print();
   // ------------------------------------------------------------------------

   run->Run(nEvents);
   run->CreateGeometryFile("data/geofile_full.root");

   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is " << mcFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << "s" << endl;
   // ------------------------------------------------------------------------
}

