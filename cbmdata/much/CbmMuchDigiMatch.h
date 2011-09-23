/** CbmMuchDigiMatch.h
 **@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since   23.03.07
 **@version 1.0
 **
 ** Data class for matching CbmMuchDigi with CbmMuchPoint.
 ** The indices of CbmMuchDigi and CbmMuchDigiMatch correspond to that
 ** in their respective arrays.
 ** The class holds the indices of all MuchPoints corresponding to
 ** the MuchDigi.
 **/


#ifndef CBMMUCHDIGIMATCH_H
#define CBMMUCHDIGIMATCH_H 1


#include "TObject.h"
#include "TArrayI.h"
#include "TArrayD.h"
class TClonesArray;


class CbmMuchDigiMatch : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchDigiMatch();

  /** Standard constructor. Initializes this CbmMuchDigiMatch object from the given one. **/
  CbmMuchDigiMatch(CbmMuchDigiMatch* match);

  /** Destructor **/
  virtual ~CbmMuchDigiMatch();

  /**
   * Add a MuchPoint index
   * @param iPoint Point index
   * @value Number of indices present
   */
  Int_t AddPoint(Int_t iPoint);

  /**
   * Add a charge from a MC point. Index of the point can
   * be accessed through GetRefIndex(i).
   * @param iCharge Charge from the MuchPoint [electrons]
   * @value Number of points the digi contains
   */
  UInt_t AddCharge(UInt_t iCharge);

  /**
   * Accessor to MuchPoint index.
   * @param i MuchPoint index in the internal array of the digi
   * @value   MuchPointIndex in the output array of MC transport file
   */
  Int_t GetRefIndex(Int_t i = 0) const;

  /**
   * Gets number of points corresponding to the digi.
   * @value Number of MuchPoints contained in the digi
   */
  Int_t GetNPoints() const {return fRefIndex.GetSize();}

  /**
   * Gets charge from a MC point. Index of the point can
   * be accessed through GetRefIndex(i).
   * @param i MuchPoint index in the internal array of the digi
   * @value Charge from the corresponding MuchPoint [electrons]
   */
  UInt_t GetCharge(Int_t i) const;
  UInt_t GetTotalCharge() const;
  void SortPointsInTime(TClonesArray* points);
  
  void Reset();

  /**
   * Add a charge from a MC point. Index of the point can
   * be accessed through GetRefIndex(i).
   * @param iCharge Charge from the MuchPoint [electrons]
   * @value Number of points the digi contains
   */
  UInt_t AddCharge(Int_t iPoint, UInt_t iCharge);

  UInt_t AddCharge(Int_t iPoint, UInt_t iCharge, Double_t driftTime);
 
  Int_t GetNoPrimaryElectrons() { return fDriftTimePerPrimaryElectron.GetSize(); }
  Int_t    GetRefIndexPerPrimaryElectron(Int_t i)  { return fRefIndexPerPrimaryElectron[i]; }
  Int_t    GetChargePerPrimaryElectron(Int_t i)    { return fChargePerPrimaryElectron[i]; }
  Double_t GetDriftTimePerPrimaryElectron(Int_t i) { return fDriftTimePerPrimaryElectron[i]; }
 private:

  TArrayI fRefIndex;                    // Array of MC point indices
  TArrayI fCharge;                      // Array of charges from each MC point
  TArrayI fRefIndexPerPrimaryElectron;  // Array of ref indices per primary electron 
  TArrayI fChargePerPrimaryElectron;    // Array of charges per primary electron
  TArrayD fDriftTimePerPrimaryElectron; // Array of driftTimes per primary electron 
  TArrayD fSignalShape;                 // Array of time bins
  ClassDef(CbmMuchDigiMatch,1);

};

#endif
