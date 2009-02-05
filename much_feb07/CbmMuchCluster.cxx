/** CbmMuchCluster.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 10.11.07
 *@version 1.0
 **
 ** Class describing a cluster consising of fired pads
 **/

#include "CbmMuchCluster.h"
#include "CbmMuchPad.h"


// ------ Default constructor ----------------------------------------------
CbmMuchCluster::CbmMuchCluster(){
   fCharge = 0;
   fQMax = 0;
}
// -------------------------------------------------------------------------

// ------ Standard constructor  --------------------------------------------
CbmMuchCluster::CbmMuchCluster(vector<Int_t> digiIndices, Int_t charge){
   fCharge = charge;
   fQMax = 0;
   Int_t nDigis = digiIndices.size();
   Int_t inc = 0;
   fDigiIndices.Set(nDigis);
   for(vector<Int_t>::iterator it = digiIndices.begin(); it != digiIndices.end(); it++){
      fDigiIndices.AddAt(*it, inc++);
   }
}
// -------------------------------------------------------------------------

// ------ Standard constructor  --------------------------------------------
CbmMuchCluster::CbmMuchCluster(vector<Int_t> digiIndices, Int_t charge , Int_t qMax){
   fCharge = charge;
   fQMax = qMax;
   Int_t nDigis = digiIndices.size();
   Int_t inc = 0;
   fDigiIndices.Set(nDigis);
   for(vector<Int_t>::iterator it = digiIndices.begin(); it != digiIndices.end(); it++){
      fDigiIndices.AddAt(*it, inc++);
   }
}
// -------------------------------------------------------------------------

// ------  Destructor  -----------------------------------------------------
CbmMuchCluster::~CbmMuchCluster(){
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchCluster)
