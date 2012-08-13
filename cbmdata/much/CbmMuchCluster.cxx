#include "CbmMuchCluster.h"


// -------------------------------------------------------------------------
CbmMuchCluster::CbmMuchCluster()
  : TObject(),
    fDigiIndices()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchCluster::CbmMuchCluster(Int_t iDigi)
  : TObject(),
    fDigiIndices(1,&iDigi)
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchCluster::CbmMuchCluster(vector<Int_t> digiIndices)
  : TObject(),
    fDigiIndices(digiIndices.size(),&(digiIndices[0]))
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchCluster::CbmMuchCluster(vector<Int_t> digiIndices, Int_t /* dummy*/, Int_t /* dummy*/)
  : TObject(),
    fDigiIndices(digiIndices.size(),&(digiIndices[0]))
{
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchCluster)
