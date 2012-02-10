#include "CbmTrdHitRateTest.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TColor.h"

#include "TH2.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TLine.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TPRegexp.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
#include "CbmTrdStringToken.h"
#endif

#include <vector>
#include <iostream>
#include "Riostream.h"
#include <fstream>
#include <cmath>

#define winsize 6000

using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::vector;


// ---- Default constructor -------------------------------------------
CbmTrdHitRateTest::CbmTrdHitRateTest()
  : FairTask("TrdCluster"),
    Digicounter(-1),
    tFile(NULL),
    fDraw(kFALSE),
    fStation(-1),
    fLayer(-1),
    fModuleType(-1),
    fModuleCopy(-1),
    fCol_mean(-1),
    fCol_in(-1),
    fCol_out(-1),
    fRow_mean(-1),
    fRow_in(-1),
    fRow_out(-1),
    fModuleID(-1),
    fMCindex(-1),
    local_meanLL(),
    local_meanC(),
    global_meanLL(),
    global_meanC(),
    local_inLL(),
    local_inC(),
    global_inLL(),
    global_inC(),
    local_outLL(),
    local_outC(),
    global_outLL(),
    global_outC(),
    fx_in(0.), 
    fx_out(0.), 
    fy_in(0.), 
    fy_out(0.), 
    fz_in(0.), 
    fz_out(0.), 
    fx_mean(0.), 
    fy_mean(0.), 
    fz_mean(0.),
    fSector(-1),
    padsize(),
    modulesize(),
    sectorsize(),
    sectorrows(),
    fELoss(-1.),
    fELossdEdX(-1.),
    fELossTR(-1.),
    fPosXLL(0.),
    fPosYLL(0.),
    fPadPosxLL(0.), 
    fPadPosyLL(0.),
    fPadPosxC(0.), 
    fPadPosyC(0.),
    fDeltax(0.), 
    fDeltay(0.),
    fPadCharge(),
    fPRFHitPositionLL(0.), 
    fPRFHitPositionC(0.),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(NULL),
    fDigiMatchCollection(NULL),
    fMCStacks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiators(NULL),
    fTrdId(),
    fDigiMap(),
    fDigiMapIt()
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdHitRateTest::CbmTrdHitRateTest(const char *name, const char *title,
                 CbmTrdRadiator *radiator)
  : FairTask(name),
    Digicounter(-1),
    tFile(NULL),
    fDraw(kFALSE),
    fStation(-1),
    fLayer(-1),
    fModuleType(-1),
    fModuleCopy(-1),
    fCol_mean(-1),
    fCol_in(-1),
    fCol_out(-1),
    fRow_mean(-1),
    fRow_in(-1),
    fRow_out(-1),
    fModuleID(-1),
    fMCindex(-1),
    local_meanLL(),
    local_meanC(),
    global_meanLL(),
    global_meanC(),
    local_inLL(),
    local_inC(),
    global_inLL(),
    global_inC(),
    local_outLL(),
    local_outC(),
    global_outLL(),
    global_outC(),
    fx_in(0.), 
    fx_out(0.), 
    fy_in(0.), 
    fy_out(0.), 
    fz_in(0.), 
    fz_out(0.), 
    fx_mean(0.), 
    fy_mean(0.), 
    fz_mean(0.),
    fSector(-1),
    padsize(),
    modulesize(),
    sectorsize(),
    sectorrows(),
    fELoss(-1.),
    fELossdEdX(-1.),
    fELossTR(-1.),
    fPosXLL(0.),
    fPosYLL(0.),
    fPadPosxLL(0.), 
    fPadPosyLL(0.),
    fPadPosxC(0.), 
    fPadPosyC(0.),
    fDeltax(0.), 
    fDeltay(0.),
    fPadCharge(),
    fPRFHitPositionLL(0.), 
    fPRFHitPositionC(0.),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(NULL),
    fDigiMatchCollection(NULL),
    fMCStacks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiators(radiator),
    fTrdId(),
    fDigiMap(),
    fDigiMapIt()
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdHitRateTest::~CbmTrdHitRateTest()
{
  //    FairRootManager *ioman =FairRootManager::Instance();
  //ioman->Write();
  fDigiCollection->Clear("C");
  delete fDigiCollection;
  fDigiMatchCollection->Clear("C");
  delete fDigiMatchCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitRateTest::SetParContainers()
{
    cout<<" * CbmTrdHitRateTest * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitRateTest::ReInit(){

  cout<<" * CbmTrdHitRateTest * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitRateTest::Init()
{

    cout<<" * CbmTrdHitRate * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
    fTrdPoints=(TClonesArray *)ioman->GetObject("TrdPoint"); 
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdCluster::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fMCStacks = (TClonesArray*)ioman->GetObject("MCTrack");
  if ( ! fMCStacks ) {
      cout << "-W CbmTrdCluster::Init: No MCTrack array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fDigiCollection = new TClonesArray("CbmTrdDigi", 100);
    ioman->Register("TrdDigi","TRD Digis",fDigiCollection,kTRUE);

    fDigiMatchCollection = new TClonesArray("CbmTrdDigiMatch", 100);
    ioman->Register("TrdDigiMatch","TRD Digis",fDigiMatchCollection,kTRUE);

    fRadiators->Init();

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdHitRateTest::Exec(Option_t * option)
{
  /*
  const Int_t Number = 11;
  Double_t r[Number] =      {1.00, 0.50, 0.00, 0.00, 0.00, 0.00, 0.00, 0.50, 1.00, 0.50, 1.00};
  Double_t g[Number] =      {0.00, 0.00, 0.00, 0.50, 1.00, 1.00, 1.00, 1.00, 1.00, 0.50, 1.00};
  Double_t b[Number] =      {1.00, 1.00, 1.00, 1.00, 1.00, 0.50, 0.00, 0.00, 0.00, 0.50, 1.00};
  Double_t length[Number] = {0.00, 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00};
  Int_t nb = 99;
  Int_t a = TColor::CreateGradientColorTable(Number,length,r,g,b,nb);
  */
  printf("Introduction:\n");
  HitRateGeoPara *GeoPara = new HitRateGeoPara;
  Bool_t Lines;
  Bool_t Fast = false;
  //  Bool_t Fast = false;
  Bool_t firstLayer = false;
  //  fDraw = false;
  fDraw = true;
  Double_t ZRangeL = 1e00;//1e05;
  Double_t ZRangeU = 1e05;//1e06;
  Double_t mm2bin = 2.5;
 
  fStation = 0;
  fLayer = 0;
  tFile = new TFile("CbmTrdHitRateTest.root","RECREATE"," ROOT file with histograms");

  TH1F* HitPad = NULL;
  TH2F* Layer = NULL;
  TH2F* Topview[3] = {NULL,NULL,NULL};
  TCanvas* c1 = NULL;
  TCanvas* c2 = NULL;
  TCanvas* c0 = NULL;
  if(fDraw) {
    c0 = new TCanvas("c0","c0",1800,450);	
    c0->Divide(3,1);
  }
  Topview[0] = new TH2F("TopView","TopView",    int(2* winsize /mm2bin), -winsize, winsize, int(12000 /mm2bin),    0, 12000);
  Topview[0]->SetXTitle("x-Coordinate [mm]");
  Topview[0]->SetYTitle("z-Coordinate [mm]");
  Topview[0]->SetZTitle("#");
  Topview[1] = new TH2F("FrontView","FrontView",int(2* winsize /mm2bin), -winsize, winsize, int(2* winsize /mm2bin), -winsize, winsize);
  Topview[1]->SetXTitle("x-Coordinate [mm]");
  Topview[1]->SetYTitle("y-Coordinate [mm]");
  Topview[1]->SetZTitle("#");
  Topview[2] = new TH2F("SideView","SideView",  int(12000 /mm2bin),    0, 12000, int(2* winsize /mm2bin), -winsize, winsize);
  Topview[2]->SetXTitle("z-Coordinate [mm]");
  Topview[2]->SetYTitle("y-Coordinate [mm]");
  Topview[2]->SetZTitle("#");
  for (Int_t i = 0; i < 3; i++) {
    Topview[i]->SetContour(99);
    Topview[i]->SetStats(kFALSE);
    Topview[i]->GetXaxis()->SetLabelSize(0.02);
    Topview[i]->GetYaxis()->SetLabelSize(0.02);
    Topview[i]->GetZaxis()->SetLabelSize(0.02);
    Topview[i]->GetXaxis()->SetTitleSize(0.02);
    Topview[i]->GetXaxis()->SetTitleOffset(1.5);
    Topview[i]->GetYaxis()->SetTitleSize(0.02);
    Topview[i]->GetYaxis()->SetTitleOffset(2);
    Topview[i]->GetZaxis()->SetTitleSize(0.02);
    Topview[i]->GetZaxis()->SetTitleOffset(-2);
    if(fDraw) {
      c0->cd(i+1);
      Topview[i]->Draw("colz");
    }
  }
  //Topview->GetZaxis()->SetRangeUser(ZRangeL,ZRangeU);


  vector<int> L1S1;
  vector<int> L2S1;
  vector<int> L3S1;
  vector<int> L4S1;
  vector<int> L1S2;
  vector<int> L2S2;
  vector<int> L3S2;
  vector<int> L4S2;
  vector<int> L1S3;
  vector<int> L2S3;
  vector<int> L3S3;
  vector<int> L4S3;

  Int_t ModuleID;
  Int_t Sector = 0;
  const Int_t NofModules = fDigiPar->GetNrOfModules();
  //TH2F *Module[NofModules];
  for (Int_t i = 0; i < NofModules; i++) {  
    ModuleID = fDigiPar->GetModuleId(i);
    // TString name;
    //name.Form("Module%d",ModuleID);
    //Module[i] = new TH2F(name,name,1000,-500,500,1000,-500,500);
    fModuleInfo = fDigiPar->GetModule(ModuleID);
    //printf("%d:\n  %.1f %.1f %.1f\n",ModuleID,fModuleInfo->GetX(), fModuleInfo->GetY(),fModuleInfo->GetZ());
    GetModuleInformationSL(ModuleID);
    FillVector(Fast, ModuleID, L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3);
    //cout << i << " " << ModuleID << endl;
  }
  vector< vector<int> > LiSi;
  LiSi.push_back(L1S1);
  LiSi.push_back(L2S1);
  LiSi.push_back(L3S1);
  LiSi.push_back(L4S1);
  LiSi.push_back(L1S2);
  LiSi.push_back(L2S2);
  LiSi.push_back(L3S2);
  LiSi.push_back(L4S2);
  LiSi.push_back(L1S3);
  LiSi.push_back(L2S3);
  LiSi.push_back(L3S3);
  LiSi.push_back(L4S3);

  Char_t Canfile1[200];
  Char_t Canfile2[200];

  TImage *Outimage1;
  TImage *Outimage2;
  TLine* border = new TLine(1e05,0,1e05,1e04);
  border->SetLineColor(2);
  /*
    Layer  = new TH2F("dummy1","dummy1",1,-0.5,0.5,1,-0.5,0.5);
    HitPad = new TH1F("dummy2","dummy2",1,-0.5,0.5);
    c1 = new TCanvas("dummy3","dummy3",10,10);
    c2 = new TCanvas("dummy4","dummy4",10,10);
  */
  //**************************************************************************************
  fStation = 1;
  fLayer = 1;
  for (vector< vector<int> >::size_type j = 0; j < LiSi.size(); j++)
    {
      Int_t NumberOfLayerPerStation = LiSi[j].size();
 
      Topview[0]->Reset();
      Topview[1]->Reset();
      Topview[2]->Reset();

      sprintf(Canfile1,"pics/HitPerPadFrontView_S%d_L%d.png",fStation,fLayer);
      sprintf(Canfile2,"pics/HitPerPadSpectra_S%d_L%d.png",fStation,fLayer);

      HistoInit(c1, c2,  Layer, HitPad, Canfile1, Canfile2, ZRangeL, ZRangeU, mm2bin);
      if(fDraw)
	border->Draw("same");
      Lines = false;
      for (vector<int>::size_type i = 0; i < NumberOfLayerPerStation; i++)
	{	 
	  GetModuleInformationFromDigiPar(GeoPara, Fast, Lines, LiSi[j][i], Layer ,c1, Canfile1, HitPad, c2, Topview, c0, mm2bin);
	}
      if(fDraw){
	c1->cd(1)->SetLogz(1);
	Layer->Draw("colz");
      }
      Lines = true;
      for (vector<int>::size_type i = 0; i < NumberOfLayerPerStation; i++)
	{
	  GetModuleInformationFromDigiPar(GeoPara, Fast, Lines, LiSi[j][i], Layer ,c1, Canfile1, HitPad, c2, Topview, c0, mm2bin);
	}
      if(fDraw){
	Outimage1 = TImage::Create();
	Outimage1->FromPad(c1);
	Outimage1->WriteImage(Canfile1);
      }
      /*
	sprintf(Canfile1,"pics/%s_S%d_L%d.eps",trddigiparpath,fStation,fLayer);
	c1->cd(1)->Print(Canfile1);
      */
      delete Layer;

      if(fDraw){
	delete c1;
	delete Outimage1;   
	HitPad->Draw("same");      
	Outimage2 = TImage::Create();
	Outimage2->FromPad(c2);
	Outimage2->WriteImage(Canfile2);
      }
      /*
	sprintf(Canfile2,"pics/%s_HitPerPad_S%d_L%d.eps",trddigiparpath,fStation,fLayer);
	c2->cd(1)->Print(Canfile2);
      */
      delete HitPad;

      if(fDraw){
	delete c2;  
	delete Outimage2;
      }
      if (fLayer == 4)
	{
	  fStation++;
	  fLayer = 1;
	}
      else
	{
	  fLayer++;
	}  
      if(fDraw)
	c0->Update();
    }
  if(fDraw)
    c0->Update();
  /*
    for (Int_t i = 0; i < NofModules; i++)
    Module[i]->Write("", TObject::kOverwrite);
  */
  tFile->Close();
}
void CbmTrdHitRateTest::HistoInit(TCanvas*& c1, TCanvas*& c2,TH2F*& Layer,TH1F*& HitPad, Char_t* Canfile1, Char_t* Canfile2, Double_t ZRangeL, Double_t ZRangeU, Double_t mm2bin)
{
  Char_t name[50];
  Char_t title[50];

  sprintf(name,"S%d_L%d",fStation,fLayer);
  sprintf(title,"Station %d, Layer %d",fStation,fLayer);
  cout << title << endl;
  //sprintf(Canfile1,"pics/Station%dLayer%d.png",fStation,fLayer);
  //sprintf(Canfile2,"pics/HitPerPadStation%dLayer%d.png",fStation,fLayer);
  Layer = new TH2F(name,title,int(2* winsize /mm2bin), -winsize, winsize, int(2* winsize /mm2bin), -winsize, winsize);
  Layer->SetContour(99);
  Layer->SetXTitle("x-Coordinate [mm]");
  Layer->SetYTitle("y-Coordinate [mm]");
  Layer->SetZTitle("Hits/Pad [Hz]");
  Layer->SetStats(kFALSE);
  Layer->GetXaxis()->SetLabelSize(0.02);
  Layer->GetYaxis()->SetLabelSize(0.02);
  Layer->GetZaxis()->SetLabelSize(0.02);
  Layer->GetXaxis()->SetTitleSize(0.02);
  Layer->GetXaxis()->SetTitleOffset(1.5);
  Layer->GetYaxis()->SetTitleSize(0.02);
  Layer->GetYaxis()->SetTitleOffset(2);
  Layer->GetZaxis()->SetTitleSize(0.02);
  Layer->GetZaxis()->SetTitleOffset(-2);
  Layer->GetZaxis()->SetRangeUser(ZRangeL,ZRangeU);

  sprintf(name,"HP_S%d_L%d",fStation,fLayer);
  sprintf(title,"HitPad_Station %d, Layer %d",fStation,fLayer);
  HitPad = new TH1F(name,title,10000,1e00,1e06);
  HitPad->SetXTitle("Hits/Pad [Hz]");
  HitPad->SetYTitle("count");
  HitPad->GetYaxis()->SetRangeUser(1,1e04);

  sprintf(name,"c1_S%d_L%d",fStation,fLayer);
  sprintf(title,"c1 Station %d, Layer %d",fStation,fLayer);
  if(fDraw){
    c1 = new TCanvas(name,title,1000,900);	
    c1->Divide(1,1);
    c1->cd(1)->SetLogz(1);
    Layer->Draw();
  }
  sprintf(name,"c2_S%d_L%d",fStation,fLayer);
  sprintf(title,"c2 Station %d, Layer %d",fStation,fLayer);
  if(fDraw){
    c2 = new TCanvas(name,title,1000,900/2);	
    c2->Divide(1,1);
    c2->cd(1)->SetLogx(1);
    c2->cd(1)->SetLogy(1);
    c2->cd(1)->SetGridx(1);
    c2->cd(1)->SetGridy(1);  
    HitPad->Draw();
  }
}

  // --------------------------------------------------------------------

  // ---- FinishTask-----------------------------------------------------
void CbmTrdHitRateTest::FinishEvent()
{
  fDigiMap.clear();
  if ( fDigiCollection ) fDigiCollection->Clear();
  if ( fDigiMatchCollection ) fDigiMatchCollection->Clear();
}

  // --------------------------------------------------------------------
  // ----GetModuleInformation ------------------------------------------
  void CbmTrdHitRateTest::GetModuleInformation()
  {
    // Extract the information about station, layer, module type
    // and cpoy number of the module from the full path to the
    // node.
    // The full path is tokenized at the "/" which diide the different
    // levels of the geometry.
    // Knowing the nameing scheme of the volumes one gets the required
    // information with simple string manipulation.
    // This is probably not the fastes way, but the speed has to be checked.
    // The methode works only for versions of Root > 5.20.0, before the
    // class TStringTocken is not implemented

    TString path = gGeoManager->GetPath();
    cout<<"Path: "<<path<<endl;
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,20,0)
    TStringToken* bla = new TStringToken(path,"/");
#else
    CbmTrdStringToken* bla = new CbmTrdStringToken(path,"/");
#endif

    while (bla->NextToken()) {
      if (bla->Contains("layer")) {
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3((bla3.Last('_')+1),substringLength);
	TString bla1 = bla3(3,1);
	fStation=bla1.Atoi();
	fLayer=bla2.Atoi();
      }
      if (bla->Contains("mod")){
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3(pos+1,substringLength);
	substringLength=pos-7;
	TString bla1 = bla3(7,substringLength);     
	fModuleType = bla1.Atoi();
	fModuleCopy = bla2.Atoi();
	break;
      } 
    }

  }
void CbmTrdHitRateTest::GetModuleInformationSL(Int_t VolumeID)
{
      Int_t* detInfo = fTrdId.GetDetectorInfo(VolumeID); 
      fStation    = detInfo[1];
      fLayer      = detInfo[2];     
      fModuleType = detInfo[3];
      fModuleCopy = detInfo[4];
      //printf(" (S%d L%d)  ",fStation,fLayer);
}
  // --------------------------------------------------------------------
  // ----GetModuleInformationFromDigiPar ------------------------------------------
void CbmTrdHitRateTest::GetModuleInformationFromDigiPar(HitRateGeoPara *GeoPara, Bool_t Fast, Bool_t Lines, Int_t VolumeID, TH2F* Layer, TCanvas* c1, Char_t* Canfile1, TH1F* HitPad, TCanvas* c2, TH2F* Topview[3], TCanvas* c0, Double_t mm2bin)
{
  // fPos is >0 for x and y and not rotated
  // origin of the local coordinate system in 
  // the lower left corner of the chamber, 
  // x to the right (small side of the pads), y up
  //cout << "GetModuleInformationFromDigiPar" << endl;
  fModuleInfo = fDigiPar->GetModule(VolumeID);
  if (fModuleInfo != NULL)
    {
      Int_t detID = fModuleInfo->GetDetectorId();

      if (detID != VolumeID ){
	cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
      }
      Int_t nRow = 0;
      Int_t nCol = 0;
      Double_t Mpos[3];
      Mpos[0] = fModuleInfo->GetX() * 10;
      Mpos[1] = fModuleInfo->GetY() * 10;
      Mpos[2] = fModuleInfo->GetZ() * 10; // == z(station) ??
      
      Double_t Msize[3];
      Msize[0] = fModuleInfo->GetSizex() * 10;
      Msize[1] = fModuleInfo->GetSizey() * 10;
      Msize[2] = 0; 

      const Int_t NoSectors = fModuleInfo->GetNoSectors();
      Int_t nSec = NoSectors;
      Double_t Ssize[3*NoSectors];
      Double_t Psize[3*NoSectors];

      for (Int_t i = 0; i < NoSectors; i++)// i = Sector
	{
	  Ssize[0+i*NoSectors] = fModuleInfo->GetSectorSizex(i) * 10;
	  Ssize[1+i*NoSectors] = fModuleInfo->GetSectorSizey(i) * 10;
	  Ssize[2+i*NoSectors] = 0;
	  
	  Psize[0+i*NoSectors] = fModuleInfo->GetPadSizex(i) * 10;
	  Psize[1+i*NoSectors] = fModuleInfo->GetPadSizey(i) * 10;
	  Psize[2+i*NoSectors] = 0;
	}

      Int_t* detInfo = fTrdId.GetDetectorInfo(VolumeID); 
      fStation    = detInfo[1];
      fLayer      = detInfo[2];     
      fModuleType = detInfo[3];
      fModuleCopy = detInfo[4];
      GeoPara->moduleId  = VolumeID;
      GeoPara->layerId   = detInfo[2];
      GeoPara->stationId = detInfo[1];
      
      //nCol = (Int_t)(2*Msize[0] / (int(Psize[0] * 100) / 100.0));   
      Int_t tempY = 0;
      Int_t tempX = 0;
      for (Int_t i = 0; i < fNoSectors; i++)
	{
	  if (Ssize[0+i*NoSectors] < 2 * Msize[0] && Ssize[0+i*NoSectors] > 0)
	    {
	      tempX += int(Ssize[0+i*NoSectors]/Psize[0+i*NoSectors]); 
	    }
	  else
	    {
	      tempX  = int(Ssize[0+i*NoSectors]/Psize[0+i*NoSectors]); 
	    }
	  if (Ssize[1+i*NoSectors] < 2 * Msize[1] && Ssize[1+i*NoSectors] > 0)
	    {
	      tempY += int(Ssize[1+i*NoSectors]/Psize[1+i*NoSectors]); 
	    }
	  else
	    {
	      tempY  = int(Ssize[1+i*NoSectors]/Psize[1+i*NoSectors]); 
	    }
	}

      TVector3 padPos;
      TVector3 padSize;
      
      for (Int_t s = 0; s < fNoSectors; s++) {
	GeoPara->sCol[s] = 0;
	GeoPara->sRow[s] = 0;
      }
      for (Int_t i = 0; i < 3; i++) {
	GeoPara->mPos[i]  = Mpos[i];
	GeoPara->mSize[i] = Msize[i];
	for (Int_t s = 0; s < fNoSectors; s++){
	  fModuleInfo->GetPosition(0, 0, VolumeID, s, padPos, padSize);
	  GeoPara->pSize[s][i] = padSize[i] * 10;
	  if (i == 0) {
	    GeoPara->sSize[s][i] = fModuleInfo->GetSectorSizex(s) * 10;
	    if (GeoPara->sSize[s][i] < 2 * GeoPara->mSize[i]) {
	      GeoPara->sCol[s] = GeoPara->sSize[s][i] / GeoPara->pSize[s][i];
	    }
	    else {
	      if (s == 0) GeoPara->sCol[s] = GeoPara->sSize[s][i] / GeoPara->pSize[s][i];
	      else        GeoPara->sCol[s] = 0;
	    }
	    GeoPara->nCol += GeoPara->sCol[s];
	  }
	  if (i == 1) {
	    GeoPara->sSize[s][i] = fModuleInfo->GetSectorSizey(s) * 10;
	    if (GeoPara->sSize[s][i] < 2 * GeoPara->mSize[i]) {
	      GeoPara->sRow[s] = GeoPara->sSize[s][i] / GeoPara->pSize[s][i];
	    }
	    else {
	      if (s == 0) GeoPara->sRow[s] = GeoPara->sSize[s][i] / GeoPara->pSize[s][i];
	      else        GeoPara->sRow[s] = 0;
	    }
	    GeoPara->nRow += GeoPara->sRow[s];	    
	  }
	}
      }
      fModuleInfo->GetPosition(0, 0, VolumeID, 0, padPos, padSize);
      GeoPara->vOrigin[0] = padPos[0] * 10;
      GeoPara->vOrigin[1] = padPos[1] * 10; 
      GeoPara->vOrigin[2] = padPos[2] * 10;
      
      fModuleInfo->GetPosition(1, 0, VolumeID, 0, padPos, padSize);

      GeoPara->vX[0]      = padPos[0] * 10 - GeoPara->vOrigin[0]; 
      GeoPara->vX[1]      = padPos[1] * 10 - GeoPara->vOrigin[1];			   
      GeoPara->vX[2]      = padPos[2] * 10 - GeoPara->vOrigin[2];

      fModuleInfo->GetPosition(0, 1, VolumeID, 0, padPos, padSize);

      GeoPara->vY[0]      = padPos[0] * 10 - GeoPara->vOrigin[0]; 
      GeoPara->vY[1]      = padPos[1] * 10 - GeoPara->vOrigin[1]; 
      GeoPara->vY[2]      = padPos[2] * 10 - GeoPara->vOrigin[2];

      GeoPara->vN[0]      = GeoPara->vX[1]*GeoPara->vY[2] - GeoPara->vX[2]*GeoPara->vY[1];
      GeoPara->vN[1]      = GeoPara->vX[2]*GeoPara->vY[0] - GeoPara->vX[0]*GeoPara->vY[2];
      GeoPara->vN[2]      = GeoPara->vX[0]*GeoPara->vY[1] - GeoPara->vX[1]*GeoPara->vY[0];

      GeoPara->lambda = (GeoPara->vOrigin[0] * GeoPara->vN[0] +
			 GeoPara->vOrigin[1] * GeoPara->vN[1] +
			 GeoPara->vOrigin[2] * GeoPara->vN[2]);

      /*x-direction*/
      GeoPara->cosX = (GeoPara->vX[0] * 1 + GeoPara->vX[1] * 0 + GeoPara->vX[2] * 0) / sqrt(pow(GeoPara->vX[0],2) + pow(GeoPara->vX[1],2) + pow(GeoPara->vX[2],2));
      /*y-direction*/
      GeoPara->cosY = (GeoPara->vY[0] * 0 + GeoPara->vY[1] * 1 + GeoPara->vY[2] * 0) / sqrt(pow(GeoPara->vY[0],2) + pow(GeoPara->vY[1],2) + pow(GeoPara->vY[2],2));
      
      GeoPara->stepDirection[0] = GeoPara->vX[0] / fabs(GeoPara->vX[0]); // is the next pad (1,0,0) on the left or right side?
      GeoPara->stepDirection[1] = GeoPara->vY[1] / fabs(GeoPara->vY[1]); // is the next pad (0,1,0) on the upper or lower side?

      
      
      Topview[0]->Fill(GeoPara->mPos[0],GeoPara->mPos[2]);
      Topview[1]->Fill(GeoPara->mPos[0],GeoPara->mPos[1]);
      Topview[2]->Fill(GeoPara->mPos[2],GeoPara->mPos[1]);
      
      nRow = tempY;
      nCol = tempX;
      //cout << nRow << " " << nCol << endl;
      if(Lines)
	{
	  DrawLines( Mpos, Msize,Ssize, Psize, nRow, nCol, nSec, Layer, c1, Topview, c0);
	}
      else
	{
	  Histo(GeoPara, Fast, Mpos, Msize, Ssize, Psize, nRow, nCol, nSec, Layer, c1, Canfile1, HitPad, c2, Topview, c0, mm2bin);
	}
    }
  else
    {
      printf("fModuleInfo == NULL\n");
    }
}


float CbmTrdHitRateTest::CalcHitRate(HitRateGeoPara *GeoPara, Float_t StartX, Float_t StopX, Int_t xSteps, Float_t StartY, Float_t StopY, Int_t ySteps, Double_t* Mpos, TH2F* Topview[3], TCanvas* c0)
{
  //cout << "CalcHitRate" << endl;
  Float_t HitRate = 0;//1. / sqrt( pow( StartX,2) + pow( StartY,2));
  Float_t r = 0;
  Float_t alpha = 0;
  Int_t counter = 0;
  Float_t a[3] = { 7.66582e+00,  6.97712e+00,  6.53780e+00};
  Float_t b[3] = {-2.72375e-03, -1.85168e-03, -1.42673e-03};

  Double_t xStepWidth = GeoPara->cosX;
  Double_t yStepWidth = GeoPara->cosY;
  Double_t x = StartX + 0.5 * GeoPara->cosX; // don't start on the pad border line
  Double_t y = StartY + 0.5 * GeoPara->cosY;
  Double_t z = (GeoPara->lambda - (x * GeoPara->vN[0] + y * GeoPara->vN[1])) / GeoPara->vN[2];
  for (Int_t yStep = 0; yStep < ySteps; yStep++) {
    x = StartX + 0.5 * GeoPara->cosX;
    for (Int_t xStep = 0; xStep < xSteps; xStep++) {
      z = (GeoPara->lambda - (x * GeoPara->vN[0] + y * GeoPara->vN[1])) / GeoPara->vN[2];
      r = sqrt( pow(x, 2) + pow(y,2));
      alpha = atan(r/z)*1000.;
      
      HitRate += 
	exp(4.54156e00 + -8.47377e-03 * alpha) + 
	exp(2.40005e01 + -1.19541e-02 * alpha) /
	(z * z)
	;
      
      Topview[0]->Fill(x,z);
      Topview[1]->Fill(x,y);
      Topview[2]->Fill(z,y);
      
      x += xStepWidth;
    }
    y += yStepWidth;
  }
  return (HitRate/*/(counter/100.)*/);/*Convertes Hits/Pad -> Hits/cm² on each Pad*/
}
void CbmTrdHitRateTest::Histo(HitRateGeoPara *GeoPara, Bool_t Fast, Double_t* Mpos, Double_t* Msize,Double_t* Ssize, Double_t* Psize, Int_t nRow, Int_t nCol, Int_t nSec, TH2F* Layer, TCanvas* c1, Char_t* Canfile1, TH1F* HitPad, TCanvas* c2, TH2F* Topview[3], TCanvas* c0, Double_t mm2bin)
{
  Double_t ZRangeL = 1e00;//1e05;
  Double_t ZRangeU = 1e05;//1e06;
  TString name;
  name.Form("Module%d",GeoPara->moduleId);
  TH2F *Module = new TH2F(name,name,1500/mm2bin,-750,750,1500/mm2bin,-750,750);
  Module->GetZaxis()->SetRangeUser(ZRangeL,ZRangeU);
  //cout << "Histo" << endl;
  //cout << Mpos[2] << endl;
  const Int_t nR = nRow;
  const Int_t nC = nCol;
  Float_t HiteRate = 0;
  Int_t iSecX = 0;
  Int_t iSecY = 0;
  Double_t planeStartX = Mpos[0]-Msize[0];
  Double_t planeStartY = Mpos[1]-Msize[1];
  Double_t planeStopX = planeStartX + GeoPara->pSize[iSecX][0];
  Double_t planeStopY = planeStartY + GeoPara->pSize[iSecX][1];
  Float_t StartX = GeoPara->vOrigin[0] - 0.5 * GeoPara->pSize[iSecX][0] * GeoPara->cosX;// vOrigin points to the center of pad (0,0,0)
  Float_t StartY = GeoPara->vOrigin[1] - 0.5 * GeoPara->pSize[iSecY][1] * GeoPara->cosY;
  Float_t StopX = StartX + GeoPara->pSize[iSecX][0] * GeoPara->cosX;
  Float_t StopY = StartY + GeoPara->pSize[iSecY][1] * GeoPara->cosY;
  Int_t xSteps = 0;
  Int_t ySteps = 0;
  //Int_t xStepDirection = GeoPara->vX[0] / fabs(GeoPara->vX[0]); // is the next pad (1,0,0) on the left or right side?
  //Int_t yStepDirection = GeoPara->vY[1] / fabs(GeoPara->vY[1]); // is the next pad (0,1,0) on the upper or lower side?
  for (Int_t iR = 0; iR < nR; iR++)
    {
      StartX = GeoPara->vOrigin[0] - 0.5 * GeoPara->pSize[iSecX][0] * GeoPara->cosX; 
      StopX  = StartX + GeoPara->pSize[iSecX][0] * GeoPara->cosX;
      planeStartX = GeoPara->mPos[0] - GeoPara->mSize[0];
      planeStopX  = planeStartX + GeoPara->pSize[iSecX][0];
      for (Int_t iC = 0; iC < nC; iC++)
	{
	  xSteps = GeoPara->pSize[iSecX][0] - 1; // 1 Step / mm
	  ySteps = GeoPara->pSize[iSecY][1] - 1;

	  if (Fast)
	    {
	      if (Mpos[0] > -1 && Mpos[1] > -1)
		{
		  HiteRate = CalcHitRate(GeoPara, StartX, StopX, xSteps, StartY, StopY, ySteps, Mpos, Topview, c0);
		  HitPad->Fill(HiteRate);
		  HitPad->Fill(HiteRate);
		  HitPad->Fill(HiteRate);
		  HitPad->Fill(HiteRate);
		}
	    }
	  else
	    {
	      HiteRate = CalcHitRate(GeoPara, StartX, StopX, xSteps, StartY, StopY, ySteps, Mpos, Topview, c0);
	      HitPad->Fill(HiteRate);
	    }
	  Int_t mStepY = Int_t((planeStartY-Mpos[1]));
	  Int_t mStepX = Int_t((planeStartX-Mpos[0]));
	  for (Int_t stepY = int(planeStartY/mm2bin); stepY < int(planeStopY/mm2bin); stepY++)
	    {
	      mStepX = Int_t((planeStartX-Mpos[0]));
	      for (Int_t stepX = int(planeStartX/mm2bin); stepX < int(planeStopX/mm2bin); stepX++)
		{
		  if (Fast)
		    {
		      if (GeoPara->mPos[0]/*Mpos[0]*/ > -1 && GeoPara->mPos[1]/*Mpos[1]*/ > -1)
			{
			  /*
			    Layer->Fill(     stepX*mm2bin,     stepY*mm2bin,HiteRate);
			    Layer->Fill(-1 * stepX*mm2bin,     stepY*mm2bin,HiteRate);
			    Layer->Fill(     stepX*mm2bin,-1 * stepY*mm2bin,HiteRate);
			    Layer->Fill(-1 * stepX*mm2bin,-1 * stepY*mm2bin,HiteRate);
			  */
			  
			  Layer->SetBinContent(     stepX+int( winsize /mm2bin),      stepY+int( winsize /mm2bin), HiteRate);
			  Layer->SetBinContent(-1 * stepX+int( winsize /mm2bin),      stepY+int( winsize /mm2bin), HiteRate);
			  Layer->SetBinContent(     stepX+int( winsize /mm2bin), -1 * stepY+int( winsize /mm2bin), HiteRate);
			  Layer->SetBinContent(-1 * stepX+int( winsize /mm2bin), -1 * stepY+int( winsize /mm2bin), HiteRate);
			  
			}
		    }
		  else
		    {
		      //Layer->Fill(     stepX*mm2bin,     stepY*mm2bin,HiteRate);
		      Module->Fill(mStepX,mStepY,HiteRate);
		      Layer ->SetBinContent(stepX+int( winsize /mm2bin), stepY+int( winsize /mm2bin), HiteRate);
		    }
		  mStepX += mm2bin;
		}
	      mStepY += mm2bin;
	    }
	  //printf("Sx Sy (%d,%d)     nC nR (%d,%d) iC iR (%d,%d) SC SR (%d,%d)             Px Py (%.1f,%.1f) StartX StartY (%.1f,%.1f)\n",iSecX,iSecY,nC,nR,iC,iR,SecCol,SecRow,Psize[0+iSecX*nSec],Psize[1+iSecY*nSec],StartX,StartY);

	  if (iC == GeoPara->sCol[iSecX]-1)	   
	    iSecX++;
	    
	  StartX += GeoPara->stepDirection[0] * GeoPara->pSize[iSecX][0] * GeoPara->cosX;
	  StopX  += GeoPara->stepDirection[0] * GeoPara->pSize[iSecX][0] * GeoPara->cosX;
	  planeStartX += GeoPara->stepDirection[0] * GeoPara->pSize[iSecX][0];
	  planeStopX  += GeoPara->stepDirection[0] * GeoPara->pSize[iSecX][0];
	}
      iSecX = 0;
   
      if (iR == GeoPara->sRow[iSecY]-1)       
	iSecY++;

      StartY += GeoPara->stepDirection[1] * GeoPara->pSize[iSecY][1] * GeoPara->cosY;
      StopY  += GeoPara->stepDirection[1] * GeoPara->pSize[iSecY][1] * GeoPara->cosY;
      planeStartY += GeoPara->stepDirection[1] * GeoPara->pSize[iSecY][1];
      planeStopY  += GeoPara->stepDirection[1] * GeoPara->pSize[iSecY][1];
    }
  if(fDraw){
    c1->cd(1);
    Layer->Draw("colz");
    for (Int_t i = 0; i < 3; i++) {
      c0->cd(i+1);
      Topview[i]->Draw("colz");
    }
  }
  Layer->Write("", TObject::kOverwrite);
  HitPad->Write("", TObject::kOverwrite);
  Module->Write("", TObject::kOverwrite);
  delete Module;
  for (Int_t i = 0; i < 3; i++)
    Topview[i]->Write("", TObject::kOverwrite);

  //c0->Update();
  /*
    c2->cd(1);
    HitPad->Draw();
  */
  //DrawLines( Mpos, Msize,Ssize, Psize, nRow, nCol, nSec, Layer, c1);
 


}
void CbmTrdHitRateTest::FillVector(Bool_t firstLayer, Int_t VolumeID,
				   vector<int>& L1S1, vector<int>& L2S1, vector<int>& L3S1, vector<int>& L4S1, 
				   vector<int>& L1S2, vector<int>& L2S2, vector<int>& L3S2, vector<int>& L4S2, 
				   vector<int>& L1S3, vector<int>& L2S3, vector<int>& L3S3, vector<int>& L4S3)
{
  //cout << "FillVector" << endl;
  if (fStation == 1)
    {
      if (fLayer == 1)
	{
	  //Mcounter[0]++;
	  L1S1.push_back (VolumeID);
	}
      if (!firstLayer) {
	if (fLayer == 2)
	  {
	    L2S1.push_back (VolumeID);
	  }
	if (fLayer == 3)
	  {
	    L3S1.push_back (VolumeID);
	  }
	if (fLayer == 4)
	  {
	    L4S1.push_back (VolumeID);
	  }
      }
    }
  if (fStation == 2)
    {
      if(fLayer == 1)
	{
	  //Mcounter[1]++;
	  L1S2.push_back (VolumeID);
	}
      if (!firstLayer) {
	if (fLayer == 2)
	  {
	    L2S2.push_back (VolumeID);
	  }
	if (fLayer == 3)
	  {
	    L3S2.push_back (VolumeID);
	  }
	if (fLayer == 4)
	  {
	    L4S2.push_back (VolumeID);
	  }
      }
    }
  if (fStation == 3)
    {
      if (fLayer == 1)
	{
	  //Mcounter[2][0]++;
	  L1S3.push_back (VolumeID);
	}
      if (!firstLayer) {
	if (fLayer == 2)
	  {
	    L2S3.push_back (VolumeID);
	  }
	if (fLayer == 3)
	  {
	    L3S3.push_back (VolumeID);
	  }
	if (fLayer == 4)
	  {
	    L4S3.push_back (VolumeID);
	  }
      }
    }
}
void CbmTrdHitRateTest::DrawLines(Double_t* Mpos, Double_t* Msize,Double_t* Ssize, Double_t* Psize, Int_t nRow, Int_t nCol, Int_t nSec, TH2F* Layer, TCanvas* c1, TH2F* Topview[3], TCanvas* c0/*, TLine* a, TLine* b, TLine* c, TLine* d*/)
{
  /*
  //----------------------Pad--------------------------------------
  const Int_t nR = nRow;
  const Int_t nC = nCol;
  Int_t iSecX = 0;
  Int_t iSecY = 0;
  Float_t StartX = Mpos[0]-Msize[0];
  Float_t StartY = Mpos[1]-Msize[1];
  Int_t SecRow = int(Ssize[1+iSecY*nSec]/Psize[1+iSecY*nSec]);
  Int_t SecCol = int(Ssize[0+iSecX*nSec]/Psize[0+iSecX*nSec]);
  for (Int_t iR = 0; iR < nR; iR++)
  {
  StartX = Mpos[0]-Msize[0];
  for (Int_t iC = 0; iC < nC; iC++)
  {
  TLine* Pa = new TLine(StartX,
  StartY,
  StartX+Psize[0+iSecX*nSec],
  StartY);
  TLine* Pb = new TLine(StartX,
  StartY+Psize[1+iSecY*nSec],
  StartX+Psize[0+iSecX*nSec],
  StartY+Psize[1+iSecY*nSec]);
  TLine* Pc = new TLine(StartX,
  StartY,
  StartX,
  StartY+Psize[1+iSecY*nSec]);
  TLine* Pd = new TLine(StartX+Psize[0+iSecX*nSec],
  StartY,
  StartX+Psize[0+iSecX*nSec],
  StartY+Psize[1+iSecY*nSec]);
  c1->cd(1);
  Pa->SetLineColor(17);
  //Pa->SetLineStyle(3);
  Pa->Draw("same");
  Pb->SetLineColor(17);
  //Pb->SetLineStyle(3);
  Pb->Draw("same");
  Pc->SetLineColor(17);
  //Pc->SetLineStyle(3);
  Pc->Draw("same");
  Pd->SetLineColor(17);
  //Pd->SetLineStyle(3);
  Pd->Draw("same");
  //printf("Sx Sy (%d,%d)     nC nR (%d,%d) iC iR (%d,%d) SC SR (%d,%d)              Px Py (%.1f,%.1f) StartX StartY (%.1f,%.1f)\n",iSecX,iSecY,nC,nR,iC,iR,SecCol,SecRow,Psize[0+iSecX*nSec],Psize[1+iSecY*nSec],StartX,StartY);

  if (iC == SecCol-1)
  {
  iSecX++;
  SecCol += int(Ssize[0+iSecX*nSec]/Psize[0+iSecX*nSec]);
  }
  StartX += Psize[0+iSecX*nSec];
  }
  iSecX = 0;
  if (iR == SecRow-1)
  {          
  iSecY++;
  SecRow += int(Ssize[1+iSecY*nSec]/Psize[1+iSecY*nSec]);
  }
  StartY += Psize[1+iSecY*nSec];
  }

  */
  //----------------------Sector--------------------------------------
  Float_t SecYStart = 0.0;
  Float_t SecXStart = 0.0;
  Float_t SecYStop  = 0.0;
  Float_t SecXStop  = 0.0;
  for (Int_t iSec = 0; iSec < nSec-1; iSec++)//would be enought to iterate up to nSec-1
    {
      if (Ssize[0+iSec*nSec] < 2 * Msize[0] && Ssize[0+iSec*nSec] > 0)
	{
	  SecXStart += Ssize[0+iSec*nSec];
	  SecXStop   = SecXStart;
	}
      else
	{
	  SecXStart = 0.0;
	  SecXStop  = Ssize[0+iSec*nSec];
	}
      if (Ssize[1+iSec*nSec] < 2 * Msize[1] && Ssize[1+iSec*nSec] > 0)
	{
	  SecYStart += Ssize[1+iSec*nSec];
	  SecYStop = SecYStart;
	}
      else
	{
	  SecYStart = 0.0;
	  SecYStop  = Ssize[1+iSec*nSec];
	}


      TLine* S1 = new TLine(Mpos[0]-Msize[0]+SecXStart,
			    Mpos[1]-Msize[1]+SecYStart,
			    Mpos[0]-Msize[0]+SecXStop,
			    Mpos[1]-Msize[1]+SecYStop);
      S1->SetLineColor(15);
      S1->SetLineStyle(2);
      if(fDraw){
	c1->cd(1);
	S1->Draw("same");
      }
    }
  
  
  //----------------------Module--------------------------------------
  TLine* Ma = new TLine(Mpos[0]-Msize[0],
			Mpos[1]-Msize[1],
			Mpos[0]+Msize[0],
			Mpos[1]-Msize[1]);
  TLine* Mb = new TLine(Mpos[0]-Msize[0],
			Mpos[1]+Msize[1],
			Mpos[0]+Msize[0],
			Mpos[1]+Msize[1]);
  TLine* Mc = new TLine(Mpos[0]-Msize[0],
			Mpos[1]-Msize[1],
			Mpos[0]-Msize[0],
			Mpos[1]+Msize[1]);
  TLine* Md = new TLine(Mpos[0]+Msize[0],
			Mpos[1]-Msize[1],
			Mpos[0]+Msize[0],
			Mpos[1]+Msize[1]);
  if(fDraw){
    c1->cd(1);
    Ma->Draw("same");
    Mb->Draw("same");
    Mc->Draw("same");
    Md->Draw("same");
    //c1->Update();
    c1->Write("", TObject::kOverwrite);
  }
}

  // -------------------------------------------------------------------
  // ------AddDigi--------------------------------------------------------------

  //void CbmTrdHitRateTest::AddDigi() 
  void CbmTrdHitRateTest::DrawDigi() 
  {
 
  }

  // ---- Register ------------------------------------------------------
  void CbmTrdHitRateTest::Register()
  {
    FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
    FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
  }
  // --------------------------------------------------------------------

  ClassImp(CbmTrdHitRateTest)
