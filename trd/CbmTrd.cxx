#include "CbmTrd.h"

#include "CbmGeoTrdPar.h"
#include "CbmTrdPoint.h"
#include "CbmGeoTrd.h"
#include "CbmTrdGeoHandler.h"
#include "CbmStack.h"

//#include "CbmDetectorList.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"
#include "FairLogger.h"

#include "TObjArray.h"
#include "TClonesArray.h"
//#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
//#include "TGeoMaterial.h"
// for CBM TRD support structure
#include "TGeoXtru.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrd::CbmTrd() 
  : FairDetector("TRD", kTRUE, kTRD),
    fTrackID(0),
    fVolumeID(0),
    fPosIn(),
    fMomIn(),
    fPosOut(),
    fMomOut(),
    fTime(0),
    fLength(0),
    fELoss(0),
    fPosIndex(0),
    fTrdCollection(new TClonesArray("CbmTrdPoint")),
    fGeoHandler(new CbmTrdGeoHandler())
{
  fVerboseLevel = 1;
}


// -----   Standard constructor   ------------------------------------------
CbmTrd::CbmTrd(const char* name, Bool_t active)
  : FairDetector(name, active, kTRD),
    fTrackID(0),
    fVolumeID(0),
    fPosIn(),
    fMomIn(),
    fPosOut(),
    fMomOut(),
    fTime(0),
    fLength(0),
    fELoss(0),
    fPosIndex(0),
    fTrdCollection(new TClonesArray("CbmTrdPoint")),
    fGeoHandler(new CbmTrdGeoHandler())
{
  fVerboseLevel = 1;
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrd::~CbmTrd() {
 if (fTrdCollection) {
   fTrdCollection->Delete();
   delete fTrdCollection;
 }
 if (fGeoHandler) {
   delete fGeoHandler;
 }
}
// -------------------------------------------------------------------------
void CbmTrd::Initialize()
{
  FairDetector::Initialize();
 
  // Initialize the CbmTrdGeoHandler helper class from the
  // TVirtualMC interface
  fGeoHandler->Init();
  
}

//*************************************************************************

void CbmTrd::SetSpecialPhysicsCuts(){
    FairRun* fRun = FairRun::Instance();
 
    // Setting the properties for the TRDgas is only tested
    // with TGeant3, so we check for the simulation engine
    // and change the standard properties only for TGeant3
    if (strcmp(fRun->GetName(),"TGeant3") == 0) {

      // Get Material Id and some material properties from
      // the geomanager
      Int_t mat = gGeoManager->GetMaterialIndex("TRDgas");
      TGeoMaterial *trdgas =  gGeoManager->GetMaterial(mat);
      Double_t mass = trdgas->GetA();
      Double_t charge = trdgas->GetZ();
     
      // Get the material properties for material with id+1
      // (of-by-one problem) from the Virtual Monte Carlo
      Int_t matIdVMC = mat+1;
      char name[20];
      Double_t a[1]={0.};
      Double_t z[1]={0.};
      Double_t dens[1]={0.};
      Double_t radl[1]={0.};
      Double_t absl[1]={0.};
      Double_t *ubuf;
      Int_t nbuf[1]={0};
      gMC->Gfmate(matIdVMC,name, *a, *z, *dens, *radl, *absl, ubuf, *nbuf);

      // Check if the material properties are the same for TGeoManager
      // and TVirtualMC. If not stop the execution of the simulation.
      // This is done to stop the program whenever the
      // of-by-one problem is fixed in the Virtual Monte Carlo
      if ( ( TMath::Abs(mass - a[0]) > 0.0001 ) ||  
	   ( TMath::Abs(charge - z[0]) ) > 0.0001 ) {
	cout<<"**********************************"<<endl;
        cout<<TMath::Abs(mass - a[0])<<" , "<<TMath::Abs(charge - z[0])<<endl;
	cout <<"Parameters from Geomanager:"<<endl;
	trdgas->Print();
	cout <<"Parameters from Virtual Montecarlo:"<<endl;
	cout <<"Name "<<name<<" Aeff="<<a[0]<<" Zeff="<<z[0]<<endl;
	Fatal("SetSpecialPhysicsCuts","Material parameters different between TVirtualMC and TGeomanager");
      }

      // Set new properties, physics cuts etc. for the TRDgas
      gMC->Gstpar(matIdVMC,"STRA",1.0);
      gMC->Gstpar(matIdVMC,"PAIR",1.0);
      gMC->Gstpar(matIdVMC,"COMP",1.0);
      gMC->Gstpar(matIdVMC,"PHOT",1.0);
      gMC->Gstpar(matIdVMC,"LOSS",2.0);
      //gMC->Gstpar(matIdVMC,"PFIS",0.0);
      gMC->Gstpar(matIdVMC,"ANNI",1.0);
      gMC->Gstpar(matIdVMC,"BREM",1.0);
      gMC->Gstpar(matIdVMC,"HADR",1.0);
      //gMC->Gstpar(matIdVMC,"MUNU",1.0);
      gMC->Gstpar(matIdVMC,"DCAY",1.0);
      gMC->Gstpar(matIdVMC,"MULS",1.0);
      //gMC->Gstpar(matIdVMC,"LABS",0.0);
      gMC->Gstpar(matIdVMC,"DRAY",1.0);
      gMC->Gstpar(matIdVMC,"RAYL",1.0);
      gMC->Gstpar(matIdVMC,"CUTELE",10.e-6);
      gMC->Gstpar(matIdVMC,"CUTGAM",10.e-6);
      gMC->Gstpar(matIdVMC,"CUTNEU",10.e-4);
      gMC->Gstpar(matIdVMC,"CUTHAD",10.e-4);
      gMC->Gstpar(matIdVMC,"CUTMUO",10.e-6);
      gMC->Gstpar(matIdVMC,"BCUTE",10.e-6);
      gMC->Gstpar(matIdVMC,"BCUTM",10.e-6);
      gMC->Gstpar(matIdVMC,"DCUTE",10.e-6);
      gMC->Gstpar(matIdVMC,"DCUTM",10.e-6);
      gMC->Gstpar(matIdVMC,"PPCUTM",-1.);
    }
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmTrd::ProcessHits(FairVolume* vol)
{


    // Set parameters at entrance of volume. Reset ELoss.
    if ( gMC->IsTrackEntering() ) {
	fELoss   = 0.;
	fTime    = gMC->TrackTime() * 1.0e09;
	fLength  = gMC->TrackLength();
	gMC->TrackPosition(fPosIn);
	gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();



    // Create CbmTrdPoint at exit of active volume
    if ( gMC->IsTrackExiting()    ||
	 gMC->IsTrackStop()       ||
	 gMC->IsTrackDisappeared()   ) 
    {
      
      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      
      if (fELoss == 0. ) return kFALSE;  // no neutrals
      
      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    
      fVolumeID = fGeoHandler->GetUniqueDetectorId();

      CbmTrdPoint *fPoint= AddHit(fTrackID, fVolumeID, 
			    TVector3(fPosIn.X(),  fPosIn.Y(),  fPosIn.Z()),
	       		    TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
                            TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
			    TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
                            fTime, fLength, fELoss);

      // Increment number of trd points in TParticle
      CbmStack* stack = (CbmStack*) gMC->GetStack();
      stack->AddPoint(kTRD);
      
      ResetParameters();
    }
    
    return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void CbmTrd::EndOfEvent() {
  if (fVerboseLevel) Print();
  fTrdCollection->Clear();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Public method Register   ----------------------------------------
void CbmTrd::Register() {
  FairRootManager::Instance()->Register("TrdPoint", "Trd", fTrdCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* CbmTrd::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fTrdCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmTrd::Print() const {
  Int_t nHits = fTrdCollection->GetEntriesFast();
  cout << "-I- CbmTrd: " << nHits << " points registered in this event."
       << endl;
  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++) (*fTrdCollection)[i]->Print();
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmTrd::Reset() {
  fTrdCollection->Clear();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void CbmTrd::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset){
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmTrd: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmTrdPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (CbmTrdPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmTrdPoint(*oldpoint);
    fPosIndex++;
  }
  cout << "-I- CbmTrd: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// -------------------------------------------------------------------------

void CbmTrd::ConstructGeometry()
{
  TString fileName=GetGeometryFileName();
  if (fileName.EndsWith(".geo")) {	
    ConstructASCIIGeometry();
  } else if (fileName.EndsWith(".root")) {
    ConstructRootGeometry();
  } else {
    std::cout << "Geometry format not supported." << std::endl;
  }
}


// -----   Public method ConstructGeometry   -------------------------------
void CbmTrd::ConstructASCIIGeometry() {

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmGeoTrd*        trdGeo  = new CbmGeoTrd();
  trdGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(trdGeo);

  Bool_t rc = geoFace->readSet(trdGeo);
  if (rc) trdGeo->create(geoLoad->getGeoBuilder());
  TList* volList = trdGeo->getListOfVolumes();

  // store TRD geo parameter
  FairRun *fRun       = FairRun::Instance();
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  CbmGeoTrdPar* par   = (CbmGeoTrdPar*)(rtdb->getContainer("CbmGeoTrdPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol = NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
       if ( node->isSensitive()  ) {
           fSensNodes->AddLast( aVol );
       }else{
           fPassNodes->AddLast( aVol );
       }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  ProcessNodes( volList );

// // add structures a la RICH
// TGeoMaterial * matAl = new TGeoMaterial("Al", 26.98, 13, 2.7);
// TGeoMedium * Al      = new TGeoMedium("Al",1, matAl);
// TGeoVolume * volume  = gGeoManager->MakeTube("grid", Al, 1.3, 1.5, 180);
// 
// gGeoManager->Matrix(123456, 180,   0,  90,  90,  90,   0); // z rotation
// gGeoManager->Matrix(123457,  90,   0, 180,   0,  90,  90); // y rotation
//  
// Double_t * buf = 0;
// for (Int_t i = 0; i< 11; i++) {
//   if (i == 5) continue;
//   gGeoManager->Node("grid", 2*i+1, "trd1", 36*i - 180,              350, 40, 123457, kTRUE, buf, 0);
//   gGeoManager->Node("grid", 2*i+2, "trd1", 0         , 36*i - 180 + 350, 48, 123456, kTRUE, buf, 0);
// }

/*
// // add TRD support structure
// 
   TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98, 13, 2.7);
   TGeoMedium *Al      = new TGeoMedium("Al", 1, matAl);
 //  TGeoRotation  *rotx090 = new TGeoRotation("rotx090"); rotx090->RotateX(90.); // rotate  90 deg around x-axis
 //  TGeoRotation  *roty090 = new TGeoRotation("roty090"); roty090->RotateY(90.); // rotate  90 deg around y-axis
 
   TGeoVolume *vol1 = gGeoManager->MakeXtru("hbar1", Al, 2);
   TGeoXtru *hbar1 = (TGeoXtru*)vol1->GetShape();
   //   vol1->SetLineColor(kRed);
 
   TGeoVolume *vol2 = gGeoManager->MakeXtru("hbar2", Al, 2);
   TGeoXtru *hbar2 = (TGeoXtru*)vol2->GetShape();
 
   TGeoVolume *vol3 = gGeoManager->MakeXtru("hbar3", Al, 2);
   TGeoXtru *hbar3 = (TGeoXtru*)vol3->GetShape();
 
   TGeoVolume *vol4 = gGeoManager->MakeXtru("hbar4", Al, 2);
   TGeoXtru *hbar4 = (TGeoXtru*)vol4->GetShape();
 
   TGeoVolume *vol5 = gGeoManager->MakeXtru("hbar5", Al, 2);
   TGeoXtru *hbar5 = (TGeoXtru*)vol5->GetShape();
 
   TGeoVolume *vol6 = gGeoManager->MakeXtru("hbar6", Al, 2);
   TGeoXtru *hbar6 = (TGeoXtru*)vol6->GetShape();
 
   Double_t x[12] = { -10,-10, -1, -1,-10,-10, 10, 10,  1,  1, 10, 10 };
   Double_t y[12] = { -10, -8, -8,  8,  8, 10, 10,  8,  8, -8, -8,-10 };
 
   hbar1->DefinePolygon(12,x,y);
   hbar1->DefineSection( 0,-450, 0, 0, 1.0);
   hbar1->DefineSection( 1, 450, 0, 0, 1.0);
 
   hbar2->DefinePolygon(12,x,y);
   hbar2->DefineSection( 0,-340, 0, 0, 1.0);
   hbar2->DefineSection( 1, 540, 0, 0, 1.0);
 
   hbar3->DefinePolygon(12,x,y);
   hbar3->DefineSection( 0,-550, 0, 0, 1.0);
   hbar3->DefineSection( 1, 550, 0, 0, 1.0);
 
   hbar4->DefinePolygon(12,x,y);
   hbar4->DefineSection( 0,-440, 0, 0, 1.0);
   hbar4->DefineSection( 1, 540, 0, 0, 1.0);
 
   hbar5->DefinePolygon(12,x,y);
   hbar5->DefineSection( 0,-650, 0, 0, 1.0);
   hbar5->DefineSection( 1, 650, 0, 0, 1.0);
 
   hbar6->DefinePolygon(12,x,y);
   hbar6->DefineSection( 0,-540, 0, 0, 1.0);
   hbar6->DefineSection( 1, 540, 0, 0, 1.0);
 
   // matrices are not tested
   gGeoManager->Matrix(123466,  90,  0,  0,   0, 90, 270); // x rotation
   gGeoManager->Matrix(123467, 180,  0, 90,  90, 90,   0); // y rotation
   gGeoManager->Matrix(123468,  90, 90, 90, 180,  0,   0); // z rotation
 
   Double_t *buf = 0;
//   gGeoManager->Node("vol1", 1, "trd1", 0, 300, 0, 123467, kTRUE, buf, 0);
//   gGeoManager->Node("vol1", 1, "trd1", 0, 300, 0, 123457, kTRUE, buf, 0);

// trd1
   gGeoManager->Node("hbar1", 1, "trd1",  0, 350, -75, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar2", 5, "trd1", -440, 0, -75, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar2", 6, "trd1",  440, 0, -75, 123466, kTRUE, buf, 0);

   gGeoManager->Node("hbar1", 2, "trd1",  0, 350, -25, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar2", 5, "trd1", -440, 0, -25, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar2", 6, "trd1",  440, 0, -25, 123466, kTRUE, buf, 0);

   gGeoManager->Node("hbar1", 3, "trd1",  0, 350,  25, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar2", 5, "trd1", -440, 0,  25, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar2", 6, "trd1",  440, 0,  25, 123466, kTRUE, buf, 0);

   gGeoManager->Node("hbar1", 4, "trd1",  0, 350,  75, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar2", 5, "trd1", -440, 0,  75, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar2", 6, "trd1",  440, 0,  75, 123466, kTRUE, buf, 0);

// trd2
   gGeoManager->Node("hbar3", 7, "trd2",  0, 450, -75, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar4", 8, "trd2", -540, 0, -75, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar4", 9, "trd2",  540, 0, -75, 123466, kTRUE, buf, 0);
						     
   gGeoManager->Node("hbar3",10, "trd2",  0, 450, -25, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar4",11, "trd2", -540, 0, -25, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar4",12, "trd2",  540, 0, -25, 123466, kTRUE, buf, 0);
						     
   gGeoManager->Node("hbar3",13, "trd2",  0, 450,  25, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar4",14, "trd2", -540, 0,  25, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar4",15, "trd2",  540, 0,  25, 123466, kTRUE, buf, 0);
						     
   gGeoManager->Node("hbar3",16, "trd2",  0, 450,  75, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar4",17, "trd2", -540, 0,  75, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar4",18, "trd2",  540, 0,  75, 123466, kTRUE, buf, 0);

// trd3
   gGeoManager->Node("hbar5",19, "trd3",  0, 550, -75, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar6",20, "trd3", -640, 0, -75, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar6",21, "trd3",  640, 0, -75, 123466, kTRUE, buf, 0);

   gGeoManager->Node("hbar5",22, "trd3",  0, 550, -25, 123467, kTRUE, buf, 0);
   gGeoManager->Node("hbar6",23, "trd3", -640, 0, -25, 123466, kTRUE, buf, 0);
   gGeoManager->Node("hbar6",24, "trd3",  640, 0, -25, 123466, kTRUE, buf, 0);
*/
}
// -------------------------------------------------------------------------

Bool_t CbmTrd::CheckIfSensitive(std::string name)
{
  TString tsname = name; 
  if (tsname.Contains("gas")){
    fLogger->Info(MESSAGE_ORIGIN, "*** Register %s as active volume.",tsname.Data());
    return kTRUE;
  }
  return kFALSE;
}

// -----   Private method AddHit   -----------------------------------------
CbmTrdPoint* CbmTrd::AddHit(
      Int_t trackID,
      Int_t detID, 
      TVector3 posIn,
      TVector3 momIn, 
      TVector3 posOut, 
      TVector3 momOut, 
      Double_t time, 
      Double_t length, 
      Double_t eLoss) {
  TClonesArray& clref = *fTrdCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmTrdPoint(trackID, detID, posIn, momIn, posOut,
				      momOut, time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(CbmTrd)
