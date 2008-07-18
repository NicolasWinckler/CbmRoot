// Macro to produce Tof Hit from transported file

{
    TStopwatch timer;
    timer.Start();
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    gSystem->Load("libGeoBase");
    gSystem->Load("libBase");
    gSystem->Load("libPassive");
    gSystem->Load("libSts");
    gSystem->Load("libTof");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libGen");


    CbmRunAna *fRun= new CbmRunAna();
    fRun->SetInputFile("../run/auau.25gev.centr.mc.root");
    fRun->SetOutputFile("auau.25gev.centr.tofHits.root");


    // Init Simulation Parameters from Root File
    CbmRuntimeDb* rtdb=fRun->GetRuntimeDb();
    CbmParRootFileIo* io1=new CbmParRootFileIo();
    io1->open("../run/parfiles/params.root");

    // Init Digitization Parameters from Ascii File
    //CbmParAsciiFileIo* input2 = new CbmParAsciiFileIo();
    //TString StsDigiFile(gSystem->Getenv("VMCWORKDIR"));
    //StsDigiFile = StsDigiFile + "/parameters/sts/sts_digi.par";
    //input2->open(StsDigiFile.Data(),"in");

    rtdb->setFirstInput(io1);
    //rtdb->setSecondInput(input2);


    CbmTofHitProducer *hp = new CbmTofHitProducer("tof", 1);
    Double_t SigmaXY = 0.2 ;   // check unit
    Double_t SigmaT = 8.E-2; // tof resol in ns
    hp->SetSigmaXY(SigmaXY);
    hp->SetSigmaT(SigmaT);
    fRun->AddTask( hp );


    fRun->Init();

    fRun->Run();


    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
