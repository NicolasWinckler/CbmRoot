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
    fGeoVersion(0),
    fTrdCollection(new TClonesArray("CbmTrdPoint")),
    fGeoHandler(),
    fStationId(),
    fModuleId()
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
    fGeoVersion(0),
    fTrdCollection(new TClonesArray("CbmTrdPoint")),
    fGeoHandler(),
    fStationId(),
    fModuleId()
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
  
  FairLogger *logger = FairLogger::GetLogger();
  const char* bla = "DEBUG";
  int number =1;

  logger->Debug4(MESSAGE_ORIGIN, "Dies ist ein %s Test", bla);
  logger->Debug3(MESSAGE_ORIGIN, "Dies ist ein DEBUG3 Test");
  logger->Debug2(MESSAGE_ORIGIN, "Dies ist ein DEBUG2 Test");
  logger->Debug1(MESSAGE_ORIGIN, "Dies ist ein DEBUG1 Test");
  logger->Debug(MESSAGE_ORIGIN, "Dies ist ein DEBUG Test");
  logger->Info(MESSAGE_ORIGIN, "Dies ist ein INFO Test");
  logger->Warning(MESSAGE_ORIGIN, "Dies ist ein WARNING Test");
  logger->Error(MESSAGE_ORIGIN, "Dies ist ein '%d' Test", 1);
  logger->Error(MESSAGE_ORIGIN, "Dies ist ein '%d' Test", number);
  logger->Error(MESSAGE_ORIGIN, "Dies ist ein %s Test\n", bla);
  logger->Error(MESSAGE_ORIGIN,"\033[5m\033[31m invalid time function, Event time is not SET \033[0m");

  //  logger->Fatal(MESSAGE_ORIGIN, "Dies ist ein FATAL Test");

  // Extract geometry information from gGeoManager instead of
  // CbmGeoTrdPar. All such geometry handling is done now in the
  //separate utility class CbmTrdGeoHandler

  fGeoVersion = fGeoHandler->CheckGeometryVersion();
  
  if (-1 == fGeoVersion) {
    Fatal("Initialize","unknown TRD geometry");
  }
  if (fGeoVersion == kOldMonolithic) {
    cout<<"-EE- CbmTrd: Old implementation of simple TRD geometry ('PGON')" <<endl;
    cout<<"-EE- This version does not work with newer ROOT versions and is obsolete."<<endl;
    cout<<"-EE- If you see this version you're using a rather old version of CbmRoot. Please update to a new version."<<endl;
    cout<<"-EE- Stop execution at this point."<<endl;
    Fatal("Initialize","See error message above.");
  }  
  
  Int_t stationNr = 1;
  char volumeName[10];
  Bool_t result;
  
  if (fGeoVersion == kNewMonolithic) {
    
    fStationId.clear();
    do {
      sprintf(volumeName, "trd%dgas", stationNr);
      result = fGeoHandler->GetMCId(volumeName, fStationId);
      stationNr++;
    }
    while (result);
    
  } else {
    
    fStationId.clear();
    do {
      sprintf(volumeName, "trd%d", stationNr);
      result = fGeoHandler->GetMCId(volumeName, fStationId);
      stationNr++;
    }
    while (result); 
    Int_t maxStationNr = --stationNr; 
    cout<<"Max Station: "<<maxStationNr<<endl;
    
    Int_t layerNr = 1;
    
    fModuleId.clear();
    std::vector<Int_t> temp;
    Int_t maxModuleTypes = 3;
    for (Int_t iStation = 1; iStation < maxStationNr; iStation++) {
      temp.clear();
      for (Int_t iModule = 1; iModule <= maxModuleTypes; iModule++) {
	sprintf(volumeName, "trd%dmod%d", iStation, iModule);
	Int_t fMCid = gMC->VolId(volumeName);
	temp.push_back(fMCid);      
      }
      fModuleId.push_back(temp);
    }
  }
  
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
    
      // Get the unique detector ID from helper class
      fVolumeID = fGeoHandler->GetUniqueDetectorId(fGeoVersion, fStationId,
						   fModuleId);
            
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



// -----   Public method ConstructGeometry   -------------------------------
void CbmTrd::ConstructGeometry() {

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

//  // add structures a la RICH
//  TGeoMaterial * matAl = new TGeoMaterial("Al", 26.98, 13, 2.7);
//  TGeoMedium * Al      = new TGeoMedium("Al",1, matAl);
//  TGeoVolume * volume  = gGeoManager->MakeTube("grid", Al, 1.3, 1.5, 180);
//
//  gGeoManager->Matrix(123456, 180,   0,  90,  90,  90,   0); // z rotation
//  gGeoManager->Matrix(123457,  90,   0, 180,   0,  90,  90); // y rotation
//   
//  Double_t * buf = 0;
//  for (Int_t i = 0; i< 11; i++) {
//    if (i == 5) continue;
//    gGeoManager->Node("grid", 2*i+1, "trd3", 36*i - 180, 0, 40, 123457, kTRUE, buf, 0);
//    gGeoManager->Node("grid", 2*i+2, "trd3", 0, 36*i - 180, 48, 123456, kTRUE, buf, 0);
//  }

}
// -------------------------------------------------------------------------

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
