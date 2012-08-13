#include "CbmTrdHitProducerCluster.h"

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
CbmTrdHitProducerCluster::CbmTrdHitProducerCluster()
  :FairTask("CbmTrdHitProducerCluster",1),
   fTrdPoints(NULL),
   fDigis(NULL),
   fClusters(NULL),
   fClusterHits(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(new CbmTrdGeoHandler()),
   fPrfSingleRecoCounter(-1.),
   fPrfDoubleRecoCounter(-1.),
   fSimpleRecoCounter(-1.),
   fStation(-1.),
   fLayer(-1.),
   fmoduleId(-1.),
   fxPos(0.),
   fyPos(0.),
   fzPos(0.),
   fnCol(-1.),
   fnRow(-1.),
   fNoSectors(-1.),
   fClusterId(-1.),
   fSectorSizeX(),
   fSectorSizeY(),
   fPadSizeX(),
   fPadSizeY(),
   fSecCol(),
   fSecRow(),
   fPadSizeXArray(),
   fPadSizeYArray(),
   fModuleSize(),
   fModulePosition(),
   fPadSizeLongMap(),
   ModuleHitMap(),
   moduleDigiMap(),
   fHitOutOfModuleCounter(0)
{
}

// ---- Destructor ----------------------------------------------------
CbmTrdHitProducerCluster::~CbmTrdHitProducerCluster()
{
  if(fTrdPoints){
    fTrdPoints->Clear("C");
    fTrdPoints->Delete();
    delete fTrdPoints;
  }
  if(fClusters){
    fClusters->Clear("C");
    fClusters->Delete();
    delete fClusters;
  }
  if(fDigis){
    fDigis->Clear("C");
    fDigis->Delete();
    delete fDigis;
  }    
  if(fClusterHits){
    fClusterHits->Clear("C");
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
void CbmTrdHitProducerCluster::SetParContainers()
{
  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  cout << " * CbmTrdHitProducerCluster * :: SetParContainers()" << endl;
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
}
// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitProducerCluster::ReInit()
{  
  cout << " * CbmTrdHitProducerCluster * :: ReInit()" << endl;
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

  

  return kSUCCESS;
}
// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitProducerCluster::Init()
{
  cout << " * CbmTrdHitProducerCluster * :: Init()" << endl;
  FairRootManager *ioman = FairRootManager::Instance();
  fTrdPoints=(TClonesArray *)ioman->GetObject("TrdPoint"); 
  if ( ! fTrdPoints ) {
    cout << "-W CbmTrdHitProducerCluster::Init: No TrdPoints array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerCluster::Init: No TrdDigi array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }
  fClusters =(TClonesArray *)  ioman->GetObject("TrdCluster");
  if ( ! fClusters ) {
    cout << "-W CbmTrdHitProducerCluster::Init: No TrdCluster array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }    
  fClusterHits = new TClonesArray("CbmTrdHit", 100);
  ioman->Register("TrdHit","TRD Hit",fClusterHits,kTRUE);
  
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

//----------------------------------------------------------------------
bool digiCidSorter(MyDigi *a, MyDigi *b)
{ return (a->combiId < b->combiId); }

// ---- Exec ----------------------------------------------------------
void CbmTrdHitProducerCluster::Exec(Option_t * option)
{ 
  TStopwatch timer;
  timer.Start();
  fHitOutOfModuleCounter = 0;
  Bool_t drawing = false;//true;
  Bool_t meanPosition(false),  drawMCPoints(false),  drawDigis(false),  drawClusters(false),  drawHits(false);
  Bool_t pr = true;//false;//true;
  Bool_t combinatoric = false;//true;
  cout << "================CbmTrdHitProducerCluster==============" << endl;
  cout << " Position resolution:";
  if (pr) {
    cout << " on" << endl;
  }
  else {
    cout << " off" << endl;
  }
  cout << " Full hit to mc-point combinatoric:";
  if (combinatoric) {
    cout << " on" << endl;
  }
  else {
    cout << " off" << endl;
  }

  // TProfile *fDeltaRR = new TProfile("fDeltaRR","fDeltaRR",250,0,10000);
  TProfile *fDeltaRR = new TProfile("fDeltaRR","fDeltaRR",1000,0,100);
  TProfile *fMCPHR = new TProfile("fMCPHR","fMCPHR",250,0,10000);
  TProfile *fDeltaRMCPH = new TProfile("fDeltaRMCPH","fDeltaRMCPH",10,0,10);
  //std::list<Int_t> qMaxMcIdList;
  TH1F *hit2Mc      = NULL;
  TLegend *legend   = NULL;
  TH1F *shortPR     = NULL;
  TH1F  *longPR[20] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  /*
    for (Int_t l = 0; l < 20; l++) {
    longPR[l] = NULL;
    }
  */
  TH2F *PRF = NULL;
  if (drawing) {
    hit2Mc = new TH1F("hit2Mc","MC-points per Hit",11,-0.5,10.5);
    hit2Mc->SetXTitle("Number of associated MC-Points per Hit");
    //hit2Mc->SetYTitle("#");
    hit2Mc->SetYTitle("Percentage");
    hit2Mc->SetStats(kFALSE);
    hit2Mc->GetXaxis()->SetLabelSize(0.02);
    hit2Mc->GetYaxis()->SetLabelSize(0.02);
    hit2Mc->GetXaxis()->SetTitleSize(0.025);
    hit2Mc->GetYaxis()->SetTitleSize(0.025);
    hit2Mc->GetXaxis()->SetTitleOffset(1.5);
    hit2Mc->GetYaxis()->SetTitleOffset(2);
  }
  if (pr) {
    legend = new TLegend(0.55,0.55,0.85,0.85);

    PRF = new TH2F("PRF","PRF for short pad size direction",15*10 , -7.5, 7.5 , 100, 0, 1);
    shortPR = new TH1F("shortPR","PR in short pad size direction", 2*30*10, -30, 30); 
    shortPR->SetXTitle("Hit Position - Contributing MC-Point Positions [mm]");
    shortPR->SetYTitle("#");
    shortPR->SetStats(kFALSE);
    shortPR->GetXaxis()->SetLabelSize(0.02);
    shortPR->GetYaxis()->SetLabelSize(0.02);
    shortPR->GetXaxis()->SetTitleSize(0.025);
    shortPR->GetYaxis()->SetTitleSize(0.025);
    shortPR->GetXaxis()->SetTitleOffset(1.5);
    shortPR->GetYaxis()->SetTitleOffset(2);
    Char_t tempPR[15];
    for (Int_t i = 0; i < 20; i++){
      sprintf(tempPR,"longPR_%d",i);
      longPR[i] = new TH1F(tempPR, "PR in long pad size direction", 2*150/**10*/, -150, 150);
      longPR[i]->SetXTitle("Hit Position - Contributing MC-Point Positions [mm]");
      longPR[i]->SetYTitle("#");
      longPR[i]->SetStats(kFALSE);
      longPR[i]->GetXaxis()->SetLabelSize(0.02);
      longPR[i]->GetYaxis()->SetLabelSize(0.02);
      longPR[i]->GetXaxis()->SetTitleSize(0.025);
      longPR[i]->GetYaxis()->SetTitleSize(0.025);
      longPR[i]->GetXaxis()->SetTitleOffset(1.5);
      longPR[i]->GetYaxis()->SetTitleOffset(2);
    }
  }

  fPrfSingleRecoCounter = 0;
  fPrfDoubleRecoCounter = 0;
  fSimpleRecoCounter = 0;
  if (fDigis == NULL)
    cout << " DEBUG: fdigis is NULL" << endl;

 

  Int_t nDigis = fDigis->GetEntries();
  for (Int_t iDigi=0; iDigi < nDigis; iDigi++ ) {
    
    //cout << (fDigis->GetEntries()-iDigi) << endl;
    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
    
    Int_t moduleId = digi->GetDetId();

    Int_t Station  = fGeoHandler->GetStation(moduleId);
    Int_t Layer    = fGeoHandler->GetLayer(moduleId);
    MyDigi *d = new MyDigi;
	    
    fModuleInfo = fDigiPar->GetModule(moduleId);
    d->digiId = iDigi;
    /*
      if (Layer%2 == 0) {
      d->colId = digi->GetRow();
      d->rowId = digi->GetCol();
      d->combiId = d->rowId * (fModuleInfo->GetnRow() + 1) + d->colId;
      }
      else {
    */
    d->rowId = digi->GetRow();
    d->colId = digi->GetCol();
    d->combiId = d->rowId * (fModuleInfo->GetnCol() + 1) + d->colId;
    /*
      }
    */
    d->charge = digi->GetCharge();
    if (moduleDigiMap.find(moduleId) == moduleDigiMap.end()) {
      moduleDigiMap[moduleId] = new MyDigiList;      
    } 
    moduleDigiMap[moduleId]->push_back(d);

    //    delete d;    
  }
  for (std::map<Int_t, MyDigiList*>::iterator it = moduleDigiMap.begin(); it != moduleDigiMap.end(); ++it) {
    (*it).second->sort(digiCidSorter);
  }
  

  if (fClusters == NULL)
    cout << " DEBUG: fCluster is NULL" << endl;

  //MHitMap* ModuleHitMap = new MHitMap;
  Int_t iHit = 0;    //reconstructed hit counter
  Int_t nDigi;       //Number of digis within a cluster
  Int_t DigiIndex;   //Index of the digi
  Float_t DigiCharge;//Charge of the digi
  Int_t DigiRow;     //Row of the digi
  Int_t DigiCol;     //Column of the digi
  Float_t qMax;      //Maximum digi charge of the activ cluster
  Int_t qMaxIndex;   //Index of the digi containing the maximum charge of the activ cluster
  Int_t NoMcPointsPerHit[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Int_t nCluster = fClusters->GetEntries(); // Number of clusters found by CbmTrdClusterFinderFast
  cout << " Found " << nCluster << " Cluster in Collection" << endl;

  for (Int_t iCluster = 0; iCluster < nCluster; iCluster++) {
    //cout << iCluster << endl;
    CbmTrdCluster *cluster = (CbmTrdCluster*) fClusters->At(iCluster);//pointer to the acvit cluster
    fClusterId = iCluster;
    //cout << "NoDigis:" << cluster->GetNDigis() << endl;
    nDigi = cluster->GetNDigis();
    qMax = 0;
    qMaxIndex = -1;
    for (Int_t iDigi = 0; iDigi < nDigi; iDigi++) {
      DigiIndex = cluster->GetDigiIndex(iDigi);
      CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(DigiIndex);
      DigiCharge = digi->GetCharge();
      //-----------unrotated--------------------
      DigiCol    = digi->GetCol();
      DigiRow    = digi->GetRow();
      //-----------unrotated--------------------
      //cout << "   ID:" << DigiIndex << "  Col:" << DigiCol << "  Row:" << DigiRow << "  Charge:" << DigiCharge << endl;
      if (DigiCharge > qMax) {
	qMax = DigiCharge;
	qMaxIndex = DigiIndex;
      }
    }
    if (qMaxIndex > -1 && qMax > 0.0) {
      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //qMaxMcIdList.push_back(qMaxIndex);
      CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(qMaxIndex);
      Int_t size = digi->GetMCIndex().size();
      Int_t MCs = size;
      std::vector<int> MCIndex = digi->GetMCIndex();
      //cout << size << ": ";
      /*
	for (Int_t i = 0; i < size; i++) {
	CbmTrdDigi *d = (CbmTrdDigi*) fDigis->At(MCIndex[i]);
	if (d->GetCharge() < 0.001) {
	MCs--;
	}
	}
      */
      if (size < 11) {
	NoMcPointsPerHit[MCs]++;
      }
      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      MyHit* hit = new MyHit;
      GetModuleInfo(qMaxIndex, qMax, hit, PRF);
      if (pr) {
	CalcPR(combinatoric, qMaxIndex, shortPR, longPR, legend, PRF, hit, fDeltaRR, fMCPHR, fDeltaRMCPH, MCs);
      }
      if (!drawing)
	delete hit;
    }
  }

  std::map<Int_t, MyHitList* >::iterator it;
  for ( it = ModuleHitMap.begin(); it != ModuleHitMap.end(); it++)
    {
      iHit += Int_t((*it).second->size());
    }
  cout << " Found " << iHit << " Hits, " << fHitOutOfModuleCounter << " Hits out of module reconstructed and not stored" << endl << endl;
  cout << "======================================================" << endl;
  if (drawing) {
    DrawHits( meanPosition,  drawMCPoints,  drawDigis,  drawClusters,  drawHits);
  }
  if (pr) {
    cout << "position resolution" << endl;
    TF1 *fit = new TF1("fit","gaus",-50,50);
    Float_t shortSigma;
    Float_t longSigma;
    Int_t entries;
    if (drawing) {
      TCanvas *cPD = new TCanvas("PositionDiviation","PositionDiviation",1800,600);
      cPD->Divide(3,1);
      cPD->cd(1); 
      fDeltaRR->DrawCopy();
      cPD->cd(2);
      fMCPHR->DrawCopy();
      cPD->cd(3);
      fDeltaRMCPH->DrawCopy();

      fit->SetLineColor(2);
      fit->SetLineWidth(1);
      TCanvas *cPR = new TCanvas("PR","PR",1400,600);
      cPR->Divide(2,1);
      cPR->cd(1)->SetTickx(1);
      cPR->cd(1)->SetTicky(1);
      shortPR->DrawCopy();
      shortPR->Fit("fit","Q","Q",-1,1);
      shortPR->DrawCopy("same");
      shortSigma = fit->GetParameter(2);
      printf("  %6.3f mm position resolution for short pad size direction (including x and y)\n",shortSigma);
      TLegend* shortL = new TLegend(0.55,0.55,0.85,0.85);
      shortL ->AddEntry( shortPR,"Residuals for short pad size direction","l");
      Char_t LegendTitle[70];
      sprintf(LegendTitle,"Position Resolution = %6.3f mm",shortSigma);
      shortL ->AddEntry(fit,LegendTitle,"l");
      shortL ->Draw("same");
      cPR->cd(2)->SetTickx(1);
      cPR->cd(2)->SetTicky(1);   
      Float_t integral = longPR[0]->Integral(); 
      //longPR[0]->Scale(1/integral); 
      //longPR[0]->Reset();  
      Int_t max = 0;
      Int_t maxTemp = 0;
      for (Int_t l = 1; l < 20; l++){
	maxTemp = longPR[l]->GetBinContent(longPR[l]->GetMaximumBin());
	if (maxTemp > max)
	  max = maxTemp;
      }
      for (Int_t l = 0; l < 20; l++)
	longPR[l]->GetYaxis()->SetRangeUser(0.1,max*2);

      longPR[0]->SetLineColor(0); 
      longPR[19]->DrawCopy();   
      //shortPR->DrawCopy("same");
      //fit->SetLineColor(1);
      legend->Draw("same"); 
      longPR[0]->Fit("fit","0Q");
      longSigma = fit->GetParameter(2);
      printf("  %6.3f mm position resolution for long pad size direction  ('') 'longPR_0' (Sum  over all pad sizes)\n",longSigma);   

     

      for (Int_t l = 1; l < 20; l++) {
	entries = longPR[l]->GetEntries();
	if (entries > 0) {
	  integral = longPR[l]->Integral();
	  //longPR[l]->Scale(1/integral);
	  longPR[l]->SetLineColor(l+1);
	  longPR[l]->DrawCopy("same");
	  fit->SetLineColor(l+1);
	  longPR[l]->Fit("fit","0Q");
	  longSigma = fit->GetParameter(2);
	  printf("  %6.3f mm position resolution for long pad size direction  ('') 'longPR_%d'\n",longSigma,l);
	}
      }    
      cPR->cd(1)->SetLogy(1);
      cPR->cd(2)->SetLogy(1);
      cPR->SaveAs("PR.pdf");
      if(!drawing) {
	delete cPD;
	delete cPR;
      }
      delete shortL;    
    }
    else {
      shortPR->Fit("fit","0Q");
      shortSigma = fit->GetParameter(2);
      printf("  %6.3f mm position resolution for short pad size direction (independent of global x and y)\n",shortSigma);
      longPR[0]->Fit("fit","0Q");
      longSigma = fit->GetParameter(2);
      printf("  %6.3f mm position resolution for long pad size direction  ('') 'longPR_0' (Sum  over all pad sizes)\n",longSigma); 
      for (Int_t l = 1; l < 20; l++) {
	entries = longPR[l]->GetEntries();
	if (entries > 0) {
	  longPR[l]->Fit("fit","0Q");
	  longSigma = fit->GetParameter(2);
	  printf("  %6.3f mm position resolution for long pad size direction  ('') 'longPR_%d'\n",longSigma,l);
	}
      }     
    }
    delete fit;
    /*
      cout << "  " << shortSigma << " mm position resolution for short pad size direction (including x and y)" << endl;
      cout << "   " << longSigma  << " mm position resolution for long pad size direction  ('')"  << endl << endl;
    */

  }



  Int_t nPoints = fTrdPoints->GetEntriesFast();
  
  printf("\n  %6.3f%% MC-point to hit efficiency\n  %6d Hits are PRF based reconstructed in one dimension\n  %6d Hits are PRF based reconstructed in both dimension\n  %6d Hits are 'center position of pad with maximum charge' based reconstructed in both dimension\n\n",
	 iHit*100/Float_t(nPoints),fPrfSingleRecoCounter,fPrfDoubleRecoCounter,fSimpleRecoCounter);
  /*
    cout << "  " << iHit*100/Float_t(nPoints)<< "% MC-point to hit efficiency" << endl;
    cout << "  " << fPrfSingleRecoCounter << " Hits are PRF based reconstructed in one dimension" << endl;
    cout << "  " << fPrfDoubleRecoCounter << " Hits are PRF based reconstructed in both dimension" << endl;
    cout << "   " << fSimpleRecoCounter << " Hits are 'center position of pad with maximum charge' based reconstructed in both dimension" << endl << endl;
  */
  for (Int_t i = 0; i < 11; i++) {
    printf("  %2d MC-points per Hit: %5d Hits  (%6.3f%%)\n",i,NoMcPointsPerHit[i],NoMcPointsPerHit[i]*100/Float_t(iHit));
    if (drawing) {
      hit2Mc->SetBinContent(i+1,NoMcPointsPerHit[i]*100/Float_t(iHit));
    }
    //cout << "  " << i << " MC-points per Hit: " << NoMcPointsPerHit[i] << " Hits  (" << NoMcPointsPerHit[i]*100/Float_t(iHit) << "%)" << endl;;
  }
  if (drawing) {
    Float_t K3 = 0.525; 
    Float_t K2 = 3.14159265 / 2.* ( 1. - sqrt(K3)/2.);
    Float_t K1 = (K2 * sqrt(K3)) / (4. * atan(sqrt(K3)));
    Float_t W = 7.5;
    Float_t par = 1;
    Float_t h = 3;
    Char_t formula[500];
    sprintf(formula," -1. / (2. * atan(sqrt(%f))) * (atan(sqrt(%f) *tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f + 2.* x * %f) / (8.* %f) )) +  atan(sqrt(%f) *  tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f - 2.* x * %f) / (8.* %f) )) )",K3,K3,K3,W,par,h,K3,K3,W,par,h);
    TF1* Mathieson = new TF1("mathieson",formula, -7.5, 7.5);
    TCanvas* c2 = new TCanvas("hit","hit 2 mc",1400,600);
    c2->Divide(2,1);
    c2->cd(1)->SetTickx(1);
    c2->cd(1)->SetTicky(1);
    hit2Mc->DrawCopy();
    c2->cd(2)->SetTickx(1);
    c2->cd(2)->SetTicky(1);
    PRF->DrawCopy("colz");
    Mathieson->DrawCopy("same");
    c2->SaveAs("MC2hit.pdf");
    delete Mathieson;    
    delete hit2Mc;
    delete c2;
  }
  delete PRF;
  if (pr) {
    delete legend;
    delete shortPR;
    for (Int_t l = 0; l < 20; l++) {
      delete longPR[l];
    }
  }
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");

  fPadSizeLongMap.clear();
  for (std::map<Int_t, MyHitList*>::iterator it2 = ModuleHitMap.begin(); it2 != ModuleHitMap.end(); ++it2) {
    it2->second->clear();
  }
  ModuleHitMap.clear();
  for (std::map<Int_t, MyDigiList*>::iterator it1 = moduleDigiMap.begin(); it1 != moduleDigiMap.end(); ++it1) {
    for (MyDigiList::iterator digisIt = it1->second->begin(); 
	 digisIt != it1->second->end(); ++digisIt) {
      delete *digisIt;
    }
    delete it1->second;
  }
  moduleDigiMap.clear();

  delete fDeltaRR;
  delete fMCPHR;
  delete fDeltaRMCPH;

  // temp fix of nClusters
  //    fClusters->Clear("C");
  //fClusterHits->Clear("C");
}



  // --------------------------------------------------------------------
void CbmTrdHitProducerCluster::GetModuleInfo(Int_t qMaxIndex, Double_t qMax, MyHit* hit, TH2F*& PRF)
{
  //cout << "GetModuleInfo" << endl;
  //ModulePara* mPara = new ModulePara;
  //MyHit* hit = new MyHit;
 
  CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(qMaxIndex);  
  Int_t moduleId = digi->GetDetId();
  fModuleInfo = fDigiPar->GetModule(moduleId);

  fStation  = fGeoHandler->GetStation(moduleId);
  fLayer    = fGeoHandler->GetLayer(moduleId);
  fmoduleId = moduleId;
  //cout << detInfo[1] << "  " << detInfo[2] << endl;
  //-----------rotated----------------------

  hit -> rowId = digi->GetRow();
  hit -> colId = digi->GetCol();
  hit -> digiId = qMaxIndex;
  hit -> moduleId = moduleId;
  /*
    mPara -> xPos = (Int_t)(10 * fModuleInfo->GetX());
    mPara -> yPos = (Int_t)(10 * fModuleInfo->GetY());
    mPara -> zPos = (Int_t)(10 * fModuleInfo->GetZ());
    mPara -> nCol = fModuleInfo->GetnCol();
    mPara -> nRow = fModuleInfo->GetnRow();
    mPara -> NoSectors = fModuleInfo->GetNoSectors();
  */
  fModulePosition[0]/*xPos*/ = (Int_t)(10 * fModuleInfo->GetX());
  fModulePosition[1]/*yPos*/ = (Int_t)(10 * fModuleInfo->GetY());
  fModulePosition[3]/*zPos*/ = (Int_t)(10 * fModuleInfo->GetZ());
  fnCol = fModuleInfo->GetnCol();
  fnRow = fModuleInfo->GetnRow();
  fNoSectors = fModuleInfo->GetNoSectors();
  
  const Int_t NoSectors = fModuleInfo->GetNoSectors();
  
  fSectorSizeX.resize(NoSectors);
  fSectorSizeY.resize(NoSectors);
  fPadSizeX.resize(NoSectors);
  fPadSizeY.resize(NoSectors);
  fSecRow.resize(NoSectors);
  fSecCol.resize(NoSectors);   

  for (Int_t i = 0; i < NoSectors; i++)
    {
      /*
	mPara -> SectorSizeX[i] = 10 * fModuleInfo->GetSectorSizex(i);
	mPara -> SectorSizeY[i] = 10 * fModuleInfo->GetSectorSizey(i);
	mPara -> PadSizeX[i]    = 10 * fModuleInfo->GetPadSizex(i);
	mPara -> PadSizeY[i]    = 10 * fModuleInfo->GetPadSizey(i);
	mPara -> SecRow[i]      = Int_t(mPara->SectorSizeY[i] / mPara->PadSizeY[i]);
	mPara -> SecCol[i]      = Int_t(mPara->SectorSizeX[i] / mPara->PadSizeX[i]);
      */
      fSectorSizeX[i] = 10 * fModuleInfo->GetSectorSizex(i);
      fSectorSizeY[i] = 10 * fModuleInfo->GetSectorSizey(i);
      fPadSizeX[i]    = 10 * fModuleInfo->GetPadSizex(i);
      fPadSizeY[i]    = 10 * fModuleInfo->GetPadSizey(i);
      fSecRow[i]      = Int_t(fSectorSizeY[i] / fPadSizeY[i]);
      fSecCol[i]      = Int_t(fSectorSizeX[i] / fPadSizeX[i]);
      //printf("M(%.1f,%.1f) SS(%.1f,%.1f) N(%d,%d) S(%d,%d) PS(%.1f,%.1f)\n",ModuleSizeX,ModuleSizeY,SectorSizeX[i],SectorSizeY[i],nCol,nRow,SecCol[i],SecRow[i],PadSizeX[i],PadSizeY[i]);
    }
  /*
    hit -> xPos = (mPara->xPos - 10 * fModuleInfo-> GetSizex());
    hit -> yPos = (mPara->yPos - 10 * fModuleInfo-> GetSizey());
  */
  /*
    Row and Columns are counted across sectors. At this point this absolute row and column ID has to be converted to sector row and column IDs within sectors.
  */
  Int_t rs = 0;
  Int_t RowOfSec = hit -> rowId;
  while (RowOfSec > fSecRow[rs]) {   
    RowOfSec -= fSecRow[rs];
    rs++;
  }
  Int_t cs = 0; 
  Int_t ColOfSec = hit -> colId;
  while (ColOfSec > fSecCol[cs]) {    
    ColOfSec -= fSecCol[cs];
    cs++;
  }
 
  Int_t Sector;
  if (cs > rs) Sector = cs;
  else Sector = rs;

  TVector3 posHit;
  TVector3 padSize;
  fModuleInfo->GetPosition(ColOfSec+1, RowOfSec+1, moduleId, Sector, posHit, padSize);
  
  hit -> xPos = posHit[0]*10;
  hit -> yPos = posHit[1]*10;
  
  hit -> zPos = posHit[2]*10;//mPara->zPos;
  hit -> secIdX = GetSector(true,  digi->GetCol()/*, mPara*/);
  hit -> secIdY = GetSector(false, digi->GetRow()/*, mPara*/);
 
  Int_t neighbourIds[4] = {-1, -1, -1, -1};
  SearchNeighbours(qMaxIndex, qMax, neighbourIds/*, mPara*/, moduleDigiMap[fmoduleId], hit, PRF);
  //delete mPara;
}// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::SearchNeighbours(Int_t qMaxIndex, Double_t qMax, Int_t *neighbourIds/*, ModulePara* mPara*/, MyDigiList *neighbours, MyHit* hit, TH2F*& PRF)
  {
    // cout << "SearchNeighbours |->" << endl;
    Int_t counterX = 0;
    Int_t counterY = 0;

    //Float_t qMax = 0;
    Int_t hitCombiId   = hit-> rowId     * (fnCol + 1) + hit->colId;
    Int_t leftCombiId  = hitCombiId - 1;
    Int_t rightCombiId = hitCombiId + 1;
    Int_t downCombiId  = (hit->rowId+1) * (fnCol + 1) + hit->colId;
    Int_t upCombiId    = (hit->rowId-1) * (fnCol + 1) + hit->colId;
    MyDigiList::iterator it = neighbours->begin();
  
    while (it != neighbours->end()) {
      /*
	| |3| | 
	|0| |1| 
	| |2| |
      */

      if ((*it)->combiId == rightCombiId){
	neighbourIds[1] = (*it)->digiId;
	counterX++;
      }
      if ((*it)->combiId == leftCombiId){
	neighbourIds[0] = (*it)->digiId;
	counterX++;
      }
      if((*it)->combiId == downCombiId){
	neighbourIds[2] = (*it)->digiId;
	counterY++;
      }
      if((*it)->combiId == upCombiId){
	neighbourIds[3] = (*it)->digiId;
	counterY++;
      }
      ++it;
    }

    /*
      Char_t orientation[15];
      sprintf(orientation," x is short");
      if (mPara->Layer%2 == 0) {
      sprintf(orientation," x is long");
      }
      cout << "x: " << counterX << " y: " << counterY << orientation << endl;
    */
    SimpleReco(qMaxIndex, qMax/*, mPara*/, neighbourIds, hit/*, ModuleHitMap*/, PRF);
    //cout << "SearchNeighbours ->|" << endl;
  }
  // --------------------------------------------------------------------
Float_t CbmTrdHitProducerCluster::Prf(Float_t padWidth, Float_t sigma, Float_t qLeft, Float_t qMax, Float_t qRight)
{
  //cout << "Prf |->" << endl;
  Float_t recoPos = 0;
  if (qMax > 0 && qLeft > 0 && qRight > 0 && qLeft != qRight /*&& qMax > qLeft && qMax > qRight*/) {
    if (sigma > 0) {
   
      Float_t wLeft = pow(qLeft,2);
      Float_t wRight = pow(qRight,2);
      recoPos = 1. / (wLeft + wRight) * (
					 wLeft  * (-0.5 * padWidth + pow(sigma,2) / padWidth * log(qMax   / qLeft)) 
					 +
					 wRight * (+0.5 * padWidth + pow(sigma,2) / padWidth * log(qRight /  qMax)) 
					 );
    }
    else {
      recoPos = 0.5 * padWidth * log((qRight / qLeft)) / log((pow(qMax,2) / (qRight * qLeft)));
    }
  }
  //cout << "Prf ->|" << endl;
  return recoPos;
}
 // --------------------------------------------------------------------
Float_t CbmTrdHitProducerCluster::dPrf(Float_t padWidth, Float_t sigma, Float_t qLeft, Float_t qMax, Float_t qRight)
{
  Float_t dqLeft  = 0.1 * qLeft; // Asume 10% charge measurement error
  Float_t dqRight = 0.1 * qRight;
  Float_t dqMax   = 0.1 * qRight;
  Float_t PosError = 
    /*
      dqLeft * 
      (0.5 * padWidth * -1. / qLeft * (log(qMax * qMax / (qRight * qLeft)) - log(qRight / qLeft)) / (pow(log(qMax * qMax / (qRight * qLeft)),2))) +
      dqRight *
      ((1. / (qLeft * qRight) * log(qMax * qMax / (qRight * qLeft)) - (1. / qRight * log(qRight / qLeft))) / (pow(log(qMax * qMax / (qRight * qLeft)),2))) +
      dqMax *
      (-1. / qMax * padWidth * log(qRight / qLeft) / log(qMax * qMax / (qRight * qLeft)));
    */
    /*
    dqLeft * 
    (-0.5 * padWidth * 1.0 / (qLeft * qLeft)   * 1.0 / (log(qMax * qMax / (qRight * qLeft)) * log(qMax * qMax / (qRight * qLeft)))) +
    dqRight *
    ( 0.5 * padWidth * 1.0 / (qRight * qRight) * 1.0 / (log(qMax * qMax / (qRight * qLeft)) * log(qMax * qMax / (qRight * qLeft)))) +
    dqMax *
    (padWidth / qMax * log(qRight / qLeft) / (log((qMax * qMax) / (qRight * qLeft)) * log((qMax * qMax) / (qRight * qLeft))));
    */
    0.1 * (0.5 * padWidth - pow(0.5 * padWidth * log((qRight / qLeft)) / log((pow(qMax,2) / (qRight * qLeft))),2));
  //pow(0.1 * 0.5 * padWidth * log((qRight / qLeft)) / log((pow(qMax,2) / (qRight * qLeft))),2);

  return PosError;
    }
  // --------------------------------------------------------------------
Float_t CbmTrdHitProducerCluster::PrfReco(Int_t qMaxIndex, Float_t qMax/*, ModulePara* mPara*/, Int_t *neighbourIds, MyHit* hit, TH2F*& PRF)
{
  //cout << "PrfReco |->" << endl;
  Double_t K3 = 0.525;
  Float_t dxPos = 0;
  Float_t dyPos = 0;
  Float_t dzPos = 0;
  Float_t sigma = 0;//4 * pow(atanh(2 + K3),-0.5) / (3.14159265 * (1 - 0.5 * sqrt(K3)));
  Float_t padWidth;
  Float_t qLeft = 0;
  Float_t qRight = 0;
  Float_t qSum = qMax;
  Int_t left  = neighbourIds[0];
  Int_t right = neighbourIds[1];
  Int_t up    = neighbourIds[3];
  Int_t down  = neighbourIds[2];
  CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(qMaxIndex);
  //if (qMax != digi->GetCharge())
  //printf("%.2e   %.2e\n",qMax,digi->GetCharge());
  //qMax = digi->GetCharge();

  if (left >= 0 && right >= 0) {
    digi = (CbmTrdDigi*) fDigis->At(left);
    qLeft = digi->GetCharge();
    digi = (CbmTrdDigi*) fDigis->At(right);
    qRight = digi->GetCharge();
    padWidth = fPadSizeX[hit->secIdX];
    dxPos = Prf( padWidth, sigma, qLeft, qMax, qRight);
    hit->exPos = dPrf( padWidth, sigma, qLeft, qMax, qRight);
  }
  if (up >= 0 && down >= 0) {
    digi = (CbmTrdDigi*) fDigis->At(up);
    qLeft = digi->GetCharge();
    digi = (CbmTrdDigi*) fDigis->At(down);
    qRight = digi->GetCharge();
    padWidth = fPadSizeY[hit->secIdY];
    dyPos = Prf( padWidth, sigma, qLeft, qMax, qRight);
    hit->eyPos = dPrf( padWidth, sigma, qLeft, qMax, qRight);
  }
  //cout << " 1" << endl;
  if ((left >= 0 && right >= 0) && (up >= 0 && down >= 0)) {
    fPrfDoubleRecoCounter++;
    hit->NoPrfRecoDim = 2;
  }
  else {
    if ((left >= 0 && right >= 0) || (up >= 0 && down >= 0)) {
      fPrfSingleRecoCounter++;
      hit->NoPrfRecoDim = 1;
    }
    else {
      fSimpleRecoCounter++;
      hit->NoPrfRecoDim = 0;
    }
  }
  //cout << " 2" << endl;
  //cout << " x: " << dxPos << " y: " << dyPos << endl;

  hit->xPos += dxPos;
  hit->yPos += dyPos;
  //cout << " 3" << endl;
  if (PRF){
    if (qLeft > 0 && qRight > 0) {
      qSum += qLeft + qRight; 
      if (qMax/qSum < 1) {
	//cout << qLeft << " " << qMax << " " << qRight << "     " << qSum << endl; 
	/*
	  cout << "Y" << hit->secIdY << "X" << hit->secIdX << endl;
	  cout << "Y" << fPadSizeY.size() << "X" << fPadSizeX.size() << endl;
	  cout << "Y" << fPadSizeY[hit->secIdY] << "X" << fPadSizeX[hit->secIdX] << endl;
	  cout << fLayer << "%2=" << fLayer %2 << endl;
	*/
	if (fLayer %2 == 0) {
	  //cout << "Y" << endl;
	  PRF->Fill(dyPos,                             qMax  /qSum);
	  //cout << " 4" << endl;
	  PRF->Fill(dyPos-fPadSizeY[hit->secIdY],qRight/qSum);
	  //cout << " 5" << endl;
	  PRF->Fill(dyPos+fPadSizeY[hit->secIdY],qLeft /qSum);
	  //cout << " 6" << endl;
	}
	else {   
	  //cout << "X" << endl;
	  PRF->Fill(dxPos,                             qMax  /qSum);
	  //cout << " 4" << endl;
	  PRF->Fill(dxPos-fPadSizeX[hit->secIdX],qRight/qSum);
	  //cout << " 5" << endl;
	  PRF->Fill(dxPos+fPadSizeX[hit->secIdX],qLeft /qSum);
	  //cout << " 6" << endl;
	}
      }
    }
  }
  //within each row
  //also possible to reconstruct in x and y direction by prf neighbours
  //if no neighbours found -> simple reco
  //cout << "PrfReco ->|" << endl;
  return qSum;
}
  // --------------------------------------------------------------------
Int_t CbmTrdHitProducerCluster::GetSector(Bool_t x, Int_t DigiCol/*, ModulePara* mPara*/)
  {
    //cout << "GetSector |->" << endl;
    //cout << x << "  " << DigiCol << endl;
    Int_t iSec = 0;
    if (x) {
      while (DigiCol > 0)
	{
	  if (fSecCol[iSec] < fnCol)
	    {
	      if (DigiCol > fSecCol[iSec])
		{
		  DigiCol -= fSecCol[iSec];
		  iSec++;
		}
	      if (DigiCol <= fSecCol[iSec] && DigiCol > 0)
		{
		  DigiCol -= DigiCol;
		}
	    }    
	  else {
	    DigiCol -= DigiCol;
	  }     
	}
    }
    else {
      while (DigiCol > 0)
	{
	  if (fSecRow[iSec] < fnRow)
	    {
	      if (DigiCol > fSecRow[iSec])
		{
		  DigiCol -= fSecRow[iSec];
		  iSec++;
		}
	      if (DigiCol <= fSecRow[iSec] && DigiCol > 0)
		{
		  DigiCol -= DigiCol;
		}
	    }   
	  else {
	    DigiCol -= DigiCol;
	  }   
	}
    }
    // cout << "GetSector ->|" << endl;
    return iSec;
  }
  // --------------------------------------------------------------------
void CbmTrdHitProducerCluster::SimpleReco(Int_t qMaxIndex, Float_t qMax/*, ModulePara* mPara*/, Int_t *neighbourIds, MyHit* hit/*, MHitMap* ModuleHitMap*/,TH2F*& PRF)
{
  //cout << "SimpleReco |->" << endl;
  Int_t DigiCol = hit-> colId;
  Int_t iSec = 0;
  hit->dxPos = 0.5 * fPadSizeX[iSec];
  hit->exPos = hit->dxPos;
  if (DigiCol == 0)
    {
      //hit->xPos += 0.5 * mPara->PadSizeX[iSec];
      hit->dxPos = 0.5 * fPadSizeX[iSec];
    }
  /*
    while (DigiCol > 0)
    {
    if (mPara->SecCol[iSec] < mPara->nCol)
    {
    if (DigiCol > mPara->SecCol[iSec])
    {
    hit->xPos += mPara->SectorSizeX[iSec];
    DigiCol -= mPara->SecCol[iSec];
    iSec++;
    }
    if (DigiCol <= mPara->SecCol[iSec] && DigiCol > 0)
    {
    hit->xPos += DigiCol * mPara->PadSizeX[iSec] + 0.5 * mPara->PadSizeX[iSec];
    DigiCol -= DigiCol;
    hit->dxPos = 0.5 * mPara->PadSizeX[iSec];
    }
    }      
    if (iSec == 0 && mPara->SecCol[iSec] == mPara->nCol && DigiCol > 0)
    {
    hit->xPos += DigiCol * mPara->PadSizeX[iSec] + 0.5 * mPara->PadSizeX[iSec];
    DigiCol -= DigiCol;
    hit->dxPos = 0.5 * mPara->PadSizeX[iSec];
    }
    //cout << "  " << iSec << "   " << Layer << "  col " << DigiCol << "  row " << DigiRow << endl;  
    }
  */
  iSec = 0;
  Int_t DigiRow = hit-> rowId;
  hit->dyPos = 0.5 * fPadSizeY[iSec];
  hit->eyPos = hit->dyPos;
  if (DigiRow == 0)
    {
      //hit->yPos += 0.5 * mPara->PadSizeY[iSec];
      hit->dyPos = 0.5 * fPadSizeY[iSec];
    }
  /*
    while (DigiRow > 0)
    {
    if (mPara->SecRow[iSec] < mPara->nRow)
    {
    if (DigiRow > mPara->SecRow[iSec])
    {
    hit->yPos += mPara->SectorSizeY[iSec];
    DigiRow -= mPara->SecRow[iSec];
    iSec++;
    }
    if (DigiRow <= mPara->SecRow[iSec] && DigiRow > 0)
    {
    hit->yPos += DigiRow * mPara->PadSizeY[iSec] + 0.5 * mPara->PadSizeY[iSec];
    DigiRow -= DigiRow;
    hit->dyPos = 0.5 * mPara->PadSizeY[iSec];
    }
    }      
    if (iSec == 0 && mPara->SecRow[iSec] == mPara->nRow && DigiRow > 0)
    {
    hit->yPos += DigiRow * mPara->PadSizeY[iSec] + 0.5 * mPara->PadSizeY[iSec];
    DigiRow -= DigiRow;
    hit->dyPos = 0.5 * mPara->PadSizeY[iSec];
    }    
    //cout << "  " << iSec << "   " << Layer << "  col " << DigiCol << "  row " << DigiRow << endl;  
    }
  */
  Double_t qCluster = PrfReco(qMaxIndex, qMax/*, mPara*/, neighbourIds, hit, PRF);
  
  //cout << "layer " << Layer << "  col " << nCol << "  row " << nRow << endl;
  //cout << "      " << Layer << "  col " << DigiCol << "  row " << DigiRow << endl;
  //cout << "         (" << x << "," << y << "," << z << ")" << endl;
  
  TVector3  pos(hit->xPos/10.,   hit->yPos/10.,  hit->zPos/10.);
  TVector3 dpos(hit->exPos/10., hit->eyPos/10., hit->ezPos/10.);
  Double_t dxy = 0;
  Int_t planeId = 0;
  Double_t eLossTR = 0;
  Double_t eLossdEdx = 0;
  Double_t eLoss = qCluster;
  if (eLoss == 0.0)
    printf("%i  %.2e  %.2e\n",qMaxIndex,eLoss,qMax);

  planeId=fGeoHandler->GetPlane(hit->moduleId);
  // if (hit within module) else printf()
 
  if (hit->xPos/10. <= fModuleInfo->GetX()+fModuleInfo->GetSizex() && hit->xPos/10. >= fModuleInfo->GetX()-fModuleInfo->GetSizex()
      && 
      hit->yPos/10. <= fModuleInfo->GetY()+fModuleInfo->GetSizey() && hit->yPos/10. >= fModuleInfo->GetY()-fModuleInfo->GetSizey())
       AddHit( qMaxIndex, hit->moduleId, pos, dpos, dxy, planeId, eLossTR, eLossdEdx, eLoss);
  else {
    fHitOutOfModuleCounter++;
    /*
      printf("hit out of module (%.2f, %.2f) (%.2f - %.2f, %.2f - %.2f)\n",hit->xPos/10.,hit->yPos/10.,
      fModuleInfo->GetX()-fModuleInfo->GetSizex(),
      fModuleInfo->GetX()+fModuleInfo->GetSizex(),
      fModuleInfo->GetY()-fModuleInfo->GetSizey(),
      fModuleInfo->GetY()+fModuleInfo->GetSizey());
    */
  }
  std::map<Int_t, MyHitList* >::iterator it = ModuleHitMap.find(fmoduleId);
  if (it == ModuleHitMap.end())
    {
      ModuleHitMap[fmoduleId] = new MyHitList;
    }
  hit->digiId = 13;
  ModuleHitMap[fmoduleId]->push_back(hit);
  //printf("(%6.f,%6.f)\n",hit->xPos/10.,hit->yPos/10.);
  // cout << "SimpleReco ->|" << endl;
}

  // --------------------------------------------------------------------
void CbmTrdHitProducerCluster::DrawHits(Bool_t mean, Bool_t drawMCPoints, Bool_t drawDigis, Bool_t drawClusters, Bool_t drawHits)
{  
  Char_t tracer[5] = "";
  Bool_t picPdf = false;
  Bool_t picPng = true;
  if (picPdf) {
    picPng = false;
  }
  Double_t DigiMax = 0;//0.1;
  /*
  //Bool_t mean = false;
  Bool_t mean = true;

  Bool_t drawMCPoints = false;
  //Bool_t drawMCPoints = true;

  Bool_t drawDigis = false;
  //Bool_t drawDigis = true;

  Bool_t drawClusters = false;
  //Bool_t drawClusters = true;

 Bool_t drawHits = false;
 //Bool_t drawHits = true;
 */
  cout << "  Drawing ";
  if(drawMCPoints) {
    cout << "MC-Points ";
    if(mean) {
      cout << "mean, ";
    }
    else {
      cout << "in and out, ";
    }
  }
  if (drawDigis) {
    cout << "Digis, ";
  }
  if (drawClusters) {
    cout << "Clusters, ";
  }
  if (drawHits) {
    cout << "Hits, ";
  }
  //cout << endl;
  Int_t cs = 800;
  Int_t moduleId;
  //_____________MC Points_______________________________
  std::map<Int_t, MyHitList*> ModuleMCPointMap;
  //if (drawMCPoints)
  {
    Int_t nPoints = fTrdPoints->GetEntriesFast();
    CbmTrdPoint *pt = NULL;
    for (Int_t iPoint = 0; iPoint < nPoints ; iPoint++ ) 
      {
	pt = (CbmTrdPoint*) fTrdPoints->At(iPoint);
	moduleId = pt->GetDetectorID();
	Double_t x_in   = pt->GetXIn();
	Double_t x_out  = pt->GetXOut();
	Double_t y_in   = pt->GetYIn();
	Double_t y_out  = pt->GetYOut();
	Double_t z_in   = pt->GetZIn();
	Double_t z_out  = pt->GetZOut();
	Double_t x_mean = (x_in + x_out)/2.;
	Double_t y_mean = (y_in + y_out)/2.;
	Double_t z_mean = (z_in + z_out)/2.;

	const Double_t *global_point = gGeoManager->GetCurrentPoint();
	Double_t local_point[3];
	/*
	  global_meanC= {x_mean, y_mean , z_mean};//[cm]      
	  global_inC  = {x_in  , y_in   , z_in  };//[cm]      
	  global_outC = {x_out , y_out  , z_out };//[cm]
	*/
            
	Double_t global_inC[3];
	Double_t global_outC[3];
	Double_t global_meanC[3];
      
	Double_t local_inC[3];
	Double_t local_outC[3];
	Double_t local_meanC[3];

	global_inC[0]  = x_in  ;//[cm]      
	global_outC[0] = x_out ;//[cm]
	global_meanC[0]= x_mean;//[cm]
            
	global_inC[1]  = y_in  ;//[cm]      
	global_outC[1] = y_out ;//[cm]
	global_meanC[1]= y_mean;//[cm]
           
	global_inC[2]  = z_in  ;//[cm]      
	global_outC[2] = z_out ;//[cm]
	global_meanC[2]= z_mean;//[cm] 

	gGeoManager->MasterToLocal(global_point, local_point);
	gGeoManager->MasterToLocal(global_meanC, local_meanC);
	gGeoManager->MasterToLocal(global_inC, local_inC);
	gGeoManager->MasterToLocal(global_outC, local_outC);
      
	for ( Int_t idim = 0; idim < 3; idim++)
	  {
	    local_meanC[idim]  *= 10; //[cm]->[mm]
	    local_inC[idim]    *= 10; //[cm]->[mm]
	    local_outC[idim]   *= 10; //[cm]->[mm]

	    global_meanC[idim] *= 10; //[cm]->[mm]
	    global_inC[idim]   *= 10; //[cm]->[mm]
	    global_outC[idim]  *= 10; //[cm]->[mm]
	  }
      
	//printf("g   (%f,%f)\nl   (%f,%f)\n\n",global_meanC[0],global_meanC[1],local_meanC[0],local_meanC[1]);

	if (mean)
	  {
	    MyHit* hit = new MyHit;
	    fModuleInfo         = fDigiPar->GetModule(moduleId);
	    hit -> nCol     = fModuleInfo->GetnCol();
	    hit -> nRow     = fModuleInfo->GetnRow();
	    hit -> charge   = 0;//digi->GetCharge();
	    hit -> digiId   = iPoint;
	    hit -> moduleId = moduleId;
	    hit -> xPos     = global_meanC[0];//local_meanC[0];
	    hit -> yPos     = global_meanC[1];//local_meanC[1];

	    if (ModuleMCPointMap.find(moduleId) == ModuleMCPointMap.end())
	      {
		ModuleMCPointMap[moduleId] = new MyHitList;
	      }
	    ModuleMCPointMap[moduleId]->push_back(hit);
	  }
	else
	  {
	    MyHit* hit = new MyHit;
	    fModuleInfo     = fDigiPar->GetModule(moduleId);
	    hit -> nCol     = fModuleInfo->GetnCol();
	    hit -> nRow     = fModuleInfo->GetnRow();
	    hit -> xPos     = global_inC[0];//local_inC[0];
	    hit -> yPos     = global_inC[1];//local_inC[1];
	    hit -> charge   = 0;//digi->GetCharge();
	    hit -> digiId   = iPoint;
	    hit -> moduleId = moduleId;
	    hit -> dxPos    = global_outC[0];//local_outC[0];
	    hit -> dyPos    = global_outC[1];//local_outC[1];

	    if (ModuleMCPointMap.find(moduleId) == ModuleMCPointMap.end())
	      {
		ModuleMCPointMap[moduleId] = new MyHitList;
	      }
	    ModuleMCPointMap[moduleId]->push_back(hit);
	  }
      }
  }

  //_____________Digi_____________________________________
    
  std::map<Int_t, MyHitList*> ModuleDigiMap;
  //if (drawDigis)
  {
    Int_t nDigi = fDigis->GetEntries();
    for (Int_t iDigi = 0; iDigi < nDigi; iDigi++)
      {
	CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
	moduleId = digi->GetDetId();

	MyHit* hit = new MyHit;
	fModuleInfo     = fDigiPar->GetModule(moduleId);
	hit -> nCol     = fModuleInfo->GetnCol();
	hit -> nRow     = fModuleInfo->GetnRow();
	hit -> rowId    = digi->GetRow();
	hit -> colId    = digi->GetCol();
	hit -> charge   = digi->GetCharge();
	hit -> digiId   = iDigi;
	hit -> moduleId = moduleId;
      
	if (ModuleDigiMap.find(moduleId) == ModuleDigiMap.end())
	  {
	    ModuleDigiMap[moduleId] = new MyHitList;
	  }
	ModuleDigiMap[moduleId]->push_back(hit);

      }
  }
  
  //_____________Cluster__________________________________
  std::map<Int_t, Int_t> clusterId;
  std::map<Int_t, MyHitList*> ModuleClusterMap;
  //if (drawClusters)
  {
    Int_t nCluster = fClusters->GetEntries();
    for (Int_t iCluster = 0; iCluster < nCluster; iCluster++)
      { 
	CbmTrdCluster *cluster = (CbmTrdCluster*) fClusters->At(iCluster);
	Int_t dId = cluster->GetDigiIndex(0);
	CbmTrdDigi *d = (CbmTrdDigi*) fDigis->At(dId);
	moduleId = d->GetDetId();
	if (clusterId.find(moduleId) == clusterId.end())
	  {
	    clusterId[moduleId] = 0;
	  }
	clusterId[moduleId] += 1;

	Int_t NDigis = cluster->GetNDigis();
	for (Int_t iDigi = 0; iDigi < NDigis; iDigi++)
	  {
	    Int_t digiId = cluster->GetDigiIndex(iDigi);
	    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(digiId);
	    moduleId = digi->GetDetId();

	    MyHit* hit = new MyHit;
	    fModuleInfo     = fDigiPar->GetModule(moduleId);
	    hit -> nCol     = fModuleInfo->GetnCol();
	    hit -> nRow     = fModuleInfo->GetnRow();
	    hit -> rowId    = digi->GetRow();
	    hit -> colId    = digi->GetCol();
	    hit -> charge   = clusterId[moduleId];//digi->GetCharge();
	    hit -> digiId   = digiId;
	    hit -> moduleId = moduleId;      
	  
	    if (ModuleClusterMap.find(moduleId) == ModuleClusterMap.end())
	      {
		ModuleClusterMap[moduleId] = new MyHitList;
	      }
	    ModuleClusterMap[moduleId]->push_back(hit);
	  }
      }
  }
  //_____________Hit______________________________________
  Int_t nHit = fClusterHits->GetEntries();
  for (Int_t iHit = 0; iHit < nHit; iHit++)
    {
      CbmTrdHit * hit = (CbmTrdHit*) fClusterHits->At(iHit);

    }
  //------------------------Draw--------------------------
  
  Char_t title[30];
  Char_t name[100];
  Char_t picPath[200];
  
    
  std::map<Int_t, MyHitList* >::iterator it;
  //std::map<Int_t, MyDigiList* >::iterator itDigi;
  std::map<Int_t, MyHitList* >::iterator itHit;
  Int_t nEntries = Int_t(ModuleDigiMap.size());
  Int_t counter = 0;
  Int_t done = 0;
  Int_t last = 0;
  if (drawDigis)
    {
      //printf("   Drawing Digis\n");  
      for ( it = ModuleDigiMap.begin(); it != ModuleDigiMap.end(); it++)
	{
	  counter++;
	  done = Int_t((counter*10)/Float_t(nEntries));
	  if (done > last)
	    {
	      cout << "    " << done*10 << endl;
	      last = done;
	    }
	  sprintf(title,"Module_%d_1_Digi",(*it).first);
	  sprintf(name,"%d_Digis",Int_t((*it).second->size()));
	  if (picPng) {
	    sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/%s_%s_%s.png",title,name,tracer);
	  }
	  TCanvas* c = new TCanvas(title,name,cs,cs);
	  c->Divide(1,1);
	  c->cd(1);
	  fModuleInfo     = fDigiPar->GetModule((*it).first);
	  Int_t mSizeX = fModuleInfo->GetnCol();
	  Int_t mSizeY = fModuleInfo->GetnRow();
	  TH2F* Digi = new TH2F (title,name,mSizeX,0,mSizeX,mSizeY,0,mSizeY);
	  Digi->SetXTitle("Pad column");
	  Digi->SetYTitle("Pad row");
	  Digi->SetZTitle("Charge [a.u.]");
	  Digi->SetStats(kFALSE);
	  Digi->GetXaxis()->SetLabelSize(0.02);
	  Digi->GetYaxis()->SetLabelSize(0.02);
	  Digi->GetZaxis()->SetLabelSize(0.02);
	  Digi->GetXaxis()->SetTitleSize(0.02);
	  Digi->GetXaxis()->SetTitleOffset(1.5);
	  Digi->GetYaxis()->SetTitleSize(0.02);
	  Digi->GetYaxis()->SetTitleOffset(2);
	  Digi->GetZaxis()->SetTitleSize(0.02);
	  Digi->GetZaxis()->SetTitleOffset(-2);
	  if (DigiMax > 0)
	    Digi->GetZaxis()->SetRangeUser(0,DigiMax);
	  Digi->SetContour(99);
	  //c->cd(1)->SetLogz(1);
	  Int_t xPos;
	  Int_t yPos;
	  Float_t charge;
	  MyHitList::iterator listIt;
	  for ( listIt = (*it).second->begin(); listIt != (*it).second->end(); listIt++)
	    {
	      xPos = (*listIt) -> colId;
	      yPos = (*listIt) -> rowId;
	      charge = (*listIt) -> charge;
	      Digi->Fill(xPos,yPos,charge);
	    }
	  Digi->Draw("colz");
	  TImage *Outimage = TImage::Create();
	  if (picPng) {
	    Outimage->FromPad(c);
	    Outimage->WriteImage(picPath);
	  }
	  if (picPdf) {
	    sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/pdf/%s_%s_%s.pdf",title,name,tracer);
	    c->cd(1)->Print(picPath);
	  }
	  delete Digi;
	  delete c;
	  delete Outimage;
	}
    }
  if (drawClusters)
    {
      //printf("   Drawing Cluster\n");
      nEntries = Int_t(ModuleClusterMap.size());
      counter = 0;
      done = 0;
      last = 0;
      for ( it = ModuleClusterMap.begin(); it != ModuleClusterMap.end(); it++)
	{
	  counter++;
	  done = Int_t((counter*10)/Float_t(nEntries));
	  if (done > last)
	    {
	      cout << "    " <<  done*10 << endl;
	      last = done;
	    }
	  sprintf(title,"Module_%d_2_Cluster",(*it).first);
	  sprintf(name,"%d_Cluster",Int_t((*it).second->size()));
	  if (picPng) {
	    sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/%s_%s_%s.png",title,name,tracer);
	  }
	  TCanvas* c = new TCanvas(title,name,cs,cs);
	  c->Divide(1,1);
	  c->cd(1);
	  fModuleInfo     = fDigiPar->GetModule((*it).first);
	  Int_t mSizeX = fModuleInfo->GetnCol();
	  Int_t mSizeY = fModuleInfo->GetnRow();
	  TH2F* Cluster = new TH2F (title,name,mSizeX,0,mSizeX,mSizeY,0,mSizeY);
	  Cluster->SetXTitle("Pad column");
	  Cluster->SetYTitle("Pad row");
	  Cluster->SetZTitle("Cluster ID ");
	  Cluster->SetStats(kFALSE);
	  Cluster->GetXaxis()->SetLabelSize(0.02);
	  Cluster->GetYaxis()->SetLabelSize(0.02);
	  Cluster->GetZaxis()->SetLabelSize(0.02);
	  Cluster->GetXaxis()->SetTitleSize(0.02);
	  Cluster->GetXaxis()->SetTitleOffset(1.5);
	  Cluster->GetYaxis()->SetTitleSize(0.02);
	  Cluster->GetYaxis()->SetTitleOffset(2);
	  Cluster->GetZaxis()->SetTitleSize(0.02);
	  Cluster->GetZaxis()->SetTitleOffset(-2);
	  Cluster->SetContour(99);
	  Int_t xPos;
	  Int_t yPos;
	  Float_t charge;
	  MyHitList::iterator listIt;
	  for ( listIt = (*it).second->begin(); listIt != (*it).second->end(); listIt++)
	    {
	      xPos = (*listIt) -> colId;
	      yPos = (*listIt) -> rowId;
	      charge = (*listIt) -> charge;
	      Cluster->Fill(xPos,yPos,charge);
	    }
	  Cluster->DrawCopy("colz");

	  TImage *Outimage = TImage::Create();
	  if (picPng) {
	    Outimage->FromPad(c);
	    Outimage->WriteImage(picPath);
	  }
	  if (picPdf) {
	    sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/pdf/%s_%s_%s.pdf",title,name,tracer);
	    c->cd(1)->Print(picPath);
	  }
	  delete Cluster;
	  delete c;
	  delete Outimage;
	}
    }
  if (drawHits)
    {
      //printf("   Drawing Hits\n");
      nEntries = Int_t(ModuleHitMap.size());
      counter = 0;
      done = 0;
      last = 0;
      for ( it = ModuleHitMap.begin(); it != ModuleHitMap.end(); it++)
	{
	  counter++;
	  done = Int_t((counter*10)/Float_t(nEntries));
	  if (done > last)
	    {
	      cout << "    " <<  done*10 << endl;
	      last = done;
	    }
	  sprintf(title,"Module_%d_3_Hit",(*it).first);
	  sprintf(name,"%d_Hits",Int_t((*it).second->size()));
	  if (picPng) {
	    sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/%s_%s_%s.png",title,name,tracer);
	  }
	  TCanvas* c = new TCanvas(title,name,cs,cs);
	  c->Divide(1,1);
	  c->cd(1);
      
	  fModuleInfo     = fDigiPar->GetModule((*it).first);
	  Float_t mSizeX  = (fModuleInfo->GetSizex()) * 2;
	  Float_t mSizeY  = (fModuleInfo->GetSizey()) * 2;
	  Float_t mPosX   = fModuleInfo->GetX();
	  Float_t mPosY   = fModuleInfo->GetY();
      
	  TProfile* Avatar = new TProfile (title,name,Int_t(mSizeX*10),0,Int_t(mSizeX),0,Int_t(mSizeY));
	  Avatar->SetXTitle("x-Coordinate [cm]");
	  Avatar->SetYTitle("y-Coordinate [cm]");
	  Avatar->SetStats(kFALSE);

	  Avatar->GetXaxis()->SetLabelSize(0.02);
	  Avatar->GetYaxis()->SetLabelSize(0.02);
	  Avatar->GetZaxis()->SetLabelSize(0.02);
	  Avatar->GetXaxis()->SetTitleSize(0.02);
	  Avatar->GetXaxis()->SetTitleOffset(1.5);
	  Avatar->GetYaxis()->SetTitleSize(0.02);
	  Avatar->GetYaxis()->SetTitleOffset(2);
	  Avatar->GetZaxis()->SetTitleSize(0.02);
	  Avatar->GetZaxis()->SetTitleOffset(-2);
	  Avatar->GetYaxis()->SetRangeUser(0,mSizeY);
	  Avatar->SetMarkerStyle(4);
	  Avatar->Draw();
	  Float_t xPos;
	  Float_t yPos;
	  Int_t i = 0;
	  MyHitList::iterator listIt;
	  for ( listIt = (*it).second->begin(); listIt != (*it).second->end(); listIt++)
	    {
	      i++;
	      sprintf(title,"%d_Hit",i);
	      sprintf(name,"%d Hits",i);
	      TProfile* Hit = new TProfile (title,name,Int_t(mSizeX*10),0,Int_t(mSizeX),0,Int_t(mSizeY));
	      Hit->SetXTitle("x-Coordinate [cm]");
	      Hit->SetYTitle("y-Coordinate [cm]");
	      Hit->SetStats(kFALSE);
	      Hit->GetXaxis()->SetLabelSize(0.02);
	      Hit->GetYaxis()->SetLabelSize(0.02);
	      Hit->GetZaxis()->SetLabelSize(0.02);
	      Hit->GetXaxis()->SetTitleSize(0.02);
	      Hit->GetXaxis()->SetTitleOffset(1.5);
	      Hit->GetYaxis()->SetTitleSize(0.02);
	      Hit->GetYaxis()->SetTitleOffset(2);
	      Hit->GetZaxis()->SetTitleSize(0.02);
	      Hit->GetZaxis()->SetTitleOffset(-2);
	      xPos = ((*listIt) -> xPos)/10. - mPosX + 0.5 * mSizeX;
	      yPos = ((*listIt) -> yPos)/10. - mPosY + 0.5 * mSizeY;
	      /*
		printf("  %d  %d  %d  |  %d   %d  |  %d  |  %d  %d\n  (%6.2f,%6.2f) mSize\n  (%6.2f,%6.2f) mPos\n  (%6.2f,%6.2f) Hit\n\n",
		(*it).first,(*listIt)->moduleId, (*listIt)->digiId,
		counter,Int_t((*it).second->size()),
		i,
		(*listIt)->rowId,(*listIt)->colId,
		mSizeX,mSizeY,
		mPosX,mPosY,
		xPos,yPos);
	      */
	      Hit->Fill(xPos,yPos);
	      Hit->GetYaxis()->SetRangeUser(0,mSizeY);
	      Hit->SetMarkerStyle(4);
	      Hit->DrawCopy("P,same");
	      delete Hit;
	    }
     
	  TImage *Outimage = TImage::Create();
	  if (picPng) {
	    Outimage->FromPad(c);
	    Outimage->WriteImage(picPath);
	  }
	  sprintf(title,"Module_%d_3_Hit",(*it).first);
	  sprintf(name,"%d_Hits",Int_t((*it).second->size()));
	  if (picPdf) {
	    sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/pdf/%s_%s_%s.pdf",title,name,tracer);
	    c->cd(1)->Print(picPath);
	  }
	  delete Avatar;
	  delete c;
	  delete Outimage;
	}
    }
  if (drawMCPoints)
    {
      //printf("   Drawing MC-Points\n");
      nEntries = Int_t(ModuleMCPointMap.size());
      counter = 0;
      done = 0;
      last = 0;
      for ( it = ModuleMCPointMap.begin(); it != ModuleMCPointMap.end(); it++)
	{
	  counter++;
	  done = Int_t((counter*10)/Float_t(nEntries));
	  if (done > last)
	    {
	      cout <<  "    " << done*10 << endl;
	      last = done;
	    }
	  sprintf(title,"Module_%d_0_Point",(*it).first);
	  if (mean)
	    {
	      sprintf(name,"%d_Points_mean",Int_t((*it).second->size()));
	    }
	  else
	    {
	      sprintf(name,"%d_Points_in_out",Int_t((*it).second->size()));
	    }
	  if (picPng) {
	    sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/%s_%s_%s.png",title,name,tracer);
	  }
	  TCanvas* c = new TCanvas(title,name,cs,cs);
	  c->Divide(1,1);
	  c->cd(1);
      
	  fModuleInfo     = fDigiPar->GetModule((*it).first);
	  Float_t mSizeX  = (fModuleInfo->GetSizex()) * 2;
	  Float_t mSizeY  = (fModuleInfo->GetSizey()) * 2;
	  Float_t mPosX   = fModuleInfo->GetX();
	  Float_t mPosY   = fModuleInfo->GetY();
	  TProfile* Avatar = new TProfile (title,name,Int_t(mSizeX*10),0,Int_t(mSizeX),0,Int_t(mSizeY));
	  Avatar->SetXTitle("x-Coordinate [cm]");
	  Avatar->SetYTitle("y-Coordinate [cm]");
	  Avatar->SetStats(kFALSE);
	  Avatar->GetXaxis()->SetLabelSize(0.02);
	  Avatar->GetYaxis()->SetLabelSize(0.02);
	  Avatar->GetZaxis()->SetLabelSize(0.02);
	  Avatar->GetXaxis()->SetTitleSize(0.02);
	  Avatar->GetXaxis()->SetTitleOffset(1.5);
	  Avatar->GetYaxis()->SetTitleSize(0.02);
	  Avatar->GetYaxis()->SetTitleOffset(2);
	  Avatar->GetZaxis()->SetTitleSize(0.02);
	  Avatar->GetZaxis()->SetTitleOffset(-2);
	  Avatar->GetYaxis()->SetRangeUser(0,mSizeY);
	  Avatar->SetMarkerStyle(4);
	  Avatar->Draw();
	  Float_t xPos;
	  Float_t yPos;
	  Int_t i = 0;

	  //std::vector tempMCpoints;
	  //tempMCpoints.size();

	  MyHitList::iterator listIt;
	  for ( listIt = (*it).second->begin(); listIt != (*it).second->end(); listIt++)
	    {
	      i++;
	      sprintf(title,"%d Point",i);
	      sprintf(name,"%d_Points",i);
	      TProfile* Hit = new TProfile (title,name,Int_t(mSizeX*10),0,Int_t(mSizeX),0,Int_t(mSizeY));
	      Hit->SetXTitle("x-Coordinate [cm]");
	      Hit->SetYTitle("y-Coordinate [cm]");
	      Hit->SetStats(kFALSE);
	      Hit->GetXaxis()->SetLabelSize(0.02);
	      Hit->GetYaxis()->SetLabelSize(0.02);
	      Hit->GetZaxis()->SetLabelSize(0.02);
	      Hit->GetXaxis()->SetTitleSize(0.02);
	      Hit->GetXaxis()->SetTitleOffset(1.5);
	      Hit->GetYaxis()->SetTitleSize(0.02);
	      Hit->GetYaxis()->SetTitleOffset(2);
	      Hit->GetZaxis()->SetTitleSize(0.02);
	      Hit->GetZaxis()->SetTitleOffset(-2);
	      xPos = ((*listIt) -> xPos)/10. - mPosX + 0.5 * mSizeX;
	      yPos = ((*listIt) -> yPos)/10. - mPosY + 0.5 * mSizeY;
	      //printf("  (%f,%f) \n  (%f,%f) \n  (%f,%f) \n\n",mSizeX,mSizeY,mPosX,mPosY,xPos,yPos);
	      Hit->Fill(xPos,yPos);	   
	      Hit->GetYaxis()->SetRangeUser(0,mSizeY);
	      Hit->SetMarkerStyle(2);
	      Hit->DrawCopy("P,same");
	      if (!mean)
		{
		  Float_t xPos2 = ((*listIt) -> dxPos)/10. - mPosX + 0.5 * mSizeX;
		  Float_t yPos2 = ((*listIt) -> dyPos)/10. - mPosY + 0.5 * mSizeY;
		  //printf("  (%f,%f) \n  (%f,%f) \n  (%f,%f) \n\n",mSizeX,mSizeY,mPosX,mPosY,xPos,yPos);
		  Hit->Fill(xPos2,yPos2);	 
		  Hit->GetYaxis()->SetRangeUser(0,mSizeY);
		  Hit->SetMarkerStyle(5);
		  Hit->DrawCopy("P,same");
	      
		  TLine* Track = new TLine(xPos, yPos, xPos2, yPos2);
		  Track -> Draw("same");
		  //delete Track;
		}
	      delete Hit;
	      //delete Track;
	    }
     
	  TImage *Outimage = TImage::Create();
	  if (picPng) {
	    Outimage->FromPad(c);
	    Outimage->WriteImage(picPath);
	  }
	  sprintf(title,"Module_%d_0_Point",(*it).first);
	  if (mean)
	    {
	      sprintf(name,"%d_Points_mean",Int_t((*it).second->size()));
	    }
	  else
	    {
	      sprintf(name,"%d_Points_in_out",Int_t((*it).second->size()));
	    }
	  if (picPdf) {
	    sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/pdf/%s_%s_%s.pdf",title,name,tracer);
	    c->cd(1)->Print(picPath);
	  }
	  delete Avatar;
	  delete c;
	  delete Outimage;
	}
    }
}
// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::CalcPR(Bool_t combinatoric, Int_t qMaxDigiIndex, TH1F*& shortPR, TH1F* longPR[20], TLegend*& legend, TH2F*& PRF, MyHit* hit, TProfile*& fDeltaRR, TProfile*& fMCPHR, TProfile*& fDeltaRMCPH, Int_t MCs)
{
  //Bool_t combinatoric = true; //true := every paricipating mc-point is used as potential base of the reconstructed hit || false := the mc-point next to the reco hit is used
  CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(qMaxDigiIndex);
  Int_t size = digi->GetMCIndex().size();
  std::vector<int> MCIndex = digi->GetMCIndex();

  Int_t moduleId = digi->GetDetId();
  Int_t Station = fGeoHandler->GetStation(moduleId);
  Int_t Layer   = fGeoHandler->GetLayer(moduleId);
  fModuleInfo = fDigiPar->GetModule(moduleId);
  Int_t NoSectors = fModuleInfo->GetNoSectors();
  Float_t pSizeLong = 0;
  for (Int_t iSec = 0; iSec < NoSectors; iSec++) {
    if (Layer%2 == 0) {
      pSizeLong += (fModuleInfo->GetPadSizex(iSec)) * 10;
    }
    else {
      pSizeLong += (fModuleInfo->GetPadSizey(iSec)) * 10;
    }
  }
  pSizeLong /= NoSectors;
  //std::map<Int_t,Int_t> fPadSizeLongMap;
  std::map<Int_t,Int_t>::iterator it;
  it = fPadSizeLongMap.find(pSizeLong);
  Char_t title[50];
  if (it == fPadSizeLongMap.end()) {
    if (fPadSizeLongMap.size() == 0) {
      sprintf(title,"PR sum over all pad length");
      //legend->AddEntry(longPR[0],title,"l");
    }
    fPadSizeLongMap[Int_t(pSizeLong)] = fPadSizeLongMap.size();
    //cout << "   longPR_" << fPadSizeLongMap[Int_t(pSizeLong)] << " corresponds to " << pSizeLong << " mm pad height" << endl;
    sprintf(title,"PR for %4d mm long pads",Int_t(pSizeLong));
    legend->AddEntry(longPR[fPadSizeLongMap.size()],title,"l");
    //cout << mSizeLong << " " << fPadSizeLongMap[mSizeLong] << endl;
  }

  Double_t xPosHit = hit->xPos;
  Double_t yPosHit = hit->yPos;
  Double_t xPosMCP;
  Double_t yPosMCP;

  Double_t x_in;
  Double_t x_out;
  Double_t y_in;
  Double_t y_out;
  Double_t z_in;
  Double_t z_out;
  Double_t x_mean;
  Double_t y_mean;
  Double_t z_mean;  
  Double_t x_track, y_track, z_track;
  Double_t r_track;

  Double_t xPrMin = 100000;
  Double_t yPrMin = 100000;

  Double_t yDelta;
  Double_t xDelta;

  Float_t r, x, y;
  Float_t dr, dx, dy;
  if (!combinatoric)
    size = 1;
  for (Int_t i = 0; i < size; i++) {
    CbmTrdPoint *pt = (CbmTrdPoint*) fTrdPoints->At(MCIndex[i]);
    moduleId = pt->GetDetectorID();
    x_in   = pt->GetXIn()  * 10;
    x_out  = pt->GetXOut() * 10;
    y_in   = pt->GetYIn()  * 10;
    y_out  = pt->GetYOut() * 10;
    z_in   = pt->GetZIn()  * 10;
    z_out  = pt->GetZOut() * 10;
    x_mean = (x_in + x_out)/2.;
    y_mean = (y_in + y_out)/2.;
    z_mean = (z_in + z_out)/2.;
    x_track = fabs(x_in - x_out);
    y_track = fabs(y_in - y_out);
    r_track = sqrt(x_track * x_track + y_track * y_track);
    xPosMCP = x_mean;
    yPosMCP = y_mean;

    xDelta = xPosMCP-xPosHit;
    yDelta = yPosMCP-yPosHit;

    if (sqrt(pow(xDelta,2) + pow(yDelta,2)) < sqrt(pow(xPrMin,2) + pow(yPrMin,2))) {
      xPrMin = xDelta;
      yPrMin = yDelta;
      r = sqrt(x_mean * x_mean + y_mean * y_mean);
      dr = r - sqrt(xPosHit * xPosHit + yPosHit * yPosHit);
      dx = (x_mean - xPosHit);
      dy = (y_mean - yPosHit);
    }
   
    if (combinatoric) {
      if (Layer%2 == 0) {
	shortPR->Fill(yDelta);
	longPR[0]->Fill(xDelta);
	longPR[fPadSizeLongMap[Int_t(pSizeLong)]] -> Fill(xDelta);
      }
      else {
	shortPR->Fill(xDelta);
	longPR[0]->Fill(yDelta);
	longPR[fPadSizeLongMap[Int_t(pSizeLong)]] -> Fill(yDelta);
      }
    }

  }
  //fMCPHR->Fill(r,MCs);
  fMCPHR->Fill(r,r_track);
  fDeltaRMCPH->Fill(MCs,dr);
  if (Layer%2 == 0) {
    //fDeltaRR->Fill(yPrMin,y_track);
    //fDeltaRR->Fill(r,y_track);
    fDeltaRR->Fill(y_track,dy);
    //fDeltaRR->Fill(r,xPrMin);
  }
  else {
    //fDeltaRR->Fill(xPrMin,x_track);
    //fDeltaRR->Fill(r,x_track);
    fDeltaRR->Fill(x_track,dx);
    //fDeltaRR->Fill(r,yPrMin);
  }

  if (!combinatoric) {
    if (Layer%2 == 0) {
      shortPR->Fill(yPrMin);
      longPR[0] ->Fill(xPrMin);
      longPR[fPadSizeLongMap[Int_t(pSizeLong)]] -> Fill(xPrMin);
    }
    else {
      shortPR->Fill(xPrMin);
      longPR[0] ->Fill(yPrMin);
      longPR[fPadSizeLongMap[Int_t(pSizeLong)]] -> Fill(yPrMin);
    }
  }

}
  // --------------------------------------------------------------------
  void CbmTrdHitProducerCluster::AddHit(Int_t iHit, Int_t detectorId, TVector3& pos, TVector3& dpos, Double_t dxy, Int_t planeId, Double_t eLossTR, Double_t eLossdEdx, Double_t eLoss)
  {
    if (eLoss == 0.0)
      printf("%i\n",iHit);
    /** HitParameter
     *@param detectorId Unique detector ID
     *@param pos Position in global c.s. [cm]
     *@param dpos Errors of position in global c.s. [cm]
     *@param dxy XY correlation of the hit
     *@param refId Index of digi or cluster
     *@param planeId Detector plane identifier
     *@param eLossTR Energy losses of electron via TR
     *@param eLossdEdx Energy deposition in the detector without TR
     *@param eLoss TR + dEdx
     **/  
    //printf ("det: %i   plane: %i\n",detectorId,planeId);
    TClonesArray& clref = *fClusterHits;
    Int_t size = clref.GetEntriesFast();

    //eLoss has to be calibrated due to losses corensponding to the limitation to 3 pad cluster
    eLoss *= 0.5 * (
		    3.71211 / 3.07664 + // Pion MPV MC / Pion MPV Hit (100 events 25 AGeV)
		    5.14198 / 4.06185   //Electron MPV MC / Electron MPV Hit (100 events 25 AGeV)
		    );

    new ((*fClusterHits)[size]) CbmTrdHit( detectorId,
					   pos,
					   dpos,
					   dxy,
					   fClusterId,//iHit,
					   planeId,
					   eLossTR,
					   eLossdEdx,
					   eLoss);
  
  }
  // ---- Register ------------------------------------------------------
  void CbmTrdHitProducerCluster::Register()
  {
    cout << " * CbmTrdHitProducerCluster * :: Register()" << endl;
    //FairRootManager::Instance()->Register("TrdHit","Trd Hit", fClusterHits, kTRUE);
  }
  // --------------------------------------------------------------------


  // ---- Finish --------------------------------------------------------
  void CbmTrdHitProducerCluster::Finish()
  {
    //cout << " * CbmTrdHitProducerCluster * :: Finish()" << endl;
  }

// -----   Public method EndOfEvent   --------------------------------------
void CbmTrdHitProducerCluster::FinishEvent() {
  //  cout<<"In CbmTrdHitProducerCluster::FinishEvent()"<<endl;
  if (fClusterHits) fClusterHits->Clear();
  fSectorSizeX.clear();
  fSectorSizeY.clear();
  fPadSizeX.clear();
  fPadSizeY.clear();
  fSecCol.clear();
  fSecRow.clear();
  fPadSizeXArray.clear();
  fPadSizeYArray.clear();
  fPadSizeLongMap.clear();
}
// -------------------------------------------------------------------------

  ClassImp(CbmTrdHitProducerCluster)
