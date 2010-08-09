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

  MHitMap* ModuleHitMap = new MHitMap;
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
      GetModuleInfo(qMaxIndex, ModuleHitMap);
      /*
      PrfReco(qMaxIndex, qMax);
      SimpleReco(qMaxIndex, qMax);
      AddHit(iHit);
      */
      iHit++;
      //cout << "      MaxID: " << qMaxIndex << "   MaxCharge: " << qMax << endl;
    }  
  cout << " Found " << iHit << " Hits" << endl;
}

// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::GetModuleInfo(Int_t qMaxIndex, MHitMap* ModuleHitMap)
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
  SimpleReco(qMaxIndex, qMax, mPara, hit, ModuleHitMap);
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
void CbmTrdHitProducerCluster::SimpleReco(Int_t qMaxIndex, Float_t qMax, ModulePara* mPara, MyHit* hit, MHitMap* ModuleHitMap)
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
  /*
  // TODO: Fill MyHitList and ModuleHItMap
  MHitMap::iterator it = ModuleHitMap->find(mPara->moduleId);
  if (it == ModuleHitMap.end())
    {
      MyHitList hitList = new MyHitList;
      ModuleHitMap->insert(mPara->moduleId,hitList);
    }
 
  ModuleHitMap->push_back(hit);
  */
}

void CbmTrdHitProducerCluster::DrawHits()
{

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
