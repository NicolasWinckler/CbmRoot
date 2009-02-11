{

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
  gSystem->Load("libGeom");

//  TFile* f = new TFile("auaumbias.root"); //data/test.mc.root
  TFile* f = new TFile("data/test.mc.root");
  TTree *t=f->Get("cbmsim") ;
  TClonesArray *fT=new TClonesArray("TGeoTrack");

  t->SetBranchAddress("GeoTracks",&fT) ;

 TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
 TCanvas* c1 = new TCanvas("glcanvas", "openGL", 100, 100, 800, 800);
 c1->SetFillColor(10);

 
// geoMan->SetVisLevel(10);   
 geoMan->GetTopVolume()->Draw();
 
 TGeoTrack *tr;
 Int_t color;
 TObjArray *TrList= geoMan->GetListOfTracks(); 
   geoMan->SetAnimateTracks();
  for (Int_t j=0; j< t->GetEntriesFast(); j++)	{
 	t->GetEntry(j);
        Double_t *point;
	for (Int_t i=0; i<fT->GetEntriesFast(); i++)	{
    		tr=(TGeoTrack *)fT->At(i);
		if(tr->GetPDG()==2212){
                  Int_t Np=tr->GetNpoints();
                  FairVTrack *pt = new FairVTrack(Np); 
                  pt->SetLineWidth(2);
                  pt->SetTrack(tr);
                  pt->SetParticle((TParticle *)tr->GetParticle());
                  for (Int_t n=0; n<Np; n++){
                     point=tr->GetPoint(n);
                     pt->SetPoint(n,point[0],point[1],point[2]);
		  }
                   pt->Draw();
		}
	}
  }
}

