#ifndef ICALDET_H
#define ICALDET_H

#include "FairDetector.h"

#include "TVector3.h"
#include "TLorentzVector.h"

class IcalPoint;
class FairVolume;
class TClonesArray;

class IcalDet: public FairDetector {
public:

    /**      Name :  Detector Name
     *       Active: kTRUE for active detectors (ProcessHits() will be called)
     *               kFALSE for inactive detectors     
    */
    IcalDet(const char * Name, Bool_t Active);

    /**      default constructor    */
    IcalDet();

    /**       destructor     */
    virtual ~IcalDet();

    /**      Initialization of the detector is done here    */
    virtual void   Initialize();

    /**       this method is called for each step during simulation 
     *       (see FairMCApplication::Stepping())    
    */
    virtual Bool_t ProcessHits( FairVolume *v=0);

    /**       Registers the produced collections in CBMRootManager.     */
    virtual void   Register();

    /** Gets the produced collections */
    virtual TClonesArray* GetCollection(Int_t iColl) const ;

    /**      has to be called after each event to reset the containers      */
    virtual void   Reset();

    /**      Create the detector geometry        */
    void ConstructGeometry();

    /**      This method is an example of how to add your own point 
     *       of type IcalDetPoint to the clones array
    */
   IcalPoint* AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
   			      TVector3 posOut, TVector3 momIn,
   			      TVector3 momOut, Double_t time,
   			      Double_t length, Double_t eLoss);
 
    /**	The following methods can be implemented if you need to make 
     *	any optional action in your detector during the transport.  
    */

    virtual void   CopyClones( TClonesArray *cl1,  TClonesArray *cl2 , 
			       Int_t offset) {;}
    virtual void   SetSpecialPhysicsCuts(){;} 
    virtual void   EndOfEvent();
    virtual void   FinishPrimary(){;}                               
    virtual void   FinishRun(){;}
    virtual void   BeginPrimary(){;}   
    virtual void   PostTrack(){;}                                     
    virtual void   PreTrack(){;}                                      
    virtual void   BeginEvent(){;}

private:

    /** Track information to be stored until the track leaves the
	active volume. 
    */
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    TLorentzVector fPosOut;            //!  position at exit
    TLorentzVector fMomOut;            //!  momentum at exit
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss

    /** container for data points */
    
    TClonesArray  *fIcalDetPointCollection;

    ClassDef(IcalDet,1);
};

#endif //ICALDET_H
