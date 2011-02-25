// --------------------------------------------------------------------------
//
// Macro to create a geometry file for the TRD
// This file is needed to create the digi parameters as
// next step
//
// F. Uhlig 07.05.2010
//
// D. Emschermann - 26.05.2010 - fix TRD geo input file
// --------------------------------------------------------------------------

//void create_geometry_file(TString geomFile="trd_squared_modules.geo")
void create_geometry_file(TString geomFile="trd_standard.geo")
{

  // ========================================================================
  //          Adjust this part according to your requirements


  //Extract Filename without extension and path 
  //TODO: Do this in a better way.
  TString fileName = geomFile;
  fileName.ReplaceAll(".geo","");
  if (fileName.Contains("/")){
    fileName=fileName("/.*$"); //extract substring with filename
    fileName.ReplaceAll("/","");
  }
  //  cout<<"fileName: "<<fileName<<endl;

  // ----- Paths and file names  --------------------------------------------
  TString inDir   = gSystem->Getenv("VMCWORKDIR");
  TString inFile  = inDir + "/input/urqmd.ftn14";
  TString outDir  = "data";
  TString outFile = outDir + "/test.mc."+ fileName + ".root";
  TString parFile = outDir + "/params.root";
  
  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  //  TString trdGeom = "../macro/trd/" + geomFile;
  TString trdGeom = geomFile;
  TString trdGeomOut = "geofile_" + fileName + ".root";

  // In general, the following parts need not be touched
  // ========================================================================

  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libTrd");
  // -----------------------------------------------------------------------
 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------


  // -----   Create media   -------------------------------------------------
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------


  // -----   Create detectors and passive volumes   -------------------------
  if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
  }
  if ( trdGeom != "" ) {
    FairDetector* trd = new CbmTrd("TRD",kTRUE );
    trd->SetGeometryFileName(trdGeom);
    fRun->AddModule(trd);
  }

  // ------------------------------------------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, 20);
  boxGen1->SetPRange(2.,2.);
  boxGen1->SetPhiRange(0.,0.);
  boxGen1->SetThetaRange(0.,0.);
  boxGen1->SetCosTheta();
  boxGen1->Init();
  primGen->AddGenerator(boxGen1);

  // Constant Field
  CbmFieldConst *fMagField1=new CbmFieldConst();
  fMagField1->SetField(0, 14 ,0 ); // values are in kG
  fMagField1->SetFieldRegion(-74, -39 ,-22 , 74, 39 , 160 );
  fRun->SetField(fMagField1);
 
  // -----   Run initialisation   -------------------------------------------
  fRun->Init();
  // ------------------------------------------------------------------------
  fRun->Run(1);
 
  // Create separate file with the geomanager only
  // This version will work only with new versions of
  // the base classes
  fRun->CreateGeometryFile(trdGeomOut);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime 
       << "s" << endl << endl;
  // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);
}

