// -------------------------------------------------------------------------
// -----                  CbmEcalClusterFinder source file             -----
// -----                 Created 12/07/06  by D.Konstantinov           -----
// -------------------------------------------------------------------------

/**  CbmEcalClusterFinder.cxx
 *@author D.Konstantinov 
 **
 ** Task for ECAL clusterization, which is a procedure to group the hits with
 ** the common edge into clusters
 **/

/* $Id: CbmEcalClusterFinder.cxx,v 1.3 2006/07/19 15:29:03 kharlov Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalClusterFinder.cxx,v $
 * Revision 1.3  2006/07/19 15:29:03  kharlov
 * Init geometry via constuctor, add cluster hit multiplicity, commenting
 *
 * Revision 1.2  2006/07/19 14:21:38  kharlov
 * Set W0 is included
 *
 * Revision 1.1  2006/07/12 14:22:56  kharlov
 * Adding ClusterFinder
 *
 */

#include "CbmEcalClusterFinder.h"

#include "CbmEcalCluster.h"
#include "CbmEcalHit.h"

#include "FairRootManager.h"

#include "TVector3.h"

#include <vector>

using std::vector;

// -----   Default constructor   -------------------------------------------
CbmEcalClusterFinder::CbmEcalClusterFinder() :
  FairTask(),fNClusters(0),fEvent(0){}
// -------------------------------------------------------------------------

CbmEcalClusterFinder::CbmEcalClusterFinder(const char *name, const Int_t iVerbose,
					   const char *fileGeo) :
  FairTask(name,iVerbose),fNClusters(0),fEvent(0),fWzero(5.0),
  fHitThreshold(8.0e-4),fFileGeo(fileGeo)
{}


// -----   Destructor   ----------------------------------------------------
CbmEcalClusterFinder::~CbmEcalClusterFinder()
{
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}
// -------------------------------------------------------------------------

/** ------------------------------------------------------------------------
 ** Public method Init()
 ** Activate branch with ECAL hits,
 ** register a new branch with cluster collection
 ** and initialize geometry from the geometry file
 **/
// -----   Initialization   ------------------------------------------------
InitStatus CbmEcalClusterFinder::Init()
{
  FairRootManager* fManager = FairRootManager::Instance();

  // new list of ECAL hits
  fHitCollection = (TClonesArray*)fManager->ActivateBranch("EcalHit");

  fClusterCollection = new TClonesArray("CbmEcalCluster",100);
  fManager->Register("EcalCluster","ECAL",fClusterCollection,kTRUE);
  
  fInf=CbmEcalInf::GetInstance(fFileGeo);
  fStr=new CbmEcalStructure(fInf);
  fStr->Construct();
  return kSUCCESS;
}
// -------------------------------------------------------------------------

/** ------------------------------------------------------------------------
 ** Public method Exec()
 ** It makes a loop over hits and combine then into clusters
 **/
// -----   Execution of Task   ---------------------------------------------
void CbmEcalClusterFinder::Exec(Option_t* option)
{

  fClusterCollection->Clear();
  TClonesArray* fHitCopyCollection;
  fHitCopyCollection = static_cast<TClonesArray*>(fHitCollection->Clone());  

  Int_t x;
  Int_t y;

  fEvent++;
  fNClusters = 0;
  TIter nextHit(fHitCopyCollection);
  CbmEcalHit *hit =NULL  ; 

  while ( (hit = (CbmEcalHit*)nextHit() ) ) {
    // Take new seed hit for cluster

    Double32_t energy  = hit->GetEnergy();
    if (energy < fHitThreshold ) { 
      fHitCopyCollection->Remove(hit);
      continue;
    }

    // 

    Int_t detId    = hit->GetDetectorId();
    Int_t seedtype = fStr->GetType(detId);
    // finding x,y of seed
    fStr->GetGlobalCellXY(detId  ,  x,  y); 
    
    // construct new cluster with index of cell in fHitCopyCollection
    CbmEcalCluster * cl = new CbmEcalCluster( fHitCopyCollection->IndexOf(hit) );
    // removing hit from collection
    fHitCopyCollection->Remove(hit);
    
    Int_t ix = 0;
    
    while (ix<cl->GetSize()){   
      
      // checking neighboring hits and addin to cluster
      CbmEcalHit* pt = (CbmEcalHit*)fHitCollection->At(cl->GetHitIndex(ix));

      fStr->GetGlobalCellXY(pt->GetDetectorId() ,  x,  y); 
      //      printf (" Looking around cell %d x = %d y = %d", pt->GetDetectorId() ,  x,  y);
      TIter nextHit2(fHitCopyCollection);
      CbmEcalHit* hit2=NULL;
      
      while ( (hit2  = (CbmEcalHit*)nextHit2())  ){
	Int_t x1=0;
	Int_t y1=0;
	if (hit2->GetEnergy()<fHitThreshold ) { 
	  fHitCopyCollection->Remove(hit2);
	  continue;
	}
	detId = hit2 ->GetDetectorId();
	// checking for same type like seed
	if ( seedtype != fStr->GetType(detId) )  continue;
	fStr->GetGlobalCellXY(detId,x1,y1);
	
	if ( ( x+1 == x1 && y == y1)|| ( x-1 == x1 && y == y1)
	     ||( x == x1 && y+1 == y1)|| ( x == x1 && y-1 == y1)) {
	  energy += (hit2->GetEnergy());
	  cl->AddHitIndex(fHitCopyCollection->IndexOf(hit2)); 
	  fHitCopyCollection->Remove(hit2);
	}      
      }
      ix++;
    }
    AddCluster(cl, energy);
  }
  if (fVerbose!=0)
    if (fEvent%fVerbose == 0)
      printf("Event %d has %d clusters\n",fEvent,fNClusters);
}


