#ifndef CBMSTTDET_H
#define CBMSTTDET_H
#include "FairDetector.h"
#include "TLorentzVector.h"

class FairVolume;
class TClonesArray;
class TVector3;
class CbmSttPoint;

class CbmStt: public FairDetector {

public:
  /**      Name :  Detector Name
	   Active: kTRUE for active detectors  (ProcessHits() will be called)
	   kFALSE for inactive detectors     */
  CbmStt(const char * Name, Bool_t Active);
  /**      default constructor    */
  CbmStt();
  /**       destructor     */
  virtual ~CbmStt();
  /**      Initialization of the detector is done here    */
  virtual void   Initialize();
  /**       this method is called for each step during dsimulation (see FairMCApplication::Stepping())    */
  virtual Bool_t ProcessHits( FairVolume *v=0);
  /**       Registers the produced collections in CBMRootManager.     */
  virtual void   Register();
  /** Gets the produced collections */
  virtual TClonesArray* GetCollection(Int_t iColl) const ;
  /** Screen output of hit collection. */
  virtual void Print() const;
  /**      has to be called after each event to reset the containers        */
  virtual void   Reset();
  
  /**      Create the detector geometry        */
  void ConstructGeometry();

  /**      This method is an example of how to add your own point of type MyDetPoint to the clones array       */
  CbmSttPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
		      Double_t time, Double_t length, Double_t eLoss);

  /**	The following methods can be implemented if you need to make any optional action in your detector during the transport.  */

  virtual void   SetSpecialPhysicsCuts();
  virtual void   CopyClones( TClonesArray *cl1,  TClonesArray *cl2 , Int_t offset);
  virtual void   EndOfEvent();
  virtual void   FinishPrimary(){;};
  virtual void   FinishRun(){;};
  virtual void   BeginPrimary(){;};
  virtual void   PostTrack(){;};
  virtual void   PreTrack(){;};
  virtual void   BeginEvent(){;};

 private: 
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  Double32_t     fELoss;             //!  energy loss
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
  TLorentzVector fPos;	             //!  position at entrance
  TLorentzVector fMom;	             //!  momentum at entrance

  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();
			 
  Int_t          fPosIndex;          //!
  /** container for data points */
  TClonesArray  *fSttCollection;

  CbmStt(const CbmStt&);
  CbmStt& operator=(const CbmStt&);

  ClassDef(CbmStt,1)
};
#endif //CbmSttDET_H

