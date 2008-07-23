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

class CbmMuchPad : public TPolyLine {

public:
  CbmMuchPad();
  CbmMuchPad (CbmMuchSector* sector, Int_t iChannel);
  ~CbmMuchPad();

  Long64_t GetDetectorId()  const {return fDetectorId; }
  Double_t GetSectorX0()    const ;
  Double_t GetSectorY0()    const ;
  Double_t GetX0()          const {return fX0;}
  Double_t GetY0()          const {return fY0;}
  Double_t GetLx()          const ;
  Double_t GetLy()          const ;
  void SetNeighbours(TArrayL64 neighbourIDs) {  fNeighbours = neighbourIDs; }
  vector<CbmMuchPad*> GetNeighbours();


  void Reset();
  void SetFired(Int_t digiId,Int_t charge,Int_t ADCcharge);
  void DrawPad();
  TString GetInfo();
  Int_t GetDigiIndex() { return fDigiIndex; }

private:
  Long64_t            fDetectorId;        // Unique ID of the pad
  Long64_t            fSectorId;          // Parent sector ID
  Double_t            fX0, fY0;           // Coordinates of the pad center
  TArrayL64           fNeighbours;        // Array of IDs of neighbour pads
  Bool_t              fFired;             // Defines whether pad is fired
  UInt_t              fCharge;            // Charge collected by the pad
  Int_t               fDigiIndex;         // Index of the corresponding CbmMuchDigi (if any)
  ClassDef(CbmMuchPad,1);
};
#endif
