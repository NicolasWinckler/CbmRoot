/*
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "Riostream.h"
#include "TFolder.h"
#include  "FairMCEventHeader.h"
#include "CbmZdcPoint.h"

*/
void analPSD ()
{
  
  TStopwatch timer;
  timer.Start();
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libZdc");


   
  Float_t edep[120][60];
  Float_t  energy, income;
  TVector3 position;
  TLorentzVector momentum;
  Float_t de[12][10][60];
  Float_t fullEdep=0, full=0;
  
  TH2F * hLayer = new TH2F("hLayer"," XY",12,0,12,10,0,10);
  TH2F * hXY = new TH2F ("hXY"," XY ",130,-60,70, 100, -50,50);
  TH1F * hZ = new TH1F ("hZ"," Z ",480,1500,1620);
  TH2F *hEB = new TH2F("hEB"," E vs B",1000,0,20, 1000,0,200);

  for (Int_t ifile=0; ifile<1; ifile++)
    {
      
      //    TString numFile = "000";
      //     numFile +=ifile;
      
         TString infilename = Form("/d/cbm04/alla/zdcJun08/g3/auau.25gev.mbias.%.4i.root", ifile); 
	 //  TString infilename = Form("/d/cbm04/alla/zdcJun08/g3/Stsauau.25gev.mbias.%.4i.root", ifile);
	 // TString infilename = "zdcSts.mc.root";
      cout<<infilename<<endl;
      TFile *f = new TFile(infilename,"R");
      
      TTree* t = f->Get("cbmsim"); 
      //   t->Print();
      TFolder *fd = f->Get("cbmroot");
      fd->ls();
      TClonesArray *listZDCpts = (TClonesArray*) fd->FindObjectAny("ZDCPoint");
      t->SetBranchAddress(listZDCpts->GetName(),&listZDCpts);
      //  chain->SetBranchAddress(listZDCpts->GetName(),&listZDCpts);
      FairMCEventHeader *fHeader;  
      t->SetBranchAddress("MCEventHeader.",&fHeader); 
      TClonesArray *fMCtrack = (TClonesArray*) fd->FindObjectAny("MCTrack");
      t->SetBranchAddress("MCTrack",&fMCtrack);
       
      Int_t fNtrack=0;
      cout<<" entries "<<t->GetEntries()<<endl;
      //  Int_t nEvents=chain->GetEntries();
      Int_t nEvents=t->GetEntries();
      
      for(Int_t i=0;i<nEvents;i++ ) 
	//     cout<<" event "<<i<<endl;
      //   for(Int_t i=0;i<200;i++ ) 
	{
	  //	  cout<<" event "<<i<<endl;
	  
	  fullEdep=0;
	  memset(edep,0,6480*sizeof(Float_t));
	  CbmZdcPoint *point_zdc=NULL;
	  //   chain->GetEntry(i); 
	  t->GetEntry(i); 
	  Double_t  imp = fHeader->GetB();
	  

	  for (int j=0;j<listZDCpts->GetEntries();j++ ) {

	    point_zdc = (CbmZdcPoint*) listZDCpts->At(j);
	    if (point_zdc) {
            Int_t id = point_zdc->GetTrackID();
	      Int_t detID=point_zdc->GetDetectorID();
	      Int_t modID=point_zdc->GetModuleID();
	      edep[modID][detID] += point_zdc->GetEnergyLoss();
	      point_zdc->Position(position);
	      hXY->Fill(position.X(),position.Y());
	      hZ->Fill(position.Z());
	    }
	  }// loop over MC point
       
	  
	  full=0;	  
	  fullEdep=0;
	  Int_t k=1;
	  for (Int_t il=0; il<108; il++)
	    {
	      for (Int_t im=0; im<60; im++)
		{
		  Int_t raw=il/12;
		  Int_t colmn=il%12;
		  
		  fullEdep += edep[il][im];
		  full += edep[il][im];
		  if(im%6 == 0) 
		    {
		      hLayer->Fill(colmn,raw,fullEdep);
		      fullEdep = 0; 
		      k++;
		    }
		  
		}
	    }
	  hEB->Fill(imp,full);
	  
	  
	  
	} // loop over event 
   
      
      f->Close();
    }
  //   timer.Stop();
  //   outfile->Write();
  
  //  TString outfilename = "urqmd.g3.dosa.newgeo.root";
  TString outfilename = "test.root";
  TFile *outfile = new TFile(outfilename,"RECREATE");
  hLayer->Write();
  hXY->Write();
  hEB->Write();   
  hZ->Draw();
}
