// -------------------------------------------------------------------------
// -----                     CbmVisHit source file                     -----
// -----                  Created 14/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmVisHit.h"
#include "CbmHit.h"
#include "CbmMCTrack.h"



// -------------------------------------------------------------------------
CbmVisHit::CbmVisHit(CbmHit* hit):TMarker(hit->GetX(),hit->GetY(),21)
{
    fDefaultSize = 0.5;
    SetMarkerSize(fDefaultSize);
    SetMarkerColor(kBlack);
    fHit = hit;
    fPointId   = -1;
    fTrackId   = -1;
    fTrackPdg  = 0;
    fMotherId  = -2;
    fMotherPdg = 0;
    fTrack     = NULL;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisHit::SetTrack(CbmMCTrack* track){
    fTrack    = track;
    fTrackPdg = track->GetPdgCode();
    fMotherId = track->GetMotherId();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
TString CbmVisHit::GetInfo(){
  return Form("Hit: (%.2f,%.2f) pdg=%i MoId=%i MoPdg=%i",
      	      fX,fY,fTrackPdg,fMotherId,fMotherPdg);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisHit::SetVisible(Bool_t isVisible){
  if (isVisible) SetMarkerSize(fDefaultSize);
  else           SetMarkerSize(0);
  fVisible = isVisible;
}
// -------------------------------------------------------------------------
ClassImp(CbmVisHit)
