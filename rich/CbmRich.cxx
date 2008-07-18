/* $Id: CbmRich.cxx,v 1.11 2006/06/21 13:49:31 hoehne Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmRich.cxx,v $
 * Revision 1.11  2006/06/21 13:49:31  hoehne
 * MirrorPoint added (again) to be stored during simulation
 *
 * Revision 1.10  2006/03/31 11:22:51  turany
 * Set the Optical properties for detectors can be done on detector level using ConstructOpGeometry in the detector, The Cerenkov proprties are done centraly
 *
 * Revision 1.9  2006/02/09 15:12:57  hoehne
 * count RichPoints for mother of Cherenkov photons
 * store hadrons crossing the PMT plane (may result in noise)
 * store only tracks in ImPlane with at least 1 StsPoint
 *
 * Revision 1.8  2006/01/30 16:53:38  hoehne
 * remove counting of RichPoints for particles traversing the Imaginary Plane
 *
 * Revision 1.7  2006/01/09 19:15:48  hoehne
 * store only tracks in Imaginary Plane which have pz > 0
 *
 * Revision 1.6  2006/01/09 17:27:57  hoehne
 * number stored as CbmRichPoints changed to 10 if track passes ImPlane
 * (number is 15 if point in RICH photodetector)
 *
 * Revision 1.5  2005/12/08 15:11:22  turany
 * change GetRegistered, ActivateBranch and CheckActivatedBranch
 * to GetObject, also add a flag to the arguments of Register which control saving
 * to file(kTRUE) or only in memory(kFALSE)
 *
 * Revision 1.4  2005/11/30 15:36:13  hoehne
 * RichMirrorPoint removed (normal not available anymore)
 * debug output removed
 *
 * Revision 1.3  2005/07/18 10:10:39  kharlov
 * Typos are corrected
 *
 * Revision 1.2  2005/07/18 09:52:49  kharlov
 * Collection of points at imaginary plane is added
 *
 * Revision 1.3  2005/11/28 09:52:49  hoehne
 * MirrorPoints removed (normal not available anymore)
 * other not used lines removed
 *
 */

// -------------------------------------------------------------------------
// -----                       CbmRich source file                     -----
// -----              Created  28/04/04  by B. Polichtchouk            -----
// -----                 modified 26/07/04  by C. Hoehne               -----
// -----               modified 15/07/05  by P. Stolpovsky             -----
// -----                 modified 28/11/05 by C. Hoehne                -----
// -------------------------------------------------------------------------

#include "CbmRich.h"

#include "CbmGeoRichPar.h"
#include "CbmGeoRich.h"
#include "CbmRichPoint.h"

#include "CbmDetectorList.h"
#include "CbmGeoInterface.h"
#include "CbmGeoLoader.h"
#include "CbmGeoNode.h"
#include "CbmGeoRootBuilder.h"
#include "CbmStack.h"
#include "CbmRootManager.h"
#include "CbmVolume.h"
#include "CbmGeoG3Builder.h"
#include "CbmRuntimeDb.h"
#include "CbmRun.h"

