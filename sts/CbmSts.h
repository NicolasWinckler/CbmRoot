// -------------------------------------------------------------------------
// -----                        CbmSts header file                     -----
// -----                  Created 26/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

/**  CbmSts.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Defines the active detector STS. Constructs the geometry and
 ** registeres MCPoints.
 **/


#ifndef CBMSTS_H
#define CBMSTS_H

#include <map>
#include "FairDetector.h"

#include "TLorentzVector.h"

class TClonesArray;
class CbmStsPoint;
class FairVolume;

using namespace std;



class CbmSts : public FairDetector
{

 public:

  /** Default constructor **/
  CbmSts();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmSts(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmSts();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates a CbmStsPoint and adds it to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume* vol = 0);


  /** Virtual method BeginEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/

  virtual void BeginEvent();

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


  /** Construct the STS geometry.
   ** Depending on the geometry file extension, the base class method
   ** ConstructRootGeometry is called (.root), or the ASCII geometry
   ** file is read by the FairGeoLoader (.geo).
   ** @since 11.06.2012
   **/
  virtual void ConstructGeometry();
   


  /** Construct the geometry from an ASCII geometry file.
   **/
  virtual void ConstructAsciiGeometry();


  /** Check whether a volume is sensitive.
   ** The decision is based on the volume name. Only used in case
   ** of ROOT geometry.
   ** @since 11.06.2012
   ** @param(name)  Volume name
   ** @value        kTRUE if volume is sensitive, else kFALSE
   **/
  virtual Bool_t CheckIfSensitive(std::string name);


//  void SaveGeoParams();

  private:


    /** Track information to be stored until the track leaves the
	active volume. **/
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id
    Int_t          fDetectorId;        // ! Unique detector ID
    TLorentzVector fPosIn, fPosOut;    //!  position
    TLorentzVector fMomIn, fMomOut;    //!  momentum
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss

    Int_t          fPosIndex;          //!
    TClonesArray*  fStsCollection;     //!  The hit collection
    Bool_t         kGeoSaved;          //!
    TList *flGeoPar; //!
    map<Int_t, Int_t> fVolumeMap;       //! Map from MC volume ID to unique detector ID
    map<Int_t, Int_t>::iterator fVolumeMapIter;  //! Map iterator

    /** Private method AddHit
     **
     ** Adds a StsPoint to the HitCollection
     **/
    CbmStsPoint* AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			TVector3 pos_out, TVector3 momIn, 
			TVector3 momOut, Double_t time, 
			Double_t length, Double_t eLoss);

    //Int_t GetCurrentDetectorId();


    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();
    CbmSts(const CbmSts&);
    CbmSts operator=(const CbmSts&);

    ClassDef(CbmSts,1);

};


inline void CbmSts::ResetParameters() {
  fTrackID = fVolumeID = 0;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif 
