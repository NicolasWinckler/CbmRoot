#include "CbmTrdHitDensityQa.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdPoint.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TArray.h"
#include "TF1.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TLegend.h"
#include "TStopwatch.h"

//#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdHitDensityQa::CbmTrdHitDensityQa()
  :FairTask("CbmTrdHitDensityQa",1),
   fTrdPoints(NULL),
   fDigis(NULL),
   fClusters(NULL),
   fClusterHits(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(new CbmTrdGeoHandler()),
   fStation(-1),
   fLayer(-1), 
   fModuleID(-1),
   fMCindex(-1),
   ModulePointMap()
{
}

// ---- Destructor ----------------------------------------------------
CbmTrdHitDensityQa::~CbmTrdHitDensityQa()
{
  if(fClusters){
    fClusters->Clear("C");
    fClusters->Delete();
    delete fClusters;
  }
  if(fDigis){
    fDigis->Delete();
    delete fDigis;
  }    
    if(fClusterHits){
    fClusterHits->Delete();
    delete fClusterHits;
  }    
    if(fDigiPar){
    delete fDigiPar;
  }
  if(fModuleInfo){
    delete fModuleInfo;
  }
}

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitDensityQa::SetParContainers()
{
  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  cout << " * CbmTrdHitDensityQa * :: SetParContainers()" << endl;
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
}
// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitDensityQa::ReInit()
{  
  cout << " * CbmTrdHitDensityQa * :: ReInit()" << endl;
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  //fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitDensityQa::Init()
{
  cout << " * CbmTrdHitDensityQa * :: Init()" << endl;
  FairRootManager *ioman = FairRootManager::Instance();
  fTrdPoints=(TClonesArray *)ioman->GetObject("TrdPoint"); 
  if ( ! fTrdPoints ) {
    cout << "-W CbmTrdHitDensityQa::Init: No TrdPoints array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }
  /*
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitDensityQa::Init: No TrdDigi array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }
  fClusters =(TClonesArray *)  ioman->GetObject("TrdCluster");
  if ( ! fClusters ) {
    cout << "-W CbmTrdHitDensityQa::Init: No TrdCluster array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }    
  fClusterHits = new TClonesArray("CbmTrdHit", 100);
  ioman->Register("TrdHit","TRD Hit",fClusterHits,kTRUE);
  */
  // Extract information about the number of TRD stations and
  // the number of layers per TRD station from the geomanager.
  // Store the information about the number of layers at the entrance
  // of subsequent stations in a vector. 
  // This allows to calculate the layer number starting with 1 for the
  // first layer of the first station at a later stage by only adding 
  // the layer number in the station to the number of layers in 
  // previous stations 

  fGeoHandler->Init();

  return kSUCCESS;
  
}
void CbmTrdHitDensityQa::Exec(Option_t * option)
{
  Int_t nStation = 3;
  Int_t  nLayer = 4;
  TFile *tFile;
  TH1F *hPointDistance;
  TProfile *hPointDistanceRadius;
  TH2F *hPointDistanceRadius2;
  TH1F *hPointDistanceM[nStation][nLayer];
  TProfile *hPointDistanceRadiusM[nStation][nLayer];
  TH2F *hPointDistanceRadius2M[nStation][nLayer];
  TH2F *hHitRatePosition[nStation][nLayer];
  
  if(!gSystem->AccessPathName("CbmTrdHitDensityQa.root")){
    cout << "  << found existing root file" << endl;
    tFile = new TFile("CbmTrdHitDensityQa.root","UPDATE");
    hPointDistance = (TH1F*)tFile->Get("PointDistance");
    hPointDistanceRadius = (TProfile*)tFile->Get("PointDistanceRadius");
    hPointDistanceRadius2 = (TH2F*)tFile->Get("PointDistanceRadius2");
    TString name;
    for (Int_t s = 0; s < nStation; s++) 
      for(Int_t l = 0; l < nLayer; l++) {
	name.Form("PointDistanceS%dL%d",s+1,l+1);
	hPointDistanceM[s][l] = (TH1F*)tFile->Get(name);
	name.Form("PointDistanceRadiusS%dL%d",s+1,l+1);
	hPointDistanceRadiusM[s][l] = (TProfile*)tFile->Get(name);
	name.Form("PointDistanceRadius2S%dL%d",s+1,l+1);
	hPointDistanceRadius2M[s][l] = (TH2F*)tFile->Get(name);
	name.Form("HitRate2S%dL%d",s+1,l+1);
	hHitRatePosition[s][l] = (TH2F*)tFile->Get(name);
      }
  }
  else {    
    tFile = new TFile("CbmTrdHitDensityQa.root",/*"CREATE"*/"RECREATE"," ROOT file with histograms");
    cout << "  << file new created" << endl;
    hPointDistance = new TH1F("PointDistance","PointDistance", 10000, 0, 1500);
    hPointDistance->SetXTitle("point to point distance within same module (each pair once) [mm]");
    hPointDistance->SetYTitle("counts");
    hPointDistanceRadius = new TProfile("PointDistanceRadius","PointDistanceRadius", 1000, 0, 10000, 0, 1500);
    hPointDistanceRadius->SetXTitle("radial position of first point [mm]");
    hPointDistanceRadius->SetYTitle("point to point distance within same module (each pair once) [mm]");
    hPointDistanceRadius2 = new TH2F("PointDistanceRadius2","PointDistanceRadius2", 1000, 0, 10000, 10000, 0, 1500);
    hPointDistanceRadius2->SetXTitle("radial position of first point [mm]");
    hPointDistanceRadius2->SetYTitle("point to point distance within same module (each pair once) [mm]");
    TString name;
    for (Int_t s = 0; s < nStation; s++) 
      for(Int_t l = 0; l < nLayer; l++) {
	name.Form("PointDistanceS%dL%d",s+1,l+1);
	hPointDistanceM[s][l] = new TH1F(name, name, 10000, 0, 1500);
	hPointDistanceM[s][l]->SetXTitle("point to point distance within same module (each pair once) [mm]");
	hPointDistanceM[s][l]->SetYTitle("counts");
	name.Form("PointDistanceRadiusS%dL%d",s+1,l+1);
	hPointDistanceRadiusM[s][l] = new TProfile(name, name, 1000, 0, 10000, 0, 1500);
	hPointDistanceRadiusM[s][l]->SetXTitle("radial position of first point [mm]");
	hPointDistanceRadiusM[s][l]->SetYTitle("point to point distance within same module (each pair once) [mm]");
	name.Form("PointDistanceRadius2S%dL%d",s+1,l+1);
	hPointDistanceRadius2M[s][l] =  new TH2F(name, name, 1000, 0, 10000, 10000, 0, 1500);
	hPointDistanceRadius2M[s][l]->SetXTitle("radial position of first point [mm]");
	hPointDistanceRadius2M[s][l]->SetYTitle("point to point distance within same module (each pair once) [mm]");
	name.Form("HitRate2S%dL%d",s+1,l+1);
	hHitRatePosition[s][l] =  new TH2F(name, name, 2000, -10000, 10000, 2000, -10000, 10000);
	hHitRatePosition[s][l]->SetXTitle("x-coordinate [mm]");
	hHitRatePosition[s][l]->SetYTitle("y-coordinate [mm]");
	hHitRatePosition[s][l]->SetZTitle("hits / cm^{2}");
      }
  }
  CbmTrdPoint *pt = NULL;
  //MyPoint* point = new MyPoint;
  Int_t nEntries = fTrdPoints->GetEntries();
  for (int j = 0; j < nEntries ; j++ ) {

    MyPoint* point = new MyPoint;

    pt = (CbmTrdPoint*) fTrdPoints->At(j);
    if(NULL == pt) {
      cout << " no point found " << endl;
    }
    if(NULL == pt) continue;
    fMCindex=pt->GetTrackID();
    // --------------->[mm]<---------------
    Double_t x_in   = pt->GetXIn();
    Double_t x_out  = pt->GetXOut();
    Double_t y_in   = pt->GetYIn();
    Double_t y_out  = pt->GetYOut();
    Double_t z_in   = pt->GetZIn();
    Double_t z_out  = pt->GetZOut();
    Double_t x_mean = (x_in + x_out)/2.;
    Double_t y_mean = (y_in + y_out)/2.;
    Double_t z_mean = (z_in + z_out)/2.;
    gGeoManager->FindNode(x_mean, y_mean, z_mean);
    const Double_t *global_point = gGeoManager->GetCurrentPoint();
    Double_t local_point[3];
    gGeoManager->MasterToLocal(global_point, local_point);
 
    point->global_inC[0]  = x_mean * 10.;
    point->global_inC[1]  = y_mean * 10.;
    point->global_inC[2]  = z_mean * 10.;

    

    fModuleID = pt->GetDetectorID();

    std::map<Int_t, MyPointList* >::iterator it = ModulePointMap.find(fModuleID);
    if (it == ModulePointMap.end())
      {
	ModulePointMap[fModuleID] = new MyPointList;
      }
    ModulePointMap[fModuleID]->push_back(point);
  }
  std::map<Int_t, MyPointList* >::iterator it;
  for ( it = ModulePointMap.begin(); it != ModulePointMap.end(); it++) {
    fModuleInfo     = fDigiPar->GetModule((*it).first);
    fStation = fGeoHandler->GetStation((*it).first);
    fLayer = fGeoHandler->GetLayer((*it).first);
    Float_t xPos1(0), yPos1(0), r1(0), xPos2(0), yPos2(0), r2(0), dr(0);
    MyPointList::iterator listIt1;
    MyPointList::iterator listIt2;
    for ( listIt1 = (*it).second->begin(); listIt1 != (*it).second->end(); listIt1++) {
      xPos1 = (*listIt1)->global_inC[0];
      yPos1 = (*listIt1)->global_inC[1];

      hHitRatePosition[fStation-1][fLayer-1]->Fill(xPos1,yPos1);

      r1 = sqrt(xPos1 * xPos1 + yPos1 * yPos1);

      for ( listIt2 = listIt1; listIt2 != (*it).second->end(); listIt2++) {
	if (listIt2 != listIt1) {
	  xPos2 = (*listIt2)->global_inC[0];
	  yPos2 = (*listIt2)->global_inC[1];	  
	  r2 = sqrt(xPos2 * xPos2 + yPos2 * yPos2);

	  dr = fabs(r1 - r2);

	  hPointDistance->Fill(dr);
	  hPointDistanceRadius->Fill(r1,dr);
	  hPointDistanceRadius2->Fill(r1,dr);

	  hPointDistanceM[fStation-1][fLayer-1]->Fill(dr);
	  hPointDistanceRadiusM[fStation-1][fLayer-1]->Fill(r1,dr);
	  hPointDistanceRadius2M[fStation-1][fLayer-1]->Fill(r1,dr);
	}
      }
    }
  }

  hPointDistance->Write("", TObject::kOverwrite);
  hPointDistanceRadius->Write("", TObject::kOverwrite);
  hPointDistanceRadius2->Write("", TObject::kOverwrite);
  for (Int_t s = 0; s < nStation; s++) 
    for(Int_t l = 0; l < nLayer; l++) {
      hPointDistanceM[s][l]->Write("", TObject::kOverwrite);
      hPointDistanceRadiusM[s][l]->Write("", TObject::kOverwrite);
      hPointDistanceRadius2M[s][l]->Write("", TObject::kOverwrite);
      hHitRatePosition[s][l]->Write("", TObject::kOverwrite);
    }
  for (std::map<Int_t, MyPointList*>::iterator it1 = ModulePointMap.begin(); it1 != ModulePointMap.end(); ++it1) {
    for (MyPointList::iterator pointIt = it1->second->begin(); pointIt != it1->second->end(); ++pointIt) {
      delete *pointIt;
    }
    delete it1->second;
  }
  ModulePointMap.clear();
  tFile->Close();
  //delete tFile;
  /*
  delete hPointDistance;
  delete hPointDistanceRadius;
  delete hPointDistanceRadius2;
  for (Int_t s = 0; s < nStation; s++) 
    for(Int_t l = 0; l < nLayer; l++) {
      delete hPointDistanceM[s][l];
      delete hPointDistanceRadiusM[s][l];
      delete hPointDistanceRadius2M[s][l];
    }
  */
  //delete point;
  //delete pt;
}
    // ---- Register ------------------------------------------------------
void CbmTrdHitDensityQa::Register()
{
  cout << " * CbmTrdHitDensityQa * :: Register()" << endl;
  //FairRootManager::Instance()->Register("TrdHit","Trd Hit", fClusterHits, kTRUE);
}
    // --------------------------------------------------------------------


    // ---- Finish --------------------------------------------------------
void CbmTrdHitDensityQa::Finish()
{
  //cout << " * CbmTrdHitDensityQa * :: Finish()" << endl;
}

    // -----   Public method EndOfEvent   --------------------------------------
void CbmTrdHitDensityQa::FinishEvent() {
  //  cout<<"In CbmTrdHitDensityQa::FinishEvent()"<<endl;
  if (fClusterHits) fClusterHits->Clear();
}
    // -------------------------------------------------------------------------

ClassImp(CbmTrdHitDensityQa)
