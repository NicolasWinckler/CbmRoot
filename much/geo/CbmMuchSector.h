/** CbmMuchSector.h
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@since   02.08.12
 *@version 2.0
 **
 ** This class describes the digitization scheme for a sector of the MuCh.
 ** The sector is a rectangle of size fLx and fLy.
 **
 **/
#ifndef CBMMUCHSECTOR_H
#define CBMMUCHSECTOR_H 1


#include "TObject.h"
#include <vector>
class CbmMuchPad;

using std::vector;

class CbmMuchSector: public TObject{
public:
  CbmMuchSector();
  CbmMuchSector(Int_t detId, Int_t secId, Int_t nChannels);
  virtual ~CbmMuchSector(){}
  Int_t GetDetectorId() const { return fDetectorId; }
  Int_t GetSectorId()   const { return fSectorId; }
  Int_t GetNChannels()  const { return fNChannels; }
  CbmMuchPad* GetPadByChannelIndex(Int_t iChannel) const { return fPads[iChannel]; }
//  virtual void GetPadVertices(Int_t iChannel, Double_t* xPad, Double_t* yPad) {}
  virtual void AddPads() {}
  virtual void DrawPads() {}
protected:
  Int_t      fDetectorId;    //  Detector ID (including module number)
  Int_t      fSectorId;      //  Sector index within the module
  Int_t      fNChannels;     //  Number of channels
  vector<CbmMuchPad*> fPads; //! Vector of pads

  ClassDef(CbmMuchSector,1);

};

#endif