// -----   Finish Task   ---------------------------------------------------
void CbmEcalClusterFinder::Finish()
{ }


/** -------------------------------------------------------------------------
 ** Add a new cluster to array of clusters
 **/
void CbmEcalClusterFinder::AddCluster(CbmEcalCluster * cl, Double32_t energy)
{ 
  new((*fClusterCollection)[fNClusters++]) CbmEcalCluster(cl->GetVector(),energy);
  CbmEcalCluster * cluster = (CbmEcalCluster *)fClusterCollection->Last();
  Calculate(fHitCollection,cluster);
}

/** -------------------------------------------------------------------------
 ** Public method Calculate()
 ** It evaluates cluster parameters, such as gravity center, 2nd moments
 ** and hit multiplicity
 **/

void CbmEcalClusterFinder::Calculate(TClonesArray * hitsArray, CbmEcalCluster * cl)
{
    
  CbmEcalHit* hits=NULL;
  Double_t weight = 0;
  Double_t wtot = 0.;
  Double_t x    = 0.;
  Double_t y    = 0.;
  Double_t dxx  = 0.;
  Double_t dyy  = 0.;
  Double_t dxy  = 0.;
  Double_t lambda0 = 0., lambda1=0.;
  Double_t etot = cl->GetEnergy();
  Float_t xi = 0.;
  Float_t yi = 0.;
  Float_t wzero = fWzero;
  //  cout << fWzero << endl;
  vector<Int_t> CluList = cl->GetVector();
  for (UInt_t j = 0; j < CluList.size();j++){

    hits = (CbmEcalHit*)hitsArray->At(CluList[j]);
    fStr->GetHitXY( hits->GetDetectorId()  ,  xi,  yi);
    weight = TMath::Max(0., wzero + log( hits->GetEnergy()/etot) );
    x += weight * xi;
    y += weight * yi;
    dxx += weight * xi * xi;
    dyy += weight * yi * yi;
    dxy += weight * xi * yi;
    wtot += weight;
  }

  if (wtot>0.) {
    x /= wtot;
    y /= wtot;
    dxx /=wtot;
    dyy /=wtot;
    dxy /=wtot;
    dxx -= x * x;
    dyy -= y * y;
    dxy -= x * y;

    lambda0 = 0.5 * (dxx + dyy) + TMath::Sqrt(0.25 * (dxx - dyy) * (dxx - dyy) + dxy * dxy );
    lambda1 = 0.5 * (dxx + dyy) - TMath::Sqrt(0.25 * (dxx - dyy) * (dxx - dyy) + dxy * dxy ); 
    lambda1 = TMath::Max(0.,lambda1);
  }
  cl->SetXY(x,y);
  //  cout << " x, y " << x << ", " << y << endl;
  cl->SetLambdas(lambda0,lambda1);
  cl->SetSize(cl->GetSize());
}

// -------------------------------------------------------------------------
ClassImp(CbmEcalClusterFinder)
