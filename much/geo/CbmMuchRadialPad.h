/** CbmMuchRadialPad.h
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 6.02.12
 *@version 1.0
 **
 ** Class describing a single radial pad
 **/
#ifndef CBMMUCHRADIALPAD_H
#define CBMMUCHRADIALPAD_H 1

#include "TCrown.h"
#include "CbmMuchRadialSector.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"

#include "TArrayL64.h"

class CbmMuchRadialPad : public TCrown {

public:
  CbmMuchRadialPad();
  /**
   * Standard constructor
   * @param sector   Sector which this pad belongs to
   * @param iChannel Channel index corresponding to this pad
   */
  CbmMuchRadialPad (CbmMuchRadialSector* sector, Int_t iChannel);
  ~CbmMuchRadialPad();

  Int_t    GetDetectorId()     const {return fDetectorId; }
  Long64_t GetChannelId()      const {return fChannelId; }
  void SetNeighbours(TArrayL64 neighbourIDs) {  fNeighbours = neighbourIDs; }
  vector<CbmMuchRadialPad*> GetNeighbours();
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
  TArrayL64           fNeighbours;        // Array of channel IDs of neighbour pads
  Int_t               fDigiIndex;         // Index of the corresponding CbmMuchDigi (if any)
  Double_t            fCharge;
  Double_t            fTime;
  CbmMuchDigi*        fDigi;
  CbmMuchDigiMatch*   fMatch;

  CbmMuchRadialPad(const CbmMuchRadialPad&);
  CbmMuchRadialPad& operator=(const CbmMuchRadialPad&);

  ClassDef(CbmMuchRadialPad,1);
};
#endif
