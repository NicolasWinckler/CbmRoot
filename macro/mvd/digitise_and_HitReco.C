// --------------------------------------------------------------------------
//
// Macro for local MVD reconstruction from MC data
//
// Tasks:  CbmMvdHitProducer
// 
//
// V. Friese   06/02/2007
//
// --------------------------------------------------------------------------


{

    // ========================================================================
    //          Adjust this part according to your requirements


    // Input file (MC events)
    TString inFile = "mvd.mc.root";

    // Parameter file name
    TString parFile = "params.root";

    // Output file
    TString outFile = "dg.reco.root";;

    // Background file (MC events, for pile-up)
    TString bgFile = inFile;

    // Delta file (Au Ions)
    TString deltaFile = inFile;

    // Number of events to process
    Int_t nEvents = 5;

    // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    Int_t iVerbose = 0;

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
    gSystem->Load("libCLHEP");
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
    gSystem->Load("libField");
    gSystem->Load("libGen");
    gSystem->Load("libPassive");
    gSystem->Load("libMvd");
    gSystem->Load("libSts");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");
    gSystem->Load("libGlobal");
    gSystem->Load("libEcal");
    gSystem->Load("libKF");
    gSystem->Load("libL1");

    // ------------------------------------------------------------------------



    // -----   Reconstruction run   -------------------------------------------
    FairRunAna *fRun= new FairRunAna();
    fRun->SetInputFile(inFile);
    fRun->SetOutputFile(outFile);
    // ------------------------------------------------------------------------



    // -------   MVD Digitiser   ----------------------------------------------
    CbmMvdDigitiser* digi = new CbmMvdDigitiser("MVDDigitiser", 0, iVerbose);
    cout << "Adding Task:  CbmMvdDigitiser... " << endl;
    fRun->AddTask(digi);

    digi->SetEpiThickness(0.0014);
    digi->SetElectronsPerKeV(300);
    digi->SetSegmentLength(0.0003);
    digi->SetPixelSizeX(0.003);
    digi->SetPixelSizeY(0.003);
    digi->SetDiffusionCoef(0.0055); // adapted for 30um pixels
    //digi->SetDiffusionCoef(0.0018); // adapted for 10um pixels
    digi->SetWidthOfCluster(3);


    //--- Pile Up -------
    digi->SetBgFileName(bgFile);
    digi->SetBgBufferSize(1000);
    digi->SetPileUp(0);


    //--- Delta electrons -------
    digi->SetDeltaName(deltaFile);
    digi->SetDeltaBufferSize(10000);
    digi->SetDeltaEvents(0);


    //----------------------------------------------------------------------------
    // -----   MVD Cluster Finder   ----------------------------------------------

    CbmMvdFindHits* mvd_hit   = new CbmMvdFindHits("MVDFindHits", 0, iVerbose);
    mvd_hit->SetSigmaNoise(15,kTRUE);  // kTRUE = add noise to digis, kFALSE = ideal detector
    mvd_hit->SetSeedThreshold(50); //in electrons!
    mvd_hit->SetNeighbourThreshold(30);
    mvd_hit->SetAdcBits(12);
    mvd_hit->SetHitPosErrX(0.0005);
    mvd_hit->SetHitPosErrY(0.0005);
    fRun->AddTask(mvd_hit);


    //----------------------------------------------------------------------------
    // -----  Parameter database   -----------------------------------------------
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo*  parIo1 = new FairParRootFileIo();
    parIo1->open(parFile.Data());

    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    rtdb->print();
    fRun->LoadGeometry();
    // ---------------------------------------------------------------------------


    // -----   Run initialisation   ----------------------------------------------
    fRun->Init();
    // ---------------------------------------------------------------------------



    // -----   Start run   -------------------------------------------------------
    fRun->Run(0,nEvents);
    // ---------------------------------------------------------------------------



    // -----   Finish   ----------------------------------------------------------
    timer.Stop();

    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is "    << outFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ---------------------------------------------------------------------------


}
