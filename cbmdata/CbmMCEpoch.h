/** CbmMCEpoch.h
 *@author Volker Friese v.friese@gsi.de
 *@since 13.11.2009
 *@version 1.0
 **
 ** CBM data class representing one read-out epoch (time slice) on MC level.
 ** It manages all CBM MCPoint collections within this time slice in arrays.
 ** Within each array, the points are sorted w.r.t. time.
 **/


#ifndef CBMEPOCH_H
#define CBMEPOCH_H 1


#include "sts/CbmStsPoint.h"
#include "CbmDetectorList.h"


#include "TClonesArray.h"
#include "TNamed.h"

#include <vector>




class CbmMCEpoch : public TNamed
{

 public:


  /**   Default constructor  **/
  CbmMCEpoch();


  /**   Standard constructor 
   *@param startTime   Begin of epoch [ns]
   *@param epochLength Duration of epoch [ns]
   **/
  CbmMCEpoch(Double_t startTime, Double_t epochLength);


  /**   Destructor   **/
  virtual ~CbmMCEpoch();


  /**   Add one MCPoint   **/
  void AddPoint(DetectorId det, CbmStsPoint& stsPoint, Double_t eventTime);


  /**   Clear data   **/
  void Clear();


  /**   Get number of points in this epoch for a given detector   **/
  Int_t GetNofPoints(DetectorId det);


  /**   Get epoch start time   **/
  Double_t GetStartTime() { return fStartTime; }


  /**   Check whether epoch is empty (no MC points)   **/
  Bool_t IsEmpty();


  /**   Print info   **/
  virtual void Print(Option_t* opt ="") const;


  /**   Set epoch start time   **/
  void SetStartTime(Double_t time) { fStartTime = time; }




 private:

  Double_t fStartTime;             /** Start time of epoch [ns] **/
  Double_t fEpochLength;           /** Duration of epoch [ns] **/ 

  TClonesArray* fStsPoints;        /** Array of StsPoints in epoch **/


  ClassDef(CbmMCEpoch,1);


};

#endif


