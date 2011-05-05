/** CbmMuchPad.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 10.11.07
 *@version 1.0
 **
 ** Class describing a standalone pad
 **/
#ifndef CBMMUCHPAD_H
#define CBMMUCHPAD_H 1

#include "TObject.h"
#include "TPolyLine.h"
#include "CbmMuchSector.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"

#include "TArrayL64.h"

class CbmMuchPad : public TPolyLine {

public:
  CbmMuchPad();
  /**
   * Standard constructor
   * @param sector   Sector which this pad belongs to
   * @param iChannel Channel index corresponding to this pad
   */
  CbmMuchPad (CbmMuchSector* sector, Int_t iChannel);
  ~CbmMuchPad();

  Int_t    GetDetectorId()     const {return fDetectorId; }
  Long64_t GetChannelId()      const {return fChannelId; }
  Double_t GetSectorX0()       const ;
  Double_t GetSectorY0()       const ;
  Double_t GetX0()             const {return fX0;}
  Double_t GetY0()             const {return fY0;}
  Double_t GetLx()             const ;
  Double_t GetLy()             const ;
  void SetNeighbours(TArrayL64 neighbourIDs) {  fNeighbours = neighbourIDs; }
  vector<CbmMuchPad*> GetNeighbours();
  Double_t GetTime()           const {return fTime; }
  Double_t GetCharge()         const {return fCharge; }
  CbmMuchDigi* GetDigi()       const {return fDigi; }
  CbmMuchDigiMatch* GetMatch() const {return fMatch; }
  
  void SetTime(Double_t time)   { fTime = time; }
  void AddCharge(Double_t time) { fTime = time; }
  
  void Reset();
  void SetFired(Int_t iDigi, Int_t ADCcharge, Int_t nADCChannels=256);
  void DrawPad();
  TString GetInfo();
  Int_t GetDigiIndex() { return fDigiIndex; }

private:
  Int_t               fDetectorId;        // Detector ID (including module number)
  Long64_t            fChannelId;         // Channel ID within the module
  Double_t            fX0, fY0;           // Coordinates of the pad center
  TArrayL64           fNeighbours;        // Array of channel IDs of neighbour pads
  Int_t               fDigiIndex;         // Index of the corresponding CbmMuchDigi (if any)
  Double_t            fCharge;
  Double_t            fTime;
  CbmMuchDigi*        fDigi;
  CbmMuchDigiMatch*   fMatch;
  ClassDef(CbmMuchPad,1);
};
#endif
