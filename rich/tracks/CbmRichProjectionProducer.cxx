/* $Id: CbmRichProjectionProducer.cxx,v 1.9 2006/06/22 08:23:40 hoehne Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmRichProjectionProducer.cxx,v $
 * Revision 1.9  2006/06/22 08:23:40  hoehne
 * bug found in filling some variables
 *
 * Revision 1.8  2006/06/21 13:54:23  hoehne
 * flag added in order to choose between projection from imaginary z-plane or mirror
 *
 * Revision 1.7  2006/01/26 09:56:36  hoehne
 * updates due to changes in extrapolation routines: acts now on "RichTrackZ"
 *
 * Revision 1.6  2005/12/19 19:04:31  friese
 * New FairTask design
 *
 * Revision 1.5  2005/12/08 15:13:04  turany
 * change GetRegistered, GetObject and CheckActivatedBranch
 * to GetObject, also add a flag to the arguments of Register which control saving
 * to file(kTRUE) or only in memory(kFALSE)
 *
 * Revision 1.4  2005/11/30 15:38:17  hoehne
 * hardcoded parameters removed
 *
 * Revision 1.3  2005/07/18 15:03:09  kharlov
 * Correct projection counter
 *
 * Revision 1.2  2005/07/18 10:11:29  kharlov
 * Typos are corrected
 *
 * Revision 1.1  2005/07/18 09:53:44  kharlov
 * Track projection from imaginary plane onto a photodetector
 *
 */

// -------------------------------------------------------------------------
// -----              CbmRichProjectionProducer source file            -----
// -----   Created 23/06/05  by P.Stolpovsky(P.Stolpovsky at gsi.de)   -----
// ----- Project track by strate line from imagiry plane to the mirror -----
// -----           and reflect it to the photodetector plane           -----
// -------------------------------------------------------------------------

#include "CbmRichProjectionProducer.h"

#include "CbmRichRing.h"
#include "CbmRichPoint.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairTrackParam.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairGeoNode.h"
#include "CbmGeoRichPar.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairRunAna.h"

#include "TObjArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TRandom.h"
#include "TFormula.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichProjectionProducer::CbmRichProjectionProducer() :
FairTask("RichProjectionProducer")
{
SetDefaultParameters();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmRichProjectionProducer::CbmRichProjectionProducer(Int_t verbose, Int_t zflag)
  :FairTask("RichProjectionProducer")
{
fVerbose = verbose;
fZflag = zflag;     // 1 - use imaginary plane; 2 - use mirror point for extrapolation
}

// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
/** Constructor with name and title */
CbmRichProjectionProducer::CbmRichProjectionProducer(const char *name,
					             const char *title)
  : FairTask(name) {}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmRichProjectionProducer::~CbmRichProjectionProducer()
{
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
/** Private method SetDefaultParameters */
void CbmRichProjectionProducer::SetDefaultParameters() {
  fVerbose  = 1;
  fZflag = 1;
}
// -------------------------------------------------------------------------


//-----   Initialization  of Parameter Containers -------------------------
void CbmRichProjectionProducer::SetParContainers()
{

  FairRunAna* sim = FairRunAna::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  par=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));

  }

