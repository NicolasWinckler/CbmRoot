/** CbmMuchCluster.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 10.11.07
 *@version 1.0
 **
 ** Class describing a cluster consisting of fired pads
 **/

#ifndef CBMMUCHCLUSTER_H
#define CBMMUCHCLUSTER_H 1

#include <vector>

#include "TObject.h"
#include "TArrayI.h"

using std::vector;

class CbmMuchCluster : public TObject{

public:

  CbmMuchCluster();
  CbmMuchCluster(vector<Int_t> digiIndices, Int_t charge);
  CbmMuchCluster(vector<Int_t> digiIndices, Int_t charge, Int_t qMax);

  ~CbmMuchCluster();
  
  inline TArrayI     GetDigis()                const {return fDigiIndices;}
  inline Int_t       GetNDigis()               const {return fDigiIndices.GetSize();}
  inline Int_t       GetDigiIndex(Int_t iDigi) const {return fDigiIndices.At(iDigi);}
  inline Int_t       GetCharge()               const {return fCharge;}
  inline Int_t       GetMaxCharge()            const {return fQMax;}


private:
  TArrayI             fDigiIndices;       // Array of digi indices which form the cluster
  Int_t               fCharge;            // Summary charge of the cluster
  Int_t               fQMax;              // Maximal charge collected by one of pads of the cluster

ClassDef(CbmMuchCluster,1)
};
#endif
