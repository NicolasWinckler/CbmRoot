// -------------------------------------------------------------------------
// -----                 CbmRichProtHitProducer source file            -----
// -------------------------------------------------------------------------
#include "CbmRichProtHitProducer.h"

#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmGeoRichPar.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"

#include "TVector3.h"
#include "TRandom.h"
#include "TFormula.h"
#include "TString.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichProtHitProducer::CbmRichProtHitProducer() :FairTask("RichHitProducerProt")
{
SetDefaultParameters();
}
// -------------------------------------------------------------------------

// -----   Standard constructor with verbosity level  -------------------------------------------
CbmRichProtHitProducer::CbmRichProtHitProducer(Double_t pmt_rad, Double_t pmt_dist,
		Int_t det_type, Int_t noise, Int_t verbose,
		Double_t colleff, Double_t s_mirror)
  :FairTask("RichHitProducer")
{
  fPhotomulRadius = pmt_rad;
  fPhotomulDist = pmt_dist;
  fDetType = det_type;
  fNoise = noise;
  fVerbose = verbose;
  fColl = colleff;
  fSMirror = s_mirror;

  c = 2.998E8;                // speed of light
  h = 6.626E-34;              // Plancks constant
  e = 1.6022E-19;             // Eulers constant

  nevents = 0;
  fNHits  = 0;
  fNDoubleHits = 0;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
/** Constructor with name and title */
CbmRichProtHitProducer::CbmRichProtHitProducer(const char *name, const char *title)
  : FairTask(name) {}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmRichProtHitProducer::~CbmRichProtHitProducer()
{
  fManager->Write();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
/** Private method SetDefaultParameters */
 void CbmRichProtHitProducer::SetDefaultParameters() {
  fPhotomulRadius   = 0.3;
  fPhotomulDist     = 0.;
  fDetType          = 4;
  fNoise            = 220;
  fVerbose = 1;
  fColl = 0.7;
  fSMirror = 0.06;
}
// -------------------------------------------------------------------------



// -----   Initialization of parameter Containers  ------------------------------------------------
void CbmRichProtHitProducer::SetParContainers()
{
  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fPar=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));
  // fPar1=(FairBaseParSet*)(rtdb->getContainer("FairBaseParSet"));
  // fPar->print();
//  fPar->setStatic();    // setting the parameters on static mode: <explanation>
}

// -----   Initialization   -----------------------------------------------

