// -------------------------------------------------------------------------
// -----                     CbmTrdPoint header file                   -----
// -----                 Created 27/07/04  by V. Friese                -----
// -------------------------------------------------------------------------

/**  CbmTrdPoint.h
 *@author V. Friese
 **
 ** Interception of MC track with a TR detector.
 **/


#ifndef CBMTRDPOINT_H
#define CBMTRDPOINT_H 1


#include "FairMCPoint.h"
 
#include "TVector3.h"


class CbmTrdPoint : public FairMCPoint
{

public:

    /** Default constructor **/
    CbmTrdPoint();


    /** Constructor with arguments
     *@param trackID  Index of MCTrack
     *@param detID    Detector ID
     *@param posIn    Coordinates at entrance to active volume [cm]
     *@param posOut   Coordinates at exit of active volume [cm]
     *@param momIn    Momentum of track at entrance [GeV]
     *@param momOut   Momentum of track at exit [GeV]
     *@param tof      Time since event start [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     **/
    CbmTrdPoint(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 momIn,
		TVector3 posOut, TVector3 momOut, Double_t tof, 
                Double_t length, Double_t eLoss);


    /** Copy constructor **/
    CbmTrdPoint(const CbmTrdPoint& point) { *this = point; };


    /** Destructor **/
    virtual ~CbmTrdPoint();

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

    /** Accessors **/
    Double_t GetXIn()   const { return fX; }
    Double_t GetYIn()   const { return fY; }
    Double_t GetZIn()   const { return fZ; }
    Double_t GetXOut()  const { return fX_out; }
    Double_t GetYOut()  const { return fY_out; }
    Double_t GetZOut()  const { return fZ_out; }
    Double_t GetPxOut() const { return fPx_out; }
    Double_t GetPyOut() const { return fPy_out; }
    Double_t GetPzOut() const { return fPz_out; }
    Double_t GetPxIn()  const { return fPx; }
    Double_t GetPyIn()  const { return fPy; }
    Double_t GetPzIn()  const { return fPz; }
 
private:

    Double32_t fX_out,  fY_out,  fZ_out;
    Double32_t fPx_out, fPy_out, fPz_out;


    ClassDef(CbmTrdPoint,3)

};


#endif
