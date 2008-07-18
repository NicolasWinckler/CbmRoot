// -------------------------------------------------------------------------
// -----                    CbmEcalPoint header file                   -----
// -----                 Created 28/07/04  by V. Friese                -----
// -------------------------------------------------------------------------

/**  CbmEcalPoint.h
 *@author V. Friese
 **
 ** Interception of MC track with the plane representing the ECAL.
 **/


#ifndef CBMECALPOINT_H
#define CBMECALPOINT_H 1


#include "CbmMCPoint.h"

#include "TObject.h"
#include "TVector3.h"


class CbmEcalPoint : public CbmMCPoint
{

 public:

  /** Default constructor **/
  CbmEcalPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  CbmEcalPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, 
	       Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  CbmEcalPoint(const CbmEcalPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~CbmEcalPoint();


  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

  
  ClassDef(CbmEcalPoint,1)

};


#endif
