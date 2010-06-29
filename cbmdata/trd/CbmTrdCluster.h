/** CbmTrdCluster.h
 *@author Florian Uhlig <f.uhlig@gsi.de>
 **
 ** Data Container for Trd clusters which are collection 
 ** of neighbouring pads above a defined threshold.
 ** TODO: It has to be defined if the threshold is defined here
 **       or in the digi class. It has also to be defined if 
 **       neighbouring also includes pads which are only touched 
 **       at the edges of the pad (diagonal)
 **/

#ifndef CBMTRDCLUSTER_H
#define CBMTRDCLUSTER_H 

#include "TObject.h"
#include "TArrayI.h"

class CbmTrdCluster : public TObject{

public:

  /**
   * Default constructor.
   */
  CbmTrdCluster();

  /**
   * Standard constructor.
   * @param digiIndices Indices of the digis that are contained in the cluster
   * @param charge      The whole cluster charge
   */
  CbmTrdCluster(TArrayI digiIndices, Float_t charge);

  /**
   * Standard constructor.
   * @param digiIndices Indices of the digis that are contained in the cluster
   * @param charge      The whole cluster charge
   * @param qMax        Maximum charge on a pad from the cluster
   */
  CbmTrdCluster(TArrayI digiIndices, Float_t charge, Float_t qMax);

  ~CbmTrdCluster();

  /** Gets TArrayI of digi indices. */
  TArrayI GetDigiIndices() const {return fDigiIndices;}

  /** Gets number of digis/pads in the cluster. */
  Int_t GetNDigis() const {return fDigiIndices.GetSize();}

  /**
   * Gets the index of a digi.
   * @param iDigi Zero based ordinal number of digi in the internal list.
   */
  Int_t GetDigiIndex(Int_t iDigi) const {return fDigiIndices.At(iDigi);}

  /** Total charge in the cluster. */
  Float_t GetCharge() const {return fCharge;}

  /** Maximum charge on a single pad. */
  Float_t GetMaxCharge() const {return fQMax;}


private:
  TArrayI  fDigiIndices; // Array of digi indices which form the cluster
  Float_t    fCharge;      // Summary charge of the cluster
  Float_t    fQMax;        // Maximal charge collected on a single pad

ClassDef(CbmTrdCluster,1)
};
#endif