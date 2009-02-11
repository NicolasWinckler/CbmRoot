// Macro for running STS -> TRD track finder
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
    gSystem->Load("libKF");
    gSystem->Load("libL1");


    TString mcFile = "test.mc.root";
    TString paramFile = "test.mc.param.root";
    TString hitFile = "test.trdHits.root";
    TString stsTracksFile = "test.stsTracks.root";
    TString outFile = "test.trdTracks.root";


    FairRunAna *fRun= new FairRunAna();
    fRun->SetInputFile(mcFile);
    fRun->SetOutputFile(outFile);
    fRun->AddFriend(hitFile);
    fRun->AddFriend(stsTracksFile);


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



    // -------------- Kalman filter --------------------
    CbmKF* fKalman = new CbmKF();
    fRun->AddTask(fKalman);
    // -------------------------------------------------

    // ----------- STS -> TRD track finder -------------
    // Create and add finder task
    CbmTrdFindTracks* finderTask =
        new CbmTrdFindTracks("trdFindTracks");
    fRun->AddTask(finderTask);
    CbmL1TrdTrackFinderSts* trackFinder =
        new CbmL1TrdTrackFinderSts();
    trackFinder->SetVerbose(1);       // verbosity level
    trackFinder->SetPid(211);         // pion PDG
    trackFinder->SetChi2Cut(35);      // chi2 to hit cut
    finderTask->UseFinder(trackFinder);
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
    trdFindQa->SetNormType(2);  // normalise to number of reco. STS tracks
    trdFindQa->SetVerbose(1);   // verbosity level
    fRun->AddTask(trdFindQa);
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


    trackFinder->WriteHistogramms();
    trdFindQa->WriteHistogramms();
    trdFitQa->WriteHistogramms();
}


