﻿#include "CbmTrdHitRateTest.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"

#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TLine.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TPRegexp.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
#include "CbmTrdStringToken.h"
#endif

//#include "sqlite/sqlite3.h" // used for the lookup table option

//#include <RE2>
#include <vector>
#include <iostream>
#include "Riostream.h"
#include <fstream>
#include <string>
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::vector;
//using std::push_back;


// ---- Default constructor -------------------------------------------
CbmTrdHitRateTest::CbmTrdHitRateTest()
    :FairTask("TrdCluster")
	//:fRef(0)
{
  //  fDigiCollection = new TClonesArray("CbmTrdDigi");
  fEfficiency = 1;
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdHitRateTest::CbmTrdHitRateTest(const char *name, const char *title,
                 CbmTrdRadiator *radiator)
	:FairTask(name)
{
  fRadiators = radiator;
  fEfficiency = 1;
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

    cout<<" * CbmTrdCluster * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
    fTrdPoints=(TClonesArray *)ioman->ActivateBranch("TrdPoint"); 
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdCluster::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fMCStacks = (TClonesArray*)ioman->ActivateBranch("MCTrack");
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
  TH2F *Layer = NULL;//new TH2F("Dummy","Dummy",1,-1,1,1,-1,1);
  fStation = 0;
  fLayer = 0;
  Int_t prevLayer = 0;
  Int_t prevStation = 0;
  TCanvas *c1 = NULL;
  /*
    TCanvas *c1 = new TCanvas("c1","c1",1000,900);	
    c1->Divide(1,1);
    c1->cd(1);
  */
  Char_t Canfile1[100];
  
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
  
  //TVector3 mom;
  Int_t ModuleID[10];
  Int_t Sector = 0;
  Char_t trddigiparpath[50] = "trd.digi.par";
  std::ifstream digifile;
  digifile.open(trddigiparpath);
  if(!digifile.good()) 
    {
      printf("ERROR      trd.digi.par was not found at ../%s\n",trddigiparpath);
    } 
  Int_t counter = 0;
  do {
    /*
      counter++;
      cout << counter << endl;
    */
    for (Int_t i = 0; i < 10; i++)
      {
	ModuleID[i] = -1000;
      }
    //cout << "reading" << endl;
    digifile >> skipws >> ModuleID[0] >> ModuleID[1] >> ModuleID[2] >> ModuleID[3] >> ModuleID[4] >> ModuleID[5] >> ModuleID[6] >> ModuleID[7] >> ModuleID[8] >> ModuleID[9];
    //cout << "done" << endl;
    if (ModuleID[0] == 0)
      {
	break;
      }
    for (Int_t i = 0; i < 10; i++)
      {
	if (ModuleID[i] > -1000)
	  {
	    //cout << ModuleID[i] << " ";
	    GetModuleInformationSL(ModuleID[i]);
	    
	    FillVector(ModuleID[i], L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3);
	    prevLayer = fLayer;
	    prevStation = fStation;
	  }
      }
    //cout << endl;
    if (!digifile.good() && !digifile.eof())
      {
	digifile.clear();
      }
    digifile.ignore(1000,'\n');
  
  } while (digifile.good());

  //sprintf(Canfile1,"Pics/Station%dLayer%d.png",fStation,fLayer);
  Char_t name[50];
  Char_t title[50];
  TImage *Outimage1;

  TLine* a = NULL;
  TLine* b = NULL;
  TLine* c = NULL;
  TLine* d = NULL;

  fStation = 1;
  fLayer = 1;
  sprintf(name,"S%d_L%d",fStation,fLayer);
  sprintf(title,"Station %d, Layer %d",fStation,fLayer);
  sprintf(Canfile1,"Pics/Station%dLayer%d.png",fStation,fLayer);
  Layer = new TH2F(name,title,18000,-9000,9000,18000,-9000,9000);
  Layer->SetContour(99);
  Layer->SetXTitle("x-Coordinate [mm]");
  Layer->SetYTitle("y-Coordinate [mm]");
  Layer->SetZTitle("Hits/Pad");//"Data Flow [Bit/s]
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
  Layer->GetZaxis()->SetRangeUser(1,1.e05);//1.e06,5.e07);
  c1 = new TCanvas("c1","c1",1000,900);	
  c1->Divide(1,1);
  c1->cd(1)->SetLogz(1);
  //c1->cd(1);
  //Layer->Draw("colz");
  for (Int_t i = 0; i < L1S1.size(); i++)
    {
      GetModuleInformationFromDigiPar(Sector, L1S1[i], Layer ,c1, Canfile1, prevLayer, prevStation, L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3, a, b, c, d);
    }
  Outimage1 = TImage::Create();
  Outimage1->FromPad(c1);
  Outimage1->WriteImage(Canfile1);
  cout << L1S1.size() << endl;
  delete Layer;
  delete c1;
  /*
    fStation = 1;
    fLayer = 2;
    sprintf(name,"S%d_L%d",fStation,fLayer);
    sprintf(title,"Station %d, Layer %d",fStation,fLayer);
    sprintf(Canfile1,"Pics/Station%dLayer%d.png",fStation,fLayer);
    Layer = new TH2F(name,title,18000,-9000,9000,18000,-9000,9000);
    Layer->SetContour(99);
    Layer->SetXTitle("x-Coordinate [mm]");
    Layer->SetYTitle("y-Coordinate [mm]");
    Layer->SetZTitle("Bit/Pad");//"Data Flow [Hits/Pad]
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
    Layer->GetZaxis()->SetRangeUser(1.e06,5.e07);
    c1 = new TCanvas("c1","c1",1000,900);	
    c1->Divide(1,1);
    c1->cd(1)->SetLogz(1);
    //c1->cd(1);
    //Layer->Draw("colz");
    for (Int_t i = 0; i < L2S1.size(); i++)
    {
    GetModuleInformationFromDigiPar(Sector, L2S1[i], Layer ,c1, Canfile1, prevLayer, prevStation, L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3, a, b, c, d);
    }
    Outimage1 = TImage::Create();
    Outimage1->FromPad(c1);
    Outimage1->WriteImage(Canfile1);
    //cout << L1S1.size() << endl;
    delete Layer;
    delete c1;

    fStation = 2;
    fLayer = 1;
    sprintf(name,"S%d_L%d",fStation,fLayer);
    sprintf(title,"Station %d, Layer %d",fStation,fLayer);
    sprintf(Canfile1,"Pics/Station%dLayer%d.png",fStation,fLayer);
    Layer = new TH2F(name,title,18000,-9000,9000,18000,-9000,9000);
    Layer->SetContour(99);
    Layer->SetXTitle("x-Coordinate [mm]");
    Layer->SetYTitle("y-Coordinate [mm]");
    Layer->SetZTitle("Bit/Pad");//"Data Flow [Hits/Pad]
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
    Layer->GetZaxis()->SetRangeUser(1.e06,5.e07);
    c1 = new TCanvas("c1","c1",1000,900);	
    c1->Divide(1,1);
    c1->cd(1)->SetLogz(1);
    //c1->cd(1);
    //Layer->Draw("colz");
    for (Int_t i = 0; i < L1S2.size(); i++)
    {
    GetModuleInformationFromDigiPar(Sector, L1S2[i], Layer ,c1, Canfile1, prevLayer, prevStation, L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3, a, b, c, d);
    }
    Outimage1 = TImage::Create();
    Outimage1->FromPad(c1);
    Outimage1->WriteImage(Canfile1);
    //cout << L1S1.size() << endl;
    delete Layer;
    delete c1;

    fStation = 2;
    fLayer = 2;
    sprintf(name,"S%d_L%d",fStation,fLayer);
    sprintf(title,"Station %d, Layer %d",fStation,fLayer);
    sprintf(Canfile1,"Pics/Station%dLayer%d.png",fStation,fLayer);
    Layer = new TH2F(name,title,18000,-9000,9000,18000,-9000,9000);
    Layer->SetContour(99);
    Layer->SetXTitle("x-Coordinate [mm]");
    Layer->SetYTitle("y-Coordinate [mm]");
    Layer->SetZTitle("Bit/Pad");//"Data Flow [Hits/Pad]
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
    Layer->GetZaxis()->SetRangeUser(1.e06,5.e07);
    c1 = new TCanvas("c1","c1",1000,900);	
    c1->Divide(1,1);
    c1->cd(1)->SetLogz(1);
    //c1->cd(1);
    //Layer->Draw("colz");
    for (Int_t i = 0; i < L2S2.size(); i++)
    {
    GetModuleInformationFromDigiPar(Sector, L2S2[i], Layer ,c1, Canfile1, prevLayer, prevStation, L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3, a, b, c, d);
    }
    Outimage1 = TImage::Create();
    Outimage1->FromPad(c1);
    Outimage1->WriteImage(Canfile1);
    //cout << L1S1.size() << endl;
    delete Layer;
    delete c1;

    fStation = 3;
    fLayer = 1;
    sprintf(name,"S%d_L%d",fStation,fLayer);
    sprintf(title,"Station %d, Layer %d",fStation,fLayer);
    sprintf(Canfile1,"Pics/Station%dLayer%d.png",fStation,fLayer);
    Layer = new TH2F(name,title,18000,-9000,9000,18000,-9000,9000);
    Layer->SetContour(99);
    Layer->SetXTitle("x-Coordinate [mm]");
    Layer->SetYTitle("y-Coordinate [mm]");
    Layer->SetZTitle("Bit/Pad");//"Data Flow [Hits/Pad]
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
    Layer->GetZaxis()->SetRangeUser(1.e06,5.e07);
    c1 = new TCanvas("c1","c1",1000,900);	
    c1->Divide(1,1);
    c1->cd(1)->SetLogz(1);
    //c1->cd(1);
    //Layer->Draw("colz");
    for (Int_t i = 0; i < L1S3.size(); i++)
    {
    GetModuleInformationFromDigiPar(Sector, L1S3[i], Layer ,c1, Canfile1, prevLayer, prevStation, L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3, a, b, c, d);
    }
    //c1->Update();
    Outimage1 = TImage::Create();
    Outimage1->FromPad(c1);
    Outimage1->WriteImage(Canfile1);
    //cout << L1S1.size() << endl;
    delete Layer;
    delete c1;

    fStation = 3;
    fLayer = 2;
    sprintf(name,"S%d_L%d",fStation,fLayer);
    sprintf(title,"Station %d, Layer %d",fStation,fLayer);
    sprintf(Canfile1,"Pics/Station%dLayer%d.png",fStation,fLayer);
    Layer = new TH2F(name,title,18000,-9000,9000,18000,-9000,9000);
    Layer->SetContour(99);
    Layer->SetXTitle("x-Coordinate [mm]");
    Layer->SetYTitle("y-Coordinate [mm]");
    Layer->SetZTitle("Bit/Pad");//"Data Flow [Hits/Pad]
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
    Layer->GetZaxis()->SetRangeUser(1.e06,5.e07);
    c1 = new TCanvas("c1","c1",1000,900);	
    c1->Divide(1,1);
    c1->cd(1)->SetLogz(1);
    //c1->cd(1);
    //Layer->Draw("colz");
    for (Int_t i = 0; i < L2S3.size(); i++)
    {
    GetModuleInformationFromDigiPar(Sector, L2S3[i], Layer ,c1, Canfile1, prevLayer, prevStation, L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3, a, b, c, d);
    }
    //c1->Update();
    Outimage1 = TImage::Create();
    Outimage1->FromPad(c1);
    Outimage1->WriteImage(Canfile1);
    //cout << L1S1.size() << endl;
    delete Layer;
    delete c1;
  */
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
void CbmTrdHitRateTest::GetModuleInformationSL( Int_t VolumeID)
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
void CbmTrdHitRateTest::GetModuleInformationFromDigiPar(Bool_t Sector, Int_t VolumeID, TH2F* Layer, TCanvas* c1, Char_t* Canfile1, Int_t prevLayer, Int_t prevStation, 
							vector<int>& L1S1, vector<int>& L2S1, vector<int>& L3S1, vector<int>& L4S1, 
							vector<int>& L1S2, vector<int>& L2S2, vector<int>& L3S2, vector<int>& L4S2, 
							vector<int>& L1S3, vector<int>& L2S3, vector<int>& L3S3, vector<int>& L4S3,
							TLine* a, TLine* b, TLine* c, TLine* d)
{
  // fPos is >0 for x and y and not rotated
  // origin of the local coordinate system in 
  // the lower left corner of the chamber, 
  // x to the right (small side of the pads), y up
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
      Mpos[2] = fModuleInfo->GetZ() * 10;
  
      Double_t Msize[3];
      Msize[0] = fModuleInfo->GetSizex() * 10;
      Msize[1] = fModuleInfo->GetSizey() * 10;
      Msize[3] = 0; 

      for (Int_t i = 0; i < 3 ; i++)
	{
	  //cout <<  " dim " << i << " Mpos " << Mpos[i] << "   Msize  " << Msize[i] << endl;
	}
      //cout << endl;
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

	  for (Int_t j = 0; j < 3 ; j++)
	    {
	      //cout << "S" << i << " dim " << j << " Ssize " << Ssize[j+i*NoSectors] << "   Psize  " << Psize[j+i*NoSectors] << endl;
	    }
	  //cout << endl;
	}

      Int_t* detInfo = fTrdId.GetDetectorInfo(VolumeID); 
      fStation    = detInfo[1];
      fLayer      = detInfo[2];     
      fModuleType = detInfo[3];
      fModuleCopy = detInfo[4];
      //printf(" (S%d L%d)  ",fStation,fLayer);
   
      /* const*/ 
      nCol = (Int_t)(2*Msize[0] / (int(Psize[0] * 100) / 100.0));   
      Int_t temp = 0;
      for (Int_t i = 0; i < fNoSectors; i++)
	{
	  temp += int(Ssize[1+i*NoSectors]/Psize[1+i*NoSectors]); //sectorrows[i];
	}
      /*const*/
      nRow = temp;
      //cout << "nRow " << nRow << "    nCol " << nCol << endl;
      Histo( Mpos, Msize, Ssize, Psize, nRow, nCol, nSec, Layer, c1, Canfile1, prevLayer, prevStation, L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3, a, b, c, d);

    }
  else
    {
      printf("fModuleInfo == NULL\n");
    }
}
void CbmTrdHitRateTest::FillVector(Int_t VolumeID,
				   vector<int>& L1S1, vector<int>& L2S1, vector<int>& L3S1, vector<int>& L4S1, 
				   vector<int>& L1S2, vector<int>& L2S2, vector<int>& L3S2, vector<int>& L4S2, 
				   vector<int>& L1S3, vector<int>& L2S3, vector<int>& L3S3, vector<int>& L4S3)
{
  if (fStation == 1)
    {
      if (fLayer == 1)
	{
	  //Mcounter[0]++;
	  L1S1.push_back (VolumeID);
	}
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
  if (fStation == 2)
    {
      if(fLayer == 1)
	{
	  //Mcounter[1]++;
	  L1S2.push_back (VolumeID);
	}
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
  if (fStation == 3)
    {
      if (fLayer == 1)
	{
	  //Mcounter[2][0]++;
	  L1S3.push_back (VolumeID);
	}
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
void CbmTrdHitRateTest::DrawLines(Double_t* Mpos, Double_t* Msize,Double_t* Ssize, Double_t* Psize, Int_t nRow, Int_t nCol, Int_t nSec, TH2F* Layer, TCanvas* c1, TLine* a, TLine* b, TLine* c, TLine* d)
{
  
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
  
  c1->cd(1);
  Ma->Draw("same");
  Mb->Draw("same");
  Mc->Draw("same");
  Md->Draw("same");
  //c1->Update();

  //----------------------Sector--------------------------------------
  Float_t SecY = 0.0;
  for (Int_t iSec = 0; iSec < nSec-1; iSec++)//would be enought to iterate up to nSec-1
    {
      SecY += Ssize[1+iSec*nSec];

      TLine* S1 = new TLine(Mpos[0]-0.5*Ssize[0+iSec*nSec],
			    Mpos[1]-Msize[1]+SecY,
			    Mpos[0]+0.5*Ssize[0+iSec*nSec],
			    Mpos[1]-Msize[1]+SecY);
      S1->SetLineColor(15);
      S1->SetLineStyle(2);
      /*
	TLine* S2 = new TLine(Mpos[0]-Msize[0],Mpos[1]-Msize[1]+Ssize[1]+Ssize[1+3],Mpos[0]+Msize[0],Mpos[1]-Msize[1]+Ssize[1]+Ssize[1+3]);
	S2->SetLineColor(15);
	S2->SetLineStyle(2);
      */
      c1->cd(1);
      S1->Draw("same");
      //S2->Draw("same");
    }
  

  //----------------------Pad--------------------------------------
  const Int_t nR = nRow;
  const Int_t nC = nCol;
  Int_t iSec = 0;
  Float_t StartX = Mpos[0]-Msize[0];
  Float_t StartY = Mpos[1]-Msize[1];
  Int_t SecRow = int(Ssize[1+iSec*nSec]/Psize[1+iSec*nSec]);
  for (Int_t iR = 0; iR < nR; iR++)
    {
      //cout <<  "iSec " << iSec << " iRow " << iR << " SecBorderRow " << SecRow << " nRow " << nR << endl;
      StartX = Mpos[0]-Msize[0];
      for (Int_t iC = 0; iC < nC; iC++)
	{
	  //cout << "Pad " << iC << "," << iR << " x " << StartX << " y " << StartY << endl;

	  TLine* Pa = new TLine(StartX,
				StartY,
				StartX+Psize[0+iSec*nSec],
				StartY);
	  TLine* Pb = new TLine(StartX,
				StartY+Psize[1+iSec*nSec],
				StartX+Psize[0+iSec*nSec],
				StartY+Psize[1+iSec*nSec]);
	  TLine* Pc = new TLine(StartX,
				StartY,
				StartX,
				StartY+Psize[1+iSec*nSec]);
	  TLine* Pd = new TLine(StartX+Psize[0+iSec*nSec],
				StartY,
				StartX+Psize[0+iSec*nSec],
				StartY+Psize[1+iSec*nSec]);
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

	  StartX += Psize[0+iSec*nSec];
	}
      if (iR == SecRow-1)
	{          
	  iSec++;
	  SecRow += int(Ssize[1+iSec*nSec]/Psize[1+iSec*nSec]);
	  //cout << iSec << " " << iR << " " << SecRow << " " << nR << endl;
	}
      StartY += Psize[1+iSec*nSec];
    }

}
void CbmTrdHitRateTest::Histo(Double_t* Mpos, Double_t* Msize,Double_t* Ssize, Double_t* Psize, Int_t nRow, Int_t nCol, Int_t nSec, TH2F* Layer, TCanvas* c1, Char_t* Canfile1, Int_t prevLayer, Int_t prevStation,
			      vector<int>& L1S1, vector<int>& L2S1, vector<int>& L3S1, vector<int>& L4S1, 
			      vector<int>& L1S2, vector<int>& L2S2, vector<int>& L3S2, vector<int>& L4S2, 
			      vector<int>& L1S3, vector<int>& L2S3, vector<int>& L3S3, vector<int>& L4S3,
			      TLine* a, TLine* b, TLine* c, TLine* d)
{
  const Int_t nR = nRow;
  const Int_t nC = nCol;
  Float_t HiteRate = 0;
  Int_t iSec = 0;
  Float_t StartX = Mpos[0]-Msize[0];
  Float_t StartY = Mpos[1]-Msize[1];
  Float_t StopX = StartX + Psize[0+iSec*nSec];
  Float_t StopY = StartY + Psize[1+iSec*nSec];
  Int_t SecRow = int(Ssize[1+iSec*nSec]/Psize[1+iSec*nSec]);
  for (Int_t iR = 0; iR < nR; iR++)
    {
      StartX = Mpos[0]-Msize[0];
      StopX  = StartX + Psize[0+iSec*nSec];

	for (Int_t iC = 0; iC < nC; iC++)
	  {
	    HiteRate = CalcHitRate(StartX, StopX, StartY, StopY);

	    for (Int_t stepY = int(StartY); stepY < int(StopY); stepY++)
	      {
		for (Int_t stepX = int(StartX); stepX < int(StopX); stepX++)
		  {
		    Layer->Fill(stepX/* + 9000*/, stepY/* + 9000*/, HiteRate);
		  }
	      }
	    StartX += Psize[0+iSec*nSec];
	    StopX  += Psize[0+iSec*nSec];
	  }
      if (iR == SecRow-1)
	{          
	  iSec++;
	  SecRow += int(Ssize[1+iSec*nSec]/Psize[1+iSec*nSec]);
	}
      StartY += Psize[1+iSec*nSec];
      StopY  += Psize[1+iSec*nSec];
    }
  
  c1->cd(1);
  Layer->Draw("colz");
  
  //DrawLines( Mpos, Msize,Ssize, Psize, nRow, nCol, nSec, Layer, c1, a, b, c, d);
  //c1->Update();


}

float CbmTrdHitRateTest::CalcHitRate(Float_t StartX, Float_t StopX, Float_t StartY, Float_t StopY)
{
  Float_t HitRate = 0;//1. / sqrt( pow( StartX,2) + pow( StartY,2));
  Float_t r = 0;
  Int_t counter = 0;
  Float_t a[3] = { 7.66582e+00,  6.97712e+00,  6.53780e+00};
  Float_t b[3] = {-2.72375e-03, -1.85168e-03, -1.42673e-03};

  for (Int_t stepY = int(StartY); stepY < int(StopY); stepY++)
    {
      for (Int_t stepX = int(StartX); stepX < int(StopX); stepX++)
	{
	  counter++;
	  r = sqrt( pow((stepX + 0.5), 2) + pow((stepY + 0.5) ,2));
	  HitRate += exp(a[fStation-1] + b[fStation-1] * r);
	}
    }

  return (HitRate/*/(counter/100.)*/);/*Convertes Hits/Pad -> Hits/cm² on each Pad*/
}

  // -------------------------------------------------------------------
  // ------AddDigi--------------------------------------------------------------

  //void CbmTrdHitRateTest::AddDigi() 
  void CbmTrdHitRateTest::AddDigi(Int_t iCol, Int_t iRow, Float_t iCharge) 
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
