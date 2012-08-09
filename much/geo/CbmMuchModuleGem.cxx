/** CbmMuchModuleGem.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one side of MuCh module.
 **/

#include "CbmMuchModuleGem.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModule.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"


#include <vector>
using std::vector;

// -------------------------------------------------------------------------
CbmMuchModuleGem::CbmMuchModuleGem()
  : CbmMuchModule(),
    fSectors()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchModuleGem::CbmMuchModuleGem(Int_t iStation, Int_t iLayer, Bool_t iSide,
    Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
  : CbmMuchModule(iStation, iLayer, iSide, iModule, position, size, cutRadius),
    fSectors()
{
}
// -------------------------------------------------------------------------


// -----   Public method GetPads -------------------------------------------
vector<CbmMuchPad*> CbmMuchModuleGem::GetPads() {
  vector<CbmMuchPad*> pads;
  for(Int_t iSector = 0; iSector < GetNSectors(); ++iSector){
    CbmMuchSector* sector = GetSector(iSector);
    if(!sector) continue;
    for(Int_t iPad=0; iPad<sector->GetNChannels(); ++iPad){
      CbmMuchPad* pad = sector->GetPad(iPad);
      if(!pad) continue;
      pads.push_back(pad);
    }
  }
  return pads;
}
// -------------------------------------------------------------------------


// -----   Public method GetPad  -------------------------------------------
CbmMuchPad* CbmMuchModuleGem::GetPad(Long64_t channelId) {
  CbmMuchSector* sector = GetSector(channelId);
  Int_t iChannel = GetChannelIndex(channelId);
  return sector ? sector->GetPad(iChannel) : NULL;
}
// -------------------------------------------------------------------------


// -----   Public method GetNPads ------------------------------------------
Int_t CbmMuchModuleGem::GetNPads() {
  Int_t nChannels = 0;
  for(Int_t iSector=0; iSector < GetNSectors(); ++iSector){
    CbmMuchSector* sector = GetSector(iSector);
    if(!sector) continue;
    nChannels += sector->GetNChannels();
  }
  return nChannels;
}
// -------------------------------------------------------------------------


// -----   Public method GetSector   ---------------------------------------
CbmMuchSector* CbmMuchModuleGem::GetSector(Long64_t channelId) {
  Int_t iSector = GetSectorIndex(channelId);
  return (CbmMuchSector*)fSectors[iSector];
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchModuleGem::DrawModule(Color_t color) {
  for (Int_t s=0;s<fSectors.size();s++){
    CbmMuchSector* sector = (CbmMuchSector*) fSectors[s];
// TODO
//    sector->SetFillColor(color);
//    sector->Draw("f");
//    sector->Draw();
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchModuleGem::DrawPads() {
  for (Int_t s=0;s<fSectors.size();s++){
    CbmMuchSector* sector = (CbmMuchSector*) fSectors[s];
    sector->DrawPads();
  }
}
// -------------------------------------------------------------------------


void CbmMuchModuleGem::SetPadFired(Long64_t channelId,Int_t digiIndex,Int_t adcCharge){
  CbmMuchSector* sector = GetSector(channelId);
  CbmMuchPad* pad = sector->GetPad(CbmMuchModuleGem::GetChannelIndex(channelId));
  pad->SetFired(digiIndex,adcCharge);
}
ClassImp(CbmMuchModuleGem)