#include "TObjArray.h"
#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmRich::CbmRich() : CbmDetector("RICH", kTRUE, kRICH) {
  fRichCollection        = new TClonesArray("CbmRichPoint");
  fRichRefPlaneCollection = new TClonesArray("CbmRichPoint");
  fRichMirrorCollection  = new TClonesArray("CbmRichPoint");
  fPosIndex   = 0;
  volRefPlane  = 0;
  volDetector = 0;
  volMir      = 0;
  volMir1     = 0;
  volMir2     = 0;
  
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRich::CbmRich(const char* name, Bool_t active)
  : CbmDetector(name, active, kRICH) {
  fRichCollection        = new TClonesArray("CbmRichPoint");
  fRichRefPlaneCollection = new TClonesArray("CbmRichPoint");
  fRichMirrorCollection  = new TClonesArray("CbmRichPoint");
  fPosIndex   = 0;
  volRefPlane  = 0;
  volDetector = 0;
  volMir      = 0;
  volMir1     = 0;
  volMir2     = 0;
  
  fVerboseLevel = 1;

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRich::~CbmRich() {
  if (fRichCollection) {
    fRichCollection->Delete();
    delete fRichCollection;
  }
  if (fRichRefPlaneCollection) {
    fRichRefPlaneCollection->Delete();
    delete fRichRefPlaneCollection;
 }
 if (fRichMirrorCollection) {
    fRichMirrorCollection->Delete();
    delete fRichMirrorCollection;
 }
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void CbmRich::Initialize() {
  CbmDetector::Initialize();
  CbmRun* sim = CbmRun::Instance();
  CbmRuntimeDb* rtdb=sim->GetRuntimeDb();
  CbmGeoRichPar *par=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  //	TObjArray *fPassNodes = par->GetGeoPassiveNodes();
  /*
    CbmGeoNode *fn;
    for (Int_t i=0; i< fSensNodes->GetEntries();i++)
    {
    fn=(CbmGeoNode *)fSensNodes->At(i);
    cout << "Volume Id :" << fn->getMCid()<< "  "  << fn->getCopyNo()<<	endl;
    }
  */
  CbmGeoNode *fm1= (CbmGeoNode *) fSensNodes->FindObject("rich1d#1");
  CbmGeoNode *fm2= (CbmGeoNode *) fSensNodes->FindObject("rich1gas2");
  CbmGeoNode *fm3= (CbmGeoNode *) fSensNodes->FindObject("rich1mgl#1");
  CbmGeoNode *fm3a= (CbmGeoNode *) fSensNodes->FindObject("rich1mglLU#1");
  CbmGeoNode *fm3b= (CbmGeoNode *) fSensNodes->FindObject("rich1mglRU#1");

  volDetector = fm1->getMCid();
  volRefPlane  = fm2->getMCid();
  volMir  = fm3->getMCid();
  volMir1  = fm3a->getMCid();
  volMir2  = fm3b->getMCid();

  if(volDetector == volRefPlane) {
    cout<<"Det"<<volDetector<<" ImPl"<<volRefPlane<<endl;
    Fatal("","");
  }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t CbmRich::ProcessHits(CbmVolume* vol) {
   
  // Get track information
  // gMC is of type TVirtualMC
 
//  Int_t      pdgCode = part->GetPdgCode();
  Int_t      pdgCode = gMC->TrackPid();
  Int_t      iVol    = vol->getMCid(); 
  //cout << "CbmRich::ProcessHits"   <<  pdgCode  <<"in volume " << iVol << endl;
  // Treat photodetectors : All particles 
  // Nota bene: Energy loss at the entrance step is zero!

  if (iVol == volDetector) {

    if (gMC->IsTrackEntering()){

      TParticle* part    = gMC->GetStack()->GetCurrentTrack();
      Double_t charge = part->GetPDG()->Charge() / 3. ;

      // Create CbmRichPoint
      Int_t    trackID = gMC->GetStack()->GetCurrentTrackNumber();
      Double_t time    = gMC->TrackTime() * 1.0e09;
      Double_t length  = gMC->TrackLength();
      Double_t eLoss   = gMC->Edep();
      TLorentzVector tPos, tMom;
      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);

      if ( pdgCode == 50000050) {
      AddHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	     TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	     time, length, eLoss);
	     
	     
         // Increment number of RichPoints for this track
         CbmStack* stack = (CbmStack*) gMC->GetStack();
         stack->AddPoint(kRICH);
    
    /*
       // Increment number of rich points in TParticle (for Mother of Cherenkovs)
        Int_t motherID = gMC->GetStack()->GetCurrentTrack()->GetFirstMother();
	TParticle *mother = ((CbmStack *) gMC->GetStack())->GetParticle(motherID);
	Int_t pointsM = mother->GetMother(1);
        Int_t nRichPointsM = (pointsM & (15<<4)) >> 4;
        nRichPointsM++;
        if (nRichPointsM > 15) nRichPointsM = 15;        // max number = 15
        pointsM = ( pointsM & ( ~ (15<<4) ) ) | (nRichPointsM << 4);
        mother->SetMother(1,pointsM);

      // Count RichPoint for Cherenkov too - otherwise it would not be stored in CbmMCTrack
      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
      Int_t nRichPoints = (points & (15<<4)) >> 4;
      nRichPoints++;
      if (nRichPoints > 15) nRichPoints = 15;        // max number = 15
      points = ( points & ( ~ (15<<4) ) ) | (nRichPoints << 4);
      gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
      */
      
      return kTRUE;
    }    // Cherenkovs only
    else
     {
      if (charge == 0.) {
	return kFALSE; // no neutrals
      }
      else {
	   AddHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	   	         TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
		         time, length, eLoss);
			 
	 // Increment number of RichPoints for this track
         CbmStack* stack = (CbmStack*) gMC->GetStack();
         stack->AddPoint(kRICH);

/*
	// Increment number of rich points in TParticle
	Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
        Int_t nRichPoints = (points & (15<<4)) >> 4;
        nRichPoints++;
        if (nRichPoints > 15) nRichPoints = 15;        // max number = 15
        points = ( points & ( ~ (15<<4) ) ) | (nRichPoints << 4);
        gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
	*/
	
	   return kTRUE;
//	}
      }  // charged particles
    }

  }
  }


  if (iVol == volRefPlane) { // Treat imaginary plane : Only charged particles at entrance
    // Collecting points of tracks and imaginary plane intersection
    if ( gMC->IsTrackEntering() ) {
      TParticle* part    = gMC->GetStack()->GetCurrentTrack();
      Double_t charge = part->GetPDG()->Charge() / 3. ;
      if (charge == 0.) {
	return kFALSE; // no neutrals
      }else {

	Int_t    trackID = gMC->GetStack()->GetCurrentTrackNumber();

	Double_t time    = gMC->TrackTime() * 1.0e09;
	Double_t length  = gMC->TrackLength();
	Double_t eLoss   = gMC->Edep();
	TLorentzVector tPos, tMom;

	gMC->TrackPosition(tPos);
	gMC->TrackMomentum(tMom);

	// check number of STS points
	
        //UInt_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
        //Int_t nStsPoints = (points & 15);

        //if (nStsPoints > 0) {              // store only particles with STSpoints (at least 1)
	   AddRefPlaneHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	   	         TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
		         time, length, eLoss);
        			 
        // Increment number of RefPlanePoints for this track
	CbmStack* stack = (CbmStack*) gMC->GetStack();
	stack->AddPoint(kREF);
	

	/* Number of Rich points in ImPlane not needed as they will never be recorded anyhow
	   // Increment number of rich ImPlane points in TParticle
	   Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
	   Int_t nRichPoints = (points & (15<<4));
	   nRichPoints += 10;
	   if (nRichPoints > 15) nRichPoints = 15;
	   points = ( points & ( ~ (15<<4) ) ) | (nRichPoints << 4);
	   gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
    */

	   return kTRUE;
	// }
      }
    }
  }

