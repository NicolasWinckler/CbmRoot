/** CbmMuchPad.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 10.11.07
 *@version 1.0
 **
 ** Class describing a standalone pad
 **/
#ifndef CBMMUCHPAD_H
#define CBMMUCHPAD_H 1

#include <map>
#include <set>

#include "TObject.h"
#include "TPolyLine.h"
#include "CbmMuchSector.h"

using std::map;
using std::pair;




class CbmMuchPad {

public:

  CbmMuchPad();
  CbmMuchPad (CbmMuchSector* sector, Int_t iChannel);

  ~CbmMuchPad();
  inline Int_t    GetDetectorId()  const {return fSector->GetDetectorId(); }
  inline Int_t    GetSystemId()    const {return fSector->GetSystemId();   }
  inline Int_t    GetStationNr()   const {return fSector->GetStationNr();  }
  inline Int_t    GetSectorNr()    const {return fSector->GetSectorNr();   }
  inline Double_t GetSectorX0()    const {return fSector->GetX0();}
  inline Double_t GetSectorY0()    const {return fSector->GetY0();}
  inline Double_t GetX0()          const {return fX0;}
  inline Double_t GetY0()          const {return fY0;}
  inline Int_t    GetChannel()     const {return fChannel;}
  inline Double_t GetDx()          const {return fSector->GetDx();}
  inline Double_t GetDy()          const {return fSector->GetDy();}
  inline TPolyLine GetPadPolygon() const {return fPadPolygon;}
  inline map<pair<Int_t, Int_t>, CbmMuchPad*> GetNeighbours() const {return fNeighbours;}

  /** Adds a pad to the list of neighbour pads of this one. **/
  void AddNeighbour(CbmMuchPad* pad);


private:
  CbmMuchSector*      fSector;            // Parent sector;
  Double_t            fX0, fY0;           // Coordinates of the pad center
  Int_t               fChannel;           // Channel number which pad corresponds to
  TPolyLine           fPadPolygon;        // TPolyLine for pad
  map<pair<Int_t, Int_t>, CbmMuchPad*> fNeighbours; // map of neighbour pads

};
#endif
