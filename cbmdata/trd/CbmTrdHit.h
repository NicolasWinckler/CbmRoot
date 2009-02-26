#ifndef CBMTRDHIT_H
#define CBMTRDHIT_H

#include "CbmHit.h"

class TVector3;


class CbmTrdHit : public CbmHit {

private:
    Int_t           fPlaneID;      // unique ID of TRD plane in the setup (1 - 12)
    Double32_t      fELossTR;      // energy losses of electron via TR
    Double32_t      fELossdEdx;    // energy deposition in the detector without TR
    Double32_t      fELoss;        // TR + dEdx

public:
    CbmTrdHit();
    CbmTrdHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index,
	      Int_t planeID, Double_t eLossTR, Double_t eLossdEdx,
	      Double_t eLoss);
    virtual ~CbmTrdHit();
    void Print(const Option_t *option) const {}
    virtual Int_t GetStationNr() const { return fPlaneID; }
    inline Int_t    GetPlaneID()              { return fPlaneID; }
    inline Double_t GetELoss()                { return fELoss; }
    inline Double_t GetELossdEdX()            { return fELossdEdx; }
    inline Double_t GetELossTR()              { return fELossTR; }
    inline void SetPlaneID(Int_t plane)       { fPlaneID = plane; }
    inline void SetELoss(Double_t loss)       { fELoss = loss; }
    inline void SetELossTR(Double_t loss)     { fELossTR = loss; }
    inline void SetELossdEdx(Double_t loss)   { fELossdEdx = loss; }

    ClassDef(CbmTrdHit, 1)
};


#endif