InitStatus CbmRichProtHitProducer::Init()
{
  fManager = FairRootManager::Instance();

  fSensNodes = fPar->GetGeoSensitiveNodes();
  fPassNodes = fPar->GetGeoPassiveNodes();
  //fSensNodes->ls();

  // get detector position:
  fDet= dynamic_cast<FairGeoNode*> (fSensNodes->FindObject("rich1detector"));
  if (! fDet) cout << " -I no RICH Geo Node  found !!!!!  " << endl;
  //fDet->Dump();
  //fDet->print();
  FairGeoTransform* detTr=fDet->getLabTransform();  // detector position in labsystem
  FairGeoVector detPosLab=detTr->getTranslation(); // ... in cm
  FairGeoTransform detCen=fDet->getCenterPosition();  // center in Detector system
  FairGeoVector detPosCen=detCen.getTranslation();
  fDetX = detPosLab.X() + detPosCen.X();   /** x coordinate of photodetector (Labsystem, cm) */
  fDetY = detPosLab.Y() + detPosCen.Y();   /** y coordinate of photodetector (Labsystem, cm) */
  fDetZ = detPosLab.Z() + detPosCen.Z();   /** z coordinate of photodetector (Labsystem, cm) */

  TArrayD *fdetA=fDet->getParameters();  // get other geometry parameters: width in x, width in y, thickness
  fDetWidthX = fdetA->At(0);
  fDetWidthY = fdetA->At(1);
  for(Int_t i=0;i<fdetA->GetSize();i++) cout << "Array detector " << fdetA->At(i)<< endl;
  FairGeoRotation fdetR=detTr->getRotMatrix();
  // detector might be rotated by theta around x-axis:
  if (fVerbose) {
     cout << "Rotation matrix of photodetector " << endl;
     for(Int_t i=0;i<9;i++) cout << "Rot(" << i << ") = " << fdetR(i) << endl;
     }

  // possible tilting around x-axis (theta) and y-axis (phi)
   // fdetR(0) = cos(phi)
   // fdetR(1) = 0
   // fdetR(2) = -sin(phi)
   // fdetR(3) = -sin(theta)sin(phi)
   // fdetR(4) = cos(theta)
   // fdetR(5) = -sin(theta)cos(phi)
   // fdetR(6) = cos(theta)sin(phi)
   // fdetR(7) = sin(theta)
   // fdetR(8) = cos(theta)cos(phi)

  // theta = tilting angle around x-axis
  theta = TMath::ASin(fdetR(7));
  // phi = tilting angle around y-axis
  phi = -1.*TMath::ASin(fdetR(2));
  if (fVerbose) cout << "Rich Photodetector was tilted around x by " << theta*180./TMath::Pi()
  << " degrees" << endl;
  if (fVerbose) cout << "Rich Photodetector was tilted around y by " << phi*180./TMath::Pi()
  << " degrees" << endl;

  // get refractive index of gas
  FairGeoNode *gas= dynamic_cast<FairGeoNode*> (fPassNodes->FindObject("rich1gas1"));
  if (! gas) cout << " -I no RICH Geo Node  found !!!!!  " << endl;
  FairGeoMedium* med = gas->getMedium();
//  med->Dump();
  Int_t npckov = med->getNpckov();
  Double_t* cerpar;
  cerpar=new Double_t[4];
  if (fVerbose) cout << "Number of optical parameters for Cherenkov " << npckov << endl;
  med->getCerenkovPar(0,cerpar);
  /*
  for(Int_t i=0;i<4;i++) {
    if (i==0) cout << " photon energy " << cerpar[0] << endl;
    if (i==1) cout << " absorption lenght" << cerpar[1] << endl;
    if (i==2) cout << " detection efficiency " << cerpar[2] << endl;
    if (i==3) cout << " refractive index (n-1)*10000 " << (cerpar[3]-1.)*10000. << endl;
    }
   */
  nrefrac = cerpar[3];
  if (fVerbose) cout << " refractive index for lowest photon energies (n-1)*10000  " << (nrefrac-1.0)*10000.0 << endl;

  // transform nominal detector position (for tilted photodetector), x>0, y>0:
    Double_t fDetY_org,fDetX_org;
    fDetX_org = fDetX;
    fDetY_org = fDetY;
    fDetZ_org = fDetZ;
/*    fDetX = fDetX_org*TMath::Cos(phi)+fDetZ_org*TMath::Sin(phi);
    fDetY = -fDetX_org*TMath::Sin(theta)*TMath::Sin(phi) + fDetY_org*TMath::Cos(theta) + fDetZ_org*TMath::Sin(theta)*TMath::Cos(phi);
    fDetZ = -fDetX_org*TMath::Cos(theta)*TMath::Sin(phi) - fDetY_org*TMath::Sin(theta) + fDetZ_org*TMath::Cos(theta)*TMath::Cos(phi);*/

  if (fVerbose > 0) {
    cout << "---------------------- RICH Hit Producer ---------------------------------------" << endl;
    cout << "   detector position in (x,y,z) [cm]: " << fDetX_org << "  " << fDetY_org << "  " << fDetZ_org << endl;
    cout << "   tilted detector position in (x,y,z) [cm]: " << fDetX << "  " << fDetY << "  " << fDetZ << endl;
    cout << "   detector size in x and y [cm]: " << fDetWidthX << "  " << fDetWidthY << endl;
    if (fDetType==0)
    cout << "   ideal detector " << endl;
    if (fDetType==1)
    cout << "   detector type: Protvino PMT with PMT radius = " << fPhotomulRadius << " cm, distance between PMTs = " << fPhotomulDist << " cm" << endl;
    if (fDetType==3)
    cout << "   detector type: CSI with pad size = " << fPhotomulRadius << " cm, distance between panels = " << fPhotomulDist << " cm" << endl;
    if (fDetType==2 || fDetType == 4)
    cout << "   detector type: Hamamatsu H8500 with pad size = " << fPhotomulRadius << " cm, distance between elements = " << fPhotomulDist << " cm" << endl;
    cout << "   number of noise hits (to be reduced by geometrical efficiency) " << fNoise << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
  }




  //------------- example for getting more parameters from the data base: -------------------
/*
  // 1) get and print medium
     FairGeoMedium* med = fDet->getMedium();
     med->Dump();

  // 2) retrieve relevant parameter
  // Shape
     Int_t npoints = fDet->getNumPoints();
     Double_t para[npoints][3];
     TString shapeName = fDet->getShape();
     for (Int_t i=0;i<npoints;i++) {
         for (Int_t j=0;j<3; j++){
              para[i][j] = fDet->getVolParameter(i,j);
              cout << "i: " << i << "j: " << j << "par: " << para[i][j] << endl;
         }
     }

  // 3) Lab Transform
     FairGeoTransform* transf =  fDet->getLabTransform();
     FairGeoRotation rot = transf->getRotMatrix();
     FairGeoVector trans = transf->getTransVector();

     Double_t rotp[9];
     cout << " Lab rotation : " << endl;
     for (Int_t i=0; i<9; i++) {
                                  rotp[i] = rot(i);
                                  cout << " i: " << i << " val: " << rotp[i] ;
     }
     cout << endl;
     cout << " Lab translation : " << endl;
     cout << " tX: " << trans.X() << " tY: " << trans.Y() << " tZ: " << trans.Z() << endl;
*/
  //------------------------------------------------------------------------------------------

  //RICH MC points
  fListRICHpts = (TClonesArray*)fManager->GetObject("RichPoint");
  // all tracks
  fListStack = (TClonesArray *)fManager->GetObject("MCTrack");
  fHitCollection = new TClonesArray("CbmRichHit");
  fManager->Register("RichHit","RICH",fHitCollection, kTRUE);
  //fPar->print();
  // setting the parameter class on static mode.
  //fPar->setStatic();

  return kSUCCESS;

}

// -------------------------------------------------------------------------


// -----   Execution of Task   ---------------------------------------------