// -----   Initialization   ------------------------------------------------
InitStatus CbmRichProjectionProducer::Init()
{
  FairRootManager* fManager = FairRootManager::Instance();

  fSensNodes = par->GetGeoSensitiveNodes();
  fPassNodes = par->GetGeoPassiveNodes();
//  par->printParams();
//  fSensNodes->Dump();
//  fPassNodes->Dump();

  // get detector position:
  FairGeoNode *det= (FairGeoNode *) fSensNodes->FindObject("rich1d#1");
  FairGeoTransform* detTr=det->getLabTransform();  // detector position in labsystem
  FairGeoVector detPosLab=detTr->getTranslation(); // ... in cm
  FairGeoTransform detCen=det->getCenterPosition();  // center in Detector system
  FairGeoVector detPosCen=detCen.getTranslation();
  fDetZ = detPosLab.Z() + detPosCen.Z();   /** z coordinate of photodetector (Labsystem, cm) */
  fDetY = detPosLab.Y() + detPosCen.Y();   /** y coordinate of photodetector (Labsystem, cm) */
  fDetX = detPosLab.X() + detPosCen.X();   /** x coordinate of photodetector (Labsystem, cm) */

  TArrayD *fdetA=det->getParameters();  // get other geometry parameters: width in x, width in y, thickness
  fDetWidthX = fdetA->At(0);
  fDetWidthY = fdetA->At(1);
//  for(Int_t i=0;i<fdetA->GetSize();i++) cout << "Array detector " << fdetA->At(i)<< endl;
  // detector might be rotated by theta around x-axis:
  FairGeoRotation fdetR=detTr->getRotMatrix();

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
  fThetaDet = TMath::ASin(fdetR(7));
  // phi = tilting angle around y-axis
  fPhiDet = -1.*TMath::ASin(fdetR(2));


  if (fVerbose) {
    cout << "---------------------- RICH Projection Producer ---------------------------------------" << endl;
    cout << "   detector position in (x,y,z): " << fDetX << "  " << fDetY << "  " << fDetZ << endl;
    cout << "   detector size in x and y: " << fDetWidthX << "  " << fDetWidthY << endl;
    cout << "   detector tilting angle (around x): " << fThetaDet*180./TMath::Pi() << " degrees" << endl;
    cout << "   detector tilting angle (around y): " << fPhiDet*180./TMath::Pi() << " degrees" << endl;
  }
  
    // transform nominal detector position (for tilted photodetector):
    // shift x back by fDetZ_org*TMath::Sin(phi) in order to avoid overlap
    fDetX_transf = fDetX*TMath::Cos(fPhiDet)+fDetZ*TMath::Sin(fPhiDet)-fDetZ*TMath::Sin(fPhiDet);
    fDetY_transf = -fDetX*TMath::Sin(fThetaDet)*TMath::Sin(fPhiDet) + fDetY*TMath::Cos(fThetaDet) + fDetZ*TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet);
    fDetZ_transf = -fDetX*TMath::Cos(fThetaDet)*TMath::Sin(fPhiDet) - fDetY*TMath::Sin(fThetaDet) + fDetZ*TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet);
  

  // get mirror position:
  //FairGeoNode *mir= (FairGeoNode *) fPassNodes->FindObject("rich1mgl#1");
  FairGeoNode *mir= (FairGeoNode *) fSensNodes->FindObject("rich1mgl#1");
  FairGeoTransform* mirTr=mir->getLabTransform();  // position of mirror center in labsystem
  FairGeoVector mirPosLab=mirTr->getTranslation(); // ... in cm
  fZm = mirPosLab.Z();
  fYm = mirPosLab.Y();
  fXm = mirPosLab.X();

  TArrayD *fmirA=mir->getParameters();  // get other geometry parameters: radius,
  fR = fmirA->At(0);                    // mirror radius
  fSPHE_theta = TMath::Abs(90. - fmirA->At(2));   // opening angle for SPHERE in theta (90 degree +- theta)
  fSPHE_phi = TMath::Abs(90. - fmirA->At(4));   // opening angle for SPHERE in phi (90 degree +- phi)
  // from that calculate (with safety factor 1.3) maximum x-y positions for track extrapolation:
  fmax_x = 1.3*(fR*TMath::Tan(fSPHE_phi*TMath::Pi()/180.));
  fmax_y = 1.3*(TMath::Abs(fYm) + fR*TMath::Tan(fSPHE_theta*TMath::Pi()/180.));
  if (fVerbose) for(Int_t i=0;i<fmirA->GetSize();i++) cout << "Array mirror " << fmirA->At(i) << endl;
  
  // mirror might be rotated by theta around x-axis:
  FairGeoRotation fmirR=mirTr->getRotMatrix();
  fThetaM = -1.*TMath::ASin(fmirR(5)) - TMath::Pi()/2 ;
  if (fVerbose) {
    cout << "Rotation matrix of mirror" << endl;
    for (Int_t i=0;i<9;i++) cout << "Rot(" << i << ") = " << fmirR(i) << endl;
    }
  // note that mirror is by default tilted by 90 degrees in order to get the necessary shape in GEANT
  // the "extra" tilting angle is then: fThetaM =  -1.*TMath::ASin(fmirR(5)) - TMath::Pi()/2.

  if (fVerbose){
    cout << "   mirror center (x,y,z): " << fXm << "  " << fYm << "  " << fZm << endl;
    cout << "   mirror radius: " << fR << endl;
    cout << "   mirror tilting angle: " << fThetaM*180./TMath::Pi() << " degrees" << endl;
    cout << "   maximum x-y positions for track extrapolation: " << fmax_x << " " << fmax_y << endl;
    }

  fEvent = 0;


  fListRICHImPlanePoint = (TClonesArray*)fManager->GetObject("RichTrackParamZ"); // imaginary plane in Rich detector for track extrapolation
  if (fVerbose && fZflag == 1) cout << "   use tracks in imaginary plane for projection to photodetector plane" << endl;
  if (fVerbose && fZflag == 2) cout << "   use tracks in RICH mirror for projection to photodetector plane" << endl;
  cout << "--------------------------------------------------------------------------------" << endl;
  
  if ( ! fListRICHImPlanePoint) {
    cout << "-W- CbmRichProjectionProducer::Init: No Rich Z-Point array!"
	 << endl;
	 }


  fListStack = (TClonesArray*)fManager->GetObject("MCTrack"); //   for checking workability

  fProjectionTrackParam = new TClonesArray("FairTrackParam"); // coordinates of projected tracks to photodetectors plane
  fManager->Register("RichProjection","RICH",fProjectionTrackParam, kTRUE);


  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Execution of Task   ---------------------------------------------
