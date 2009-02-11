#include "CbmTrd.h"

#include "CbmGeoTrdPar.h"
#include "CbmTrdPoint.h"
#include "CbmGeoTrd.h"

#include "CbmDetectorList.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "CbmStack.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TObjArray.h"
#include "TClonesArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrd::CbmTrd() : FairDetector("TRD", kTRUE, kTRD) {
  fTrdCollection = new TClonesArray("CbmTrdPoint");
  fPosIndex = 0;

  fSimple = 0;

  fVerboseLevel = 1;
}


// -----   Standard constructor   ------------------------------------------
CbmTrd::CbmTrd(const char* name, Bool_t active)
  : FairDetector(name, active, kTRD) {
  fTrdCollection = new TClonesArray("CbmTrdPoint");
  fPosIndex = 0;

  fSimple = 0;

  fVerboseLevel = 1;
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrd::~CbmTrd() {
 if (fTrdCollection) {
   fTrdCollection->Delete();
   delete fTrdCollection;
 }
}
// -------------------------------------------------------------------------
void CbmTrd::Initialize()
{
	FairDetector::Initialize();
 	FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
	CbmGeoTrdPar* par=(CbmGeoTrdPar*)(rtdb->getContainer("CbmGeoTrdPar"));
	TObjArray *SensNodes = par->GetGeoSensitiveNodes();

        FairGeoNode *fm1= (FairGeoNode *) SensNodes->FindObject("trd1gas#1");
        FairGeoNode *fm2= (FairGeoNode *) SensNodes->FindObject("trd1mod1gas");
                                                                
        if (fm1) {
          fSimple = 1;
	  cout<<" -I: CbmTrd: Simple TRD geometry ('PGON')" <<endl;
	}
        else if (fm2) {
	  cout<<" -I: CbmTrd: Segmented TRD geometry "<<endl;
	}
        else {
          cout<<"-E-: CbmTrd: geometry not known" <<endl;
          Fatal("","");
	}
 

        if ( 1 == fSimple ) {

  	  Int_t nModes = SensNodes->GetEntriesFast();
	  FairGeoNode* node = NULL;
   	  Int_t mcID =0;
	  TString name;

	  for(Int_t i=0; i<nModes; i++){
  	    node = (FairGeoNode*) SensNodes->At(i);
	    mcID = node->getMCid();
	    name = node->getName();

	    FairGeoVolume *aVol1 = dynamic_cast<FairGeoVolume*> (SensNodes->FindObject("trd1gas#1") );
	    FairGeoVolume *aVol2 = dynamic_cast<FairGeoVolume*> (SensNodes->FindObject("trd2gas#1") );
	    FairGeoVolume *aVol3 = dynamic_cast<FairGeoVolume*> (SensNodes->FindObject("trd3gas#1") );
	    FairGeoVolume *aVol4 = dynamic_cast<FairGeoVolume*> (SensNodes->FindObject("trd4gas#1") );
	    FairGeoVolume *aVol5 = dynamic_cast<FairGeoVolume*> (SensNodes->FindObject("trd5gas#1") );
	    FairGeoVolume *aVol6 = dynamic_cast<FairGeoVolume*> (SensNodes->FindObject("trd6gas#1") );

	    if(aVol1!=0) fVolid1=aVol1->getMCid();  // store volume id of first station in fVolid1 for later comparison
	    if(aVol2!=0) fVolid2=aVol2->getMCid();
	    if(aVol3!=0) fVolid3=aVol3->getMCid();
	    if(aVol4!=0) fVolid4=aVol4->getMCid();
	    if(aVol5!=0) fVolid5=aVol5->getMCid();
	    if(aVol6!=0) fVolid6=aVol6->getMCid();
	  }

	}
        else {

          Int_t fMCid;

          fMCid= gMC->VolId("trd1");
          if ( fMCid != 0) Trd1_ID = fMCid;
          fMCid= gMC->VolId("trd1layer");
          if ( fMCid != 0) Trd1_Layer_ID = fMCid;
          fMCid= gMC->VolId("trd1mod1");
          if ( fMCid != 0) Trd1_Module1_ID = fMCid;
          fMCid= gMC->VolId("trd1mod2");
          if ( fMCid != 0) Trd1_Module2_ID = fMCid;
          fMCid= gMC->VolId("trd1mod3");
          if ( fMCid != 0) Trd1_Module3_ID = fMCid;

          fMCid= gMC->VolId("trd2");
          if ( fMCid != 0) Trd2_ID = fMCid;
          fMCid= gMC->VolId("trd2layer");
          if ( fMCid != 0) Trd2_Layer_ID = fMCid;
          fMCid= gMC->VolId("trd2mod1");
          if ( fMCid != 0) Trd2_Module1_ID = fMCid;
          fMCid= gMC->VolId("trd2mod2");
          if ( fMCid != 0) Trd2_Module2_ID = fMCid;
          fMCid= gMC->VolId("trd2mod3");
          if ( fMCid != 0) Trd2_Module3_ID = fMCid;

          fMCid= gMC->VolId("trd3");
          if ( fMCid != 0) Trd3_ID = fMCid;
          fMCid= gMC->VolId("trd3layer");
          if ( fMCid != 0) Trd3_Layer_ID = fMCid;

	  /*
          fMCid= gMC->VolId("trd3mod1");
          if ( fMCid != 0) Trd3_Module1_ID = fMCid;
          fMCid= gMC->VolId("trd3mod2");
          if ( fMCid != 0) Trd3_Module2_ID = fMCid;
	  */

          fMCid= gMC->VolId("trd3mod3");
          if ( fMCid != 0) Trd3_Module3_ID = fMCid;
       
	}
}

//*************************************************************************

void CbmTrd::SetSpecialPhysicsCuts(){
    FairRun* fRun = FairRun::Instance();
    if (strcmp(fRun->GetName(),"TGeant3") == 0) {
   	  Int_t mat = gGeoManager->GetMaterialIndex("TRDgas");
	   gMC->Gstpar(mat+1,"STRA",1.0);
	   gMC->Gstpar(mat+1,"PAIR",1.0);
	   gMC->Gstpar(mat+1,"COMP",1.0);
	   gMC->Gstpar(mat+1,"PHOT",1.0);
	   gMC->Gstpar(mat+1,"LOSS",2.0);
           //gMC->Gstpar(mat+1,"PFIS",0.0);
 	   gMC->Gstpar(mat+1,"ANNI",1.0);
	   gMC->Gstpar(mat+1,"BREM",1.0);
	   gMC->Gstpar(mat+1,"HADR",1.0);
	   //gMC->Gstpar(mat+1,"MUNU",1.0);
	   gMC->Gstpar(mat+1,"DCAY",1.0);
	   gMC->Gstpar(mat+1,"MULS",1.0);
	   //gMC->Gstpar(mat+1,"LABS",0.0);
	   gMC->Gstpar(mat+1,"DRAY",1.0);
	   gMC->Gstpar(mat+1,"RAYL",1.0);
           gMC->Gstpar(mat+1,"CUTELE",10.e-6);
           gMC->Gstpar(mat+1,"CUTGAM",10.e-6);
           gMC->Gstpar(mat+1,"CUTNEU",10.e-4);
           gMC->Gstpar(mat+1,"CUTHAD",10.e-4);
           gMC->Gstpar(mat+1,"CUTMUO",10.e-6);
	   gMC->Gstpar(mat+1,"BCUTE",10.e-6);
           gMC->Gstpar(mat+1,"BCUTM",10.e-6);
           gMC->Gstpar(mat+1,"DCUTE",10.e-6);
           gMC->Gstpar(mat+1,"DCUTM",10.e-6);
           gMC->Gstpar(mat+1,"PPCUTM",-1.);
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
	 gMC->IsTrackDisappeared()   ) {
  	 
	 gMC->TrackPosition(fPosOut);
	 gMC->TrackMomentum(fMomOut);


	 if (fELoss == 0. ) return kFALSE;  // no neutrals

         Int_t mod,layer,station;
         Int_t fmodtype,fmodnumber;
	 Int_t flayer=-1;
	 Int_t fstation=-1;


	 fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
	 fVolumeID = vol->getMCid();
	
         if ( 0 == fSimple) {
         
          Int_t id1 = gMC->CurrentVolOffID(1, mod);
          gMC->CurrentVolOffID(2, layer);
          Int_t id3 = gMC->CurrentVolOffID(3, station);
	

          if ( id3 == Trd1_ID ) {
            fstation=1;
	  }
          else if ( id3 == Trd2_ID ) {
            fstation=2;
	  }
          else if ( id3 == Trd3_ID ) {
            fstation=3;
	  }
          else {
            fstation=-1;
	  }

	  flayer=layer;

          if ( (id1 == Trd1_Module1_ID) ||
               (id1 == Trd2_Module1_ID)  ) {
	    //               (id1 == Trd3_Module1_ID)    ) {
            fmodtype=1;
	  }
          else if ( (id1 == Trd1_Module2_ID) ||
                    (id1 == Trd2_Module2_ID) ) {
	    //                    (id1 == Trd3_Module2_ID)    ) {
            fmodtype=2;
	  }
          else if ( (id1 == Trd1_Module3_ID) ||
                    (id1 == Trd2_Module3_ID) ||
                    (id1 == Trd3_Module3_ID)    ) {
            fmodtype=3;
	  }
          else {
            fmodtype=0;
	  }

          fmodnumber=mod;


          fVolumeID = fstation*100000 + flayer * 10000 + fmodtype * 1000 + fmodnumber;


  	}             

	CbmTrdPoint *fPoint= AddHit(fTrackID, fVolumeID, 
                            TVector3(fPosIn.X(),  fPosIn.Y(),  fPosIn.Z()),
	       		    TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
                            TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
			    TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
                            fTime, fLength, fELoss);

        // According to different geometry
	if(1 == fSimple){

	    gMC->CurrentVolID(fCopyNo); //  Returns the current volume ID and copy number
	    fPoint->SetLayerNo(fCopyNo); // Set the layer of the station which is equal to the copy number
	    if(fVolumeID==fVolid1 ) fPoint->SetStationNo(1); // compare Volume id with stored one to get the detector station
	    if(fVolumeID==fVolid2 ) fPoint->SetStationNo(2);
	    if(fVolumeID==fVolid3 ) fPoint->SetStationNo(3);
	    if(fVolumeID==fVolid4 ) fPoint->SetStationNo(4);
	    if(fVolumeID==fVolid5 ) fPoint->SetStationNo(5);
	    if(fVolumeID==fVolid6 ) fPoint->SetStationNo(6);
	}
        else {
          fPoint->SetLayerNo(flayer);
          fPoint->SetStationNo(fstation);
	}


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
  FairRootManager::Instance()->Register("TRDPoint", "Trd", fTrdCollection, kTRUE);
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
  CbmGeoTrd*       trdGeo  = new CbmGeoTrd();
  trdGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(trdGeo);

  Bool_t rc = geoFace->readSet(trdGeo);
  if (rc) trdGeo->create(geoLoad->getGeoBuilder());
  TList* volList = trdGeo->getListOfVolumes();
 // store TRD geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoTrdPar* par=(CbmGeoTrdPar*)(rtdb->getContainer("CbmGeoTrdPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

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
}
// -------------------------------------------------------------------------



// -----   Private method AddHit   -----------------------------------------
CbmTrdPoint* CbmTrd::AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			    TVector3 momIn, TVector3 posOut, TVector3 momOut, 
                            Double_t time, Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fTrdCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmTrdPoint(trackID, detID, posIn, momIn, posOut,
				      momOut, time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(CbmTrd)