// -------------------------------------------------------------------------
void CbmRichProtHitProducer::Exec(Option_t* option)
{

  CbmRichPoint* pt=NULL;

  Int_t RichDetID = 0;

  nevents++;
  if (fVerbose > 0) printf("\n\n=====> Event no. %d\n",nevents);

  Double_t lambda_min,lambda_max,lambda_step;
  Double_t efficiency[40];
  SetPhotoDetPar(fDetType,lambda_min,lambda_max,lambda_step,efficiency);

    if (fDetType == 0){
     fPhotomulRadius = 0.;
     fPhotomulDist = 0.;
    }
    if (fDetType == 2 || fDetType == 4 || fDetType == 5) {
      fPhotomulRadius = 0.6125;
      fPhotomulDist = 0.2;
     }
    if (fDetType == 3) {
      fPhotomulRadius = 0.8;
      fPhotomulDist = 0.5;
     }

  if (fVerbose > 0){
    cout << " --------------------- RICH Hit Producer ---------------------------------------" << endl;
    cout << "   Settings chosen for PMT plane (fDetType = " << fDetType << ")" << endl;
    cout << " -------------------------------------------------------------------------------" << endl;
    if (fDetType == 0) {
    cout << "   ideal detector " << endl;
    }
    if (fDetType == 1) {
    cout << "   single PMTs (Protvino q.e.) chosen with: " << endl;
    cout << "   PMT radius and distance between tubes (cm)" << fPhotomulRadius << " " << fPhotomulDist << endl;
    }
    if (fDetType == 2 || fDetType == 4 || fDetType == 5) {
    cout << "   Hamamatsu MAPMT H8500 (8x8 pixel): " << endl;
    cout << "   pixel length and height " << fPhotomulRadius << " cm" << endl;
    cout << "   effective distance between units " << 2*fPhotomulDist << " cm" << endl;
    }
    if (fDetType == 3) {
    cout << "   Gasdetector with CsI photocathode: " << endl;
    cout << "   pixel length and height " << fPhotomulRadius << " cm" << endl;
    cout << "   effective distance between units " << 2*fPhotomulDist << " cm" << endl;
    }
    cout << "   maximum wavelength  " << lambda_max << endl;
    cout << "   minimum wavelength  " << lambda_min << endl;
    cout << "   stepsize            " << lambda_step << endl;
  }

  if (fVerbose > 2) {
    Int_t Neff=(Int_t)((lambda_max-lambda_min)/lambda_step);
    for (Int_t ii=0; ii < Neff ; ii++)
    {
     cout << efficiency[ii] << endl;
    }
  }

  if (fVerbose > 0) cout <<"   Number of input MC points: "<<fListRICHpts->GetEntries()<<endl;

  fHitCollection->Clear();
  fNHits = 0;
  fNDoubleHits = 0;



  for(Int_t j=0; j<fListRICHpts->GetEntries(); j++) {
    pt = (CbmRichPoint*)fListRICHpts->At(j);

    TVector3 posPoint;
    pt->Position(posPoint);

    Double_t xDet,yDet,zDet;

    // tilt points by
    // -theta, -phi for x>0, y>0
    //  theta, -phi for x>0, y<0
    //  theta,  phi for x<0, y<0
    // -theta,  phi for x<0, y>0
    // and shift x position in order to avoid overlap
    //-----------------------------------------------
    const Double_t ptGlobal[] = {posPoint.X(), posPoint.Y(), posPoint.Z()};
    Double_t ptLocal[3];
    gGeoManager->FindNode(ptGlobal[0], ptGlobal[1], ptGlobal[2]);
    gGeoManager->MasterToLocal(ptGlobal, ptLocal);
    xDet = ptLocal[0];
    yDet = ptLocal[1] + fDetY;
    zDet = ptLocal[2];
/*    xDet = ptGlobal[0];
    yDet = ptGlobal[1];
    zDet = ptGlobal[2];*/
/*    cout << endl;
    cout << ptGlobal[0] << "  " << ptGlobal[1] << "  " << ptGlobal[2] << endl;
    cout << ptLocal[0] << "  " << ptLocal[1] << "  " << ptLocal[2] << endl;
    cout << endl;*/
//    if (posPoint.X() > 0 && posPoint.Y() > 0) {
/*      xDet = posPoint.X()*TMath::Cos(phi) + posPoint.Z()*TMath::Sin(phi) - fDetZ_org*TMath::Sin(phi);
      yDet = -posPoint.X()*TMath::Sin(theta)*TMath::Sin(phi) + posPoint.Y()*TMath::Cos(theta) + posPoint.Z()*TMath::Sin(theta)*TMath::Cos(phi);
      zDet = -posPoint.X()*TMath::Cos(theta)*TMath::Sin(phi) - posPoint.Y()*TMath::Sin(theta) + posPoint.Z()*TMath::Cos(theta)*TMath::Cos(phi);*/
//      }
/*    if (posPoint.X() > 0 && posPoint.Y() < 0) {
      xDet = posPoint.X()*TMath::Cos(phi) + posPoint.Z()*TMath::Sin(phi) - fDetZ_org*TMath::Sin(phi);
      yDet = posPoint.X()*TMath::Sin(theta)*TMath::Sin(phi) + posPoint.Y()*TMath::Cos(theta) - posPoint.Z()*TMath::Sin(theta)*TMath::Cos(phi);
      zDet = -posPoint.X()*TMath::Cos(theta)*TMath::Sin(phi) + posPoint.Y()*TMath::Sin(theta) + posPoint.Z()*TMath::Cos(theta)*TMath::Cos(phi);
      }
    if (posPoint.X() < 0 && posPoint.Y() < 0) {
      xDet = posPoint.X()*TMath::Cos(phi) - posPoint.Z()*TMath::Sin(phi) + fDetZ_org*TMath::Sin(phi);
      yDet = -posPoint.X()*TMath::Sin(theta)*TMath::Sin(phi) + posPoint.Y()*TMath::Cos(theta) - posPoint.Z()*TMath::Sin(theta)*TMath::Cos(phi);
      zDet = posPoint.X()*TMath::Cos(theta)*TMath::Sin(phi) + posPoint.Y()*TMath::Sin(theta) + posPoint.Z()*TMath::Cos(theta)*TMath::Cos(phi);
      }
    if (posPoint.X() < 0 && posPoint.Y() > 0) {
      xDet = posPoint.X()*TMath::Cos(phi) - posPoint.Z()*TMath::Sin(phi) + fDetZ_org*TMath::Sin(phi);
      yDet = posPoint.X()*TMath::Sin(theta)*TMath::Sin(phi) + posPoint.Y()*TMath::Cos(theta) + posPoint.Z()*TMath::Sin(theta)*TMath::Cos(phi);
      zDet = posPoint.X()*TMath::Cos(theta)*TMath::Sin(phi) - posPoint.Y()*TMath::Sin(theta) + posPoint.Z()*TMath::Cos(theta)*TMath::Cos(phi);
      }*/
/*    xDet = posPoint.X();
    yDet = posPoint.Y();
    zDet = posPoint.Z();*/
    TVector3 detPoint(xDet,yDet,zDet);


    if (fVerbose > 1) cout << " position in Labsystem " << posPoint.X() << " " << posPoint.Y() << " " << posPoint.Z() << endl;
    if (fVerbose > 1) cout << " tilted position in Labsystem " << detPoint.X() << " " << detPoint.Y() << " " << detPoint.Z() << endl;

    Int_t    trackID    = pt->GetTrackID();
    CbmMCTrack *p= (CbmMCTrack*) fListStack->At(trackID);
    Int_t gcode= TMath::Abs(p->GetPdgCode());
//    cout << p->GetRichPoints() << endl;


/*    if ((fVerbose) && ((zDet < (fDetZ-0.25)) || (zDet > (fDetZ+0.25)))) {
      cout << " z-position not at " << fDetZ << " but " << zDet << endl;
    }*/



    //hit position as a center of PMT
    Double_t xHit, yHit;
    Int_t pmtID;
    Double_t sigma0 = 0.;
    Double_t sigma = 0.19;  // sigma (cm) for additional smearing of HitPosition due to WLS film 

    //FindRichHitPosition
    if (fDetType == 0) {
    xHit = detPoint.X();
    yHit = detPoint.Y();
    pmtID = j;
    }
    if (fDetType == 1) FindRichHitPositionSinglePMT(detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);
    if (fDetType == 2 || fDetType == 4 || fDetType == 5) FindRichHitPositionMAPMT(sigma0,detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);
    if (fDetType == 3) FindRichHitPositionCsI(detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);

    //Double_t zHit = detPoint.Z();
    Double_t zHit = fDetZ;           // fix z-position to nominal value: either tilted (fDetZ = zDet) or untilted (fDetZ_org)
//    TVector3 posHit(xHit,yHit,zHit);

    //error of hit position
    //at the moment nothing better than +-tube_radius
    TVector3 posHitErr(fPhotomulRadius,fPhotomulRadius,0.);


    // add Hit: Hit assigned only if xHit and yHit != 0
    if (xHit!=0.0 && yHit!=0.0) {

     if (fDetType == 1) {
       if (fVerbose)
      if (TMath::Sqrt((detPoint.X()-xHit)*(detPoint.X()-xHit)+(detPoint.Y()-yHit)*(detPoint.Y()-yHit)) > (fPhotomulRadius+fPhotomulDist)*1.5)
          cout << "-E- RichHitProducer: wrongly assigned Hits (distance point-hit too large)!" << endl;
	  }
     if (fDetType == 2 || fDetType == 3 || fDetType == 4) {
      if (fVerbose)
      if (TMath::Abs(detPoint.X()-xHit) > fPhotomulRadius || TMath::Abs(detPoint.Y()-yHit) > fPhotomulRadius*1.5)
          cout << "-E- RichHitProducer: wrongly assigned Hits (distance point-hit too large)! " <<
	  detPoint.X() << " " << xHit << " " << detPoint.Y() << " " << yHit << endl;
	  }
     if (fDetType == 5) {      // fDetType 5: additional smearing with RMS=3mm due to WLS film
      if (fVerbose)
      if (TMath::Abs(detPoint.X()-xHit) > fPhotomulRadius+1.5 || TMath::Abs(detPoint.Y()-yHit) > fPhotomulRadius*1.5)
          cout << "-E- RichHitProducer: wrongly assigned Hits ? (distance point-hit too large)! " <<
	  detPoint.X() << " " << xHit << " " << detPoint.Y() << " " << yHit << endl;
	  }

      if (gcode == 50000050) {
	//for photons weight with efficiency of PMT
	TVector3 mom;
	pt->Momentum(mom);
	Double_t etot = sqrt(mom.Px()*mom.Px()+mom.Py()*mom.Py()+
			     mom.Pz()*mom.Pz());
	Double_t lambda=c/nrefrac*h/e/etot;   // wavelength in nm
	detection=0;
	if (lambda >= lambda_min && lambda < lambda_max) {
	  Int_t ilambda=(Int_t)((lambda-lambda_min)/lambda_step);
	  Double_t rand = gRandom->Rndm();
	  detection = 0;
          if (fDetType == 5 && lambda < 300.) {   // smear Hit position for lambda < 300 nm (WLS film!)
              FindRichHitPositionMAPMT(sigma,detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);
            }

	  if (efficiency[ilambda]*fColl > rand ) detection = 1;
//          detection = 1;

	} // min <= lambda < max
      }  // if photon

      // detection efficiency for hadrons crossing the PMT-cathodes?
      //      else if (gcode ==  211) detection=1;  //pi+-
      //      else if (gcode ==  321) detection=1;  //K+-
      //      else if (gcode == 2212) detection=1;  //p+-
      else {
      // worst case: assume that all charged particles crossing the PMTplane leave Cherenkov light in the PMTglass
      // which will be detected
	detection=1;
	}

      TVector3 posHit(xHit,yHit,zHit);

      if (detection==1) {

	Int_t detID = pt->GetDetectorID();
	if (RichDetID == 0) RichDetID = detID;
	if (RichDetID != detID) cout << " - ERROR - RichDetID changed from " << RichDetID <<
	                                " to " << detID << endl;
	Double_t ampl = GetAmplitude();
	AddHit(posHit,posHitErr,detID,pmtID,ampl,j);
      }  // photon detected?
    }

    if (fVerbose > 2) {
      cout << " iHit, Point-x, Point-y, Hit-x, Hiy-y, detected, PMT? " << j << " "
           << posPoint.X() << " " << posPoint.Y() << " "
	   << xHit << " " << yHit << " " << detection << " " << pmtID << endl;
    }
  }   // loop over input points

  //   for (Int_t iHit=0; iHit<fNHits; iHit++)
  //     ((CbmRichHit*)fHitCollection->At(iHit))->Print();

  // add noise hits
  for(Int_t j=0; j < fNoise; j++) {


    Double_t rand = gRandom->Rndm();
    Double_t xRand = -fDetWidthX + rand*2.*fDetWidthX;
    rand = gRandom->Rndm();
    Double_t yRand = fDetY - fDetWidthY + rand*2.*fDetWidthY;
    //Double_t yRand = -fDetWidthY + rand*2.*fDetWidthY;

    Double_t xHit, yHit;
    Int_t pmtID;

    //FindRichHitPosition
    if (fDetType == 0) {
    xHit = xRand;
    yHit = yRand;
    pmtID = -j;
    }
    if (fDetType == 1) FindRichHitPositionSinglePMT(xRand,yRand,xHit,yHit,pmtID);
    if (fDetType == 2 || fDetType == 4 || fDetType == 5) FindRichHitPositionMAPMT(0,xRand,yRand,xHit,yHit,pmtID);
    if (fDetType == 3) FindRichHitPositionCsI(xRand,yRand,xHit,yHit,pmtID);

    // add Hit
    if (xHit!=0.0 && yHit!=0.0) {
      Double_t zHit = fDetZ;
      TVector3 posHit(xHit,yHit,zHit);
      Double_t ampl = GetAmplitude();

      //error of hit position
      //at the moment nothing better than +-tube_radius
      TVector3 posHitErr(fPhotomulRadius,fPhotomulRadius,0.);

      AddHit(posHit,posHitErr,RichDetID,pmtID,ampl,-1);
      }
  }

  if (fVerbose > 0) {
    cout <<"  --->  Number of hits: "<<fHitCollection->GetEntries()<<endl;
    cout <<"  --->  Fraction of double hits: "<<(Double_t)(fNDoubleHits)/(Double_t)(fNHits) <<endl;
    cout <<"-------------------------------------------------------------------"<<endl;
    cout << endl;
    }
}

