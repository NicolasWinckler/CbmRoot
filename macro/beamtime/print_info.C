void print_info() 
{

  // Parameter file output
  TString parFile = "./params.root";
 	   
  // Output file
  TString outFile ="./output.root";

  FairLogger* logger = FairLogger::GetLogger();
  logger->SetLogScreenLevel("INFO");
  logger->SetLogVerbosityLevel("MEDIUM"); 	   
  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // =========================================================================
  // ===                           Unpacker                                ===
  // =========================================================================
//  CbmRocSource* source = new CbmRocSource();
//  source->AddFile("./run4_0031.lmd");

  CbmSourceLmd* source = new CbmSourceLmd();
  source->AddFile("./run10_0000.lmd");

  // Event Header
  FairEventHeader* evtHeader = new FairEventHeader();
  evtHeader->SetRunId(1);
 	   
  // -----   Online/Offline MBS run   -----------------------------------
  FairRunOnline *run = new FairRunOnline(source);
  run->SetOutputFile(outFile);
  run->SetEventHeader(evtHeader);
	
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
          
/*
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parIo2 = new FairParRootFileIo(kParameterMerged);
  parIo2->open(parFile.Data(), "UPDATE");
  parIo2->print();
  rtdb->setFirstInput(parIo2);
*/
  
  /*
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(parFileList, "in");
  parIo1->print();
  //   rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo1);
  */
  
/*
  rtdb->print();
  rtdb->printParamContexts();
*/
  // ------------------------------------------------------------------------
  
  // -----   Intialise and run   --------------------------------------------
  run->Init();
  cout << "Starting run" << endl;
  run->Run(-1, 0); // <= FAIRROOT guy style, crash for me on event finish
  
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
  
  //   delete run;
  
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}
