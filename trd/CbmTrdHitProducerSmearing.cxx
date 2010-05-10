#include "CbmTrdHitProducerSmearing.h"

#include "CbmTrdRadiator.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdDetectorId.h"

#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairTask.h"

#include "TClonesArray.h"
#include "TGeoVolume.h"
#include "TGeoManager.h"
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
    :FairTask("TrdHitProducer")
	//:fRef(0)
{
    fHitCollection = new TClonesArray("CbmTrdHit");

    fDx=0.0;
    fDy=0.0;

    fNHits=0;

    fEfficency = 1.;


    // Create the radiator with default parameters
    fRadiator = new CbmTrdRadiator();

}
// --------------------------------------------------------------------

// ---- Constructor -------------------------------------------
CbmTrdHitProducerSmearing::CbmTrdHitProducerSmearing(const char *name)
    :FairTask(name)
	//:fRef(0)
{
    fHitCollection = new TClonesArray("CbmTrdHit");

    fDx=0.0;
    fDy=0.0;

    fNHits=0;

    fEfficency = 1.;

    // Create the radiator with default parameters
    fRadiator = new CbmTrdRadiator();

}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdHitProducerSmearing::CbmTrdHitProducerSmearing(const char *name, const char *title, CbmTrdRadiator *radiator)
	:FairTask(name)
{
    fEfficency = 1.;

    fHitCollection = new TClonesArray("CbmTrdHit");

    fDx=0.0;
    fDy=0.0;

    fNHits=0;

    fRadiator = radiator; //new CbmTrdRadiator();

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
}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitProducerSmearing::SetParContainers()
{
    cout<<" * HitProducer * :: SetParContainers() "<<endl;

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

    cout<<" * CbmTrdHitProducerSmearing::Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();
 
    fTrdPoints=(TClonesArray *)  ioman->ActivateBranch("TrdPoint");
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdHitProducerSmearing::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fListStack = (TClonesArray*)ioman->ActivateBranch("MCTrack");

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
    TGeoVolume *fm=NULL;
    Int_t stationNr = 1;
    Int_t totalNrOfLayers = 0;
    fLayersBeforeStation.push_back(totalNrOfLayers);
    char volumeName[10];
    sprintf(volumeName, "trd%d", stationNr);
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
    if (fm){
      Int_t nrOfLayers = fm->GetNdaughters();
      totalNrOfLayers += nrOfLayers;
      fLayersBeforeStation.push_back(totalNrOfLayers);
      do {
	stationNr++;
        sprintf(volumeName, "trd%d", stationNr);
	fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
        if (fm) {
          nrOfLayers = fm->GetNdaughters();
	  totalNrOfLayers += nrOfLayers;
	  fLayersBeforeStation.push_back(totalNrOfLayers);
	}
      } while (fm);
    } else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerSmearing        " <<endl;
      cout << " No TRD stations found in the geometry " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFATAL;
    }
         
    fRadiator->Init();

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdHitProducerSmearing::Exec(Option_t * option)
{

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

    for (int j=0; j < nentries; j++ ) {

        // if random value above fEfficency reject point
        if (gRandom->Rndm() > fEfficency ) continue;

        pt = (CbmTrdPoint*) fTrdPoints->At(j);
        if(NULL == pt) continue;

  	Int_t trackID = pt->GetTrackID();
	CbmMCTrack *p= (CbmMCTrack*) fListStack->At(trackID);

        if(NULL == p) continue;
	Int_t pdgCode = p->GetPdgCode();

        trdId = pt->GetDetectorID();
        Int_t* detInfo = fDetId.GetDetectorInfo(trdId); 
	station = detInfo[1];
	layer = detInfo[2];
        plane=fLayersBeforeStation[station-1]+layer;

	ELossTR = 0.0;
	ELossdEdX = pt->GetEnergyLoss();
	ELoss = ELossdEdX;

	pt->Position(pos);
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

	Float_t errX = gRandom->Gaus(0,fDx);
        Float_t errY = gRandom->Gaus(0,fDy);
	if (TMath::Abs(errX) > 3*fDx) errX = 3* fDx * errX/TMath::Abs(errX);
	if (TMath::Abs(errY) > 3*fDy) errY = 3* fDy * errY/TMath::Abs(errY);

        // um -> cm
	errX/=10000.0;
	errY/=10000.0;

	xHit = xHit + errX;
	yHit = yHit + errY;

	xHitErr = fDx/10000.0; //error in cm, fDx is in um
	yHitErr = fDy/10000.0; //error in cm, fDy is in um
	zHitErr = 0.0;

	TVector3 posHit(xHit, yHit, zHit);
	TVector3 posHitErr(xHitErr,yHitErr, zHitErr);

	AddHit(trdId, posHit, posHitErr, j, 
               plane , ELoss, ELossTR, ELossdEdX);

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
Double_t CbmTrdHitProducerSmearing::GetSigmaX (Int_t stack)
{
	if  (stack == 1)    	return fSigmaX[0];
	else if (stack == 2)    return fSigmaX[1];
    	else if (stack == 3)     return fSigmaX[2];
      	else return 0;
}
// --------------------------------------------------------------------

// ---- GetSigmaY -----------------------------------------------------
Double_t CbmTrdHitProducerSmearing::GetSigmaY (Double_t teta, Int_t stack )
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
