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

  /** Add a MuchPoint index
   *@value Number of indices present **/
  Int_t AddPoint(Int_t iPoint);

  /** Add a charge from a MC point. Index of the point can
   * be accessed through GetRefIndex(i). **/
  UInt_t AddCharge(UInt_t iCharge);

  /** Accessor to MuchPoint index **/
  Int_t GetRefIndex(Int_t i = 0) const;

  /** Gets number of points corresponding to the digi. **/
  Int_t GetNPoints() const {return fRefIndex.GetSize();}

  /** Gets charge from a MC point. Index of the point can
   * be accessed through GetRefIndex(i). **/
  UInt_t GetCharge(Int_t i) const;

  Int_t GetClusterIndex() const {return fClusterIndex;}
  void SetClusterIndex(Int_t i) {fClusterIndex = i;}


 private:

  TArrayI fRefIndex;      // Array of MC point indices
  TArrayI fCharge;        // Array of charges from each MC point
  Int_t   fClusterIndex;  // Index of a cluster which a digi belongs to

  ClassDef(CbmMuchDigiMatch,1);

};

#endif



