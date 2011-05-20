// -------------------------------------------------------------------------
// -----                        CbmRichProt header file                    -----
// -----                  Created 27/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

/**  CbmRichProt.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Defines the active detector RICH. Constructs the geometry and
 ** registeres MCPoints.
 **/


#ifndef CBMRICHPROT_H
#define CBMRICHPROT_H


#include "FairDetector.h"

#include "TVector3.h"

class TClonesArray;
class CbmRichProtRefPlanePoint;
class CbmRichPoint;
class CbmRichProtMirrorPoint;
class FairVolume; 



class CbmRichProt : public FairDetector 
{

 public:

  /** Default constructor **/
  CbmRichProt();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmRichProt(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmRichProt();


  /** Virtual method Initialize
   ** Initialises detetcor. Stores volume IDs for RICH detector and mirror.
   **/
  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmRichPoints and CbmRichProtMirrorPoints and adds 
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume* vol = 0);


  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();


  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
  virtual void Register();


  /** Accessor to the hit collection **/
  virtual TClonesArray* GetCollection(Int_t iColl) const;


  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print() const;    


  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
  virtual void Reset();


  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2,
			  Int_t offset);


  /** Virtual method Construct geometry
   **
   **/
  virtual void ConstructGeometry();
  
  /**
  *  put some optical properties 
  */
  void ConstructOpGeometry();


 private:

  Int_t fPosIndex;      //! 
  Int_t volDetector;    //!  MC volume ID of RICH photodetector
  Int_t volRefPlane;     //!  MC volume ID of RICH reference plane
  Int_t volMir;         //!  MC volume ID of RICH Mirror (3 parts)
  Int_t volMir1;         //!  MC volume ID of RICH Mirror
  Int_t volMir2;         //!  MC volume ID of RICH Mirror
  TClonesArray* fRichCollection;        //! Hit collection (photodetector)
  TClonesArray* fRichRefPlaneCollection; //! Hit collection (reference plane)
  TClonesArray* fRichMirrorCollection; //! Hit collection (Mirror)

  /** Private method AddHit
   **
   ** Adds a RichPoint to the HitCollection
   **/
  CbmRichPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos,
		       TVector3 mom, Double_t time, Double_t length, 
		       Double_t eLoss);

  /** Private method AddRefPlaneHit
   **
   ** Adds a RichRefPlanePoint to the HitCollection
   **/
  CbmRichPoint* AddRefPlaneHit(Int_t trackID, Int_t detID, TVector3 pos,
			      TVector3 mom, Double_t time,
			      Double_t length, Double_t eLoss);

  /** Private method AddMirrorHit
   **
   ** Adds a RichMirrorPoint to the HitCollection
   **/
  CbmRichPoint* AddMirrorHit(Int_t trackID, Int_t detID, TVector3 pos,
			      TVector3 mom, Double_t time,
			      Double_t length, Double_t eLoss);

  ClassDef(CbmRichProt,2)

};


#endif
