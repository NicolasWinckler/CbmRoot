//  Macro for producing hits in the TRD + computing the Transition
//  Radiation (see CbmTrdRadiator.cxx & .h)
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


    TString inFile = "test.mc.root";
    TString paramFile = "test.mc.param.root";
    TString bgFile = "test.mc.root";
    TString outFile = "test.trdHits.root";
    TString version = "v01a";

    CbmRunAna *fRun= new CbmRunAna();
    fRun->SetInputFile(inFile);
    fRun->SetOutputFile(outFile);


    // Init Simulation Parameters from Root File
    CbmRuntimeDb* rtdb=fRun->GetRuntimeDb();
    CbmParRootFileIo* io1=new CbmParRootFileIo();
    io1->open(paramFile.Data());

    // Init Digitization Parameters from Ascii File
    //CbmParAsciiFileIo* input2 = new CbmParAsciiFileIo();
    //TString StsDigiFile(gSystem->Getenv("VMCWORKDIR"));
    //StsDigiFile = StsDigiFile + "/parameters/sts/sts_digi.par";
    //input2->open(StsDigiFile.Data(),"in");

    rtdb->setFirstInput(io1);
    //rtdb->setSecondInput(input2);

    CbmTrdHitProducer *hp = new CbmTrdHitProducer("trd","trd task");

    // Set the sigma for the TRD in Xand Y direction - angle dependant
    // default values
    // SigmaX smaller than 200 is not feasible
    // SigmaY smaller than 1000 is not feasible
    Double_t SigmaX[] = {300, 400, 500}; // micrometers
    Double_t SigmaY1[] = {2700, 3700, 15000, 27600, 33000, 33000, 33000 };   // micrometers
    Double_t SigmaY2[] = {6300, 8300, 33000, 33000, 33000, 33000, 33000 };   // micrometers
    Double_t SigmaY3[] = {10300, 15000, 33000, 33000, 33000, 33000, 33000};   // mictcrometers

    // Set the basic parameters of the radiator
    // Thickness of the gas detector can not be changed - it is fixed in the
    // geometry file
    Int_t   Nfoils    = 150;      // number of polyetylene foils,  default = 150
    Float_t FoilThick = 0.0015;    // thickness of 1 foil [cm], default = 0.0015
    Float_t GapThick  = 0.020;    // thickness of the gap between the foils [cm], default = 0.020

    hp->SetPar(Nfoils, FoilThick, GapThick);
    hp->SetSigmaX(SigmaX);
    hp->SetSigmaY(SigmaY1, SigmaY2, SigmaY3);

    // Set the detailled (per each electron) or simplified TR production
    // Matters only for pure electron/positron samples
    // factor in speed ~ 20
    // differences in dEdX (TR only) < 5 %
    // kTRUE - siple and fast, kFALSE - slow and 5% more precise
    hp->SetSimpleTR(kTRUE);  

    // add the task
    fRun->AddTask( hp );

    // -------- Performance of hit producer ------------
    CbmTrdHitProducerQa*  trdHitQa =
        new CbmTrdHitProducerQa("HitProducerPerformance");
    fRun->AddTask(trdHitQa);
    // -------------------------------------------------



    fRun->Init();
    fRun->Run();

    // ---- store the performance information
    trdHitQa->WriteHistograms();
    // --------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}


