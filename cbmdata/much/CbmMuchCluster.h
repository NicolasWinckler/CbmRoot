/** CbmMuchCluster.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 10.11.07
 *@version 1.0
 **
 ** Class describing a cluster consisting of fired pads
 **/

#ifndef CBMMUCHCLUSTER_H
#define CBMMUCHCLUSTER_H 1

#include "TObject.h"
#include "TArrayI.h"

#include <vector>
using std::vector;

class CbmMuchCluster : public TObject{

public:

  /**
   * Default constructor.
   */
  CbmMuchCluster();
  /**
   * Standard constructor.
   * @param digiIndices Indices of the digis that are contained in the cluster
   * @param charge      The whole cluster charge
   */
  CbmMuchCluster(vector<Int_t> digiIndices, Int_t charge);
  /**
   * Standard constructor.
   * @param digiIndices Indices of the digis that are contained in the cluster
   * @param charge      The whole cluster charge
   * @param qMax        Maximum charge on a pad from the cluster
   */
  CbmMuchCluster(vector<Int_t> digiIndices, Int_t charge, Int_t qMax);

  ~CbmMuchCluster();

  /** Gets TArrayI of digi indices. */
  TArrayI GetDigiIndices() const {return fDigiIndices;}

  /** Gets number of digis in the cluster. */
  Int_t GetNDigis() const {return fDigiIndices.GetSize();}

  /**
   * Gets the index of a digi.
   * @param iDigi Zero based ordinal number of digi in the internal list.
   */
  Int_t GetDigiIndex(Int_t iDigi) const {return fDigiIndices.At(iDigi);}

  /** Gets total charge in the cluster. */
  Int_t GetCharge() const {return fCharge;}

  /** Maximum charge collected by one of the cluster's pad. */
  Int_t GetMaxCharge() const {return fQMax;}


private:
  TArrayI             fDigiIndices;       // Array of digi indices which form the cluster
  Int_t               fCharge;            // Summary charge of the cluster
  Int_t               fQMax;              // Maximal charge collected by one of pads of the cluster

ClassDef(CbmMuchCluster,1)
};
#endif
