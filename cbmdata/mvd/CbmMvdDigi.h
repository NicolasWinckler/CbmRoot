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

    CbmMvdDigi(Int_t iStation, Int_t iChannelNrX, Int_t iChannelNrY, Float_t charge, Float_t pixelSizeX,
    Float_t pixelSizeY, Float_t dominatorX, Float_t dominatorY, Short_t contributors, 
    Float_t maxChargeContribution, Int_t pointID, Int_t trackID);
    /**
     charge     : of each fired pixel in electrons
     PixelSize  : in cm
    */

    /** Destructor **/
    virtual ~CbmMvdDigi();

    /** Accessors **/
    Float_t GetCharge()   { return fCharge;     };
    Int_t    GetPixelX();
    Int_t    GetPixelY();
    Double_t GetPixelSizeX() { return  fPixelSizeX; };
    Double_t GetPixelSizeY() { return  fPixelSizeY; };
    Int_t    GetAdcCharge(Int_t adcDynamic, Int_t adcOffset, Int_t adcBits);
    Int_t    GetFlag(){return fDigiFlag;}
    Float_t  GetDominatorX(){return fDominatingPointX;}
    Float_t  GetDominatorY(){return fDominatingPointY;}
    Int_t    GetStationNr() { return StationNr(fDetectorId); }
    Int_t    GetTrackID(){return fTrackID;}
    Int_t    GetPointID(){return fPointID;}

    /** Abstract methods from base class **/
    Int_t GetAddress() const { return fDetectorId; }
    Double_t GetTime() const { return 0.; }

    /** Former methods from base class **/
    Int_t GetDetectorId() const { return fDetectorId; }
    Int_t GetChannelNr() const { return fChannelNr; }
    Int_t GetSystemId() const { return kMVD; }
   
 

    /** Modifiers **/
    void SetCharge(Float_t charge) { fCharge     = charge; };
    void SetPixelX(Int_t xIndex) { fChannelNrX = xIndex; };
    void SetPixelY(Int_t yIndex) { fChannelNrY = yIndex; };
    void SetPixelIndex(Int_t pixelIndex) { fChannelNr  = pixelIndex; };
    void SetDetectorId(Int_t detId)      { fDetectorId = detId;      };
    void SetFlag(Int_t flag)             { fDigiFlag   = flag;       }

private:

    Int_t fDetectorId;
    Int_t fChannelNr;


    Float_t fCharge;
    Int_t fChannelNrX;
    Int_t fChannelNrY;
    Int_t fTrackID;
    Int_t fPointID;
    Float_t fPixelSizeX;
    Float_t fPixelSizeY;

    Int_t fDigiFlag; // Debugging and analysis information

    Float_t fMaxChargeContribution;
    Float_t fDominatingPointX;
    Float_t fDominatingPointY;
    Short_t fContributors;
   



    ClassDef(CbmMvdDigi,1);

};


#endif

