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

    CbmMvdPixelCharge(Float_t charge, Int_t channelNrX, Int_t channelNrY, Int_t hitId,Int_t trackId);
    virtual ~CbmMvdPixelCharge();

    Bool_t TestXY(Int_t channelNrX,Int_t channelNrY);


    void DigestCharge(Float_t pointX, Float_t pointY, Int_t PointId, Int_t trackId);
    void AddCharge(Float_t charge){ fTrackCharge = fTrackCharge+charge; };


    Float_t    GetCharge()   { return fCharge;           };
    Int_t    GetX()        { return fChannelNrX;       };
    Int_t    GetY()        { return fChannelNrY;       };
    Float_t  GetPointX()   { return fDominatingPointX; };
    Float_t  GetPointY()   { return fDominatingPointY; };
    Float_t    GetMaxChargeContribution(){return fMaxChargeContribution;};
    Short_t  GetContributors(){return fContributors;};
    Int_t    GetTrackId()    {return fTrackId;}
    Int_t    GetPointId(){return fPointId;}

    void SetCharge(Float_t charge){fCharge = charge;};
    void SetMaxChargeContribution(Int_t maxContribution){ fMaxChargeContribution = maxContribution; }
    void SetDominatorXY(Float_t pointX,Float_t pointY){
	fDominatingPointX = pointX;
	fDominatingPointY = pointY;
    };





private:

    Float_t fCharge;
    Float_t fMaxChargeContribution;
    Float_t fDominatingPointX;
    Float_t fDominatingPointY;
    Short_t fContributors;
    Int_t fChannelNrX;
    Int_t fChannelNrY;
    Float_t fTrackCharge;
    Int_t fTrackId;
    Int_t fPointId;


    ClassDef(CbmMvdPixelCharge,1);

};


#endif