// -----   Add Hit to HitCollection   --------------------------------------
void CbmRichProtHitProducer::AddHit(TVector3 &posHit, TVector3 &posHitErr,
				Int_t detID, Int_t pmtID, Double_t ampl, Int_t index)
{
// Add this hit to existing one, if in the same PMT,
// otherwise create a new one.

  Bool_t hitMerged = kFALSE;
  CbmRichHit *hit;
  // Check if there was any hit in the same PMT
  for (Int_t iHit=0; iHit<fNHits; iHit++) {
    hit = (CbmRichHit*)fHitCollection->At(iHit);
    if (pmtID == hit->GetPmtId() && detID==hit->GetDetectorId()) {
      hit->SetNPhotons(hit->GetNPhotons()+1);
      hit->SetAmplitude(GetAmplitude()+ampl);
      hitMerged = kTRUE;
      fNDoubleHits++;
      break;
    }
  }

  // If no hits found in this PMT, add a new one
  if (!hitMerged) {
    new((*fHitCollection)[fNHits]) CbmRichHit();
    hit = (CbmRichHit*)fHitCollection->At(fNHits);
    hit->SetPosition     (posHit);
    hit->SetPositionError(posHitErr);
    hit->SetDetectorId(detID);
    hit->SetPmtId(pmtID);
    hit->SetNPhotons(1);
    hit->SetAmplitude(GetAmplitude());
    hit->SetRefId(index);
    fNHits++;
  }
}
// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void CbmRichProtHitProducer::Finish()
{
  fHitCollection->Clear();
}
// -------------------------------------------------------------------------

