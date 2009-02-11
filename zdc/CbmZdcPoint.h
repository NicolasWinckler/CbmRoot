// -------------------------------------------------------------------------
// -----                    CbmZdcPoint header file                   -----
// -----                 Created 28/07/04  by V. Friese                -----
// -------------------------------------------------------------------------

/**  CbmZdcPoint.h
 *@author V. Friese
 **
 ** Interception of MC track with the plane representing the ECAL.
 **/


#ifndef CBMZDCPOINT_H
#define CBMZDCPOINT_H 1


#include "FairMCPoint.h"

#include "TObject.h"
#include "TVector3.h"

class CbmZdcPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  CbmZdcPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  CbmZdcPoint(Int_t trackID, Int_t detID,  TVector3 pos, TVector3 mom, 
	       Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  CbmZdcPoint(const CbmZdcPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~CbmZdcPoint();


  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

   /** Modifiers **/
  void SetModuleID(Int_t mod) { fModuleID = mod; }
   /** Accessors **/
  Int_t GetModuleID() const {return fModuleID;}

 private:
  Int_t fModuleID; //number of module

  
  ClassDef(CbmZdcPoint,2)

};


#endif
