// -------------------------------------------------------------------------
// -----                         CbmZdcv1 header file                 -----
// -----                  Created 04/02/05  by Yu.Kharlov              -----
// -------------------------------------------------------------------------

/**  CbmZdc.h
 *@author Yuri Kharlov <Yuri.Kharlov@ihep.ru>
 **
 ** Defines the active detector ECAL with geometry coded here.
 **/



#ifndef CBMZDCV1_H
#define CBMZDCV1_H


#include "CbmZdc.h"

#include "TLorentzVector.h"

class CbmZdcPoint; 
class FairVolume;
class TClonesArray;


class CbmZdcv1 : public CbmZdc
{

 public:

  /** Default constructor **/
  CbmZdcv1();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmZdcv1(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmZdcv1();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmZdcPoints and adds them to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t  ProcessHits(FairVolume* vol = 0);


  /** Virtual method Construct geometry
   **
   ** Constructs the ZDC geometry
   **/
  virtual void CreateMaterial();
  virtual void ConstructGeometry();


 private:

  /** Track information to be stored until the track leaves the
      active volume. **/
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  Int_t          fModuleID;          //!  module id
  TLorentzVector fPos;               //!  position
  TLorentzVector fMom;               //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
   Double32_t     fEloss;             //!  energy loss
 
  Int_t          fPosIndex;          //!
  TClonesArray*  fZdcCollection;     //! Hit collection

  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();


  ClassDef(CbmZdcv1,2)

};

inline void CbmZdcv1::ResetParameters() {
  fTrackID = fVolumeID = fModuleID = 0;
  fPos.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMom.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fEloss = 0;
  fPosIndex = 0;
};


#endif
