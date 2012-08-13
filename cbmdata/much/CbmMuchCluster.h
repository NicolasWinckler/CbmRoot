/** CbmMuchCluster.cxx
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 13.08.12
 *@version 2.0

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
  CbmMuchCluster();
  CbmMuchCluster(Int_t iDigi);
  CbmMuchCluster(vector<Int_t> digiIndices);
  CbmMuchCluster(vector<Int_t> digiIndices, Int_t dum1, Int_t dum2);
  ~CbmMuchCluster(){}
  /** Gets TArrayI of digi indices. */
  TArrayI GetDigiIndices() const {return fDigiIndices;}
  /** Gets number of digis in the cluster. */
  Int_t GetNDigis() const {return fDigiIndices.GetSize();}
  /**
   * Gets the index of a digi.
   * @param iDigi Zero based ordinal number of digi in the internal list.
   */
  Int_t GetDigiIndex(Int_t iDigi) const {return fDigiIndices.At(iDigi);}
private:
  TArrayI fDigiIndices; // Array of digi indices which form the cluster

ClassDef(CbmMuchCluster,1)
};
#endif