// -----  protected method: Set Photodetector Parameters ----------------------------
void CbmRichProtHitProducer::SetPhotoDetPar(Int_t det_type, Double_t& fLambdaMin, Double_t& fLambdaMax, Double_t& fLambdaStep, Double_t fEfficiency[])
{
// gives parameters for a chosen photodetector type

  if (fVerbose > 0) cout << "SetPhotoDetPar routine called for PMT type " << fDetType << endl;

if (det_type == 1){

  /** PMT efficiencies for Protvino-type PMT
       corresponding range in lambda: (100nm)120nm - 700nm in steps of 20nm */

  fLambdaMin = 120.;
  fLambdaMax = 700.;
  fLambdaStep = 20.;

  fEfficiency[0] = 0.216;
  fEfficiency[1] = 0.216;
  fEfficiency[2] = 0.216;
  fEfficiency[3] = 0.216;
  fEfficiency[4] = 0.216;
  fEfficiency[5] = 0.216;
  fEfficiency[6] = 0.216;
  fEfficiency[7] = 0.216;
  fEfficiency[8] = 0.216;
  fEfficiency[9] = 0.216;
  fEfficiency[10] = 0.216;
  fEfficiency[11] = 0.227;
  fEfficiency[12] = 0.23;
  fEfficiency[13] = 0.227;
  fEfficiency[14] = 0.216;
  fEfficiency[15] = 0.2;
  fEfficiency[16] = 0.176;
  fEfficiency[17] = 0.15;
  fEfficiency[18] = 0.138;
  fEfficiency[19] = 0.1;
  fEfficiency[20] = 0.082;
  fEfficiency[21] = 0.06;
  fEfficiency[22] = 0.044;
  fEfficiency[23] = 0.032;
  fEfficiency[24] = 0.022;
  fEfficiency[25] = 0.015;
  fEfficiency[26] = 0.01;
  fEfficiency[27] = 0.006;
  fEfficiency[28] = 0.004;

/*
  fLambdaMin = 100.;
  fLambdaMax = 700.;
  fLambdaStep = 20.;

  fEfficiency[0] = 0.216;
  fEfficiency[1] = 0.216;
  fEfficiency[2] = 0.216;
  fEfficiency[3] = 0.216;
  fEfficiency[4] = 0.216;
  fEfficiency[5] = 0.216;
  fEfficiency[6] = 0.216;
  fEfficiency[7] = 0.216;
  fEfficiency[8] = 0.216;
  fEfficiency[9] = 0.216;
  fEfficiency[10] = 0.216;
  fEfficiency[11] = 0.216;
  fEfficiency[12] = 0.227;
  fEfficiency[13] = 0.23;
  fEfficiency[14] = 0.227;
  fEfficiency[15] = 0.216;
  fEfficiency[16] = 0.2;
  fEfficiency[17] = 0.176;
  fEfficiency[18] = 0.15;
  fEfficiency[19] = 0.138;
  fEfficiency[20] = 0.1;
  fEfficiency[21] = 0.082;
  fEfficiency[22] = 0.06;
  fEfficiency[23] = 0.044;
  fEfficiency[24] = 0.032;
  fEfficiency[25] = 0.022;
  fEfficiency[26] = 0.015;
  fEfficiency[27] = 0.01;
  fEfficiency[28] = 0.006;
  fEfficiency[29] = 0.004;
  */

  }
 else if (det_type == 3){

  /** quantum efficiency for CsI photocathode
      approximately read off from fig.3 in NIM A 433 (1999) 201 (HADES)*/

    fLambdaMin = 130.;
    fLambdaMax = 210.;
    fLambdaStep = 10.;

    fEfficiency[0] = 0.45;
    fEfficiency[1] = 0.4;
    fEfficiency[2] = 0.35;
    fEfficiency[3] = 0.32;
    fEfficiency[4] = 0.25;
    fEfficiency[5] = 0.2;
    fEfficiency[6] = 0.1;
    fEfficiency[7] = 0.03;

   }
  else if (det_type == 2){

  /** PMT efficiencies for Hamamatsu H8500
                        (Flat type Multianode Photomultiplier)
   corresponding range in lambda: 260nm - 740nm in steps of 20nm */

    fLambdaMin = 260.;
    fLambdaMax = 740.;
    fLambdaStep = 20.;

    fEfficiency[0] = 0.06;
    fEfficiency[1] = 0.12;
    fEfficiency[2] = 0.2;
    fEfficiency[3] = 0.22;
    fEfficiency[4] = 0.22;
    fEfficiency[5] = 0.22;
    fEfficiency[6] = 0.21;
    fEfficiency[7] = 0.2;
    fEfficiency[8] = 0.18;
    fEfficiency[9] = 0.16;
    fEfficiency[10] = 0.14;
    fEfficiency[11] = 0.11;
    fEfficiency[12] = 0.1;
    fEfficiency[13] = 0.06;
    fEfficiency[14] = 0.047;
    fEfficiency[15] = 0.03;
    fEfficiency[16] = 0.021;
    fEfficiency[17] = 0.012;
    fEfficiency[18] = 0.006;
    fEfficiency[19] = 0.0023;
    fEfficiency[20] = 0.0008;
    fEfficiency[21] = 0.00022;
    fEfficiency[22] = 0.00007;
    fEfficiency[23] = 0.00002;

   }
   else if (det_type == 4){

  /** PMT efficiencies for Hamamatsu H8500-03
                        (Flat type Multianode Photomultiplier with UV window)
   corresponding range in lambda: 200nm - 640nm in steps of 20nm */

    fLambdaMin = 200.;
    fLambdaMax = 640.;
    fLambdaStep = 20.;

    fEfficiency[0] = 0.095;
    fEfficiency[1] = 0.13;
    fEfficiency[2] = 0.16;
    fEfficiency[3] = 0.2;
    fEfficiency[4] = 0.23;
    fEfficiency[5] = 0.24;
    fEfficiency[6] = 0.25;
    fEfficiency[7] = 0.25;
    fEfficiency[8] = 0.24;
    fEfficiency[9] = 0.24;
    fEfficiency[10] = 0.23;
    fEfficiency[11] = 0.22;
    fEfficiency[12] = 0.2;
    fEfficiency[13] = 0.16;
    fEfficiency[14] = 0.14;
    fEfficiency[15] = 0.1;
    fEfficiency[16] = 0.065;
    fEfficiency[17] = 0.045;
    fEfficiency[18] = 0.02;
    fEfficiency[19] = 0.017;
    fEfficiency[20] = 0.007;
    fEfficiency[21] = 0.0033;

   }

else if (det_type == 5){

  /** PMT efficiencies for Hamamatsu H8500 + WLS film
                        (Flat type Multianode Photomultiplier with UV window)
   corresponding range in lambda: 150nm - 650nm in steps of 20nm */

    fLambdaMin = 160.;
    fLambdaMax = 640.;
    fLambdaStep = 20.;

    fEfficiency[0] = 0.1;
    fEfficiency[1] = 0.2;
    fEfficiency[2] = 0.2;
    fEfficiency[3] = 0.2;
    fEfficiency[4] = 0.2;
    fEfficiency[5] = 0.2;
    fEfficiency[6] = 0.23;
    fEfficiency[7] = 0.24;
    fEfficiency[8] = 0.25;
    fEfficiency[9] = 0.25;
    fEfficiency[10] = 0.24;
    fEfficiency[11] = 0.24;
    fEfficiency[12] = 0.23;
    fEfficiency[13] = 0.22;
    fEfficiency[14] = 0.2;
    fEfficiency[15] = 0.16;
    fEfficiency[16] = 0.14;
    fEfficiency[17] = 0.1;
    fEfficiency[18] = 0.065;
    fEfficiency[19] = 0.045;
    fEfficiency[20] = 0.02;
    fEfficiency[21] = 0.017;
    fEfficiency[22] = 0.007;
    fEfficiency[23] = 0.0033;

   }
    else if (det_type == 0){

  /** ideal detector */

    fLambdaMin = 100.;
    fLambdaMax = 700.;
    fLambdaStep = 600.;

    fEfficiency[0] = 1.;

   }
else {
    cout << "ERROR: photodetector type not specified" << endl;

    fLambdaMin = 100.;
    fLambdaMax = 100.;
    fLambdaStep = 100.;

    fEfficiency[0] = 0.;

    }

}

