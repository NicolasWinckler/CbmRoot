#include "CbmMuchSector.h"
#include "CbmMuchPad.h"

// -----   Default constructor   -------------------------------------------
CbmMuchSector::CbmMuchSector()
  : TObject(),
    fDetectorId(0),
    fSectorId(0),
    fNChannels(0),
    fPads()
{
}
// -------------------------------------------------------------------------

// -----  Standard constructor  --------------------------------------------
CbmMuchSector::CbmMuchSector(Int_t detId, Int_t secId, Int_t nChannels)
  : TObject(),  
    fDetectorId(detId),
    fSectorId(secId),
    fNChannels(nChannels),
    fPads()
{
//  fPads.resize(nChannels);
}
// -------------------------------------------------------------------------

//// -------------------------------------------------------------------------
//void CbmMuchSector::DrawPads(){
//  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
//     CbmMuchPad* pad = fPads[iChannel];
////     pad->Draw("f");
////     pad->Draw();
//  }
//}
//// -------------------------------------------------------------------------
ClassImp(CbmMuchSector)
