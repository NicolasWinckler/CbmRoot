// --------------------------------------------------------------------------
//
// Macro for time-based digitisation
//
// Includes (currently) only the STS digitiser

// !!! Be sure to have only STS in simulation. Otherwise, the MC Buffer
// !!! will overflow, since MCPOint other than STS wil not be processed.
// (will be catched later)
//
// V. Friese   01/02/2013
// Version     01/02/2013 (V. Friese)
//
// --------------------------------------------------------------------------


void run_digi(Int_t nEvents = 10) {

  // ========================================================================
  //          Adjust this part according to your requirements

  // Specify input file (MC events)
  TString inFile = "data/test.mc.root";

  // Specify parameter file
  TString parFile = "data/params.root";

  // Specify output file
  TString outFile = "data/test.raw.root";
  
  // Specify interaction rate in 1/s
  Double_t eventRate = 1.e6;
  
  // Specify beam profile ( 1 = constant, 2 = Poissonian beam (default) )
  Int_t beamProfile = 2;
  
  // Specify duration of time slices in output [ns]
  Double_t timeSliceSize = 1000.;
  
  // Specify log level (INFO, DEBUG, DEBUG1, ...)
  TString logLevel = "INFO";
  FairLogger* log;  

  //  Digitisation files.
  // Add TObjectString containing the different file names to
  // a TList which is passed as input to the FairParAsciiFileIo.
  // The FairParAsciiFileIo will take care to create on the fly 
  // a concatenated input parameter file which is then used during
  // the reconstruction.
  TList *parFileList = new TList();

  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters";

  TObjString stsDigiFile = paramDir + "/sts/sts_v13d_std.digi.par";
  parFileList->Add(&stsDigiFile);

  

  // In general, the following parts need not be touched
  // ========================================================================


  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *run = new CbmRunAna();
  run->SetAsync();                         // asynchroneous mode
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // ---- Set the log level 	
  gLogger->SetLogScreenLevel(logLevel.Data());

  
  // ---- MC Time simulation
  Double_t rate = 1.e6;
  FairTask* timeSim = new CbmMCTimeSim(eventRate, beamProfile);
  run->AddTask(timeSim);
 
 
  // ----- STS digitiser
  CbmStsDigitizeTb* stsDigi = new CbmStsDigitizeTb();
  run->AddTask(stsDigi);
 
 
  // ----- DAQ
  FairTask* daq = new CbmDaq(timeSliceSize);
  run->AddTask(daq);
  
 
  


  // -----  Parameter database   --------------------------------------------
  //  TString stsDigi = gSystem->Getenv("VMCWORKDIR");
  // stsDigi += "/parameters/sts/";
  //  stsDigi += stsDigiFile;
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(parFileList, "in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------


  // -----   Intialise and run   --------------------------------------------
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

//  delete run;

  cout << " Test passed" << endl;
	cout << " All ok " << endl;
}
