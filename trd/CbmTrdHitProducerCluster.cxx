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
#include "TH2F.h"
#include "TProfile.h"
#include "TLine.h"
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
   fTrdId(),
   ModuleHitMap(),
   fLayersBeforeStation()
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
  fTrdPoints=(TClonesArray *)ioman->ActivateBranch("TrdPoint"); 
  if ( ! fTrdPoints ) {
    cout << "-W CbmTrdHitProducerCluster::Init: No TrdPoints array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }
  fDigis =(TClonesArray *)  ioman->ActivateBranch("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerCluster::Init: No TrdDigi array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }
  fClusters =(TClonesArray *)  ioman->ActivateBranch("TrdCluster");
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
  CbmTrdGeoHandler trdGeoInfo;
  
  Bool_t result = trdGeoInfo.GetLayerInfo(fLayersBeforeStation);
  
  if (!result) return kFATAL;

  return kSUCCESS;
  
}

//----------------------------------------------------------------------
bool digiCidSorter(MyDigi *a, MyDigi *b)
{ return (a->combiId < b->combiId); }

// ---- Exec ----------------------------------------------------------
void CbmTrdHitProducerCluster::Exec(Option_t * option)
{
  Bool_t drawing = true;
  cout << "================CbmTrdHitProducerCluster==============" << endl;
  fPrfSingleRecoCounter = 0;
  fPrfDoubleRecoCounter = 0;
  fSimpleRecoCounter = 0;
  if (fDigis == NULL)
    cout << " DEBUG: fdigis is NULL" << endl;
  Int_t nDigis = fDigis->GetEntries();
  //std::map<Int_t, MyDigiList*> modules; //map of <moduleId, List of struct 'MyDigi' pointer>
  for (Int_t iDigi=0; iDigi < nDigis; iDigi++ ) {
    
    //cout << (fDigis->GetEntries()-iDigi) << endl;
    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
    
    Int_t moduleId = digi->GetDetId();
    Int_t* detInfo = fTrdId.GetDetectorInfo(moduleId); 
    Int_t Station  = detInfo[1];
    Int_t Layer    = detInfo[2];
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
  //_______________Drawing_______________________ 
  if (drawing) {
    DrawHits();
  }
  //_______________Drawing_______________________ 
  std::map<Int_t, MyHitList* >::iterator it;
  for ( it = ModuleHitMap.begin(); it != ModuleHitMap.end(); it++)
    {
      iHit += Int_t((*it).second->size());
    }
  Int_t nPoints = fTrdPoints->GetEntriesFast();
  cout << " Found " << iHit << " Hits" << endl;
  cout << "  " << Float_t(iHit*100./nPoints)<< "% MC-point to hit efficiency" << endl;
  cout << "  " << fPrfSingleRecoCounter << " Hits are PRF based reconstructed in one dimension" << endl;
  cout << "  " << fPrfDoubleRecoCounter << " Hits are PRF based reconstructed in both dimension" << endl;
  cout << "   " << fSimpleRecoCounter << " Hits are 'position of pad with maximum charge' based reconstructed in both dimension" << endl;
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
    //cout << detInfo[1] << "  " << detInfo[2] << endl;
    //-----------rotated----------------------

    hit -> rowId = digi->GetRow();
    hit -> colId = digi->GetCol();
    hit -> digiId = qMaxIndex;
    hit -> moduleId = moduleId;

    mPara -> xPos = (Int_t)(10 * fModuleInfo->GetX());
    mPara -> yPos = (Int_t)(10 * fModuleInfo->GetY());
    mPara -> zPos = (Int_t)(10 * fModuleInfo->GetZ());
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
    hit -> secIdX = GetSector(true,  digi->GetCol(), mPara);
    hit -> secIdY = GetSector(false, digi->GetRow(), mPara);
 
    Int_t neighbourIds[4] = {-1, -1, -1, -1};
    SearchNeighbours(qMaxIndex, neighbourIds, mPara, moduleDigiMap[mPara->moduleId], hit);
    delete mPara;
  }// --------------------------------------------------------------------
  void CbmTrdHitProducerCluster::SearchNeighbours(Int_t qMaxIndex, Int_t *neighbourIds, ModulePara* mPara, MyDigiList *neighbours, MyHit* hit)
  {
    //cout << "SearchNeighbours" << endl;
    Int_t counterX = 0;
    Int_t counterY = 0;

    Float_t qMax = 0;
    Int_t hitCombiId   = hit-> rowId     * (mPara->nCol + 1) + hit->colId;
    Int_t leftCombiId  = hitCombiId - 1;
    Int_t rightCombiId = hitCombiId + 1;
    Int_t downCombiId  = (hit->rowId+1) * (mPara->nCol + 1) + hit->colId;
    Int_t upCombiId    = (hit->rowId-1) * (mPara->nCol + 1) + hit->colId;
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
    SimpleReco(qMaxIndex, qMax, mPara, neighbourIds, hit/*, ModuleHitMap*/);
  
  }
  // --------------------------------------------------------------------
  Float_t CbmTrdHitProducerCluster::Prf(Float_t padWidth, Float_t sigma, Float_t qLeft, Float_t qMax, Float_t qRight)
  {
    //cout << "Prf" << endl;
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
    return recoPos;
  }
  // --------------------------------------------------------------------
  void CbmTrdHitProducerCluster::PrfReco(Int_t qMaxIndex, Float_t qMax, ModulePara* mPara, Int_t *neighbourIds, MyHit* hit)
  {
    //cout << "PrfReco" << endl;
    Float_t dxPos = 0;
    Float_t dyPos = 0;
    Float_t dzPos = 0;
    Float_t sigma = 0;
    Float_t padWidth;
    Float_t qLeft = 0;
    Float_t qRight = 0;
    Int_t left  = neighbourIds[0];
    Int_t right = neighbourIds[1];
    Int_t up    = neighbourIds[3];
    Int_t down  = neighbourIds[2];
    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(qMaxIndex);
    qMax = digi->GetCharge();

    if (left >= 0 && right >= 0) {
      digi = (CbmTrdDigi*) fDigis->At(left);
      qLeft = digi->GetCharge();
      digi = (CbmTrdDigi*) fDigis->At(right);
      qRight = digi->GetCharge();
      padWidth = mPara->PadSizeX[hit->secIdX];
      dxPos = Prf( padWidth, sigma, qLeft, qMax, qRight);
    }
    if (up >= 0 && down >= 0) {
      digi = (CbmTrdDigi*) fDigis->At(up);
      qLeft = digi->GetCharge();
      digi = (CbmTrdDigi*) fDigis->At(down);
      qRight = digi->GetCharge();
      padWidth = mPara->PadSizeY[hit->secIdY];
      dyPos = Prf( padWidth, sigma, qLeft, qMax, qRight);
    }
    if ((left >= 0 && right >= 0) && (up >= 0 && down >= 0)) {
      fPrfDoubleRecoCounter++;
    }
    else {
      if ((left >= 0 && right >= 0) || (up >= 0 && down >= 0)) {
	fPrfSingleRecoCounter++;
      }
      else {
	fSimpleRecoCounter++;
      }
    }
    //cout << " x: " << dxPos << " y: " << dyPos << endl;

    hit->xPos += dxPos;
    hit->yPos += dyPos;

    //within each row
    //also possible to reconstruct in x and y direction by prf neighbours
    //if no neighbours found -> simple reco
  }
  // --------------------------------------------------------------------
  Int_t CbmTrdHitProducerCluster::GetSector(Bool_t x, Int_t DigiCol, ModulePara* mPara)
  {
    // cout << "GetSector" << endl;
    //cout << x << "  " << DigiCol << endl;
    Int_t iSec = 0;
    if (x) {
      while (DigiCol > 0)
	{
	  if (mPara->SecCol[iSec] < mPara->nCol)
	    {
	      if (DigiCol > mPara->SecCol[iSec])
		{
		  DigiCol -= mPara->SecCol[iSec];
		  iSec++;
		}
	      if (DigiCol <= mPara->SecCol[iSec] && DigiCol > 0)
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
	  if (mPara->SecRow[iSec] < mPara->nRow)
	    {
	      if (DigiCol > mPara->SecRow[iSec])
		{
		  DigiCol -= mPara->SecRow[iSec];
		  iSec++;
		}
	      if (DigiCol <= mPara->SecRow[iSec] && DigiCol > 0)
		{
		  DigiCol -= DigiCol;
		}
	    }   
	  else {
	    DigiCol -= DigiCol;
	  }   
	}
    }
    return iSec;
  }
  // --------------------------------------------------------------------
  void CbmTrdHitProducerCluster::SimpleReco(Int_t qMaxIndex, Float_t qMax, ModulePara* mPara, Int_t *neighbourIds, MyHit* hit/*, MHitMap* ModuleHitMap*/)
  {
    //cout << "SimpleReco" << endl;
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
  
    PrfReco(qMaxIndex, qMax, mPara, neighbourIds, hit);
  
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

    planeId=fLayersBeforeStation[(mPara->Station)-1]+(mPara->Layer);
      
    AddHit( qMaxIndex, mPara->moduleId, pos, dpos, dxy, planeId, eLossTR, eLossdEdx, eLoss);
  
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
    Bool_t picEps = false;
    Bool_t picPng = true;
    if (picEps) {
      picPng = false;
    }
    Double_t DigiMax = 0.1;
    Bool_t mean = false;
    //Bool_t mean = true;
    Bool_t drawMCPoints = false;
    //Bool_t drawMCPoints = true;
    //Bool_t drawDigis = false;
    Bool_t drawDigis = true;
    Bool_t drawClusters = false;
    //Bool_t drawClusters = true;
    Bool_t drawHits = false;
    //Bool_t drawHits = true;
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
    cout << endl;
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
	    sprintf(name,"%d Digis",Int_t((*it).second->size()));
	    if (picPng) {
	      sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/%s_%s.png",title,name);
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
	    Digi->GetZaxis()->SetRangeUser(0,DigiMax);
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
	    if (picPng) {
	      Outimage->FromPad(c);
	      Outimage->WriteImage(picPath);
	    }
	    if (picEps) {
	      sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/eps/%s_%s.eps",title,name);
	      c->cd(1)->Print(picPath);
	    }
	    delete Digi;
	    delete c;
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
	    sprintf(name,"%d Cluster",Int_t((*it).second->size()));
	    if (picPng) {
	      sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/%s_%s.png",title,name);
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
	    if (picEps) {
	      sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/eps/%s_%s.eps",title,name);
	      c->cd(1)->Print(picPath);
	    }
	    delete Cluster;
	    delete c;
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
	    sprintf(name,"%d Hits",Int_t((*it).second->size()));
	    if (picPng) {
	      sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/%s_%s.png",title,name);
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
		sprintf(title,"%d Hit",i);
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
		//printf("  (%f,%f) \n  (%f,%f) \n\n",mSizeX,mSizeY,xPos,yPos);
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
	    sprintf(name,"%d Hits",Int_t((*it).second->size()));
	    if (picEps) {
	      sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/eps/%s_%s.eps",title,name);
	      c->cd(1)->Print(picPath);
	    }
	    delete Avatar;
	    delete c;
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
		sprintf(name,"%d Points_mean",Int_t((*it).second->size()));
	      }
	    else
	      {
		sprintf(name,"%d Points_in_out",Int_t((*it).second->size()));
	      }
	    if (picPng) {
	      sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/%s_%s.png",title,name);
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
		sprintf(title,"%d Point",i);
		sprintf(name,"%d Points",i);
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
		sprintf(name,"%d Points_mean",Int_t((*it).second->size()));
	      }
	    else
	      {
		sprintf(name,"%d Points_in_out",Int_t((*it).second->size()));
	      }
	    if (picEps) {
	      sprintf(picPath,"Pics/MCPoints_Digis_Clusters_Hits/eps/%s_%s.eps",title,name);
	      c->cd(1)->Print(picPath);
	    }
	    delete Avatar;
	    delete c;
	  }
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
