#include <iostream>

using namespace std;

void radlength_sim(Int_t nEvents = 1000000)
{
   TString dir  = "data";
   TString outFile = dir + "/radlen.mc.root";
   TString parFile = dir + "/radlen.params.root";

   TString caveGeom = "cave.geo";
   TString stsGeom = "";//"sts/sts_v12b_12344444.geo.root";
   TString trdGeom = "trd/trd_v10b.geo";

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   FairRunSim* run = new FairRunSim();
   run->SetName("TGeant3");              // Transport engine
   run->SetOutputFile(outFile);          // Output file
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

   // -----   Create magnetic field   ----------------------------------------
//   // Constant Field
//   CbmFieldConst *magField=new CbmFieldConst();
//   magField->SetField(0, 0 ,0 ); // values are in kG
//   // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
//   magField->SetFieldRegion(-74, -39 ,-22 , 74, 39 , 160 );
//   run->SetField(magField);
   // ------------------------------------------------------------------------

   // Use the experiment specific MC Event header instead of the default one
   // This one stores additional information about the reaction plane
   //  CbmMCEventHeader* mcHeader = new CbmMCEventHeader();
   //  fRun->SetMCEventHeader(mcHeader);

   // -----   Create PrimaryGenerator   --------------------------------------
   FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
   run->SetGenerator(primGen);

   const int RMax = 550; // Maximum radius of the station

   FairBoxGenerator* box = new FairBoxGenerator(0, 1);
   box->SetBoxXYZ(-RMax, -RMax, RMax, RMax, 0.);
   box->SetPRange(0.1, 10);
   box->SetThetaRange(0., 0.);
   box->SetPhiRange(0., 0.);
   primGen->AddGenerator(box);

   run->SetStoreTraj(kFALSE);
   run->SetRadLenRegister(kTRUE);
   // ------------------------------------------------------------------------

   run->Init();

   // -----   Runtime database   ---------------------------------------------
   //CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
   //fieldPar->SetParameters(magField);
   //fieldPar->setChanged();
   //fieldPar->setInputVersion(run->GetRunId(),1);
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
   cout << "Output file is " << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << "s" << endl;
   // ------------------------------------------------------------------------
}

