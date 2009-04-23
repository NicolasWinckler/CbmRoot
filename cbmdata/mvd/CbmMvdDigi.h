// -------------------------------------------------------------------------
// -----                     CbmMvdDigi header file                    -----
// -----                    Created 02/04/08  by C.Dritsa              -----
// -------------------------------------------------------------------------



#ifndef CBMMVDDIGI_H
#define CBMMVDDIGI_H 1

#include "CbmDetectorList.h"
#include "CbmDigi.h"
#include "CbmMvdDetectorId.h"

#include "TObject.h"
#include "TMath.h"
#include "TVector3.h"


class CbmMvdDigi : public CbmDigi, public CbmMvdDetectorId
{

public:

    /** Default constructor **/
    CbmMvdDigi();

    /** Constructor with all variables **/

    CbmMvdDigi(Int_t iStation, Int_t iChannelNrX, Int_t iChannelNrY, Int_t charge, Float_t pixelSizeX, 
    Float_t pixelSizeY, Float_t dominatorX, Float_t dominatorY, Short_t contributors, 
    Int_t maxChargeContribution);
    /**
     charge     : of each fired pixel in electrons
     PixelSize  : in cm
    */

    /** Destructor **/
    virtual ~CbmMvdDigi();

    /** Accessors **/
    Double_t GetCharge()   { return fCharge;     };
    Int_t    GetPixelX();
    Int_t    GetPixelY();
    Double_t GetPixelSizeX() { return  fPixelSizeX; };
    Double_t GetPixelSizeY() { return  fPixelSizeY; };
    Int_t    GetAdcCharge(Int_t adcDynamic, Int_t adcOffset, Int_t adcBits);
    Int_t    GetFlag(){return fDigiFlag;}
    Float_t  GetDominatorX(){return fDominatingPointX;}
    Float_t  GetDominatorY(){return fDominatingPointY;}
    Int_t    GetStationNr() { return StationNr(fDetectorId); }
 

    /** Modifiers **/
    void SetCharge(Int_t charge) { fCharge     = charge; };
    void SetPixelX(Int_t xIndex) { fChannelNrX = xIndex; };
    void SetPixelY(Int_t yIndex) { fChannelNrY = yIndex; };
    void SetPixelIndex(Int_t pixelIndex) { fChannelNr  = pixelIndex; };
    void SetDetectorId(Int_t detId)      { fDetectorId = detId;      };
    void SetFlag(Int_t flag)             { fDigiFlag   = flag;       }

private:


    Int_t fCharge;
    Int_t fChannelNrX;
    Int_t fChannelNrY;
    Float_t fPixelSizeX;
    Float_t fPixelSizeY;

    Int_t fDigiFlag; // Debugging and analysis information

    Int_t fMaxChargeContribution;
    Float_t fDominatingPointX;
    Float_t fDominatingPointY;
    Short_t fContributors;



    ClassDef(CbmMvdDigi,1);

};


#endif

