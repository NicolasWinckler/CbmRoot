void much_draw_digis(){
  TString inputdir = gSystem->Getenv("VMCWORKDIR");
  TString digiFile = inputdir + "/macro/much/data/much_digi_sector.root";
  Int_t iStationSelected=0;
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  // digi scheme
  TFile* file=new TFile("data/much_digi_sector.root");
  TObjArray* stations = (TObjArray*) file->Get("stations");
  CbmMuchGeoScheme* digiScheme = CbmMuchGeoScheme::Instance();
  digiScheme->Init(stations);

  TFile* fPoints = new TFile("data/Jpsi.auau.25gev.centr.mc.root");
  TTree* tPoints = (TTree*) fPoints->Get("cbmsim");
  printf("Test %i\n",tPoints->GetEntries());
  
  TClonesArray* points = new TClonesArray("CbmMuchPoint");
  tPoints->SetBranchAddress("MuchPoint",&points);
  
  tPoints->GetEntry(0);
  
  TFile* fDigis = new TFile("data/Jpsi.auau.25gev.centr.muchhits.root");
  TTree* tDigis = (TTree*) fDigis->Get("cbmsim");
  printf("Test %i\n",tDigis->GetEntries());
  
  TClonesArray* digis = new TClonesArray("CbmMuchDigi");
  tDigis->SetBranchAddress("MuchDigi",&digis);
  
  tDigis->GetEntry(0);
  printf("nDigis:%i\n",digis->GetEntriesFast());

  for (Int_t i=0;i<digis->GetEntriesFast();i++){
    CbmMuchDigi* digi = (CbmMuchDigi*) digis->At(i);
    Int_t detId        = digi->GetDetectorId();
    Long64_t channelId = digi->GetChannelId();
    Int_t iStation = digiScheme->GetStationIndex(detId);
    Int_t iLayer   = digiScheme->GetLayerIndex(detId);
    Int_t iSide    = digiScheme->GetLayerSideIndex(detId);
//
    if (iStation!=iStationSelected || iLayer!=0) continue;

//    printf("detId=%5i channelId=%i ",detId,channelId);
//    printf("iStation=%i iLayer=%i iSide=%i\n",iStation,iLayer,iSide);
    CbmMuchModuleSector* module = (CbmMuchModuleSector*) digiScheme->GetModuleByDetId(detId);
//    module->DrawModule(kRed);
    CbmMuchRadialSector* sector = module->GetSector(channelId);
    Int_t iSector  = module->GetSectorIndex(channelId);
    Int_t iChannel = module->GetChannelIndex(channelId);
    printf("iSector=%i  ",iSector);
    printf("iChannel=%i\n",iChannel);
    sector->SetFillColor(kBlue);
//    sector->Draw("f");
    if (iChannel>=sector->GetNChannels()) printf("Warning\n");
    CbmMuchRadialPad* pad = sector->GetPad(iChannel);
    if (!pad) continue; 
    pad->SetFired(i,digi->GetADCCharge());
//    printf("iPad=%i\n",pad);
//    if (pad) pad->Draw("f");
 
  }

  // Draw pads
  TCanvas* c1 = new TCanvas("station","station",1000,1000);
  c1->SetFillColor(0);
  c1->Range(-80,-80,80,80);
  CbmMuchStation* station = (CbmMuchStation*) stations->At(iStationSelected);
  CbmMuchLayer* layer = station->GetLayer(0);
  for (Int_t iSide=1;iSide>=0;iSide--){
    CbmMuchLayerSide* layerSide = layer->GetSide(iSide);
    for (Int_t iModule=0;iModule<layerSide->GetNModules();++iModule) {
      CbmMuchModule* mod = layerSide->GetModule(iModule);
      mod->SetFillStyle(0);
//        mod->Draw();
      if(mod->GetDetectorType() != 3) continue;
      CbmMuchModuleSector* module = (CbmMuchModuleSector*)mod;
      for (Int_t iSector=0;iSector<module->GetNSectors();++iSector){
        CbmMuchRadialSector* sector = module->GetSector(iSector);
//        sector->Draw("f");
        sector->DrawPads();
      } // sectors
    } // modules
  } // sides

  // Draw points
  for (Int_t i=0;i<points->GetEntriesFast();i++){
    CbmMuchPoint* point = (CbmMuchPoint*) points->At(i);
    Int_t detId = point->GetDetectorId();
    Int_t iStation = digiScheme->GetStationIndex(detId);
    Int_t iLayer   = digiScheme->GetLayerIndex(detId);
    Int_t iSide    = digiScheme->GetLayerSideIndex(detId);
    if (iStation!=iStationSelected || iLayer!=0) continue;
    Double_t x = 0.5*(point->GetXIn()+point->GetXOut());
    Double_t y = 0.5*(point->GetYIn()+point->GetYOut());
    TEllipse* p = new TEllipse(x,y,0.2);
    p->SetFillColor(kBlack);
    p->SetLineColor(kBlack);
    p->Draw();
    TLine* l = new TLine(point->GetXIn(),point->GetYIn(),point->GetXOut(),point->GetYOut());
    l->SetLineColor(kBlack);
    l->Draw();
  }
  
  // Draw a hole
  TArc* holeArc = new TArc(0.,0.,station->GetRmin());
  holeArc->Draw();
}
