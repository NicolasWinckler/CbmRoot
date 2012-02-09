/** CbmMuchModuleSector.cxx
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 1.0
 *@since   07.12.11
 *
 * This class holds geometry parameters of much sector-type modules 
 **/

#include "CbmMuchModuleSector.h"
#include "CbmMuchRadialSector.h"
#include "CbmMuchPad.h"
#include "CbmMuchGeoScheme.h"

#include "TMath.h"
#include "TPolyLine.h"
#include "TPave.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>

using std::cout;
using std::endl;
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmMuchModuleSector::CbmMuchModuleSector(): CbmMuchModule(),fDx1(0),fDx2(0),fDy(0),fDz(0) {
  fDetectorType = 3;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchModuleSector::CbmMuchModuleSector(Int_t detId, TVector3 position, TVector3 size,
    Double_t cutRadius)
: CbmMuchModule(detId, position, size, cutRadius),fDx1(0),fDx2(0),fDy(0),fDz(0) {
  fDetectorType = 3;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchModuleSector::CbmMuchModuleSector(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, TVector3 position, TVector3 size, 
    Double_t dx1, Double_t dx2, Double_t dy, Double_t dz, Double_t cutRadius)
:  CbmMuchModule(iStation, iLayer, iSide, iModule, position, size, cutRadius)
,fDx1(dx1),fDx2(dx2),fDy(dy),fDz(dz)
{
  fDetectorType = 3;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmMuchModuleSector::CbmMuchModuleSector(Int_t iStation, Int_t iLayer, Bool_t iSide,
    Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
:  CbmMuchModule(iStation, iLayer, iSide, iModule, position, size, cutRadius)
,fDx1(0),fDx2(0),fDy(0),fDz(0) {
  fDetectorType = 3;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchModuleSector::~CbmMuchModuleSector() {
}
// -------------------------------------------------------------------------

// -----   Public method AddSector   ---------------------------------------
void CbmMuchModuleSector::AddSector(CbmMuchRadialSector* sector) {
  fSectors.Add(sector);
}
// -------------------------------------------------------------------------

// -----   Public method GetPads -------------------------------------------
vector<CbmMuchPad*> CbmMuchModuleSector::GetPads() {
//  vector<CbmMuchPad*> pads;
//  for(Int_t iSector = 0; iSector < GetNSectors(); ++iSector){
//    CbmMuchSector* sector = GetSector(iSector);
//    if(!sector) continue;
//    for(Int_t iPad=0; iPad<sector->GetNChannels(); ++iPad){
//      CbmMuchPad* pad = sector->GetPad(iPad);
//      if(!pad) continue;
//      pads.push_back(pad);
//    }
//  }
//  return pads;
}
// -------------------------------------------------------------------------

// -----   Public method GetPad  -------------------------------------------
CbmMuchRadialPad* CbmMuchModuleSector::GetPad(Long64_t channelId) {
  CbmMuchRadialSector* sector = GetSector(channelId);
  Int_t iChannel = GetChannelIndex(channelId);
  return sector ? sector->GetPad(iChannel) : NULL;
}
// -------------------------------------------------------------------------

// -----   Public method GetNPads ------------------------------------------
Int_t CbmMuchModuleSector::GetNPads() {
  Int_t nChannels = 0;
  for(Int_t iSector=0; iSector < GetNSectors(); ++iSector){
    CbmMuchRadialSector* sector = GetSector(iSector);
    if(!sector) continue;
    nChannels += sector->GetNChannels();
  }
  return nChannels;
}
// -------------------------------------------------------------------------

// -----   Public method GetSector   ---------------------------------------
CbmMuchRadialSector* CbmMuchModuleSector::GetSector(Long64_t channelId) {
  Int_t iSector = GetSectorIndex(channelId);
  return (CbmMuchRadialSector*)fSectors.At(iSector);
}
// -------------------------------------------------------------------------


// -----   Public method GetSector   ---------------------------------------
CbmMuchRadialSector* CbmMuchModuleSector::GetSectorByRadius(Double_t r){
  vector<Double_t>::iterator i0,ie,i1; 
//  printf("size=%i\n",fSectorRadii.size());
  i0 = fSectorRadii.begin();
  ie = fSectorRadii.end();
  i1 = upper_bound(i0,ie,r);
  return i1-i0-1>=0 ? (CbmMuchRadialSector*) fSectors.At(i1-i0-1) : 0;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchModuleSector::DrawModule(Color_t color) {
  for (Int_t s=0;s<GetNSectors();s++){
    CbmMuchRadialSector* sector = GetSector(s);
    sector->SetFillColor(color);
    sector->Draw("f");
    sector->Draw();
  }
}


Bool_t CbmMuchModuleSector::InitModule(){
  for (Int_t s=0;s<GetNSectors();s++){
    CbmMuchRadialSector* sector = GetSector(s);
    if (!sector) continue;
    fSectorRadii.push_back(sector->GetR1());
    sector->AddPads();
  }

  return kTRUE;
}

ClassImp(CbmMuchModuleSector)
