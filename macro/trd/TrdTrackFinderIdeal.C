// Macro for running ideal TRD track finder
{
    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();


    // Load this example libraries
    gSystem->Load("libGeoBase");
    gSystem->Load("libBase");
    gSystem->Load("libPassive");
    gSystem->Load("libSts");
    gSystem->Load("libTof");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libEcal");
    gSystem->Load("libGen");
    gSystem->Load("libGlobal");
    gSystem->Load("libKF");
    gSystem->Load("libL1");


    TString mcFile = "test.mc.root";
    TString paramFile = "test.mc.param.root";
    TString hitFile = "test.trdHits.root";
    TString outFile = "test.trdIdealTracks.root";


    FairRunAna *fRun= new FairRunAna();
    fRun->SetInputFile(mcFile);
    fRun->SetOutputFile(outFile);
    fRun->AddFriend(hitFile);


    // Init Simulation Parameters from Root File
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* io1=new FairParRootFileIo();
    io1->open(paramFile.Data());

    // Init Digitization Parameters from Ascii File
    //FairParAsciiFileIo* input2 = new FairParAsciiFileIo();
    //TString StsDigiFile(gSystem->Getenv("VMCWORKDIR"));
    //StsDigiFile = StsDigiFile + "/parameters/sts/sts_digi.par";
    //input2->open(StsDigiFile.Data(),"in");

    rtdb->setFirstInput(io1);
    //rtdb->setSecondInput(input2);



    // ----------- Ideal TRD track finder --------------
    // Create and add finder task
    CbmTrdFindTracks* finderTask =
        new CbmTrdFindTracks("trdFindTracks");
    fRun->AddTask(finderTask);
    CbmTrdTrackFinderIdeal* mcTrackFinder =
        new CbmTrdTrackFinderIdeal();
    mcTrackFinder->SetVerbose(1);  // verbosity level
    finderTask->UseFinder(mcTrackFinder);
    // -------------------------------------------------

    // ------------ TRD track matching -----------------
    CbmTrdMatchTracks* matchTracks =
        new CbmTrdMatchTracks("TrdMatchTracks",
                              "TrdMatchTracks",
                             1);
    fRun->AddTask(matchTracks);
    // -------------------------------------------------

    // -------- Performance of track finder ------------
    CbmTrdFindTracksQa* trdFindQa =
        new CbmTrdFindTracksQa("FinderPerformance");
    trdFindQa->SetNormType(1); // normalise to number of reco. MC tracks
    trdFindQa->SetVerbose(1);  // verbosity level
    fRun->AddTask(trdFindQa);
    // -------------------------------------------------



    // ------------- Kalman filter ---------------------
    CbmKF* kalman = new CbmKF();
    fRun->AddTask(kalman);
    // -------------------------------------------------

    // ------------ TRD track fitter -------------------
    CbmTrdFitTracks* fitterTask =
        new CbmTrdFitTracks("trdFitTracks");
    fRun->AddTask(fitterTask);
    CbmL1TrdTrackFitter* fitter = new CbmL1TrdTrackFitter();
    fitter->SetVerbose(1);
    fitter->SetPid(211);
    fitterTask->UseFitter(fitter);
    // -------------------------------------------------

    // -------- Performance of track fitter ------------
    CbmTrdFitTracksQa* trdFitQa =
        new CbmTrdFitTracksQa("trdFitQa");
    fRun->AddTask(trdFitQa);
    // -------------------------------------------------



    fRun->Init();

    fRun->Run();


    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);


    trdFindQa->WriteHistogramms();
    trdFitQa->WriteHistogramms();
}