// -----  protected method: Find Hit Position   ----------------------------
void CbmRichProtHitProducer::FindRichHitPositionSinglePMT(Double_t xPoint, Double_t yPoint,
				         Double_t& xHit, Double_t& yHit,
                                         Int_t & pmtID)
{
/** calculate Hits for Protvino type of PMT (single PMTs with circle surface, hexagonal packing):
    hexagonal packing of round phototubes with radius fPhotomulRadius, distance between phototubes 2*fPhotomulDist
    -> effective radius is (fPhotomulRadius+fPhotomulDist) assuming that the gap is covered by Al funnels**/

  xHit = 0.;
  yHit = 0.;

  // Transformation of global (x,y) coordinates to local coordinates in photodetector plane (u,v)
  //    the center of (u,v) CS is in the lower left corner of each photodetector
  //    u is parrallel to x, v tilted by 30degree (alpha = 30 degree)

  Double_t uPoint, vPoint;
  Double_t uPMT, vPMT;
  Double_t alpha = TMath::Pi()/6.;
  Double_t distance;

  // smear points due to light scattering in mirror
   xPoint = xPoint + gRandom->Gaus(0,fSMirror);
   yPoint = yPoint + gRandom->Gaus(0,fSMirror);

  uPoint = 2.*fDetWidthX - (fPhotomulRadius+fPhotomulDist) + xPoint;
  if (yPoint > 0)
    vPoint = (- fDetY + fDetWidthY - (fPhotomulRadius+fPhotomulDist) + yPoint)/ TMath::Cos(alpha);
  if (yPoint < 0)
    vPoint = (fDetY + fDetWidthY - (fPhotomulRadius+fPhotomulDist) + yPoint)/ TMath::Cos(alpha);

  // Calculate Position of nearest PMT
  uPMT = (fPhotomulRadius+fPhotomulDist)*((Int_t)(uPoint/(fPhotomulRadius+fPhotomulDist)+0.999));
  vPMT = (fPhotomulRadius+fPhotomulDist)*((Int_t)(vPoint/(fPhotomulRadius+fPhotomulDist)+0.999));

  // Calculate distance between PMT and Point
  distance = TMath::Sqrt((uPMT-uPoint)*(uPMT-uPoint)+(vPMT-vPoint)*(vPMT-vPoint)*TMath::Cos(alpha)*TMath::Cos(alpha));

  // if distance < (fPhotomulRadius+fPhotomulDist)
  // ==> retransform to global (x,y) and store Hit (center of PMT)
  if (distance <= (fPhotomulRadius+fPhotomulDist)){

    xHit = uPMT - 2.*fDetWidthX + (fPhotomulRadius+fPhotomulDist);
    if (yPoint > 0)
    yHit = vPMT*TMath::Cos(alpha) + fDetY - fDetWidthY + (fPhotomulRadius+fPhotomulDist);
    if (yPoint < 0)
    yHit = vPMT*TMath::Cos(alpha) - fDetY - fDetWidthY + (fPhotomulRadius+fPhotomulDist);

    pmtID = (Int_t)(uPMT/(fPhotomulRadius+fPhotomulDist))*100000+(Int_t)(vPMT/(fPhotomulRadius+fPhotomulDist));
    if (yPoint<0.) pmtID = -1*pmtID;
    }  // if hit
}


