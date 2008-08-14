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
#include  "CbmMCEventHeader.h"
#include "CbmZdcPoint.h"

*/
void dosa ()
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
  TH1F * hModule = new TH1F("hModule"," Z",60,0,60);
  TH1F *hEfull = new TH1F("hEfull"," ",1000,0,200);
  TH1F *hEscin = new TH1F("hEscin"," ",1000,0,1000);
  TH2F * hXY = new TH2F ("hXY"," XY ",130,-60,70, 100, -50,50);
  TH1F * hZ = new TH1F ("hZ"," Z ",480,1500,1620);
  TH1F * hRapidity = new TH1F("hRapidity","rapidity",100,0,10);
  TH2F *hEB = new TH2F("hEB"," E vs B",1000,0,20, 1000,0,200);

  TH2F *hNB = new TH2F("hNB"," N vs B",1000,0,20, 200,0,200);
  TH2F *hEN = new TH2F("hEN"," E vs Nspect",200,0,200, 200,0,200);
  TH2F *hdNworse = new TH2F("hdNworse"," dNworse",1000,0,200,500,-1,-0.95);
  TH2F *hENworse = new TH2F("hENworse"," E vs NspectWorse",200,0,200, 1000,0,5);
  TProfile *hNE = new TProfile("hNE"," Nspec vs Efull",200,0,200);
  TProfile *hNworseE = new TProfile("hNworseE"," NspecWorse vs Efull",1000,0,5);
  TProfile *hBE = new TProfile("hBE"," E vs B",1000,0,200);

 
  TH2F *hNSpecNworse = new TH2F("hNspecNworse"," Nworse in calo vs Nspect",200,0,200, 1000,0,5);

  TH2F * hDosa[10];
  for (Int_t i =0; i<10; i++)
    hDosa[i] = new TH2F (Form("hDosa_%i",i)," Dosa ",12,0,12, 60, 0,60);
  /*  
  TH1F *hE[12][10][10];
  for (Int_t ic=0; ic<12; ic++){
    for (Int_t ir=0; ir<10; ir++) {
      for (Int_t ihist=0; ihist<10; ihist++) {
	
	hE[ic][ir][ihist] = new TH1F(Form("hE_%i_%i_%i",ic,ir,ihist)," ",1000,0,1);
      }
    }
  }
  */
  for (Int_t ifile=0; ifile<10; ifile++)
    {
      
      //    TString numFile = "000";
      //     numFile +=ifile;
      
      //  CbmRootManager *fManger = CbmRootManager::Instance();
      //   TString infilename = Form("/d/cbm04/alla/zdcJun08/g3/auau.25gev.mbias.%.4i.root", ifile); 
      //   TString infilename = Form("/d/cbm04/alla/zdcJun08/g3/Stsauau.25gev.mbias.%.4i.root", ifile);
      TString infilename = "zdcSts.mc.root";
      cout<<infilename<<endl;
      TFile *f = new TFile(infilename,"R");
      
      TTree* t = f->Get("cbmsim"); 
      //   t->Print();
      TFolder *fd = f->Get("cbmroot");
      fd->ls();
      TClonesArray *listZDCpts = (TClonesArray*) fd->FindObjectAny("ZDCPoint");
      t->SetBranchAddress(listZDCpts->GetName(),&listZDCpts);
      //  chain->SetBranchAddress(listZDCpts->GetName(),&listZDCpts);
      CbmMCEventHeader *fHeader;  
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
	  full =0 ;
	  memset(edep,0,6480*sizeof(Float_t));
	  CbmZdcPoint *point_zdc=NULL;
	  //   chain->GetEntry(i); 
	  t->GetEntry(i); 
	  Double_t  imp = fHeader->GetB();
	  
	  Int_t nSpec=0;
	  Int_t nSpecInside=0;
	  //	  Float_t enSpec=0;
	  
	  for (Int_t it=0; it<fMCtrack->GetEntries(); it++)
	    {
	      CbmMCTrack* part=(CbmMCTrack*)fMCtrack->At(it);
	      if (part->GetMotherId() = = -1 ) {
		Int_t partPdg = part->GetPdgCode();
		
		if(partPdg == 2112 || partPdg == 2212 )
		  {
		    part->Get4Momentum(momentum);
		    hRapidity->Fill(momentum.Rapidity() );
		    if( momentum.Rapidity() >3.6 )  nSpec++;
		    
		  }
	      }
	    }
	  
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
	      //	      cout<<" X "<<position.X()<<" Y "<<position.Y()<<" mod "<<modID<<" det "<< detID<<endl;
	    }
	  }// loop over MC point
       
	  
	  full = 0;
	  fullEdep=0;
	  Int_t k=1;
	  for (Int_t il=0; il<108; il++)
	    {
	      for (Int_t im=0; im<60; im++)
		{
		  Int_t raw=il/12;
		  Int_t colmn=il%12;
		  hLayer->Fill(colmn,raw,edep[il][im]);
		  //	  cout<<il<<" lay "<<im<<" colmn "<<colmn<<" raw "<<raw<<" e "<<edep[il][im]<<" k "<<k<<endl;
		  //	      de[colmn][raw][lay] = edep[im];
		  if(edep[il][im]>0) { 
		    hDosa[raw]->Fill(colmn, im, 1000.*edep[il][im]); 
		    //		    cout<<im<<" lay "<<il<<" colmn "<<colmn<<" raw "<<raw<<" e "<<1000.*edep[il][im]<<endl;
		  }
		  hModule->Fill(im,edep[il][im]);
		  if(edep[il][im]>0 )  hEscin->Fill(edep[il][im]*1000.);
		  fullEdep +=edep[il][im];
		  full += edep[il][im];

		  /*		  if(im%6 == 0 && raw == 0 &&  colmn == 0) 
		    { 
		      hE[colmn][raw][k-1]->Fill(fullEdep);
		      fullEdep = 0; 
		      k++;
		    }
		  */
		  //	      cout<<"after "<<im<<" lay "<<lay<<" colmn "<<colmn<<" raw "<<raw<<" e "<<edep[im]<<" k "<<k<<endl;
		}
	    }
	  hEB->Fill(imp,full);
	  hBE->Fill(full,imp);
	  Float_t nSpecworse = full/25;
	  cout<<" full "<<full<<" imp "<<imp<<" nspec "<<nSpec<<" nSpecworse "<<nSpecworse<<" inside "<<nSpecInside<<endl;
	  hEfull->Fill(full);
	  hdNworse->Fill(full, (nSpecworse- Float_t(nSpec))/ Float_t(nSpec));
	  hNSpecNworse->Fill(nSpec,nSpecworse);
	  hENworse->Fill(full,nSpecworse);
	  hNworseE->Fill(nSpecworse,full);
	  //	  cout<<"imp "<<imp<<" nSpec "<<nSpec<<endl;
	  hNB->Fill(imp,Float_t(nSpec));
	  hNE->Fill(full, Float_t(nSpec));
	  hEN->Fill(Float_t(nSpec), full);
	  //	  //     OutTree->Fill();
	  
	} // loop over event 
   
      
      f->Close();
    }
  //   timer.Stop();
  //   outfile->Write();
  
  //  TString outfilename = "urqmd.g3.dosa.newgeo.root";
  TString outfilename = "test.root";
  TFile *outfile = new TFile(outfilename,"RECREATE");
  hLayer->Scale(1./Float_t(nEvents));
  hLayer->Write();
  hModule->Scale(1./Float_t(nEvents));
  hModule->Write();
  hEscin->Scale(1./Float_t(nEvents));
  hEscin->Write();
   hEfull->Write();
   /*
   for (Int_t icol=0; icol<12; icol++) {
     for (Int_t jraw=0; jraw<10; jraw++) {
       for (i=0; i<10; i++) {
	 hE[icol][jraw][i]->Write();
       }
     }
   }
   */
   for (Int_t i =0; i<10; i++){
     hDosa[i]->Scale(1./Float_t(nEvents));
     hDosa[i]->Write();}
   
   hXY->Write();
   hEB->Write();   
   hBE->Write();   
   hEN->Write();   
   hNE->Write();   
   hNB->Write();
   hdNworse->Write();
   hRapidity->Write();

   hNSpecNworse->Write();
   hENworse->Write();
   hNworseE->Write();
   hZ->Draw();
}
