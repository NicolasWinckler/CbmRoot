#include "CbmTrdHitProducerSmearing.h"

#include "CbmTrdRadiator.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdGeoHandler.h"

#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairTask.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

// ---- Default constructor -------------------------------------------
CbmTrdHitProducerSmearing::CbmTrdHitProducerSmearing()
  :FairTask("TrdHitProducer"),
   fTrdPoints(NULL),
   fHitCollection(new TClonesArray("CbmTrdHit")),
   fListStack(NULL),
   fDx(0.0),
   fDy(0.0),
   fNHits(0),
   fEfficency(1.0),
   fGhostRate(0.0),
   fGhostDistance(0.0),
   fMinDist(0.0),
   fRadiator(new CbmTrdRadiator()),
   fGeoHandler(new CbmTrdGeoHandler())
{
  for (Int_t i=0; i<3; i++) {
    fSigmaX[i]=0.0;
    for (Int_t j=0; j<7; j++) {
      fSigmaY[i][j]=0.0;
    }
  }
}
// --------------------------------------------------------------------

// ---- Constructor -------------------------------------------
CbmTrdHitProducerSmearing::CbmTrdHitProducerSmearing(const char *name)
  :FairTask(name),
   fTrdPoints(NULL),
   fHitCollection(new TClonesArray("CbmTrdHit")),
   fListStack(NULL),
   fDx(0.0),
   fDy(0.0),
   fNHits(0),
   fEfficency(1.0),
   fGhostRate(0.0),
   fGhostDistance(0.0),
   fMinDist(0.0),
   fRadiator(new CbmTrdRadiator()),
   fGeoHandler(new CbmTrdGeoHandler())
{
  for (Int_t i=0; i<3; i++) {
    fSigmaX[i]=0.0;
    for (Int_t j=0; j<7; j++) {
      fSigmaY[i][j]=0.0;
    }
  }
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdHitProducerSmearing::CbmTrdHitProducerSmearing(const char *name, const char *title, CbmTrdRadiator *radiator)
  :FairTask(name),
   fTrdPoints(NULL),
   fHitCollection(new TClonesArray("CbmTrdHit")),
   fListStack(NULL),
   fDx(0.0),
   fDy(0.0),
   fNHits(0),
   fEfficency(1.0),
   fGhostRate(0.0),
   fGhostDistance(0.0),
   fMinDist(0.0),
   fRadiator(radiator),
   fGeoHandler(new CbmTrdGeoHandler())
{
  for (Int_t i=0; i<3; i++) {
    fSigmaX[i]=0.0;
    for (Int_t j=0; j<7; j++) {
      fSigmaY[i][j]=0.0;
    }
  }
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdHitProducerSmearing::~CbmTrdHitProducerSmearing()
{
    FairRootManager *ioman =FairRootManager::Instance();
    ioman->Write();
    fHitCollection->Clear("C");
    delete fHitCollection;

    if(fRadiator) delete fRadiator;

    for (fModuleHitBufferMapIt = fModuleHitBufferMap.begin(); fModuleHitBufferMapIt != fModuleHitBufferMap.end(); fModuleHitBufferMapIt++){
      for (Int_t i = 0; i < (*fModuleHitBufferMapIt).second.size(); i++)
	delete (*fModuleHitBufferMapIt).second[i];
    }
    (*fModuleHitBufferMapIt).second.clear();
}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitProducerSmearing::SetParContainers()
{
    cout<<" * HitProducer * :: SetParContainers() "<<endl;
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitProducerSmearing::ReInit(){

    cout<<" * HitProducer * :: ReInit() "<<endl;

    return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitProducerSmearing::Init()
{

  cout<<"********** Initilization of TRD Hitproducer ********"<<endl;

  FairRootManager *ioman = FairRootManager::Instance();
  
  fTrdPoints=(TClonesArray *)  ioman->GetObject("TrdPoint");
  if ( ! fTrdPoints ) {
    cout << "-W CbmTrdHitProducerSmearing::Init: No TrdPoints array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }
  
  fListStack = (TClonesArray*)ioman->GetObject("MCTrack");
  
  fHitCollection = new TClonesArray("CbmTrdHit", 100);
  ioman->Register("TrdHit","TRD",fHitCollection,kTRUE);
  
  // Extract information about the number of TRD stations and
  // the number of layers per TRD station from the geomanager.
  // Store the information about the number of layers at the entrance
  // of subsequent stations in a vector. 
  // This allows to calculate the layer number starting with 1 for the
  // first layer of the first station at a later stage by only adding 
  // the layer number in the station to the number of layers in 
  // previous stations 
  
  fGeoHandler->Init();  

  fRadiator->Init();
  
  cout<<"********** End of TRD Hitproducer init ********"<<endl;

  return kSUCCESS;
  
}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdHitProducerSmearing::Exec(Option_t * option)
{
  Bool_t debug = false;

  fNHits = 0;
  
  fHitCollection->Clear();
  
  CbmTrdPoint *pt=NULL;
  
  
  Int_t nentries = fTrdPoints->GetEntries();
  cout<<" ** "<<nentries<<" Trd hits to be created in this event** "<<endl;
  
  Double_t xHit, yHit, zHit;
  Double_t xHitErr, yHitErr, zHitErr;
  TVector3 pos, mom;
  Double_t teta;
  Double_t phi;
  Double_t X, Y, Z;
  Double_t ELoss;       // total energy loss
  Double_t ELossTR;     // TR energy loss for e- & e+
  Double_t ELossdEdX;   // ionization energy loss
  
  Int_t trdId;          // unique trd identifier
  Int_t station;        // trd station
  Int_t layer;          // trd layer in given trd station
  Int_t plane;          // unique numberof the TRD plane in the whole setup
  
  Double_t a; // used in the smearing part
  Double_t b; // used in the smearing part
  Int_t rot;  // used in the smearing part - rotation of the TRD planes
  
  Int_t nGhosts = 0;
  Int_t ghostCounter = 0;
  Int_t lostCoungter = 0;
  Int_t outputCounter = 0;
  
  printf("   Ghost rate per hit:                     %.3f\n",fGhostRate);
  printf("   min. distance between hits:             %.3f cm\n",fMinDist);
  printf("   average distance between hit and ghost: %.3f cm\n",fGhostDistance/10000.);//µm -> cm
  for (int j=0; j < nentries; j++ ) {
    
    // if random value above fEfficency reject point
    if (gRandom->Rndm() > fEfficency ) continue;
   
    if (fGhostRate > 0.0) nGhosts = gRandom->Poisson(fGhostRate);

    
    pt = (CbmTrdPoint*) fTrdPoints->At(j);
    if(NULL == pt) continue;
    
    Int_t trackID = pt->GetTrackID();
    CbmMCTrack *p= (CbmMCTrack*) fListStack->At(trackID);
    
    if(NULL == p) continue;
    Int_t pdgCode = p->GetPdgCode();
    
    trdId = pt->GetDetectorID();
    station = fGeoHandler->GetStation(trdId);
    layer = fGeoHandler->GetLayer(trdId);
    plane = fGeoHandler->GetPlane(trdId);

    
    fModuleInfo = fDigiPar->GetModule(trdId);
    Double_t moduleXmax = fModuleInfo->GetX() + fModuleInfo->GetSizex();
    Double_t moduleXmin = fModuleInfo->GetX() - fModuleInfo->GetSizex();
    Double_t moduleYmax = fModuleInfo->GetY() + fModuleInfo->GetSizey();
    Double_t moduleYmin = fModuleInfo->GetY() - fModuleInfo->GetSizey();

    ELossTR = 0.0;
    ELossdEdX = pt->GetEnergyLoss();
    ELoss = ELossdEdX;
    
    // Get the momentum and the position to calculate the TR 
    // production, the hit position and the hit error.	
    // The corresponding hit should be stored at the position of
    // the padplane which is the roughly the the exit of the gas
    // volume. So take the position from the exit point. The 
    // momentum to calculate the TR production should be taken from 
    // the entrance point.
    // FU, 21.06.10
    pt->PositionOut(pos); 
    pt->Momentum(mom);
    
    // TR
    // Sorry, Electrons & Positrons only
    if(TMath::Abs(pdgCode) == 11){
      
      ELossTR = fRadiator->GetTR(mom);
      
      ELoss += ELossTR;
      
    }
    
    X = pos(0);
    Y = pos(1);
    Z = pos(2);
    
    // rotation of the trd planes
    rot = layer%2; // event layers are rotated
    
    // New smearing
    teta = 0.;
    phi = 0.;
    teta = TMath::ATan(TMath::Abs(X / Z)) * 1000;    // mrad
    phi = TMath::ATan(TMath::Abs(Y / Z))  * 1000;     // mrad

    a = 0.;
    b = 0.;

    for (Int_t iGhost = 0; iGhost <= nGhosts; iGhost++) { //iGhost == 0 for real particle 
      if (iGhost > 0)
	ghostCounter++;
      if (rot == 1){   // ROTATED Trd x->Y  y->X
	a = GetSigmaX(station);
	SmearingY(a);
	b = GetSigmaY(teta, station);
	SmearingX(b);
      }
      else if(rot == 0) {    // NOT ROTATED Trd x->X  y->Y
	a = GetSigmaX(station);
	SmearingX(a);
	b = GetSigmaY(phi, station);
	SmearingY(b);
      }
      else cout<<" - Err - CBmTrdHitProducer :: Exec : wrong rotation of the trd layers "<<endl;
    
    
      xHit = pos.X();
      yHit = pos.Y();
      zHit = pos.Z();
      Int_t randomCounter = 0;
      do {
	randomCounter++;
	Float_t errX = gRandom->Gaus(0,fDx);
	if (iGhost > 0) { // keep real hit at its position
	  errX += gRandom->Gaus(0,fGhostDistance);
	}
	if (TMath::Abs(errX) > 3*fDx) errX = 3* fDx * errX/TMath::Abs(errX);
    
	// um -> cm
	errX/=10000.0;
    
	xHit = xHit + errX;
    
	xHitErr = fDx/10000.0; //error in cm, fDx is in um
	zHitErr = 0.0;

      } while (xHit > moduleXmax || xHit < moduleXmin);

      do {   
	randomCounter++;
	Float_t errY = gRandom->Gaus(0,fDy);
	if (iGhost > 0) { // keep real hit at its position
	  errY += gRandom->Gaus(0,fGhostDistance);
	}
	if (TMath::Abs(errY) > 3*fDy) errY = 3* fDy * errY/TMath::Abs(errY);
    
	// um -> cm
	errY/=10000.0;
    
	yHit = yHit + errY;
    
	yHitErr = fDy/10000.0; //error in cm, fDy is in um
	zHitErr = 0.0;
      } while (yHit > moduleYmax || yHit < moduleYmin); // avoid to shift the hit position out of the module in x and y
      if (debug) if (randomCounter > 1) printf("DEBUG:: %i times out of chamber\n",randomCounter);

      TVector3 posHit(xHit, yHit, zHit);
      TVector3 posHitErr(xHitErr,yHitErr, zHitErr);

      CbmTrdHit* hit = new CbmTrdHit(trdId, posHit, posHitErr, 0., 
				     j, plane, ELossTR, ELossdEdX, 
				     ELoss);
      //trdId, posHit, posHitErr, j, plane , ELoss, ELossTR, ELossdEdX);
      fModuleHitBufferMap[trdId].push_back(hit); 

      /*
	std::map<Int_t, std::vector<CbmTrdHit*> >::iterator it;
	it = fModuleHitBufferMap.find(trdId);
	if (it == fModuleHitBufferMap.end()) {
	//cout << "------new ModuleID: " << VolumeID << endl;
	CbmTrdHit* hit = new CbmTrdHit(trdId, posHit, posHitErr, 0., 
	j, plane, ELossTR, ELossdEdX, 
	ELoss);
	//trdId, posHit, posHitErr, j, plane , ELoss, ELossTR, ELossdEdX);
	fModuleHitBufferMap[trdId].push_back(hit); 
	//AddHit(trdId, posHit, posHitErr, j, plane , ELoss, ELossTR, ELossdEdX);
	}
      */
    }
  }
  
  printf("\n---Merger---\n");
  for ( fModuleHitBufferMapIt = fModuleHitBufferMap.begin();
	fModuleHitBufferMapIt != fModuleHitBufferMap.end(); ++fModuleHitBufferMapIt) {
    Double_t deltar(0.0);
    //printf("%7i hits per module befor merging\n",(*fModuleHitBufferMapIt).second.size());
    if (fMinDist > 0.0) {
      for (Int_t i = 0; i < (*fModuleHitBufferMapIt).second.size()-1; i++) {
	for (Int_t j = i+1; j < (*fModuleHitBufferMapIt).second.size(); j++) {
	  deltar =  sqrt(
			 pow((*fModuleHitBufferMapIt).second[i]->GetX() - (*fModuleHitBufferMapIt).second[j]->GetX(),2) +
			 pow((*fModuleHitBufferMapIt).second[i]->GetY() - (*fModuleHitBufferMapIt).second[j]->GetY(),2) 
			 );
	  if (deltar < fMinDist) {
	    lostCoungter++;
	    // move merged new hit to center between mother hits
	    (*fModuleHitBufferMapIt).second[i]->SetX((*fModuleHitBufferMapIt).second[i]->GetX() + 
						     0.5 * ((*fModuleHitBufferMapIt).second[j]->GetX() - (*fModuleHitBufferMapIt).second[i]->GetX()));
	    (*fModuleHitBufferMapIt).second[i]->SetY((*fModuleHitBufferMapIt).second[i]->GetY() + 
						     0.5 * ((*fModuleHitBufferMapIt).second[j]->GetY() - (*fModuleHitBufferMapIt).second[i]->GetY()));
	    (*fModuleHitBufferMapIt).second[i]->SetZ((*fModuleHitBufferMapIt).second[i]->GetZ() + 
						     0.5 * ((*fModuleHitBufferMapIt).second[j]->GetZ() - (*fModuleHitBufferMapIt).second[i]->GetZ()));
	    //Sum ELoss ELossTR ELossdEdX
	    (*fModuleHitBufferMapIt).second[i]->SetELossdEdx((*fModuleHitBufferMapIt).second[i]->GetELossdEdX() + (*fModuleHitBufferMapIt).second[j]->GetELossdEdX());
	    (*fModuleHitBufferMapIt).second[i]->SetELoss((*fModuleHitBufferMapIt).second[i]->GetELoss() + (*fModuleHitBufferMapIt).second[j]->GetELoss());
	    (*fModuleHitBufferMapIt).second[i]->SetELossTR((*fModuleHitBufferMapIt).second[i]->GetELossTR() + (*fModuleHitBufferMapIt).second[j]->GetELossTR());
	    (*fModuleHitBufferMapIt).second.erase((*fModuleHitBufferMapIt).second.begin()+j);
	  }
	}
      }
    }
    //printf("%7i hits per module after merging\n",(*fModuleHitBufferMapIt).second.size());
    for (Int_t i = 0; i < (*fModuleHitBufferMapIt).second.size(); i++) {
      TVector3 posHit((*fModuleHitBufferMapIt).second[i]->GetX(), (*fModuleHitBufferMapIt).second[i]->GetY(), (*fModuleHitBufferMapIt).second[i]->GetZ());
      TVector3 posHitErr((*fModuleHitBufferMapIt).second[i]->GetDx(), (*fModuleHitBufferMapIt).second[i]->GetDy(), (*fModuleHitBufferMapIt).second[i]->GetDz());
      AddHit((*fModuleHitBufferMapIt).first, posHit, posHitErr, (*fModuleHitBufferMapIt).second[i]->GetRefId(), (*fModuleHitBufferMapIt).second[i]->GetPlaneId(), (*fModuleHitBufferMapIt).second[i]->GetELoss(), (*fModuleHitBufferMapIt).second[i]->GetELossTR(), (*fModuleHitBufferMapIt).second[i]->GetELossdEdX());
      outputCounter++;
    
    }
  }
  printf("\n   %7i (%5.1f%%) input points\n   %7i (%5.1f%%) lost points\n   %7i (%5.1f%%) ghost hits\n   %7i (%5.1f%%) output hits\n",nentries,nentries*100./nentries,lostCoungter,lostCoungter*100./nentries,ghostCounter,ghostCounter*100./nentries,outputCounter,outputCounter*100./nentries);
  for (fModuleHitBufferMapIt = fModuleHitBufferMap.begin(); fModuleHitBufferMapIt != fModuleHitBufferMap.end(); fModuleHitBufferMapIt++){
    for (Int_t i = 0; i < (*fModuleHitBufferMapIt).second.size(); i++)
      (*fModuleHitBufferMapIt).second.clear();
  }
}


  // --------------------------------------------------------------------

  // ---- Add Hit to HitCollection --------------------------------------
  void CbmTrdHitProducerSmearing::AddHit(Int_t trdId, TVector3 &posHit, 
					 TVector3 &posHitErr,
					 Int_t ref, Int_t Plane, 
					 Double_t ELoss, Double_t ELossTR,
					 Double_t ELossdEdX) 
  {
    new((*fHitCollection)[fNHits]) CbmTrdHit(trdId, posHit, posHitErr, 0., 
					     ref, Plane, ELossTR, ELossdEdX, 
					     ELoss);
    fNHits++;
  }

  // --------------------------------------------------------------------

  // ---- Finish --------------------------------------------------------
  void CbmTrdHitProducerSmearing::Finish()
  {
    for (fModuleHitBufferMapIt = fModuleHitBufferMap.begin(); fModuleHitBufferMapIt != fModuleHitBufferMap.end(); fModuleHitBufferMapIt++){
      for (Int_t i = 0; i < (*fModuleHitBufferMapIt).second.size(); i++)
	delete (*fModuleHitBufferMapIt).second[i];
    }
    (*fModuleHitBufferMapIt).second.clear();
  }
  // --------------------------------------------------------------------

  // ---- Register ------------------------------------------------------
  void CbmTrdHitProducerSmearing::Register(){

    FairRootManager::Instance()->Register("TrdHit","Trd", fHitCollection, kTRUE);

  }
  // --------------------------------------------------------------------

  // ---- SetSigmaX -----------------------------------------------------
  void CbmTrdHitProducerSmearing::SetSigmaX(Double_t sigma[])
  {
    for(Int_t i = 0; i < 3; i++)    fSigmaX[i] = sigma[i];
  }
  // --------------------------------------------------------------------

  // ---- SetSigmaY -----------------------------------------------------
  void CbmTrdHitProducerSmearing::SetSigmaY(Double_t s1[], Double_t s2[], Double_t s3[])
  {
    for(Int_t i = 0; i < 7; i++)   fSigmaY[0][i] = s1[i];
    for(Int_t j = 0; j < 7; j++)  fSigmaY[1][j] = s2[j];
    for(Int_t k = 0; k < 7; k++ ) fSigmaY[2][k] = s3[k];
  }
  // --------------------------------------------------------------------

  // ---- GetSigmaX -----------------------------------------------------
  Double_t CbmTrdHitProducerSmearing::GetSigmaX (Int_t stack) const
  {
    if  (stack == 1)    	return fSigmaX[0];
    else if (stack == 2)    return fSigmaX[1];
    else if (stack == 3)     return fSigmaX[2];
    else return 0;
  }
  // --------------------------------------------------------------------

  // ---- GetSigmaY -----------------------------------------------------
  Double_t CbmTrdHitProducerSmearing::GetSigmaY (Double_t teta, Int_t stack ) const
  {
    if (teta <= 50)	                 return fSigmaY[stack - 1][0];
    else if(teta > 50 && teta <= 100)    return fSigmaY[stack - 1][1];
    else if(teta > 100 && teta <= 200)   return fSigmaY[stack - 1][2];
    else if(teta > 200 && teta <= 300)   return fSigmaY[stack - 1][3];
    else if(teta > 300 && teta <= 400)   return fSigmaY[stack - 1][4];
    else if(teta > 400 && teta <= 500)   return fSigmaY[stack - 1][5];
    else if(teta > 500)		         return fSigmaY[stack - 1][6];
    else return 0;
  }
  // --------------------------------------------------------------------

  ClassImp(CbmTrdHitProducerSmearing)
