#include "CbmTrdHitRateTest.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

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
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::vector;

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

    cout<<" * CbmTrdHitRate * :: Init() "<<endl;

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
  printf("Introduction:\n");
  //Bool_t Fast = true;
  Bool_t Lines;
  Bool_t Fast = false;
  Double_t ZRangeL = 1e00;
  Double_t ZRangeU = 1e05;
 
  fStation = 0;
  fLayer = 0;

  TH1F* HitPad = NULL;
  TH2F* Layer = NULL;
  TCanvas* c1 = NULL;
  TCanvas* c2 = NULL;

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

  Int_t ModuleID[10];
  Int_t Sector = 0;
  Char_t trddigiparpath[100] = "trd.digi.par";

  //// do not ask for the filename   
  //  printf("Which ...digi.par?\n");
  //  cin >> trddigiparpath;         
  //  cout << trddigiparpath << endl;

  std::ifstream digifile;
  digifile.open(trddigiparpath);
  if(!digifile.good()) 
    {
      printf("ERROR      trd.digi.par was not found at ../%s\n",trddigiparpath);
      while (!digifile.good())
	{
	  printf("Next try. Which ...digi.par?\n");
	  cin >> trddigiparpath;
	  cout << trddigiparpath << endl;
	  //std::ifstream digifile;
	  digifile.open(trddigiparpath);
	}
    } 
  Int_t counter = 0;
  do {
  
    for (Int_t i = 0; i < 10; i++)
      {
	ModuleID[i] = -1000;
      }

    digifile >> skipws >> ModuleID[0] >> ModuleID[1] >> ModuleID[2] >> ModuleID[3] >> ModuleID[4] >> ModuleID[5] >> ModuleID[6] >> ModuleID[7] >> ModuleID[8] >> ModuleID[9];

    if (ModuleID[0] == 0)
      {
	break;
      }
    for (Int_t i = 0; i < 10; i++)
      {
	if (ModuleID[i] > -1000)
	  {

	    GetModuleInformationSL(ModuleID[i]);
	    
	    FillVector(ModuleID[i], L1S1, L2S1, L3S1, L4S1, L1S2, L2S2, L3S2, L4S2, L1S3, L2S3, L3S3, L4S3);

	  }
      }
    //cout << endl;
    if (!digifile.good() && !digifile.eof())
      {
	digifile.clear();
      }
    digifile.ignore(1000,'\n');
  
  } while (digifile.good());

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

      sprintf(Canfile1,"Pics/%s_S%d_L%d.png",trddigiparpath,fStation,fLayer);
      sprintf(Canfile2,"Pics/%s_HitPerPad_S%d_L%d.png",trddigiparpath,fStation,fLayer);

      HistoInit(c1, c2,  Layer, HitPad, Canfile1, Canfile2, ZRangeL, ZRangeU);

      border->Draw("same");
      Lines = false;
      for (vector<int>::size_type i = 0; i < LiSi[j].size(); i++)
	{
	  GetModuleInformationFromDigiPar(Fast, Lines, LiSi[j][i], Layer ,c1, Canfile1, HitPad, c2);
	}
      c1->cd(1)->SetLogz(1);
      Layer->Draw("colz");
      Lines = true;
      for (vector<int>::size_type i = 0; i < LiSi[j].size(); i++)
	{
	  GetModuleInformationFromDigiPar(Fast, Lines, LiSi[j][i], Layer ,c1, Canfile1, HitPad, c2);
	}

      Outimage1 = TImage::Create();
      Outimage1->FromPad(c1);
      Outimage1->WriteImage(Canfile1);
      //c1->cd(1)->Print("Pics/Station%dLayer%d.pdf");
      delete Layer;
      delete c1;
      delete Outimage1;
 
      HitPad->Draw("same");
  
      Outimage2 = TImage::Create();
      Outimage2->FromPad(c2);
      Outimage2->WriteImage(Canfile2);
  
      delete HitPad;
      delete c2;  
      delete Outimage2;

      if (fLayer == 4)
	{
	  fStation++;
	  fLayer = 1;
	}
      else
	{
	  fLayer++;
	}      
    }
}
void CbmTrdHitRateTest::HistoInit(TCanvas*& c1, TCanvas*& c2,TH2F*& Layer,TH1F*& HitPad, Char_t* Canfile1, Char_t* Canfile2, Double_t ZRangeL, Double_t ZRangeU)
{
  Char_t name[50];
  Char_t title[50];

  sprintf(name,"S%d_L%d",fStation,fLayer);
  sprintf(title,"Station %d, Layer %d",fStation,fLayer);
  cout << title << endl;
  //sprintf(Canfile1,"Pics/Station%dLayer%d.png",fStation,fLayer);
  //sprintf(Canfile2,"Pics/HitPerPadStation%dLayer%d.png",fStation,fLayer);
  Layer = new TH2F(name,title,18000,-9000,9000,18000,-9000,9000);
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

  c1 = new TCanvas("c1","c1",1000,900);	
  c1->Divide(1,1);
  c1->cd(1)->SetLogz(1);
   Layer->Draw();
  c2 = new TCanvas("c2","c2",1000,900/2);	
  c2->Divide(1,1);
  c2->cd(1)->SetLogx(1);
  c2->cd(1)->SetLogy(1);
  c2->cd(1)->SetGridx(1);
  c2->cd(1)->SetGridy(1);
  
  HitPad->Draw();
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
void CbmTrdHitRateTest::GetModuleInformationFromDigiPar(Bool_t Fast, Bool_t Lines, Int_t VolumeID, TH2F* Layer, TCanvas* c1, Char_t* Canfile1, TH1F* HitPad, TCanvas* c2)
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
      Mpos[2] = fModuleInfo->GetZ() * 10; // == z(station) ??
      
      Double_t Msize[3];
      Msize[0] = fModuleInfo->GetSizex() * 10;
      Msize[1] = fModuleInfo->GetSizey() * 10;
      Msize[3] = 0; 

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
      
      nRow = tempY;
      nCol = tempX;
      //cout << nRow << " " << nCol << endl;
      if(Lines)
	{
	  DrawLines( Mpos, Msize,Ssize, Psize, nRow, nCol, nSec, Layer, c1);
	}
      else
	{
	  Histo(Fast, Mpos, Msize, Ssize, Psize, nRow, nCol, nSec, Layer, c1, Canfile1, HitPad, c2);
	}
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
void CbmTrdHitRateTest::DrawLines(Double_t* Mpos, Double_t* Msize,Double_t* Ssize, Double_t* Psize, Int_t nRow, Int_t nCol, Int_t nSec, TH2F* Layer, TCanvas* c1/*, TLine* a, TLine* b, TLine* c, TLine* d*/)
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
 
      c1->cd(1);
      S1->Draw("same");
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
  
  c1->cd(1);
  Ma->Draw("same");
  Mb->Draw("same");
  Mc->Draw("same");
  Md->Draw("same");
  //c1->Update();
 
  
}
void CbmTrdHitRateTest::Histo(Bool_t Fast, Double_t* Mpos, Double_t* Msize,Double_t* Ssize, Double_t* Psize, Int_t nRow, Int_t nCol, Int_t nSec, TH2F* Layer, TCanvas* c1, Char_t* Canfile1, TH1F* HitPad, TCanvas* c2)
{
  //cout << Mpos[2] << endl;
  const Int_t nR = nRow;
  const Int_t nC = nCol;
  Float_t HiteRate = 0;
  Int_t iSecX = 0;
  Int_t iSecY = 0;
  Float_t StartX = Mpos[0]-Msize[0];
  Float_t StartY = Mpos[1]-Msize[1];
  Float_t StopX = StartX + Psize[0+iSecX*nSec];
  Float_t StopY = StartY + Psize[1+iSecY*nSec];
  Int_t SecRow = int(Ssize[1+iSecY*nSec]/Psize[1+iSecY*nSec]);
  Int_t SecCol = int(Ssize[0+iSecX*nSec]/Psize[0+iSecX*nSec]);

  for (Int_t iR = 0; iR < nR; iR++)
    {
      StartX = Mpos[0]-Msize[0];
      StopX  = StartX + Psize[0+iSecX*nSec];

      for (Int_t iC = 0; iC < nC; iC++)
	{
	  if (Fast)
	    {
	      if (Mpos[0] > -1 && Mpos[1] > -1)
		{
		  HiteRate = CalcHitRate(StartX, StopX, StartY, StopY, Mpos);
		  HitPad->Fill(HiteRate);
		  HitPad->Fill(HiteRate);
		  HitPad->Fill(HiteRate);
		  HitPad->Fill(HiteRate);
		}
	    }
	  else
	    {
	      HiteRate = CalcHitRate(StartX, StopX, StartY, StopY, Mpos);
	      HitPad->Fill(HiteRate);
	    }
	  
	  for (Int_t stepY = int(StartY); stepY < int(StopY); stepY++)
	    {
	      for (Int_t stepX = int(StartX); stepX < int(StopX); stepX++)
		{
		  if (Fast)
		    {
		      if (Mpos[0] > -1 && Mpos[1] > -1)
			{
			  Layer->SetBinContent(     stepX+9000,      stepY+9000, HiteRate);
			  Layer->SetBinContent(-1 * stepX+9000,      stepY+9000, HiteRate);
			  Layer->SetBinContent(     stepX+9000, -1 * stepY+9000, HiteRate);
			  Layer->SetBinContent(-1 * stepX+9000, -1 * stepY+9000, HiteRate);
			}
		    }
		  else
		    {
		      Layer->SetBinContent(stepX+9000, stepY+9000, HiteRate);
		    }
		}
	    }
	  //printf("Sx Sy (%d,%d)     nC nR (%d,%d) iC iR (%d,%d) SC SR (%d,%d)             Px Py (%.1f,%.1f) StartX StartY (%.1f,%.1f)\n",iSecX,iSecY,nC,nR,iC,iR,SecCol,SecRow,Psize[0+iSecX*nSec],Psize[1+iSecY*nSec],StartX,StartY);

	  if (iC == SecCol-1)
	    {
	      iSecX++;
	      SecCol += int(Ssize[0+iSecX*nSec]/Psize[0+iSecX*nSec]);
	    }
	  StartX += Psize[0+iSecX*nSec];
	  StopX  += Psize[0+iSecX*nSec];
	}
      iSecX = 0;
   
      if (iR == SecRow-1)
	{          
	  iSecY++;
	  SecRow += int(Ssize[1+iSecY*nSec]/Psize[1+iSecY*nSec]);
	}
      StartY += Psize[1+iSecY*nSec];
      StopY  += Psize[1+iSecY*nSec];
    }
  
  c1->cd(1);
  Layer->Draw("colz");
  /*
  c2->cd(1);
  HitPad->Draw();
  */
  //DrawLines( Mpos, Msize,Ssize, Psize, nRow, nCol, nSec, Layer, c1);
 


}

float CbmTrdHitRateTest::CalcHitRate(Float_t StartX, Float_t StopX, Float_t StartY, Float_t StopY, Double_t* Mpos)
{
  Float_t HitRate = 0;//1. / sqrt( pow( StartX,2) + pow( StartY,2));
  Float_t r = 0;
  Float_t alpha = 0;
  Int_t counter = 0;
  Float_t a[3] = { 7.66582e+00,  6.97712e+00,  6.53780e+00};
  Float_t b[3] = {-2.72375e-03, -1.85168e-03, -1.42673e-03};

  for (Int_t stepY = int(StartY); stepY < int(StopY); stepY++)
    {
      for (Int_t stepX = int(StartX); stepX < int(StopX); stepX++)
	{
	  counter++;
	  r = sqrt( pow((stepX + 0.5), 2) + pow((stepY + 0.5) ,2));
	  alpha = atan(r/Mpos[2])*1000.;
	  //HitRate += exp(a[fStation-1] + b[fStation-1] * r);
	  HitRate += 
	    exp(4.54156e00 + -8.47377e-03 * alpha) + 
	    exp(2.40005e01 + -1.19541e-02 * alpha) /
	    (Mpos[2] * Mpos[2])
	    ;
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
