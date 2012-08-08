#include "CbmMuchSectorRectangular.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPad.h"
#include "TVector3.h"
#include "CbmMuchPadRectangular.h"

// -----   Default constructor   -------------------------------------------
CbmMuchSectorRectangular::CbmMuchSectorRectangular()
  : CbmMuchSector(),
    fPosition(),
    fSize(),
    fPadNx(0),
    fPadNy(0),
    fPadDx(0),
    fPadDy(0),
    fNeighbours()
{
}
// -------------------------------------------------------------------------


// -----  Standard constructor  --------------------------------------------
CbmMuchSectorRectangular::CbmMuchSectorRectangular(Int_t detId, Int_t secId, 
    TVector3 pos, TVector3 size, Int_t padNx, Int_t padNy)
  : CbmMuchSector(detId,secId,padNx*padNy),
    fPosition(pos),
    fSize(size),
    fPadNx(padNx),
    fPadNy(padNy),
    fPadDx(fSize[0]/padNx),
    fPadDy(fSize[1]/padNy),
    fNeighbours()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchSectorRectangular::AddPads(){
  for(Int_t ix = 0; ix < fPadNx; ix++){
    for(Int_t iy = 0; iy < fPadNy; iy++){
      Long64_t channelId = CbmMuchModuleGem::GetChannelId(fSectorId,fPadNy*ix+iy);
      // TODO
//      CbmMuchPad* pad = new CbmMuchPadRectangular(fDetectorId,channelId,fX1+ix*fPadDx,fY1+iy*fPadDy,fPadDx,fPadDy);
//      fPads.push_back(pad);
    }
  }
//  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
//    Long64_t channelId = CbmMuchModuleGem::GetChannelId(fSectorId,iChannel);
//    CbmMuchPad* pad = new CbmMuchPadRectangular(fDetectorId,channelId,fX1+iChannel*fPadDx,fY1+iChannel*fPadDy,fPadDx,fPadDy);
//    fPads.push_back(pad);
//  }
}
// -------------------------------------------------------------------------

vector<CbmMuchSectorRectangular*> CbmMuchSectorRectangular::GetNeighbours(){
  vector<CbmMuchSectorRectangular*> neighbours;
  // TODO
  return neighbours;
}

// -------------------------------------------------------------------------
void CbmMuchSectorRectangular::DrawPads(){
  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
     CbmMuchPadRectangular* pad = (CbmMuchPadRectangular*) fPads[iChannel];
     pad->DrawPad();
//     pad->Draw();
  }
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchSectorRectangular)
