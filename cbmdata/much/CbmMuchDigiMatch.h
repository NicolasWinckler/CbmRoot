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


 private:

  TArrayI fRefIndex;      // Array of MC point indices
  TArrayI fCharge;        // Array of charges from each MC point

  ClassDef(CbmMuchDigiMatch,1);

};

#endif