/*
  if (iVol == volMir || iVol == volMir1 || iVol == volMir2) { // Treat mirror : Only charged particles at entrance
    // Collecting points of tracks and imaginary plane intersection
    if ( gMC->IsTrackEntering() ) {
      TParticle* part    = gMC->GetStack()->GetCurrentTrack();
      Double_t charge = part->GetPDG()->Charge() / 3. ;
      if (charge == 0.) {
	return kFALSE; // no neutrals
      }else {

	Int_t    trackID = gMC->GetStack()->GetCurrentTrackNumber();

	Double_t time    = gMC->TrackTime() * 1.0e09;
	Double_t length  = gMC->TrackLength();
	Double_t eLoss   = gMC->Edep();
	TLorentzVector tPos, tMom;

	gMC->TrackPosition(tPos);
	gMC->TrackMomentum(tMom);


	// check number of STS points
        UInt_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
        Int_t nStsPoints = (points & 15);

        if (nStsPoints > 0) {              // store only particles with STSpoints (at least 1)
	   AddMirrorHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	   	         TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
		         time, length, eLoss);

	 Number of Rich points in ImPlane not needed as they will never be recorded anyhow
	   // Increment number of rich ImPlane points in TParticle
	   Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
	   Int_t nRichPoints = (points & (15<<4));
	   nRichPoints += 10;
	   if (nRichPoints > 15) nRichPoints = 15;
	   points = ( points & ( ~ (15<<4) ) ) | (nRichPoints << 4);
	   gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
    

	   return kTRUE;
	}
      }
    }
  }
*/
  return kFALSE;
}
// ----------------------------------------------------------------------------



