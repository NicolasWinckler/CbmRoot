#include "CbmSttHitProducer.h"

#include "FairGeoNode.h"
#include "CbmGeoSttPar.h"
#include "FairGeoTransform.h"
//#include "CbmTrdRadiator.h"
#include "CbmSttPoint.h"
#include "CbmSttHit.h"

#include "FairRootManager.h"
//#include "FairMCApplication.h"
//#include "FairDetector.h"
#include "CbmMCTrack.h"
//#include "CbmTrd.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

//#include "TParticle.h"
//#include "TRef.h"
#include "TRandom.h"
//#include "TString.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <iostream>
using std::cout;
using std::endl;




const Int_t CbmSttHitProducer::fgkNSegm[3][2] = {{2,4},{4,6},{6,8}};

// ---- Default constructor -------------------------------------------
CbmSttHitProducer::CbmSttHitProducer()
  : FairTask("SttHitProducer"),
    fSttPoints(NULL),
    fHitCollection(new TClonesArray("CbmSttHit")),
    fListStack(NULL),
    fDx(0.),
    fDy(0.),
    fSigmaX(),
    fSigmaY(),
    fTrdPerStation(0),
    fNlayer(0),
    fVersion(""),
    fNHits(0),
    fGeoPar(NULL),
    fBasePar(NULL),
    fNfoils(0),
    fFoilThick(0.),
    fGapThick(0.),
    fSimpleTR(0.),
    fSizeX(),
    fSizeY(),
    fTransl(),
    fRotat()
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmSttHitProducer::CbmSttHitProducer(const char *name, const char *title)
  : FairTask(name),
    fSttPoints(NULL),
    fHitCollection(new TClonesArray("CbmSttHit")),
    fListStack(NULL),
    fDx(0.),
    fDy(0.),
    fSigmaX(),
    fSigmaY(),
    fTrdPerStation(0),
    fNlayer(0),
    fVersion(""),
    fNHits(0),
    fGeoPar(NULL),
    fBasePar(NULL),
    fNfoils(0),
    fFoilThick(0.),
    fGapThick(0.),
    fSimpleTR(0.),
    fSizeX(),
    fSizeY(),
    fTransl(),
    fRotat()
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmSttHitProducer::~CbmSttHitProducer()
{
    FairRootManager *ioman =FairRootManager::Instance();
    ioman->Write();
    fHitCollection->Clear("C");
    delete fHitCollection;
}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmSttHitProducer::SetParContainers()
{
    cout<<" * HitProducer * :: SetParContainers() "<<endl;

    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fBasePar = (FairBaseParSet*)
	(rtdb->getContainer("FairBaseParSet"));

    fGeoPar = (CbmGeoSttPar*)(rtdb->getContainer("CbmGeoSttPar"));
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmSttHitProducer::ReInit(){

    //cout<<" * HitProducer * :: ReInit() "<<endl;

    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fGeoPar = (CbmGeoSttPar*)(rtdb->getContainer("CbmGeoSttPar"));

    return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmSttHitProducer::Init()
{

    cout<<" * HitProducer * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();
    fSttPoints=(TClonesArray *) ioman->GetObject("SttPoint");
    if ( ! fSttPoints ) {
      cout << "-W CbmSttHitProducer::Init: No SttPoints array!" << endl;
      cout << "                          Task will be inactive" << endl;
      return kERROR;
    }

    fListStack = (TClonesArray*)ioman->GetObject("MCTrack");
    fHitCollection = new TClonesArray("CbmSttHit", 100);
    ioman->Register("SttHit","STT",fHitCollection,kTRUE);
    ioman->Register("MCEventHeader.","",
		    (TClonesArray*)ioman->GetObject("MCEventHeader."),kTRUE);

    fGeoPar->print();
    // setting the parameter class on static mode
    //fPar->setStatic();

    // fBasePar->Dump();
//    TObjArray* arr = fBasePar->GetDetList();
  //  FairDetector* trd = (FairDetector*) arr->FindObject("TRD");
    // trd->Dump();
    // here get the geometry file name with version

//    TString fname = trd->GetGeometryFileName();

    FairRun *run = FairRun::Instance();
    FairRuntimeDb *rtdb = run->GetRuntimeDb();
    CbmGeoSttPar* par = (CbmGeoSttPar*)(rtdb->getContainer("CbmGeoSttPar"));
    TObjArray *sensNodes = par->GetGeoSensitiveNodes();
    TObjArray *passNodes = par->GetGeoPassiveNodes();
    Char_t volName[255];
    for (Int_t ist = 0; ist < 3; ++ist) {
      for (Int_t jch = 0; jch < 2; ++jch) {
	//sscanf(&(gMC->CurrentVolName()[3]),"%d",&fstation);
	sprintf(volName,"stt%dC%dgas",ist+1,jch+1);
	FairGeoNode *node = (FairGeoNode*) sensNodes->FindObject(volName);
	//FairGeoTransform* transf = node->getLabTransform();
	//FairGeoVector vec = transf->getTransVector();
	//FairGeoVector vec = transf->getTranslation();
	Double_t xSize = TMath::Abs ((*node->getParameters())[0]);
	Double_t ySize = TMath::Abs ((*node->getParameters())[1]);
	fSizeX[ist][jch] = xSize;
	fSizeY[ist][jch] = ySize;
	cout << ist << " " << jch << " " << fSizeX[ist][jch] << " " << fSizeY[ist][jch] << endl;
	//Double_t fSizeX[3][2]; //! 3 stations, 2 chamber types

	sprintf(volName,"stt%dC%d#1",ist+1,jch+1);
	node = (FairGeoNode*) passNodes->FindObject(volName);
	FairGeoTransform* transf = node->getLabTransform();
	FairGeoVector vec = transf->getTranslation();
	fTransl[ist][jch*2] = vec;
	sprintf(volName,"stt%dC%d#3",ist+1,jch+1);
	node = (FairGeoNode*) passNodes->FindObject(volName);
	transf = node->getLabTransform();
	vec = transf->getTranslation();
	fTransl[ist][jch*2+1] = vec;
      }
    }
    // Get rotation matrices for doublets
    for (Int_t id = 0; id < 3; ++id) {
      sprintf(volName,"stt1D#%d",id+1);
      FairGeoNode *node = (FairGeoNode*) passNodes->FindObject(volName);
      cout << node << endl;
      FairGeoTransform* transf = node->getLabTransform();
      fRotat[id] = transf->getRotMatrix();
    }

    //exit(0);
    /*
    FairGeoNode *fm1= (FairGeoNode *)fSensNodes->FindObject("trd3gas#4");
    FairGeoNode *fm2= (FairGeoNode *)fSensNodes->FindObject("trd3gas#3");
    FairGeoNode *fm3= (FairGeoNode *)fSensNodes->FindObject("trd6gas#2");
    FairGeoNode *fm4= (FairGeoNode *)fSensNodes->FindObject("trd1gas#4");
    FairGeoNode *fm5= (FairGeoNode *)fSensNodes->FindObject("trd1mod1gas");

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
      cout << "   in CbmTrdStrawHitMaker                " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFATAL;
    }


    //ReadGeo();   // reads the geomtry

    // Init the computation of the TR spectra
    // '0.0' stands for the momentum

    fRadiator->Init(fSimpleTR, fNfoils, fFoilThick, fGapThick);
    */

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmSttHitProducer::Exec(Option_t * option)
{

  static Int_t first = 1;
  static Double_t cosAng, sinAng, segLeng[3][2];
  if (first) {
    first = 0;
    cosAng = TMath::Cos(10.*TMath::DegToRad());
    sinAng = TMath::Sin(10.*TMath::DegToRad());
    for (Int_t st = 0; st < 3; ++st) {
      for (Int_t ch = 0; ch < 2; ++ch) segLeng[st][ch] = 
					 fSizeY[st][ch] / fgkNSegm[st][ch] * 2;
    }
  }

  // return;
  fNHits = 0;
  CbmSttPoint *point = NULL;

  Double_t xHit, yHit, zHit;
  Double_t xHitErr, yHitErr, zHitErr;

  fHitCollection->Clear();

  //cout<<" * No. of Trds per Station = "<<fTrdPerStation<<endl;
  Int_t nentries = fSttPoints->GetEntriesFast();
  cout<<" ** "<<nentries<<" Stt hits to be created in this event** "<<endl;

  TVector3 pos, mom;
  Double_t teta, phi;
  Double_t x, y, z;

  Int_t station;        // stt station
  Int_t layer;          // stt layer in given stt station
  Int_t plane;          // unique position of the Stt plane in the whole setup
  Int_t rot;  // used in the smearing part - rotation of the Stt planes

  for (Int_t j = 0; j < nentries; ++j) {

    //if (j%1000 == 0) cout <<"** "<<j<<" points processed "<<endl;
    point = (CbmSttPoint*) fSttPoints->UncheckedAt(j);

    if (NULL == point) continue;

    Int_t trackID = point->GetTrackID();
    CbmMCTrack *track= (CbmMCTrack*) fListStack->At(trackID);
    if (NULL == track) continue;
    //	Int_t pdgCode= TMath::Abs(p->GetPdgCode());
    Int_t pdgCode = track->GetPdgCode();

    point->Position(pos);
    point->Momentum(mom);

    xHit = pos.X();
    yHit = pos.Y();
    zHit = pos.Z();
    xHitErr = 0.02; // 200 um //fDx;
    yHitErr = 0.; //fDy;

    /*
    Float_t errX = gRandom->Gaus(0,fDx);
    Float_t errY = gRandom->Gaus(0,fDy);
    if (TMath::Abs(errX) > 3*fDx) errX = 3* fDx * errX/TMath::Abs(errX);
    if (TMath::Abs(errY) > 3*fDy) errY = 3* fDy * errY/TMath::Abs(errY);

    // um -> cm
    errX/=10000.0;
    errY/=10000.0;

    xHit = xHit + errX;
    yHit = yHit + errY;
    */

    zHitErr = 0.0;
    TVector3 posHit(xHit, yHit, zHit);
    TVector3 posHitErr(xHitErr,yHitErr, zHitErr);
    CbmSttHit *hit = AddHit(posHit, posHitErr, trackID, j, point);

    teta = 0;
    phi = 0;
    x = pos(0);
    y = pos(1);
    z = pos(2);
    station = point->GetStationNo() - 1;
    layer = point->GetLayerNo();
    // Compute the plane number - starting from 1
    //plane = fTrdPerStation*(Station - 1) + Layer;

    // TR
    // Sorry, Electrons & Positrons only
    //	if(TMath::Abs(pdgCode) == 11 && mom.Z() > 0.5){  //0.5
    /*
      if(TMath::Abs(pdgCode) == 11){ 
      
      ELossTR = fRadiator->GetTR(mom);

      ELoss += ELossTR;

      }
      // ..tr
      */

    // Get local coordinates w.r.t. the "left lower" corner of the chamber
    // (corner closest to the beam line)
    Double_t diam[3] = {0.4, 0.6, 0.6}; // tube diameters
    /*
    Double_t yloc = y - fCornerY[station][TMath::Abs(point->GetChamberNo())-1] *
      TMath::Sign(1,hit->GetChamber());
    yloc = TMath::Abs(yloc);
    Double_t xloc = x - fCornerX[station][TMath::Abs(point->GetChamberNo())-1] *
      TMath::Sign(1,hit->GetChamber());
    xloc = TMath::Abs(xloc);
    */
    
    // rotation of the stt planes
    rot = (layer-1) / 2;

    //if (point->GetTrackID() == 17) cout << layer << " " << rot << " " << xloc << endl;
    /*
    if (rot) { 
      // Stereo planes
      hit->SetAlong(yloc/cosAng);
      xloc *= cosAng;
      //if (point->GetTrackID() == 17) cout << xloc << " " << sinAng << " " << cosAng << endl;
      if (rot == 1 && TMath::Abs(hit->GetChamber()) == 1 ||
	  rot == 2 && TMath::Abs(hit->GetChamber()) == 2) 
	xloc += yloc * sinAng;
      else xloc -= yloc * sinAng;
    } else hit->SetAlong(TMath::Abs(yloc));
    Int_t itube = (Int_t) (xloc / diam[station]);
    if (xloc < 0) --itube;
    hit->SetTube(itube);
    */
    //if (point->GetTrackID() == 17) cout << station << " " << layer << " " << xloc << " " << yloc << " " << itube << endl;

    FairGeoVector p(xHit, yHit, zHit);
    Int_t ch = TMath::Abs (hit->GetChamber());
    Int_t ch1 = ch;
    if (hit->GetChamber() == 1) --ch1;
    else if (hit->GetChamber() == -2) ++ch1;
    //FairGeoVector ploc = fRotat[rot] * p + fTransl[station][ch];
    FairGeoVector ploc = fRotat[rot].inverse() * (p - fTransl[station][ch1]);
    FairGeoVector size(fSizeX[station][ch-1], fSizeY[station][ch-1], 0.);
    if (hit->GetChamber() == -1 || hit->GetChamber() == 2) size.setY(-size.getY());
    if (hit->GetChamber() == -1 || hit->GetChamber() == -2) size.setX(-size.getX());
    ploc += size;
    cout << station << " " << hit->GetChamber() << " " << ch1 << " " << rot << " " << p.getX()
	 << " " << p.getY() << " " << ploc.getX() << " " << ploc.getY() << " " << endl;
    Int_t itube = (Int_t) (TMath::Abs(ploc.getX()) / diam[station]);
    hit->SetTube (itube);
    hit->SetAlong (TMath::Abs(ploc.getY()));
    hit->SetSegment ((Int_t)(hit->GetAlong()/segLeng[station][ch-1]));

    // Global coordinates in rotated coordinate system
    ploc = fRotat[rot].inverse() * p;
    Float_t errU = gRandom->Gaus(0,xHitErr);
    hit->SetU(ploc.getX()+errU);
    
  } // for (Int_t j = 0; j < nentries;
  //exit(0);
}
// --------------------------------------------------------------------

// ---- Add Hit to HitCollection --------------------------------------
CbmSttHit* CbmSttHitProducer::AddHit(TVector3 &posHit, TVector3 &posHitErr,
				     Int_t index, Int_t ref, CbmSttPoint *point)
{

  new((*fHitCollection)[fNHits]) CbmSttHit(point->GetDetectorID(), 
					   posHit, posHitErr, index, point);
  CbmSttHit* hit = (CbmSttHit*)fHitCollection->At(fNHits); // iHit

  hit->SetPosition     (posHit);
  hit->SetPositionError(posHitErr);
  hit->SetRefIndex(ref);

  fNHits++;
  return hit;
}

// --------------------------------------------------------------------

// ---- Finish --------------------------------------------------------
void CbmSttHitProducer::Finish()
{
}
// --------------------------------------------------------------------

// ---- Register ------------------------------------------------------
void CbmSttHitProducer::Register(){

  FairRootManager::Instance()->Register("SttHit","Stt", fHitCollection, kTRUE);

}
// --------------------------------------------------------------------

// ---- SetNlayer -----------------------------------------------------
void CbmSttHitProducer::SetNlayer(Int_t a) 
{
      if (a == 0) fNlayer = 12;
      else fNlayer = 9;
}
// --------------------------------------------------------------------

ClassImp(CbmSttHitProducer)
