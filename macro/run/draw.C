{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
 

  gSystem->Load("libGeom.so");
  TFile* f = new TFile("auaumbias.root");

  TTree *t=f->Get("cbmsim") ;


  TClonesArray *fT=new TClonesArray("TGeoTrack");

  t->SetBranchAddress("GeoTracks",&fT) ;

 TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
 TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
 c1->SetFillColor(10);

 geoMan->GetTopVolume()->Draw("same");
 TGeoTrack *tr;
 TGeoTrack *tr1;

 for (Int_t j=0; j< t->GetEntriesFast(); j++)	{
 	t->GetEntry(j);
	for (Int_t i=0; i<fT->GetEntriesFast(); i++)	{
    		tr=(TGeoTrack *)fT->At(i);
     		tr->Draw("same");
	}

 }

}

