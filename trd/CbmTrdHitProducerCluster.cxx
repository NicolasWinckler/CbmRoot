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
	  DigiCol    = digi->GetCol();
	  DigiRow    = digi->GetRow();
	  //cout << "   ID:" << DigiIndex << "  Col:" << DigiCol << "  Row:" << DigiRow << "  Charge:" << DigiCharge << endl;
	  if (DigiCharge > qMax)
	    {
	      qMax = DigiCharge;
	      qMaxIndex = DigiIndex;
	    }
	}
      PrfReco(qMaxIndex, qMax);
      SimpleReco(qMaxIndex, qMax);
      AddHit(iHit);
      iHit++;
      //cout << "      MaxID: " << qMaxIndex << "   MaxCharge: " << qMax << endl;
    }  
  cout << " Found " << iHit << " Hits" << endl;
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
void CbmTrdHitProducerCluster::AddHit(Int_t iHit)
{
 new ((*fClusterHits)[iHit]) CbmTrdHit(/*TODO: include valide parameters*/);
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
