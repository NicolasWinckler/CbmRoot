#include "CbmTrdHitProducerCluster.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TArray.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TImage.h"

//#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdHitProducerCluster::CbmTrdHitProducerCluster()
  :FairTask("CbmTrdHitProducerCluster",1),
   fDigis(NULL),
   fClusters(NULL),
   fClusterHits(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fTrdId()
{

}

// ---- Destructor ----------------------------------------------------
CbmTrdHitProducerCluster::~CbmTrdHitProducerCluster()
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
  
  fDigis =(TClonesArray *)  ioman->ActivateBranch("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigi array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }
  fClusters =(TClonesArray *)  ioman->ActivateBranch("TrdCluster");
  if ( ! fClusters ) {
    cout << "-W CbmTrdHitProducerCluster::Init: No TrdCluster array!" << endl;
    cout << "                               Task will be inactive" << endl;
    return kERROR;
  }    
  fClusterHits = new TClonesArray("CbmTrdHit", 100);
  ioman->Register("TrdHit","TRD Hit",fClusterHits,kTRUE);
  
  return kSUCCESS;
  
}

// ---- Exec ----------------------------------------------------------
void CbmTrdHitProducerCluster::Exec(Option_t * option)
{
  cout << "================CbmTrdHitProducerCluster==============" << endl;
  
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

  Int_t nCluster = fClusters->GetEntries(); // Number of clusters found by CbmTrdClusterFinderFast
  cout << " Found " << nCluster << " Cluster in Collection" << endl;

  for (Int_t iCluster = 0; iCluster < nCluster; iCluster++)
    {
      CbmTrdCluster *cluster = (CbmTrdCluster*) fClusters->At(iCluster);//pointer to the acvit cluster
      //cout << "NoDigis:" << cluster->GetNDigis() << endl;
      nDigi = cluster->GetNDigis();
      qMax = 0;
      for (Int_t iDigi = 0; iDigi < nDigi; iDigi++)
	{
	  DigiIndex = cluster->GetDigiIndex(iDigi);
	  CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(DigiIndex);
	  DigiCharge = digi->GetCharge();
	  //-----------unrotated--------------------
	  DigiCol    = digi->GetCol();
	  DigiRow    = digi->GetRow();
	  //-----------unrotated--------------------
	  //cout << "   ID:" << DigiIndex << "  Col:" << DigiCol << "  Row:" << DigiRow << "  Charge:" << DigiCharge << endl;
	  if (DigiCharge > qMax)
	    {
	      qMax = DigiCharge;
	      qMaxIndex = DigiIndex;
	    }
	}
      GetModuleInfo(qMaxIndex/*, ModuleHitMap*/);
      /*
	PrfReco(qMaxIndex, qMax);
	SimpleReco(qMaxIndex, qMax);
	AddHit(iHit);
      */
     
      //cout << "      MaxID: " << qMaxIndex << "   MaxCharge: " << qMax << endl;
    }  
  //DrawHits();
  std::map<Int_t, MyHitList* >::iterator it;
  for ( it = ModuleHitMap.begin(); it != ModuleHitMap.end(); it++)
    {
      iHit += Int_t((*it).second->size());
    }
  cout << " Found " << iHit << " Hits" << endl;
}

// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::GetModuleInfo(Int_t qMaxIndex/*, MHitMap* ModuleHitMap*/)
{
  ModulePara* mPara = new ModulePara;
  MyHit* hit = new MyHit;
 
  CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(qMaxIndex);  
  Int_t moduleId = digi->GetDetId();
  fModuleInfo = fDigiPar->GetModule(moduleId);
  Int_t* detInfo = fTrdId.GetDetectorInfo(moduleId); 
  
  mPara -> Station = detInfo[1];
  mPara -> Layer = detInfo[2];
  mPara -> moduleId = moduleId;
  //-----------rotated----------------------

  hit -> rowId = digi->GetRow();
  hit -> colId = digi->GetCol();
  hit -> digiId = qMaxIndex;
  hit -> moduleId = moduleId;

  mPara -> xPos = 10 * fModuleInfo->GetX();
  mPara -> yPos = 10 * fModuleInfo->GetY();
  mPara -> zPos = 10 * fModuleInfo->GetZ();
  mPara -> nCol = fModuleInfo->GetnCol();
  mPara -> nRow = fModuleInfo->GetnRow();
  mPara -> NoSectors = fModuleInfo->GetNoSectors();

  const Int_t NoSectors = fModuleInfo->GetNoSectors();
  mPara -> SectorSizeX.resize(NoSectors);
  mPara -> SectorSizeY.resize(NoSectors);
  mPara -> PadSizeX.resize(NoSectors);
  mPara -> PadSizeY.resize(NoSectors);
  mPara -> SecRow.resize(NoSectors);
  mPara -> SecCol.resize(NoSectors);      

  for (Int_t i = 0; i < NoSectors; i++)
    {
      mPara -> SectorSizeX[i] = 10 * fModuleInfo->GetSectorSizex(i);
      mPara -> SectorSizeY[i] = 10 * fModuleInfo->GetSectorSizey(i);
      mPara -> PadSizeX[i]    = 10 * fModuleInfo->GetPadSizex(i);
      mPara -> PadSizeY[i]    = 10 * fModuleInfo->GetPadSizey(i);
      mPara -> SecRow[i]      = Int_t(mPara->SectorSizeY[i] / mPara->PadSizeY[i]);
      mPara -> SecCol[i]      = Int_t(mPara->SectorSizeX[i] / mPara->PadSizeX[i]);
     
      //printf("M(%.1f,%.1f) SS(%.1f,%.1f) N(%d,%d) S(%d,%d) PS(%.1f,%.1f)\n",ModuleSizeX,ModuleSizeY,SectorSizeX[i],SectorSizeY[i],nCol,nRow,SecCol[i],SecRow[i],PadSizeX[i],PadSizeY[i]);
    }

  hit -> xPos = (mPara->xPos - 10 * fModuleInfo-> GetSizex());
  hit -> yPos = (mPara->yPos - 10 * fModuleInfo-> GetSizey());
  hit -> zPos = mPara->zPos;

  Float_t qMax = 0;
  SimpleReco(qMaxIndex, qMax, mPara, hit/*, ModuleHitMap*/);
  PrfReco(qMaxIndex, qMax);
  delete mPara;
}
// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::SortClusterDigi()
{

}
// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::PrfReco(Int_t qMaxIndex, Float_t qMax)
{

}
// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::SimpleReco(Int_t qMaxIndex, Float_t qMax, ModulePara* mPara, MyHit* hit/*, MHitMap* ModuleHitMap*/)
{
  Int_t DigiCol = hit-> colId;
  Int_t iSec = 0;
  if (DigiCol == 0)
    {
      hit->xPos += 0.5 * mPara->PadSizeX[iSec];
      hit->dxPos = 0.5 * mPara->PadSizeX[iSec];
    }

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

  iSec = 0;
  Int_t DigiRow = hit-> rowId;
  if (DigiRow == 0)
    {
      hit->yPos += 0.5 * mPara->PadSizeY[iSec];
      hit->dyPos = 0.5 * mPara->PadSizeY[iSec];
    }
 
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
  

  
  //cout << "layer " << Layer << "  col " << nCol << "  row " << nRow << endl;
  //cout << "      " << Layer << "  col " << DigiCol << "  row " << DigiRow << endl;
  //cout << "         (" << x << "," << y << "," << z << ")" << endl;
  
  TVector3  pos(hit->xPos/10.,   hit->yPos/10.,  hit->zPos/10.);
  TVector3 dpos(hit->dxPos/10., hit->dyPos/10., hit->dzPos/10.);
  Double_t dxy = 0;
  Int_t planeId = 0;
  Double_t eLossTR = 0;
  Double_t eLossdEdx = 0;
  Double_t eLoss = 0;
  AddHit( qMaxIndex, mPara->moduleId, pos, dpos, dxy, planeId, eLossTR, eLossdEdx, eLoss);
  
  // TODO: Fill MyHitList and ModuleHItMap
  std::map<Int_t, MyHitList* >::iterator it = ModuleHitMap.find(mPara->moduleId);
  if (it == ModuleHitMap.end())
    {
      ModuleHitMap[mPara->moduleId] = new MyHitList;
    }
  ModuleHitMap[mPara->moduleId]->push_back(hit);
}
// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::DrawHits()
{  
  Int_t moduleId;

  //_____________Digi_____________________________________
  std::map<Int_t, MyHitList*> ModuleDigiMap;
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
  //_____________Cluster__________________________________
  std::map<Int_t, Int_t> clusterId;
  std::map<Int_t, MyHitList*> ModuleClusterMap;
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
  printf("   Drawing Digis\n");    
  std::map<Int_t, MyHitList* >::iterator it;
  std::map<Int_t, MyHitList* >::iterator itHit;
  Int_t nEntries = Int_t(ModuleDigiMap.size());
  Int_t counter = 0;
  Int_t done = 0;
  Int_t last = 0;
  for ( it = ModuleDigiMap.begin(); it != ModuleDigiMap.end(); it++)
    {
      counter++;
      done = Int_t((counter*10)/Float_t(nEntries));
      if (done > last)
	{
	  cout << done*10 << endl;
	  last = done;
	}
      sprintf(title,"Module_%d_1_Digi",(*it).first);
      sprintf(name,"%d Digis",Int_t((*it).second->size()));
      sprintf(picPath,"Pics/%s_%s.png",title,name);
      TCanvas* c = new TCanvas(title,name,900,900);
      c->Divide(1,1);
      c->cd(1);
      fModuleInfo     = fDigiPar->GetModule((*it).first);
      Int_t mSizeX = fModuleInfo->GetnCol();
      Int_t mSizeY = fModuleInfo->GetnRow();
      TH2F* Digi = new TH2F (title,name,mSizeX,0,mSizeX,mSizeY,0,mSizeY);
      Digi->SetContour(99);
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
      Outimage->FromPad(c);
      Outimage->WriteImage(picPath);
      sprintf(picPath,"Pics/%s_%s.eps",title,name);
      c->cd(1)->Print(picPath);
      delete Digi;
      delete c;
    }
  
  printf("   Drawing Cluster\n");
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
	  cout << done*10 << endl;
	  last = done;
	}
      sprintf(title,"Module_%d_2_Cluster",(*it).first);
      sprintf(name,"%d Cluster",Int_t((*it).second->size()));
      sprintf(picPath,"Pics/%s_%s.png",title,name);
      TCanvas* c = new TCanvas(title,name,900,900);
      c->Divide(1,1);
      c->cd(1);
      fModuleInfo     = fDigiPar->GetModule((*it).first);
      Int_t mSizeX = fModuleInfo->GetnCol();
      Int_t mSizeY = fModuleInfo->GetnRow();
      TH2F* Cluster = new TH2F (title,name,mSizeX,0,mSizeX,mSizeY,0,mSizeY);
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
      Outimage->FromPad(c);
      Outimage->WriteImage(picPath);
      sprintf(picPath,"Pics/%s_%s.eps",title,name);
      c->cd(1)->Print(picPath);
      delete Cluster;
      delete c;
    }
  
  printf("   Drawing Hits\n");
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
	  cout << done*10 << endl;
	  last = done;
	}
      sprintf(title,"Module_%d_3_Hit",(*it).first);
      sprintf(name,"%d Hits",Int_t((*it).second->size()));
      sprintf(picPath,"Pics/%s_%s.png",title,name);
      
      TCanvas* c = new TCanvas(title,name,900,900);
      c->Divide(1,1);
      c->cd(1);
      
      fModuleInfo     = fDigiPar->GetModule((*it).first);
      Float_t mSizeX  = (fModuleInfo->GetSizex()) * 2;
      Float_t mSizeY  = (fModuleInfo->GetSizey()) * 2;
      Float_t mPosX   = fModuleInfo->GetX();
      Float_t mPosY   = fModuleInfo->GetY();
      TProfile* Avatar = new TProfile (title,name,mSizeX*10,0,mSizeX,0,mSizeY);
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
	  sprintf(title,"%d Hit",i);
	  sprintf(name,"%d Hits",i);
	  TProfile* Hit = new TProfile (title,name,mSizeX*10,0,mSizeX,0,mSizeY);
	  xPos = ((*listIt) -> xPos)/10. - mPosX + 0.5 * mSizeX;
	  yPos = ((*listIt) -> yPos)/10. - mPosY + 0.5 * mSizeY;
	  //printf("  (%f,%f) \n  (%f,%f) \n\n",mSizeX,mSizeY,xPos,yPos);
	  Hit->Fill(xPos,yPos);
	  Hit->GetYaxis()->SetRangeUser(0,mSizeY);
	  Hit->SetMarkerStyle(4);
	  Hit->DrawCopy("P,same");
	  delete Hit;
	}
     
      TImage *Outimage = TImage::Create();
      Outimage->FromPad(c);
      Outimage->WriteImage(picPath);
      sprintf(title,"Module_%d_3_Hit",(*it).first);
      sprintf(name,"%d Hits",Int_t((*it).second->size()));
      sprintf(picPath,"Pics/%s_%s.eps",title,name);
      c->cd(1)->Print(picPath);
      delete Avatar;
      delete c;
    }
  
}
// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::AddHit(Int_t iHit, Int_t detectorId, TVector3& pos, TVector3& dpos, Double_t dxy, Int_t planeId, Double_t eLossTR, Double_t eLossdEdx, Double_t eLoss)
{
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

  TClonesArray& clref = *fClusterHits;
  Int_t size = clref.GetEntriesFast();

  new ((*fClusterHits)[size]) CbmTrdHit( detectorId,
					 pos,
					 dpos,
					 dxy,
					 iHit,
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

ClassImp(CbmTrdHitProducerCluster)