// -----   Public method EndOfEvent   -----------------------------------------
void CbmRich::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void CbmRich::Register() {
  CbmRootManager::Instance()->Register("RichPoint","Rich", fRichCollection, kTRUE);
  CbmRootManager::Instance()->Register("RefPlanePoint","RichRefPlane",
  				       fRichRefPlaneCollection, kTRUE);
  CbmRootManager::Instance()->Register("RichMirrorPoint","RichMirror",
  				       fRichMirrorCollection, kFALSE);  // fFALSE -> MirrorPoint not saved
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* CbmRich::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fRichCollection;
  if (iColl == 1) return fRichRefPlaneCollection;
  if (iColl == 2) return fRichMirrorCollection;
  return NULL;
}
// ----------------------------------------------------------------------------




// -----   Public method Print   ----------------------------------------------
void CbmRich::Print() const {
   Int_t nHits = fRichCollection->GetEntriesFast();
   cout << "-I- CbmRich: " << nHits << " points registered in this event."
	<< endl;
	
   if (fVerboseLevel>1)
     for (Int_t i=0; i<nHits; i++) (*fRichCollection)[i]->Print();
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void CbmRich::Reset() {
  fRichCollection->Clear();
  fRichRefPlaneCollection->Clear();
  fRichMirrorCollection->Clear();
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void CbmRich::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmRich: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmRichPoint* oldpoint = NULL;
  for (Int_t i=0; i< nEntries ; i++ ) {
    oldpoint = (CbmRichPoint*) cl1->At(i);
    Int_t index =  oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmRichPoint(*oldpoint);     
    fPosIndex++;
  }
  cout << "-I- CbmRich: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// ----------------------------------------------------------------------------

void CbmRich::ConstructOpGeometry() 
{

	cout<< "CbmRich::ConstructOpGeometry() " <<endl;

}

// -----   Public method ConstructGeometry   ----------------------------------
void CbmRich::ConstructGeometry() {

  CbmGeoLoader*    geoLoad = CbmGeoLoader::Instance();
  CbmGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmGeoRich*      richGeo = new CbmGeoRich();
  richGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(richGeo);

  Bool_t rc = geoFace->readSet(richGeo);
  if (rc) richGeo->create(geoLoad->getGeoBuilder());
  TList* volList = richGeo->getListOfVolumes();

  // store geo parameter
  CbmRun *fRun = CbmRun::Instance();
  CbmRuntimeDb *rtdb= CbmRun::Instance()->GetRuntimeDb();
  CbmGeoRichPar* par=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  CbmGeoNode* node   = NULL;
  CbmGeoVolume *aVol=NULL;

  while( (node = (CbmGeoNode*)iter.Next()) ) {
    aVol = dynamic_cast<CbmGeoVolume*> ( node );
    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
    }else{
      fPassNodes->AddLast( aVol );
    }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  ProcessNodes ( volList );
}
// ----------------------------------------------------------------------------



// -----   Private method AddHit   --------------------------------------------
CbmRichPoint* CbmRich::AddHit(Int_t trackID, Int_t detID, TVector3 pos,
			      TVector3 mom, Double_t time, Double_t length, 
			      Double_t eLoss) {
  TClonesArray& clref = *fRichCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmRichPoint(trackID, detID, pos, mom, time,
				       length, eLoss);
}
// ----------------------------------------------------------------------------

// -----   Private method AddRefPlaneHit   -------------------------------------
CbmRichPoint* CbmRich::AddRefPlaneHit(Int_t trackID, Int_t detID,
				     TVector3 pos, TVector3 mom,
				     Double_t time, Double_t length,
				     Double_t eLoss) {
  TClonesArray& clref = *fRichRefPlaneCollection;
  Int_t tsize = clref.GetEntriesFast();
  return new(clref[tsize]) CbmRichPoint(trackID, detID, pos, mom, time,
					      length, eLoss);
}
// ----------------------------------------------------------------------------


// -----   Private method AddMirrorHit   -------------------------------------
CbmRichPoint* CbmRich::AddMirrorHit(Int_t trackID, Int_t detID,
				     TVector3 pos, TVector3 mom,
				     Double_t time, Double_t length,
				     Double_t eLoss) {
  TClonesArray& clref = *fRichMirrorCollection;
  Int_t tsize = clref.GetEntriesFast();
  return new(clref[tsize]) CbmRichPoint(trackID, detID, pos, mom, time,
					      length, eLoss);
}
// ----------------------------------------------------------------------------


ClassImp(CbmRich)
