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

CbmMuchPadRectangular* CbmMuchSectorRectangular::GetPad(Double_t x, Double_t y){
  Int_t ix = Int_t(GetXmin()/fPadDx);
  Int_t iy = Int_t(GetYmin()/fPadDy);
  if (ix<0 || ix >=fPadNx) return NULL;
  if (iy<0 || iy >=fPadNy) return NULL;
  Int_t channelIndex = fPadNy*ix+iy;
  return (CbmMuchPadRectangular*) fPads[channelIndex];
}


// -------------------------------------------------------------------------
void CbmMuchSectorRectangular::AddPads(){
  for(Int_t ix = 0; ix < fPadNx; ix++){
    for(Int_t iy = 0; iy < fPadNy; iy++){
      Int_t channelIndex = fPadNy*ix+iy;
      Long64_t channelId = CbmMuchModuleGem::GetChannelId(fSectorId,channelIndex);
      Double_t x0 = GetXmin()+(ix+0.5)*fPadDx;
      Double_t y0 = GetYmin()+(iy+0.5)*fPadDy;
      CbmMuchPad* pad = new CbmMuchPadRectangular(fDetectorId,channelId,x0,y0,fPadDx,fPadDy);
      fPads.push_back(pad);
    }
  }
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
