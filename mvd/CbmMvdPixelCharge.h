// -------------------------------------------------------------------------
// -----                     CbmMvdDigi header file                    -----
// -----                    Created 02/04/08  by C.Dritsa              -----
// -------------------------------------------------------------------------



#ifndef CBMMVDPIXELCHARGE_H
#define CBMMVDPIXELCHARGE_H 1

#include "TObject.h"
#include "TMath.h"



class CbmMvdPixelCharge : public TObject
{

public:

    /** Default constructor **/
    CbmMvdPixelCharge();

    /** Constructor with all variables **/

    CbmMvdPixelCharge(Int_t charge, Int_t channelNrX, Int_t channelNrY);
    virtual ~CbmMvdPixelCharge();

    Bool_t TestXY(Int_t channelNrX,Int_t channelNrY);


    void DigestCharge(Float_t pointX, Float_t pointY);
    void AddCharge(Int_t charge){ fTrackCharge = fTrackCharge+charge; };


    Int_t    GetCharge()   { return fCharge;           };
    Int_t    GetX()        { return fChannelNrX;       };
    Int_t    GetY()        { return fChannelNrY;       };
    Float_t  GetPointX()   { return fDominatingPointX; };
    Float_t  GetPointY()   { return fDominatingPointY; };
    Int_t    GetMaxChargeContribution(){return fMaxChargeContribution;};
    Short_t  GetContributors(){return fContributors;};

    void SetCharge(Int_t charge){fCharge = charge;};
    void SetMaxChargeContribution(Int_t maxContribution){ fMaxChargeContribution = maxContribution; }
    void SetDominatorXY(Float_t pointX,Float_t pointY){
	fDominatingPointX = pointX;
	fDominatingPointY = pointY;
    };





private:

    Int_t fCharge;
    Int_t fMaxChargeContribution;
    Float_t fDominatingPointX;
    Float_t fDominatingPointY;
    Short_t fContributors;
    Int_t fChannelNrX;
    Int_t fChannelNrY;
    Int_t fTrackCharge;


    ClassDef(CbmMvdPixelCharge,1);

};


#endif

