// -------------------------------------------------------------------------
// -----                         CbmTrd header file                    -----
// -----                  Created 27/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

/**  CbmTrd.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Defines the active detector TRD. Constructs the geometry and
 ** registeres MCPoints.
 **/



#ifndef CBMTRD_H
#define CBMTRD_H


#include "FairDetector.h"

#include "CbmTrdDetectorId.h"

#include "TLorentzVector.h"
#include "TVector3.h"

class TClonesArray;
class CbmTrdPoint; 
class FairVolume;



class CbmTrd : public FairDetector 
{

 public:

  /** Default constructor **/
  CbmTrd();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmTrd(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmTrd();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmTrdPoints and adds them to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t  ProcessHits(FairVolume* vol = 0);


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
  virtual void  Register();


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
   ** Constructs the STS geometry
   **/
  virtual void ConstructGeometry();


 private:

  /** Track information to be stored until the track leaves the
      active volume. **/
  Int_t          fTrackID;           //! track index
  Int_t          fVolumeID;          //! volume id
  TLorentzVector fPosIn;             //! position at entrance
  TLorentzVector fMomIn;             //! momentum at entrance
  TLorentzVector fPosOut;            //! position at exit
  TLorentzVector fMomOut;            //! momentum at exit
  Double32_t     fTime;              //! time
  Double32_t     fLength;            //! length
  Double32_t     fELoss;             //! energy loss
  Int_t 	 fVolid1,fVolid2, fVolid3, fVolid4, fVolid5, fVolid6;  //! volume id
  Int_t 	 fCopyNo;            //! copy number
  Int_t          fPosIndex;          //! position index
  TClonesArray*  fTrdCollection;     //! Hit collection

  Int_t          fSimple;            //! Switch between different geometry versions

  Int_t Trd1_ID;                     //! MC ID of first TRD Station	     
  //  Int_t Trd1_Layer_ID;               //! MC ID of first layer of first TRD   
  Int_t Trd1_Module1_ID;             //! MC ID of small module of first TRD  
  Int_t Trd1_Module2_ID;             //! MC ID of middle module of first TRD 
  Int_t Trd1_Module3_ID;             //! MC ID of big module of first TRD    

  Int_t Trd2_ID;                     //! MC ID of second TRD Station	     
  //  Int_t Trd2_Layer_ID;		     //! MC ID of first layer of second TRD   
  Int_t Trd2_Module1_ID;	     //! MC ID of small module of second TRD  
  Int_t Trd2_Module2_ID;	     //! MC ID of middle module of second TRD 
  Int_t Trd2_Module3_ID;	     //! MC ID of big module of second TRD    

  Int_t Trd3_ID;                     //! MC ID of third TRD Station	     
  //  Int_t Trd3_Layer_ID;		     //! MC ID of first layer of third TRD   
  Int_t Trd3_Module1_ID;	     //! MC ID of small module of third TRD  
  Int_t Trd3_Module2_ID;	     //! MC ID of middle module of third TRD 
  Int_t Trd3_Module3_ID;	     //! MC ID of big module of third TRD    

  CbmTrdDetectorId fTrdId;           //! TRD detector ID
  /** Private method AddHit
   **
   ** Adds a CbmTrdPoint to the HitCollection
   **/
  CbmTrdPoint* AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
		      TVector3 momIn, TVector3 posOut, TVector3 momOut,
                      Double_t time, Double_t length, Double_t eLoss);

  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();
  virtual void Initialize();
  virtual void SetSpecialPhysicsCuts();

  ClassDef(CbmTrd,4)

};


inline void CbmTrd::ResetParameters() {
  fTrackID = fVolumeID = 0;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif
