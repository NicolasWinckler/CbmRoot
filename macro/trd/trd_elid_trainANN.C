/*  Description: This macro trains the ANN for electron
 identification in the TRD.
 Input: simulated and reconstructed events

 Author : Simeon Lebedev
 E-mail : S.Lebedev@gsi.de
 */

void trd_elid_trainANN ()
{
    TStopwatch timer;
    timer.Start();
    
        
    // ----  Load libraries   -------------------------------------------------
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libCbmBase");
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
    gSystem->Load("libL1");
    // ------------------------------------------------------------------------
  
      //load libraries for neural net
    if(!gROOT->GetClass("TMultiLayerPerceptron"))
        gSystem->Load("libMLP");

    //vector to store and transform data from file
    std::vector<Float_t> inVector;
    inVector.resize(12);//12 input elements
    Float_t x13;//output value
    //init TTree as input data to neural net
    TTree *simu = new TTree ("MonteCarlo","MontecarloData");
    simu->Branch("x1", &inVector[0],"x1/F");
    simu->Branch("x2", &inVector[1],"x2/F");
    simu->Branch("x3", &inVector[2],"x3/F");
    simu->Branch("x4", &inVector[3],"x4/F");
    simu->Branch("x5", &inVector[4],"x5/F");
    simu->Branch("x6", &inVector[5],"x6/F");
    simu->Branch("x7", &inVector[6],"x7/F");
    simu->Branch("x8", &inVector[7],"x8/F");
    simu->Branch("x9", &inVector[8],"x9/F");
    simu->Branch("x10", &inVector[9],"x10/F");
    simu->Branch("x11", &inVector[10],"x11/F");
    simu->Branch("x12", &inVector[11],"x12/F");
    simu->Branch("x13", &x13,"x13/F");
    
    
    
    Double_t result, losshit;
    char fileMC[80], fileRec[80];
    Float_t threshold=0.05;
    double NofPi = 0, NofEl = 0;
    double MaxNofPi = 2000, MaxNofEl = 2000;

    for(int iFile=4; iFile <=4; iFile++){
        if(NofPi > MaxNofPi && NofEl > MaxNofEl) continue;
        //if (iFile == 0 ) sprintf(fileMC,"/d/cbm02/uhlig/sim3/electrons_1.5.root");
        //if (iFile == 1 )sprintf(fileMC,"/d/cbm02/uhlig/sim3/pions_1.5.root");
        sprintf(fileMC,"/d/cbm02/slebedev/trd/piel.%.4i.mc.root",iFile);
        cout<<fileMC<<endl;
        TFile *f1 = new TFile(fileMC,"R");
        TTree* t1 = f1->Get("cbmsim");
        TFolder *fd1 = f1->Get("cbmroot");
        TClonesArray* fListStack = (TClonesArray*) fd1->FindObjectAny("MCTrack");
        t1->SetBranchAddress("MCTrack",&fListStack);
        TClonesArray *pointsTRD = (TClonesArray*) fd1->FindObjectAny("TRDPoint");
        t1->SetBranchAddress(pointsTRD->GetName(),&pointsTRD);
        
        
        sprintf(fileRec,"/d/cbm02/slebedev/trd/piel.%.4i.reco.root",iFile);
        //if (iFile == 0 )sprintf(fileRec,"/d/cbm02/uhlig/sim3/TrdHits_electrons_1.5.root");
        //if (iFile == 1 )sprintf(fileRec,"/d/cbm02/uhlig/sim3/TrdHits_pions_1.5.root");
        TFile *f = new TFile(fileRec,"R");
        TTree* t = f->Get("cbmsim");
        TFolder *fd = f->Get("cbmout");
        TClonesArray *tracksTRD = (TClonesArray*) fd->FindObjectAny("TRDTrack");
        t->SetBranchAddress(tracksTRD->GetName(),&tracksTRD);
        TClonesArray *matchTRD = (TClonesArray*) fd->FindObjectAny("TRDTrackMatch");
        t->SetBranchAddress(matchTRD->GetName(),&matchTRD);
        TClonesArray *hitsTRD = (TClonesArray*) fd->FindObjectAny("TRDHit");
        t->SetBranchAddress(hitsTRD->GetName(),&hitsTRD);

        Int_t nEvents=t->GetEntries();
        cout<<" nEvents ="<<nEvents<<endl;
        //Int_t nEvents=200;
        for(Int_t ievent=0;ievent<nEvents;ievent++ ) {
        if(NofPi > MaxNofPi && NofEl > MaxNofEl) continue;
        CbmTrdTrack *trdtrack=NULL;
        CbmTrdHit *trdhit=NULL;   
        CbmMCTrack* mctrack =NULL;
        
        t1->GetEntry(ievent);
        t->GetEntry(ievent);

        Int_t NofTrdTracks=tracksTRD->GetEntries();
        for(Int_t iTrdTrack=0; iTrdTrack < NofTrdTracks; iTrdTrack++){
           
            CbmTrdTrack* trdtrack = (CbmTrdTrack*)tracksTRD->At(iTrdTrack);
            Int_t nHits = trdtrack->GetNofTrdHits();
            //cout << nHits<<endl;
            if (nHits < 12){
                //cout<<"Less than 12 hits i TRD track #"<< iTrdTrack<<endl;
                continue;
            }

            CbmTrdTrackMatch* match = (CbmTrdTrackMatch*) matchTRD->At(iTrdTrack);
            Int_t iMC = match->GetMCTrackID();
            if (iMC == -1) continue;
            if(iMC> fListStack->GetEntriesFast())continue;
            
            mctrack=(CbmMCTrack*)fListStack->At(iMC);
            Int_t partPdg = mctrack->GetPdgCode();
            Double_t momMC = mctrack->GetP();
            
            if (partPdg == 50000050) continue;
            
            if(momMC < .5) continue;
            Int_t hitIndex =  trdtrack->GetTrdHitIndex(0);
            trdhit = (CbmTrdHit*) hitsTRD->At(hitIndex);
            Int_t iPoint = trdhit->GetRefIndex();        
            if ( iPoint < 0 )continue;         
            CbmMCPoint* point = (CbmMCPoint*) pointsTRD->At(iPoint);            
            TVector3 mom;
            point->Momentum(mom);
            if ( mom.Mag() < .5 ) continue;
            
            for (Int_t iHit=0; iHit<12; iHit++){
                Int_t hitIndex =  trdtrack->GetTrdHitIndex(iHit);
                trdhit = (CbmTrdHit*) hitsTRD->At(hitIndex);
                losshit =  (trdhit->GetELoss());
                inVector[iHit] = losshit*1e6;
                de[iHit] = losshit*1e6;
            } //iHit

            ANNCoef1 = 1.67;
            ANNCoef2 = 0.71;
            
            //------------------transform Data BEGIN--------------------------
            for (Int_t j = 0; j<inVector.size();j++) {
               // inVector[j]=inVector[j]*1000000;
                inVector[j]=(inVector[j]-ANNCoef1)/ANNCoef2 -0.225;
            }
            sort(inVector.begin(), inVector.end());
            for (Int_t j = 0; j<inVector.size();j++)
                inVector[j]=TMath::LandauI(inVector[j]);
            //------------------transform Data END----------------------------

            if (partPdg == 211 || partPdg == -211){
            if (NofPi > MaxNofPi) continue;
                NofPi++;
                x13 = -1.;
                simu->Fill();
            }
            if (partPdg == 11 || partPdg == -11) {
                if(NofEl> MaxNofEl ) continue;
                NofEl++;
                x13 = 1.;
                simu->Fill();
            }
            cout << "NofPi = " << NofPi << " NofEl = " << NofEl << endl;
            
        }
        } //event
        f->Close();
        f1->Close();
    }//iFile
    cout<<"Start training"<< endl;
    TMultiLayerPerceptron fNN("x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12:20:x13",simu,"(Entry$+1)");
    //fNN.LoadWeights("Neural_Net_Weights_El_ID.txt");
    fNN.Train(50,"text,graph,update=1");
    fNN.DumpWeights("Neural_Net_Weights_El_ID_mom_1.txt");
}
