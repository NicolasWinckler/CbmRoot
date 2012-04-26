void run_sim_geotest(Int_t nEvents = 10)
{
   TTree::SetMaxTreeSize(90000000000);

   TString script = TString(gSystem->Getenv("SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   TString parFile = "", outFile ="";
   TString caveGeom = "", targetGeom = "", pipeGeom   = "", magnetGeom = "",
         stsGeom = "", richGeom= "";
   TString fieldMap = "";

   // Magnetic field
   Double_t fieldZ = 50.; // field center z position
   Double_t fieldScale =  1.0; // field scaling factor

   if (script != "yes") {
      TString outDir = "/d/cbm06/user/slebedev/rich/new_rich_geo/";
      outFile = outDir + "test.mc.0004.root";
      parFile = outDir + "test.params.0004.root";

      caveGeom = "cave.geo";
      targetGeom = "target_au_250mu.geo";
      pipeGeom = "pipe_standard.geo";
      stsGeom = "sts/sts_v11a.geo";
      richGeom = "rich/rich_v08a.geo";
      fieldMap = "field_v10e";
      magnetGeom = "passive/magnet_v09e.geo";
   } else {
      outFile = TString(gSystem->Getenv("MCFILE"));
      parFile = TString(gSystem->Getenv("PARFILE"));
      caveGeom = TString(gSystem->Getenv("CAVEGEOM"));
      targetGeom = TString(gSystem->Getenv("TARGETGEOM"));
      pipeGeom = TString(gSystem->Getenv("PIPEGEOM"));
      stsGeom = TString(gSystem->Getenv("STSGEOM"));
      richGeom = TString(gSystem->Getenv("RICHGEOM"));
      fieldMap = TString(gSystem->Getenv("FIELDMAP"));
      magnetGeom = TString(gSystem->Getenv("MAGNETGEOM"));
   }

   gDebug = 0;
   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();

   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   FairRunSim* fRun = new FairRunSim();
   fRun->SetName("TGeant3"); // Transport engine
   fRun->SetOutputFile(outFile);
   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

   fRun->SetMaterials("media.geo"); // Materials


   if ( caveGeom != "" ) {
      FairModule* cave = new CbmCave("CAVE");
      cave->SetGeometryFileName(caveGeom);
      fRun->AddModule(cave);
   }

   if ( pipeGeom != "" ) {
      FairModule* pipe = new CbmPipe("PIPE");
      pipe->SetGeometryFileName(pipeGeom);
      fRun->AddModule(pipe);
   }

   if ( targetGeom != "" ) {
      FairModule* target = new CbmTarget("Target");
      target->SetGeometryFileName(targetGeom);
      fRun->AddModule(target);
   }

   if ( magnetGeom != "" ) {
      FairModule* magnet = new CbmMagnet("MAGNET");
      magnet->SetGeometryFileName(magnetGeom);
      fRun->AddModule(magnet);
   }

   if ( stsGeom != "" ) {
      FairDetector* sts = new CbmSts("STS", kTRUE);
      sts->SetGeometryFileName(stsGeom);
      fRun->AddModule(sts);
   }

   if ( richGeom != "" ) {
      FairDetector* rich = new CbmRich("RICH", kTRUE);
      rich->SetGeometryFileName(richGeom);
      fRun->AddModule(rich);
   }

   // -----   Create magnetic field   ----------------------------------------
   CbmFieldMap* magField = NULL;
   if (fieldMap == "field_electron_standard" || fieldMap == "field_v10e")
      magField = new CbmFieldMapSym2(fieldMap);
   else if (fieldMap == "field_muon_standard" )
      magField = new CbmFieldMapSym2(fieldMap);
   else if (fieldMap == "FieldMuonMagnet" )
      magField = new CbmFieldMapSym3(fieldMap);
   else {
      cout << "===> ERROR: Unknown field map " << fieldMap << endl;
      exit;
   }
   magField->SetPosition(0., 0., fieldZ);
   magField->SetScale(fieldScale);
   fRun->SetField(magField);

   FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

   Int_t kfCode1=11; // electrons
   Int_t kfCode2=-11; // positrons

   FairBoxGenerator* boxGen1 = new FairBoxGenerator(kfCode1, 100);
   boxGen1->SetPtRange(0.,3.);
   boxGen1->SetPhiRange(0.,360.);
   boxGen1->SetThetaRange(2.5,25.);
   boxGen1->SetCosTheta();
   boxGen1->Init();
   primGen->AddGenerator(boxGen1);

   FairBoxGenerator* boxGen2 = new FairBoxGenerator(kfCode2, 100);
   boxGen2->SetPtRange(0.,3.);
   boxGen2->SetPhiRange(0.,360.);
   boxGen2->SetThetaRange(2.5,25.);
   boxGen2->SetCosTheta();
   boxGen2->Init();
   primGen->AddGenerator(boxGen2);

   fRun->SetGenerator(primGen);
   fRun->Init();

   // -----   Runtime database   ---------------------------------------------
   CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
   fieldPar->SetParameters(magField);
   fieldPar->setChanged();
   fieldPar->setInputVersion(fRun->GetRunId(),1);
   Bool_t kParameterMerged = kTRUE;
   FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
   parOut->open(parFile.Data());
   rtdb->setOutput(parOut);
   rtdb->saveOutput();
   rtdb->print();

   fRun->Run(nEvents);

   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is "    << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
   cout << " Test passed" << endl;
   cout << " All ok " << endl;
   exit(0);
}
