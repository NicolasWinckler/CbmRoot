#include "CbmTrdHitProducerSmearing.h"

#include "CbmGeoTrdPar.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <iostream>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdHitProducerSmearing::CbmTrdHitProducerSmearing()
    :FairTask("TrdHitProducer")
	//:fRef(0)
{
    fHitCollection = new TClonesArray("CbmTrdHit");
    fDx=0.0;
    fDy=0.0;

    // default parameters
    fTrdPerStation = 3;

    // Default values changed since they better describe the spectrum
    // measured in test experiment
    /*
    fNfoils = 130;
    fFoilThick = 0.0013;
    fGapThick = 0.02;
    fSimpleTR = kTRUE;
    */
    fEfficency = 1.;

    fRadiator = NULL; //new CbmTrdRadiator();
    fGeoPar = NULL;


}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdHitProducerSmearing::CbmTrdHitProducerSmearing(const char *name, const char *title, CbmTrdRadiator *radiator)
	:FairTask(name)
{
    //	cl=0;

    // default parameters
    fTrdPerStation = 3;

    // Default values changed since they better describe the spectrum
    // measured in test experiment
    /*
    fNfoils = 130;
    fFoilThick = 0.0013;
    fGapThick = 0.02;
    fSimpleTR = kTRUE;
    */
    fEfficency = 1.;

    fHitCollection = new TClonesArray("CbmTrdHit");
    fDx=0.0;
    fDy=0.0;
    fNlayer = 0;


    fRadiator = radiator; //new CbmTrdRadiator();
    fGeoPar = NULL;

    fNHits=0;
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


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fBasePar = (FairBaseParSet*)
	(rtdb->getContainer("FairBaseParSet"));


    fGeoPar = (CbmGeoTrdPar*)(rtdb->getContainer("CbmGeoTrdPar"));



}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitProducerSmearing::ReInit(){

    //cout<<" * HitProducer * :: ReInit() "<<endl;


    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fGeoPar = (CbmGeoTrdPar*)(rtdb->getContainer("CbmGeoTrdPar"));

    return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitProducerSmearing::Init()
{

    cout<<" * HitProducer * :: Init() "<<endl;

    //    fRadiator = new CbmTrdRadiator();

    FairRootManager *ioman = FairRootManager::Instance();
    fTrdPoints=(TClonesArray *)  ioman->ActivateBranch("TRDPoint");
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdHitProducerSmearing::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fListStack = (TClonesArray*)ioman->ActivateBranch("MCTrack");
    fHitCollection = new TClonesArray("CbmTrdHit", 100);
    ioman->Register("TRDHit","TRD",fHitCollection,kTRUE);

    fGeoPar->print();
    // setting the parameter class on static mode
    //fPar->setStatic();

    // fBasePar->Dump();
//    TObjArray* arr = fBasePar->GetDetList();
  //  FairDetector* trd = (FairDetector*) arr->FindObject("TRD");
    // trd->Dump();
    // here get the geometry file name with version

//    TString fname = trd->GetGeometryFileName();


    FairRun *fRun = FairRun::Instance();
    FairRuntimeDb *rtdb= fRun->GetRuntimeDb();
    CbmGeoTrdPar* par=(CbmGeoTrdPar*)(rtdb->getContainer("CbmGeoTrdPar"));
    TObjArray *fSensNodes = par->GetGeoSensitiveNodes();


    FairGeoNode *fm1 = NULL;
    FairGeoNode *fm2 = NULL;
    FairGeoNode *fm3 = NULL;
    FairGeoNode *fm4 = NULL;
    FairGeoNode *fm5 = NULL;

    if ((FairGeoNode *)fSensNodes->FindObject("trd3gas#4"))
      fm1= (FairGeoNode *)fSensNodes->FindObject("trd3gas#4");
    if ((FairGeoNode *)fSensNodes->FindObject("trd3gas#3"))
      fm2= (FairGeoNode *)fSensNodes->FindObject("trd3gas#3");
    if ((FairGeoNode *)fSensNodes->FindObject("trd6gas#2"))
      fm3= (FairGeoNode *)fSensNodes->FindObject("trd6gas#2");
    if ((FairGeoNode *)fSensNodes->FindObject("trd1gas#4"))
      fm4= (FairGeoNode *)fSensNodes->FindObject("trd1gas#4");
    if ((FairGeoNode *)fSensNodes->FindObject("trd1mod1gas"))
      fm5= (FairGeoNode *)fSensNodes->FindObject("trd1mod1gas");

    if (fm1) {
      cout << " -I version is with 12 layers (3 stations x 4 layers each) " << endl;
      fTrdPerStation = 4;
    }
    else if (fm2 && fm4) {
      cout << " -I Special version is with 10 layers (station 1 with 4 layers, station 2+3 with 3 layers each ) " << endl;
      cout << " -I Dont know how to handle this correctly" << endl;
      fTrdPerStation = 10;
    }
    else if (fm2) {
      cout << " -I version is with 9 layers (3 stations x 3 layers each) " << endl;
      fTrdPerStation = 3;
    }
    else if (fm3) {
      cout << " -I version is with 12 layers (6 stations x 2 layers each)" << endl;
      fTrdPerStation = 2;
    }
    else if (fm5) {
      cout << " -I version is with 12 layers (3 stations x 4 layers each) and new segmented geometry" << endl;
      fTrdPerStation = 4;
    }
    else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerSmearing                " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFATAL;
    }


    //ReadGeo();   // reads the geomtry

    // Init the computation of the TR spectra
    // '0.0' stands for the momentum

    //    fRadiator->Init(fSimpleTR, fNfoils, fFoilThick, fGapThick);
    fRadiator->Init();

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdHitProducerSmearing::Exec(Option_t * option)
{

    // return;
    fNHits = 0;
    fHitCollection->Clear();
    CbmTrdPoint *pt=NULL;

    Double_t xHit, yHit, zHit;
    Double_t xHitErr, yHitErr, zHitErr;

    fHitCollection->Clear();

    cout<<" * No. of Trds per Station = "<<fTrdPerStation<<endl;
    Int_t nentries = fTrdPoints->GetEntries();
    cout<<" ** "<<nentries<<" Trd hits to be created in this event** "<<endl;

    TVector3 pos, mom;
    Double_t teta;
    Double_t phi;
    Double_t X, Y, Z;
    Double_t ELoss;       // total energy loss
    Double_t ELossTR;     // TR energy loss for e- & e+
    Double_t ELossdEdX;   // ionization energy loss

    Int_t Station;        // trd station
    Int_t Layer;          // trd layer in given trd station
    Int_t Plane;          // unique position of the TRD plane in the whole setup

    Double_t a; // used in the smearing part
    Double_t b; // used in the smearing part
    Int_t rot;  // used in the smearing part - rotation of the TRD planes

    for (int j=0; j < nentries; j++ ) {

        // if random value above fEfficency reject point
        if (gRandom->Rndm() > fEfficency ) continue;

        //if (j%1000 == 0) cout <<"** "<<j<<" points processed "<<endl;
        pt = (CbmTrdPoint*) fTrdPoints->At(j);

        if(NULL == pt) continue;

	Int_t trackID = pt->GetTrackID();
	CbmMCTrack *p= (CbmMCTrack*) fListStack->At(trackID);
        if(NULL == p) continue;
	//	Int_t pdgCode= TMath::Abs(p->GetPdgCode());
	Int_t pdgCode = p->GetPdgCode();

	pt->Position(pos);
	pt->Momentum(mom);
	teta = 0;
	phi = 0;
	X = pos(0);
	Y = pos(1);
	Z = pos(2);
	ELoss = 0.0;
	ELossTR = 0.0;
	ELossdEdX = 0.0;
	Station = pt->GetStationNo();
	Layer = pt->GetLayerNo();
        // Compute the plane number - starting from 1
        Plane = fTrdPerStation*(Station - 1) + Layer;

	// correct on higher ammount of stations in the 6x2 geometry (6 stations)

	if(fTrdPerStation == 2){
            Station = (Station + 1) / 2 -1;
	}

	if(fTrdPerStation == 10){
	  if (Station == 1) Plane = Layer;
          else if (Station == 2) Plane = 4 + Layer;
          else if (Station == 3) Plane = 7 + Layer;
	}

	ELossdEdX = pt->GetEnergyLoss();
	ELoss = ELossdEdX;

	// TR
	// Sorry, Electrons & Positrons only
	//	if(TMath::Abs(pdgCode) == 11 && mom.Z() > 0.5){  //0.5
	if(TMath::Abs(pdgCode) == 11){

	    ELossTR = fRadiator->GetTR(mom);

	    ELoss += ELossTR;

	}
	// ..tr



	// rotation of the trd planes
	rot = Layer%2; // event layers are rotated

	// New smearing
	teta = TMath::ATan(TMath::Abs(X / Z)) * 1000;    // mrad
	phi = TMath::ATan(TMath::Abs(Y / Z))  * 1000;     // mrad

	a = 0;
	b = 0;

	if (rot == 1){   // ROTATED Trd x->Y  y->X
	    a = GetSigmaX(Station);
	    SmearingY(a);
	    b = GetSigmaY(teta, Station);
	    SmearingX(b);
	}
	else if(rot == 0) {    // NOT ROTATED Trd x->X  y->Y
	    a = GetSigmaX(Station);
	    SmearingX(a);
	    b = GetSigmaY(phi, Station);
	    SmearingY(b);
	}
	else cout<<" - Err - CBmTrdHitProducer :: Exec : wrong rotation of the trd layers "<<endl;


	xHit = pos.X();
	yHit = pos.Y();
	zHit = pos.Z();
	xHitErr = fDx/10000.0; //error in cm, fDx is in um
	yHitErr = fDy/10000.0; //error in cm, fDy is in um

	Float_t errX = gRandom->Gaus(0,fDx);
        Float_t errY = gRandom->Gaus(0,fDy);
	if (TMath::Abs(errX) > 3*fDx) errX = 3* fDx * errX/TMath::Abs(errX);
	if (TMath::Abs(errY) > 3*fDy) errY = 3* fDy * errY/TMath::Abs(errY);

        // um -> cm
	errX/=10000.0;
	errY/=10000.0;

	xHit = xHit + errX;
	yHit = yHit + errY;

	zHitErr = 0.0;
	TVector3 posHit(xHit, yHit, zHit);
	TVector3 posHitErr(xHitErr,yHitErr, zHitErr);
	AddHit(posHit, posHitErr, trackID, Plane, j , ELoss, ELossTR, ELossdEdX);

    }

}
// --------------------------------------------------------------------

// ---- Add Hit to HitCollection --------------------------------------
void CbmTrdHitProducerSmearing::AddHit(TVector3 &posHit, TVector3 &posHitErr,
			       Int_t index, Int_t Plane, Int_t ref,
			       Double_t ELoss, Double_t ELossTR,
			       Double_t ELossdEdX)
{
	new((*fHitCollection)[fNHits]) CbmTrdHit(0, posHit, posHitErr, 0., ref, Plane, ELossTR, ELossdEdX, ELoss);

//	CbmTrdHit* hit;
//
//    new((*fHitCollection)[fNHits]) CbmTrdHit();
//    hit = (CbmTrdHit*)fHitCollection->At(fNHits); // iHit
//
//    hit->SetPosition     (posHit);
//    hit->SetPositionError(posHitErr);
//    hit->SetPlaneID(Plane);
//    hit->SetRefIndex(ref);
//    hit->SetELoss(ELoss);
//    hit->SetELossTR(ELossTR);
//    hit->SetELossdEdx(ELossdEdX);

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

// ---- SetNlayer -----------------------------------------------------
void CbmTrdHitProducerSmearing::SetNlayer(Int_t a)
{
      if (a == 0) fNlayer = 12;
      else fNlayer = 9;
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