// -----  protected method: Find Hit Position   ----------------------------
void CbmRichProtHitProducer::FindRichHitPositionMAPMT(Double_t sigma, Double_t xPoint, Double_t yPoint,
				         Double_t& xHit, Double_t& yHit,
                                         Int_t & pmtID)
{
/** calculate Hits for MAPMT (Hamamatsu H8500, 8x8 MAPMT):
    dimensions:
    length = 52mm x 52mm, active area = 49mm x 49mm
    pixel (mean size) = 6.125mm x 6.125mm
    assume some spacing between single units of s=1mm
    ==> use as effective values   fPhotomulRadius = 6.125mm = 0.6125cm
                                  fPhotomulDist = 0.5mm + 1.5mm = 2mm = 0.2cm

    sigma (cm) : add WLS film --> hits smeared with sigma
    **/


  xHit = 0.;
  yHit = 0.;

  Int_t nPixel = 8;
  Double_t length = (Double_t)(nPixel)*fPhotomulRadius+2*fPhotomulDist;   //effective size

//  if (fVerbose > 3 ) cout << "FindHitPos_MAPMT: " << nPixel << " " << length << " " <<  fPhotomulRadius << " " << fPhotomulDist << endl;


  // Transformation of global (x,y) coordinates to local coordinates in photodetector plane (u,v)
  //                  the center of (u,v) CS is in the lower left corner of each photodetector

  Double_t uPoint, vPoint;
  Double_t uPMT, vPMT, uPMTs, vPMTs;

  // smear points due to light scattering in mirror
   xPoint = xPoint + gRandom->Gaus(0,fSMirror);
   yPoint = yPoint + gRandom->Gaus(0,fSMirror);

// smear Point if photon is converted via WLS film:
  if (sigma > 0.) {
   xPoint = xPoint + gRandom->Gaus(0,sigma);
   yPoint = yPoint + gRandom->Gaus(0,sigma);
  }

  uPoint = 2.*fDetWidthX + xPoint;
//  if (yPoint > 0)
    vPoint = - fDetY + fDetWidthY + yPoint;
    //vPoint = 2.*fDetWidthY + yPoint;
//  if (yPoint < 0)
//    vPoint = fDetY + fDetWidthY + yPoint;

  // calculate lower left corner of effective area of MAPMT unit which has been hit
  uPMT = length*(Int_t)(uPoint/length)+fPhotomulDist;
  vPMT = length*(Int_t)(vPoint/length)+fPhotomulDist;

  // reject points not lying in the effective area of the MAPMT units:
  if ((TMath::Abs((uPMT+(Double_t)(nPixel)/2.*fPhotomulRadius)-uPoint) < ((Double_t)(nPixel)/2.*fPhotomulRadius)) &&
      (TMath::Abs((vPMT+(Double_t)(nPixel)/2.*fPhotomulRadius)-vPoint) < ((Double_t)(nPixel)/2.*fPhotomulRadius))) {

    // check that uPoint > uPMT and vPoint > vPMT
    if (uPoint < uPMT) cout << " -E- HitProducer: calculation of MAPMT unit (u) " << uPoint << " " << uPMT << endl;
    if (vPoint < vPMT) cout << " -E- HitProducer: calculation of MAPMT unit (v) " << vPoint << " " << vPMT << endl;

    // calculate center of PMT cell hit in this MAPMT unit -> Hit
    uPMTs = fPhotomulRadius*(Int_t)((uPoint-uPMT)/fPhotomulRadius)+fPhotomulRadius/2. + uPMT;
    vPMTs = fPhotomulRadius*(Int_t)((vPoint-vPMT)/fPhotomulRadius)+fPhotomulRadius/2. + vPMT;

    // ==> retransform to global (x,y) and store Hit
    xHit = uPMTs - 2.*fDetWidthX;
    yHit = vPMTs + fDetY - fDetWidthY;
    //yHit = vPMTs - 2.*fDetWidthY;

    pmtID = ((Int_t)(uPoint/length)*100 + (Int_t)((uPoint-uPMT)/fPhotomulRadius))*100000
          + ((Int_t)(vPoint/length)*100 + (Int_t)((vPoint-vPMT)/fPhotomulRadius));

  } // point in effective area ?

  if (fVerbose > 3) cout << "FindHitPos_MAPMT: " << xPoint << " " << yPoint << " " << xHit << " " << yHit << endl;

}

