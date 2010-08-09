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
      GetModuleInfo(qMaxIndex);
      PrfReco(qMaxIndex, qMax);
      SimpleReco(qMaxIndex, qMax);
      //AddHit(iHit);
      iHit++;
      //cout << "      MaxID: " << qMaxIndex << "   MaxCharge: " << qMax << endl;
    }  
  cout << " Found " << iHit << " Hits" << endl;
}

// --------------------------------------------------------------------
void CbmTrdHitProducerCluster::GetModuleInfo(Int_t qMaxIndex)
{
  Float_t x = 0;
  Float_t y = 0;
  Float_t z = 0;
  Float_t dx = 0;
  Float_t dy = 0;
  Float_t dz = 0;
  CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(qMaxIndex);  
  Int_t moduleId = digi->GetDetId();
  fModuleInfo = fDigiPar->GetModule(moduleId);
  Int_t* detInfo = fTrdId.GetDetectorInfo(moduleId); 
  Int_t Station  = detInfo[1];
  Int_t Layer    = detInfo[2];
  //-----------unrotated--------------------
  Int_t DigiCol = digi->GetCol();
  Int_t DigiRow = digi->GetRow();
  /*
    if  (Layer%2 == 0) {
    DigiCol = digi->GetRow();
    DigiRow = digi->GetCol();
    }
  */
  //-----------unrotated--------------------
 
  //-------------rotated--------------------
  Int_t nCol;
  Int_t nRow;
  //-------------rotated--------------------

  const Int_t NoSectors = fModuleInfo->GetNoSectors();

  Float_t SectorSizeX[NoSectors];
  Float_t SectorSizeY[NoSectors];
  Int_t SecRow[NoSectors];
  Int_t SecCol[NoSectors];
  Float_t PadSizeX[NoSectors]; 
  Float_t PadSizeY[NoSectors];
  Float_t ModuleSizeX;
  Float_t ModuleSizeY;

  Float_t ModulePositionX = 10 * fModuleInfo->GetX();
  Float_t ModulePositionY = 10 * fModuleInfo->GetY();
  Float_t ModulePositionZ = 10 * fModuleInfo->GetZ();
  /*
    if (Layer%2 == 0) {
    nCol = fModuleInfo->GetnRow();
    nRow = fModuleInfo->GetnCol();
    ModuleSizeX     = 10 * fModuleInfo-> GetSizey();
    ModuleSizeY     = 10 * fModuleInfo-> GetSizex();
    for (Int_t i = 0; i < NoSectors; i++)
    {
    SectorSizeX[i] = 10 * fModuleInfo->GetPadSizey(i);
    SectorSizeY[i] = 10 * fModuleInfo->GetPadSizex(i);
    PadSizeX[i]    = 10 * fModuleInfo->GetPadSizey(i);
    PadSizeY[i]    = 10 * fModuleInfo->GetPadSizex(i);
    }
    }
    else {
  */
  nCol = fModuleInfo->GetnCol();
  nRow = fModuleInfo->GetnRow();
  ModuleSizeX     = 10 * fModuleInfo-> GetSizex();
  ModuleSizeY     = 10 * fModuleInfo-> GetSizey();
  for (Int_t i = 0; i < NoSectors; i++)
    {
      SectorSizeX[i] = 10 * fModuleInfo->GetSectorSizex(i);
      SectorSizeY[i] = 10 * fModuleInfo->GetSectorSizey(i);
      PadSizeX[i]    = 10 * fModuleInfo->GetPadSizex(i);
      PadSizeY[i]    = 10 * fModuleInfo->GetPadSizey(i);
      SecRow[i] = Int_t(SectorSizeY[i] / PadSizeY[i]);
      SecCol[i] = Int_t(SectorSizeX[i] / PadSizeX[i]);
      //printf("M(%.1f,%.1f) SS(%.1f,%.1f) N(%d,%d) S(%d,%d) PS(%.1f,%.1f)\n",ModuleSizeX,ModuleSizeY,SectorSizeX[i],SectorSizeY[i],nCol,nRow,SecCol[i],SecRow[i],PadSizeX[i],PadSizeY[i]);
    }
  /*
    }
  */
  
  x = (ModulePositionX - /*0.5 **/ ModuleSizeX);
  y = (ModulePositionY - /*0.5 **/ ModuleSizeY);
  z = ModulePositionZ;
  //cout << "layer " << Layer << "  col " << nCol << "  row " << nRow << endl;
  //cout << "      " << Layer << "  col " << DigiCol << "  row " << DigiRow << endl;
  //for (Int_t iSec = 0; iSec < NoSectors; iSec++)

 Int_t iSec = 0;
  if (DigiCol == 0)
    {
      x += 0.5 * PadSizeX[iSec];
      dx = 0.5 * PadSizeX[iSec];
    }
  if (DigiRow == 0)
    {
      y += 0.5 * PadSizeY[iSec];
      dy = 0.5 * PadSizeY[iSec];
    }
 
  while (DigiCol > 0)
    {
      if (SecCol[iSec] < nCol)
	{
	  if (DigiCol > SecCol[iSec])
	    {
	      x += SectorSizeX[iSec];
	      DigiCol -= SecCol[iSec];
	      iSec++;
	    }
	  if (DigiCol <= SecCol[iSec] && DigiCol > 0)
	    {
	      x += DigiCol * PadSizeX[iSec] + 0.5 * PadSizeX[iSec];
	      DigiCol -= DigiCol;
	      dx = 0.5 * PadSizeX[iSec];
	    }
	}      
      if (iSec == 0 && SecCol[iSec] == nCol && DigiCol > 0)
	{
	  x += DigiCol * PadSizeX[iSec] + 0.5 * PadSizeX[iSec];
	  DigiCol -= DigiCol;
	  dx = 0.5 * PadSizeX[iSec];
	}
      //cout << "  " << iSec << "   " << Layer << "  col " << DigiCol << "  row " << DigiRow << endl;  
    }
  iSec = 0;
  while (DigiRow > 0)
    {
      if (SecRow[iSec] < nRow)
	{
	  if (DigiRow > SecRow[iSec])
	    {
	      y += SectorSizeY[iSec];
	      DigiRow -= SecRow[iSec];
	      iSec++;
	    }
	  if (DigiRow <= SecRow[iSec] && DigiRow > 0)
	    {
	      y += DigiRow * PadSizeY[iSec] + 0.5 * PadSizeY[iSec];
	      DigiRow -= DigiRow;
	      dy = 0.5 * PadSizeY[iSec];
	    }
	}      
      if (iSec == 0 && SecRow[iSec] == nRow && DigiRow > 0)
	{
	  y += DigiRow * PadSizeY[iSec] + 0.5 * PadSizeY[iSec];
	  DigiRow -= DigiRow;
	  dy = 0.5 * PadSizeY[iSec];
	}    
      //cout << "  " << iSec << "   " << Layer << "  col " << DigiCol << "  row " << DigiRow << endl;  
    }
  

  
  //cout << "layer " << Layer << "  col " << nCol << "  row " << nRow << endl;
  //cout << "      " << Layer << "  col " << DigiCol << "  row " << DigiRow << endl;
  //cout << "         (" << x << "," << y << "," << z << ")" << endl;
  
  TVector3  pos(x/10.,   y/10.,  z/10.);
  TVector3 dpos(dx/10., dy/10., dz/10.);
  Double_t dxy = 0;
  Int_t planeId = 0;
  Double_t eLossTR = 0;
  Double_t eLossdEdx = 0;
  Double_t eLoss = 0;
  AddHit( qMaxIndex, moduleId, pos, dpos, dxy, planeId, eLossTR, eLossdEdx, eLoss);
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
void CbmTrdHitProducerCluster::SimpleReco(Int_t qMaxIndex, Float_t qMax)
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