void CbmRichProjectionProducer::Exec(Option_t* option)
{

  fEvent++;
  printf("\n\n=====> Event no. %d\n",fEvent);

  fProjectionTrackParam->Clear();

  FairTrackParam* point=NULL;
 
   // some default variables
   TMatrixFSym covMat(5);
   for(Int_t i=0;i<5;i++) for(Int_t j=0; j<=i; j++) covMat(i,j) = 0;
   covMat(0,0) = covMat(1,1) = covMat(2,2) = covMat(3,3) = covMat(4,4) = 1.e-4; 

  Double_t rho1,rho2;
  Double_t xX;
  Double_t yY;
  Double_t zZ;
  Double_t normP2,RxP,dist;
  Double_t crossPx,crossPy,crossPz;
  
  if (fVerbose > 1) cout << "Number of tracks in Imaginary z-Plane: " << fListRICHImPlanePoint->GetEntriesFast() << endl;

  for(Int_t j=0; j<fListRICHImPlanePoint->GetEntriesFast(); j++) {
    point = (FairTrackParam*)fListRICHImPlanePoint->At(j);
    new((*fProjectionTrackParam)[j]) FairTrackParam(0.,0.,0.,0.,0.,0.,covMat);
    
    if (fVerbose > 1) cout << "Data in Imaginary z-Plane: " <<  j << " " <<
        point->GetX() << " " << point->GetY() << " " << point->GetZ() << " " <<
	point->GetTx() << " " << point->GetTy() << " " << point->GetQp() << endl;

    // check if Array was filled
    if (point->GetX() == 0 && point->GetY() == 0 && point->GetZ() == 0 &&
        point->GetTx() == 0 && point->GetTy() ==0) continue; 
    if (point->GetQp()==0) continue;
    
    // check that x and y value make sense (sometimes strange extrapolations may appear)
    if (TMath::Abs(point->GetX()) > fmax_x || TMath::Abs(point->GetY()) > fmax_y){
     if (fVerbose) cout << " -W- RichProjectionProducer: strange (x,y) values for track extrapolation: " <<  
                   point->GetX() << " " << point->GetY() << endl;
		   continue;
		   }

    rho1 = rho2 = 0.;
    xX=9999.;
    yY=9999.;
    zZ=0.;
    normP2 = RxP = dist = 0.;

    TVector3 startP, momP, crossP, centerP;

    // operate on ImPlane point
    //-------------------------------------------------------------------------------------------------
    if (fZflag ==1) {
      Double_t p = 1./TMath::Abs(point->GetQp());
      Double_t pz;
      if  ((1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy()) > 0. )
       pz = p/TMath::Sqrt(1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy());
      else {
       cout << " -E- RichProjectionProducer: strange value for calculating pz: " << 
               (1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy()) << endl;
       pz = 0.;
       }
      Double_t px = pz*point->GetTx();
      Double_t py = pz*point->GetTy();
      momP.SetXYZ(px,py,pz);
      point->Position(startP);
      if ((fYm*startP.y())<0) fYm = -fYm; // check that mirror center and startP are in same hemisphere
      
      if (fVerbose > 1) cout << " --> position: " << startP.x() << " " << startP.y() << " " << startP.z() << endl;
      if (fVerbose > 1) cout << " --> momentum: " << momP.x() << " " << momP.y() << " " << momP.z() << endl;

    // calculation of intersection of track with selected mirror
    // corresponds to calculation of intersection between a straight line and a sphere:
    // vector: r = startP - mirrorCenter
    // RxP = r*momP
    // normP2 = momP^2
    // dist = r^2 - fR^2
    // -> rho1 = (-RxP+sqrt(RxP^2-normP2*dist))/normP2  extrapolation factor for:
    // intersection point crossP = startP + rho1 * momP

    RxP=(momP.x()*(startP.x()-fXm)+momP.y()*(startP.y()-fYm)+momP.z()*(startP.z()-fZm));
    normP2=(momP.x()*momP.x()+momP.y()*momP.y()+momP.z()*momP.z());
    dist=(startP.x()*startP.x()+fXm*fXm+startP.y()*startP.y()+fYm*fYm+startP.z()*startP.z()+fZm*fZm-2*startP.x()*fXm-2*startP.y()*fYm-2*startP.z()*fZm-fR*fR);

    if ((RxP*RxP-normP2*dist) > 0.) {
    if (normP2!=0.)  rho1=(-RxP+TMath::Sqrt(RxP*RxP-normP2*dist))/normP2;
    if (normP2 == 0) cout << " Error in track extrapolation: momentum = 0 " << endl;
    }
    else cout << " -E- RichProjectionProducer:  RxP*RxP-normP2*dist = " << RxP*RxP-normP2*dist << endl;

    crossPx=startP.x()+rho1*momP.x();
    crossPy=startP.y()+rho1*momP.y();
    crossPz=startP.z()+rho1*momP.z();
    crossP.SetXYZ(crossPx,crossPy,crossPz);

    // check if crosspoint with mirror and chosen mirrorcenter (y) are in same hemisphere
    // if not recalculate crossing point
    if ((fYm*crossP.y())<0) {
      fYm = -fYm;
      RxP=(momP.x()*(startP.x()-fXm)+momP.y()*(startP.y()-fYm)+momP.z()*(startP.z()-fZm));
      normP2=(momP.x()*momP.x()+momP.y()*momP.y()+momP.z()*momP.z());
      dist=(startP.x()*startP.x()+fXm*fXm+startP.y()*startP.y()+fYm*fYm+startP.z()*startP.z()+fZm*fZm-2*startP.x()*fXm-2*startP.y()*fYm-2*startP.z()*fZm-fR*fR);

      if ((RxP*RxP-normP2*dist) > 0.) {
      if (normP2!=0.)  rho1=(-RxP+TMath::Sqrt(RxP*RxP-normP2*dist))/normP2;
      if (normP2 == 0) cout << " Error in track extrapolation: momentum = 0 " << endl;
      }
      else cout << " -E- RichProjectionProducer:  RxP*RxP-normP2*dist = " << RxP*RxP-normP2*dist << endl;

      crossPx=startP.x()+rho1*momP.x();
      crossPy=startP.y()+rho1*momP.y();
      crossPz=startP.z()+rho1*momP.z();
      crossP.SetXYZ(crossPx,crossPy,crossPz);
     }

     centerP.SetXYZ(fXm,fYm,fZm);    // mirror center
    }

    // operate on Rich Mirror point
    //---------------------------------------------------------------------------------------------------------
    if (fZflag ==2) {
      Double_t p = 1./TMath::Abs(point->GetQp());
      Double_t pz;
      if  ((1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy()) > 0. )
       pz = p/TMath::Sqrt(1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy());
      else {
       cout << " -E- RichProjectionProducer: strange value for calculating pz: " << 
               (1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy()) << endl;
       pz = 0.;
       }
      Double_t px = pz*point->GetTx();
      Double_t py = pz*point->GetTy();
      momP.SetXYZ(px,py,pz);
      point->Position(crossP);
      if ((fYm*crossP.y())<0) fYm = -fYm; // check that mirror center and crossP are in same hemisphere

      centerP.SetXYZ(fXm,fYm,fZm);    // mirror center
    }

    //   calculate normal on crosspoint with mirror
    TVector3 normP(crossP.x()-centerP.x(),crossP.y()-centerP.y(),crossP.z()-centerP.z());
    normP=normP.Unit();
    // check that normal has same z-direction as momentum
    if ((normP.z()*momP.z())<0.) normP = TVector3(-1.*normP.x(),-1.*normP.y(),-1.*normP.z());


    // reflect track
    Double_t np=normP.x()*momP.x()+normP.y()*momP.y()+normP.z()*momP.z();

    Double_t refX = 2*np*normP.x()-momP.x();
    Double_t refY = 2*np*normP.y()-momP.y();
    Double_t refZ = 2*np*normP.z()-momP.z();

    // crosspoint whith photodetector plane:
    // calculate intersection between straight line and (tilted) plane:
    // normal on plane tilted by theta around x-axis: (0,-sin(theta),cos(theta)) = n
    // normal on plane tilted by phi around y-axis: (-sin(phi),0,cos(phi)) = n
    // normal on plane tilted by theta around x-axis and phi around y-axis: (-sin(phi),-sin(theta)cos(phi),cos(theta)cos(phi)) = n
    // point on plane is (fDetX,fDetY,fDetZ) = p as photodetector is tiled around its center
    // equation of plane for r being point in plane: n(r-p) = 0
    // calculate intersection point of reflected track with plane: r=intersection point 
    // intersection point = crossP + rho2 * refl_track
    // take care for all 4 cases:
    //        -> first calculate for case x>0, then check
    if (refZ!=0.) {
      if (centerP.y() > 0){
      rho2 = (-TMath::Sin(fPhiDet)*(fDetX-crossP.x())
              -TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet)*(fDetY-crossP.y()) 
	      + TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet)*(fDetZ-crossP.z()))/
              (-TMath::Sin(fPhiDet)*refX-TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet)*refY + TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet)*refZ);
	      }
      if (centerP.y() < 0){
      rho2 = (-TMath::Sin(fPhiDet)*(fDetX-crossP.x())
              -TMath::Sin(-fThetaDet)*TMath::Cos(fPhiDet)*(-fDetY-crossP.y()) 
	      + TMath::Cos(-fThetaDet)*TMath::Cos(fPhiDet)*(fDetZ-crossP.z()))/
              (-TMath::Sin(fPhiDet)*refX-TMath::Sin(-fThetaDet)*TMath::Cos(fPhiDet)*refY + TMath::Cos(-fThetaDet)*TMath::Cos(fPhiDet)*refZ);
	      }
	      
 //     rho2 = -1*(crossP.z() - fDetZ)/refZ;    // only for theta = 0, phi=0
      xX = crossP.x() + refX * rho2;
      yY = crossP.y() + refY * rho2;
      zZ = crossP.z() + refZ * rho2;
      
      if (xX < 0) {
      if (centerP.y() > 0){
      rho2 = (-TMath::Sin(-fPhiDet)*(-fDetX-crossP.x())
              -TMath::Sin(fThetaDet)*TMath::Cos(-fPhiDet)*(fDetY-crossP.y()) 
	      + TMath::Cos(fThetaDet)*TMath::Cos(-fPhiDet)*(fDetZ-crossP.z()))/
              (-TMath::Sin(-fPhiDet)*refX-TMath::Sin(fThetaDet)*TMath::Cos(-fPhiDet)*refY + TMath::Cos(fThetaDet)*TMath::Cos(-fPhiDet)*refZ);
	      }
      if (centerP.y() < 0){
      rho2 = (-TMath::Sin(-fPhiDet)*(-fDetX-crossP.x())
              -TMath::Sin(-fThetaDet)*TMath::Cos(-fPhiDet)*(-fDetY-crossP.y()) 
	      + TMath::Cos(-fThetaDet)*TMath::Cos(-fPhiDet)*(fDetZ-crossP.z()))/
              (-TMath::Sin(-fPhiDet)*refX-TMath::Sin(-fThetaDet)*TMath::Cos(-fPhiDet)*refY + TMath::Cos(-fThetaDet)*TMath::Cos(-fPhiDet)*refZ);
	      }
       
      xX = crossP.x() + refX * rho2;
      yY = crossP.y() + refY * rho2;
      zZ = crossP.z() + refZ * rho2;
      }
      
      
      // Transform intersection point in same way as MCPoints were transformed in HitProducer before stored as Hit:
      Double_t xDet,yDet,zDet;
      xDet = xX;
      
    
    // tilt points by 
    // -theta, -phi for x>0, y>0 
    //  theta, -phi for x>0, y<0 
    //  theta,  phi for x<0, y<0 
    // -theta,  phi for x<0, y>0 
    // and shift x position in order to avoid overlap
    //-----------------------------------------------
     if (xX > 0 && yY > 0 ) {
       xDet = xX*TMath::Cos(fPhiDet) + zZ*TMath::Sin(fPhiDet) - fDetZ*TMath::Sin(fPhiDet);
       yDet = -xX*TMath::Sin(fThetaDet)*TMath::Sin(fPhiDet) + yY*TMath::Cos(fThetaDet) + zZ*TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet);
       zDet = -xX*TMath::Cos(fThetaDet)*TMath::Sin(fPhiDet) - yY*TMath::Sin(fThetaDet) + zZ*TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet);
       }
     if (xX > 0 && yY < 0 ) {
       xDet = xX*TMath::Cos(fPhiDet) + zZ*TMath::Sin(fPhiDet) - fDetZ*TMath::Sin(fPhiDet);
       yDet = xX*TMath::Sin(fThetaDet)*TMath::Sin(fPhiDet) + yY*TMath::Cos(fThetaDet) - zZ*TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet);
       zDet = -xX*TMath::Cos(fThetaDet)*TMath::Sin(fPhiDet) + yY*TMath::Sin(fThetaDet) + zZ*TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet);
       }
     if (xX < 0 && yY < 0 ) {
       xDet = xX*TMath::Cos(fPhiDet) - zZ*TMath::Sin(fPhiDet) + fDetZ*TMath::Sin(fPhiDet);
       yDet = -xX*TMath::Sin(fThetaDet)*TMath::Sin(fPhiDet) + yY*TMath::Cos(fThetaDet) - zZ*TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet);
       zDet = xX*TMath::Cos(fThetaDet)*TMath::Sin(fPhiDet) + yY*TMath::Sin(fThetaDet) + zZ*TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet);
       }
     if (xX < 0 && yY > 0 ) {
       xDet = xX*TMath::Cos(fPhiDet) - zZ*TMath::Sin(fPhiDet) + fDetZ*TMath::Sin(fPhiDet);
       yDet = xX*TMath::Sin(fThetaDet)*TMath::Sin(fPhiDet) + yY*TMath::Cos(fThetaDet) + zZ*TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet);
       zDet = xX*TMath::Cos(fThetaDet)*TMath::Sin(fPhiDet) - yY*TMath::Sin(fThetaDet) + zZ*TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet);
       }
       
    
      //check that crosspoint inside the plane
      if( xDet > (-fDetX-fDetWidthX) && xDet < (fDetX+fDetWidthX)) 
        { 
	if(TMath::Abs(yDet) > (fDetY_transf-fDetWidthY) && 
	   TMath::Abs(yDet) < (fDetY_transf+fDetWidthY)) 
	   {
	   
            FairTrackParam richtrack(xDet,yDet,zDet,0.,0.,0.,covMat);
            * (FairTrackParam*)(fProjectionTrackParam->At(j)) = richtrack;

	    if (fVerbose > 1) cout << "-I- RichProjectionProducer: track extrapolated to photodetector plane: iTrack "
	                           << j << endl;

	}
      }
    }

  }
  
}
// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void CbmRichProjectionProducer::Finish()
{
  
  fProjectionTrackParam->Clear();
  fListRICHImPlanePoint->Clear();
  fListStack->Clear();
}
// -------------------------------------------------------------------------



ClassImp(CbmRichProjectionProducer)