// -----  protected method: Find Hit Position   ----------------------------
void CbmRichProtHitProducer::FindRichHitPositionCsI(Double_t xPoint, Double_t yPoint,
				         Double_t& xHit, Double_t& yHit,
                                         Int_t & pmtID)
{
/** calculate Hits for CsI
    dimensions (assume 3 panels of 1.4m x 1.067 m) per plane):
    height = (fDetWidthY - 2* fPhotomulDist) * 2
    length = (2.*fDetWidthX - 2* fPhotomulDist) * 2 / 3
    pixels (mean size) = 8mm x 8mm (similar to ALICE)
    integrate spacing between single panels into fPhotomulDist (= eff. distance between active areas)
    fPhotomulDist = 0.5cm
    fPhotomulRadius = 0.8cm
    **/

  xHit = 0.;
  yHit = 0.;
  Double_t xlength = 2.*fDetWidthX * 2. / 3.;
  Double_t ylength = fDetWidthY * 2.;

  // Transformation of global (x,y) coordinates to local coordinates in photodetector plane (u,v)
  //                  the center of (u,v) CS is in the lower left corner of each photodetector

  Double_t uPoint, vPoint;
  Double_t uPMT, vPMT, uPMTs, vPMTs;

  // smear points due to light scattering in mirror
   xPoint = xPoint + gRandom->Gaus(0,fSMirror);
   yPoint = yPoint + gRandom->Gaus(0,fSMirror);

  uPoint = 2.*fDetWidthX + xPoint;
  if (yPoint > 0)
    vPoint = - fDetY + fDetWidthY + yPoint;
  if (yPoint < 0)
    vPoint = fDetY + fDetWidthY + yPoint;

  // calculate lower left corner of effective area of panel which has been hit
  uPMT = xlength*(Int_t)(uPoint/xlength)+fPhotomulDist;
  vPMT = ylength*(Int_t)(vPoint/ylength)+fPhotomulDist;

  // reject points not lying in the effective area of the panels:
  if ((TMath::Abs(uPMT+(xlength/2.-fPhotomulDist)-uPoint) < (xlength/2.-fPhotomulDist)) &&
      (TMath::Abs(vPMT+(ylength/2.-fPhotomulDist)-vPoint) < (ylength/2.-fPhotomulDist))) {


    // check that uPoint > uPMT and vPoint > vPMT
    if (uPoint < uPMT) cout << " -E- HitProducer: calculation of CsI unit (u) " << uPoint << " " << uPMT << endl;
    if (vPoint < vPMT) cout << " -E- HitProducer: calculation of CsI unit (v) " << vPoint << " " << vPMT << endl;

    // calculate center of PMT cell hit in this MAPMT unit -> Hit
    uPMTs = fPhotomulRadius*(Int_t)((uPoint-uPMT)/fPhotomulRadius)+fPhotomulRadius/2. + uPMT;
    vPMTs = fPhotomulRadius*(Int_t)((vPoint-vPMT)/fPhotomulRadius)+fPhotomulRadius/2. + vPMT;

    // ==> retransform to global (x,y) and store Hit
    xHit = uPMTs - 2.*fDetWidthX;
    if (yPoint > 0)
      yHit = vPMTs + fDetY - fDetWidthY;
    if (yPoint < 0)
      yHit = vPMTs - fDetY - fDetWidthY;

    pmtID = ((Int_t)(uPoint/xlength)*1000 + (Int_t)((uPoint-uPMT)/fPhotomulRadius))*100000
        + ((Int_t)(vPoint/ylength)*1000 + (Int_t)((vPoint-vPMT)/fPhotomulRadius));

    if (yPoint<0.) pmtID = -1*pmtID;

  }  // point in effective area?

}


// -------------------------------------------------------------------------

Double_t CbmRichProtHitProducer::OnePhotonAmplitude(Double_t x)
{
  // Spectrum of the PMT response to one photo-electron
  // after S.Sadovsky, 9 Sep 2004
  const Float_t kn=1.85;
  const Float_t kb=1.75;
  return TMath::Power(x*kb/kn,kn) * TMath::Exp(-(kb*x-kn));
}

// -------------------------------------------------------------------------
Double_t CbmRichProtHitProducer::GetAmplitude()
{
  // Generate randomly PMT amplitude according to probability density
  // provided by OnePhotonAmplitude(x)

  const Double_t xMin=0;
  const Double_t xMax=6;
  Double_t ampl;
  while (kTRUE)
    if (gRandom->Rndm() < OnePhotonAmplitude(ampl = gRandom->Uniform(xMin,xMax)))
      break;
  return ampl;
}

ClassImp(CbmRichProtHitProducer)