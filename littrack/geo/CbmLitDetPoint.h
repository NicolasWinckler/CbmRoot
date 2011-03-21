#ifndef CbmLitDetPoint_H
#define CbmLitDetPoint_H 1


#include "FairMCPoint.h"

#include "TObject.h"
#include "TVector3.h"

class CbmLitDetPoint : public FairMCPoint
{

public:

   /** Default constructor **/
   CbmLitDetPoint();


   /** Constructor with arguments
    *@param trackID  Index of MCTrack
    *@param detID    Detector ID
    *@param pos      Ccoordinates at entrance to active volume [cm]
    *@param mom      Momentum of track at entrance [GeV]
    *@param tof      Time since event start [ns]
    *@param length   Track length since creation [cm]
    *@param eLoss    Energy deposit [GeV]
    **/
//  CbmLitDetPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
//          Double_t tof, Double_t length, Double_t eLoss);

   CbmLitDetPoint(Int_t trackID, Int_t detID, TVector3 posIn,
                  TVector3 posOut, TVector3 momIn, TVector3 momOut,
                  Double_t tof, Double_t length, Double_t eLoss);

   /** Copy constructor **/
   CbmLitDetPoint(const CbmLitDetPoint& point) { *this = point; };


   /** Destructor **/
   virtual ~CbmLitDetPoint();

   Double_t GetXOut()        const { return fX_out; }
   Double_t GetYOut()        const { return fY_out; }
   Double_t GetZOut()        const { return fZ_out; }
   Double_t GetPxOut()       const { return fPx_out; }
   Double_t GetPyOut()       const { return fPy_out; }
   Double_t GetPzOut()       const { return fPz_out; }
   void PositionOut(TVector3& pos) { pos.SetXYZ(fX_out,fY_out,fZ_out); }
   void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx_out,fPy_out,fPz_out); }

   /** Output to screen **/
   virtual void Print(const Option_t* opt) const;

protected:

   Double32_t fX_out,  fY_out,  fZ_out;
   Double32_t fPx_out, fPy_out, fPz_out;

   ClassDef(CbmLitDetPoint,1)

};

#endif
