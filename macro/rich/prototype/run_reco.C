void run_reco(Int_t nEvents, Float_t posReso, Float_t thetaReso)
{
    Int_t iVerbose = 0;
    gRandom->SetSeed(10);


    TString dir = "/d/cbm02/kresan/rich_prot/may11";
    TString inFile = dir + "/epi.standard.mc.root";
    TString parFile = dir + "/params.epi.standard.root";
    TString outFile = dir + "/epi.standard";
    if(0 != posReso) {
	char strPosReso[100];
        sprintf(strPosReso, "%1.0f", posReso);
        outFile = outFile + "." + TString(strPosReso) + "mm";
    }
    if(0 != thetaReso) {
	char strThetaReso[100];
        sprintf(strThetaReso, "%1.0f", thetaReso);
        outFile = outFile + "." + TString(strThetaReso) + "mrad";
    }
    outFile = outFile + ".reco.root";


    gDebug = 0;
    TStopwatch timer;
    timer.Start();


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
    gSystem->Load("libMvd");
    gSystem->Load("libSts");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");
    gSystem->Load("libEcal");
    gSystem->Load("libGlobal");
    gSystem->Load("libKF");


    FairRunAna *run = new FairRunAna();
    run->SetInputFile(inFile);
    run->SetOutputFile(outFile);


    CbmKF *kalman = new CbmKF();
    run->AddTask(kalman);


    // =========================================================================
    // ===                        RICH reconstruction                        ===
    // =========================================================================
    // --------------------- RICH Hit Producer ---------------------------------
    Double_t richPmtRad = 0.6125; // PMT radius [cm]
    Double_t richPmtDist = 0.2; // Distance between PMTs [cm]
    Int_t richDetType = 4; // Detector type Hamamatsu H8500-03
    Int_t richNoise = 50; // Number of noise points per event
    Double_t collectionEff = 1.;
    Double_t richSMirror = 0.00001; // Sigma for additional point smearing due to light scattering in mirror
    CbmRichProtHitProducer* richHitProd = new CbmRichProtHitProducer(richPmtRad, richPmtDist, richDetType, richNoise, iVerbose, collectionEff, richSMirror);
    run->AddTask(richHitProd);
    //--------------------------------------------------------------------------


    // --------------------- Create STS and global tracks ----------------------
    CbmRichProtPrepareExtrapolation *richPrepare =
	new CbmRichProtPrepareExtrapolation(iVerbose);
    richPrepare->SetReso(posReso/10., thetaReso/1000.);
    run->AddTask(richPrepare);
    // -------------------------------------------------------------------------


    //---------------------- RICH Track Extrapolation --------------------------
    Int_t    richNSts = 0;     // minimum number of STS hits for extrapolation
    Double_t richZPos = 50.;  // z position for extrapolation [cm]
    CbmRichTrackExtrapolation* richExtra = new CbmRichTrackExtrapolationKF(richNSts, iVerbose);
    CbmRichExtrapolateTracks* richExtrapolate = new CbmRichExtrapolateTracks();
    richExtrapolate->UseExtrapolation(richExtra,richZPos);
    run->AddTask(richExtrapolate);
    //--------------------------------------------------------------------------


    //--------------------- RICH Track Projection to photodetector -------------
    Int_t richZFlag = 1;       // Projetion from IM plane (default)
    CbmRichProtProjectionProducer* richProj = new CbmRichProtProjectionProducer(iVerbose, richZFlag);
    run->AddTask(richProj);
    //--------------------------------------------------------------------------


    //--------------------- RICH Ring Finding ----------------------------------
    TString richGeoType = "compact";
    CbmRichProtRingFinderHough* richFinder = new CbmRichProtRingFinderHough(iVerbose, richGeoType);
    CbmRichFindRings* richFindRings = new CbmRichFindRings(richFinder, iVerbose);
    run->AddTask(richFindRings);
    //--------------------------------------------------------------------------


    CbmRichRingFitter* richFitter2 = new CbmRichRingFitterCircle();
    richFinder->SetFitter(richFitter2);
    CbmRichFitRings* fitRings2 = new CbmRichFitRings("", "", richFitter2);
    run->AddTask(fitRings2);


    //-------------------- RICH Ring Fitting -----------------------------------
    CbmRichRingFitter* richFitter = new CbmRichRingFitterEllipseTau(iVerbose, 1, richGeoType);
    CbmRichFitRings* fitRings = new CbmRichFitRings("", "", richFitter);
    run->AddTask(fitRings);
    //--------------------------------------------------------------------------




    // ------------------- RICH Ring matching  ---------------------------------
    CbmRichMatchRings* matchRings = new CbmRichMatchRings(iVerbose);
    run->AddTask(matchRings);
    // -------------------------------------------------------------------------


    // -----  Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    parIo1->open(parFile.Data());
    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    // ------------------------------------------------------------------------


    // -----   Intialise and run   --------------------------------------------
    //run->LoadGeometry();
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


    cout << " Test passed" << endl;
    cout << " All ok " << endl;
}


