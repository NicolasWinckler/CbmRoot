/* $Id: CbmRich.h,v 1.6 2006/06/21 13:49:31 hoehne Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmRich.h,v $
 * Revision 1.6  2006/06/21 13:49:31  hoehne
 * MirrorPoint added (again) to be stored during simulation
 *
 * Revision 1.5  2006/03/31 11:22:51  turany
 * Set the Optical properties for detectors can be done on detector level using ConstructOpGeometry in the detector, The Cerenkov proprties are done centraly
 *
 * Revision 1.4  2005/11/30 15:36:34  hoehne
 * MirrorPoint removed (normal not available anymore)
 *
 * Revision 1.3  2005/07/18 10:10:39  kharlov
 * Typos are corrected
 *
 * Revision 1.2  2005/07/18 09:52:49  kharlov
 * Collection of points at imaginary plane is added
 *
 */

// -------------------------------------------------------------------------
// -----                        CbmRich header file                    -----
// -----                  Created 27/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

/**  CbmRich.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Defines the active detector RICH. Constructs the geometry and
 ** registeres MCPoints.
 **/


#ifndef CBMRICH_H
#define CBMRICH_H


#include "CbmDetector.h"

#include "TVector3.h"

class TClonesArray;
class CbmRichRefPlanePoint;
class CbmRichPoint;
class CbmRichMirrorPoint;
class CbmVolume; 



class CbmRich : public CbmDetector 
{

 public:

  /** Default constructor **/
  CbmRich();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmRich(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmRich();


  /** Virtual method Initialize
   ** Initialises detetcor. Stores volume IDs for RICH detector and mirror.
   **/
  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmRichPoints and CbmRichMirrorPoints and adds 
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(CbmVolume* vol = 0);


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

  ClassDef(CbmRich,2)

};


#endif
