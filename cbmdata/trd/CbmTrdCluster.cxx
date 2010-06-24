/** CbmTrdCluster.cxx
 *@author Florian Uhlig <f.uhlig@gsi.de>
 **
 ** Data Container for Trd clusters which are collection 
 ** of neighbouring pads above a defined threshold.
 **/

#include "CbmTrdCluster.h"


// ------ Default constructor ----------------------------------------------
CbmTrdCluster::CbmTrdCluster()
  :TObject(),
   fDigiIndices(0),
   fCharge(0.),
   fQMax(0.)
{
}
// -------------------------------------------------------------------------

// ------ Standard constructor  --------------------------------------------
CbmTrdCluster::CbmTrdCluster(TArrayI digiIndices, Float_t charge)
  :TObject(),
   fDigiIndices(digiIndices),
   fCharge(charge),
   fQMax(0.)
{
}
// -------------------------------------------------------------------------

// ------ Standard constructor  --------------------------------------------
CbmTrdCluster::CbmTrdCluster(TArrayI digiIndices, Float_t charge, 
			     Float_t qMax)
  :TObject(),
   fDigiIndices(digiIndices),
   fCharge(charge),
   fQMax(qMax)
{
}
// -------------------------------------------------------------------------

// ------  Destructor  -----------------------------------------------------
CbmTrdCluster::~CbmTrdCluster(){
}
// -------------------------------------------------------------------------


ClassImp(CbmTrdCluster)
