// -----------------------------------------------------------------------
// -----               CbmMvdDigi source file                        -----
// -----              Created 17/04/08  by C. Dritsa                 -----
// -----------------------------------------------------------------------




#include <iostream>
#include "CbmMvdDigi.h"
#include "TMath.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdDigi::CbmMvdDigi()
  : CbmDigi(),
    CbmMvdDetectorId(),
    fDetectorId(0),
    fChannelNr(0),
    fCharge(0.),
    fChannelNrX(0),
    fChannelNrY(0),
    fTrackID(-1),
    fPointID(-1),
    fPixelSizeX(0.),
    fPixelSizeY(0.),
    fDigiFlag(0),
    fMaxChargeContribution(0.),
    fDominatingPointX(0.),
    fDominatingPointY(0.),
    fContributors(0)
{
}
// -------------------------------------------------------------------------



// -----   Constructor with parameters   -----------------------------------
CbmMvdDigi::CbmMvdDigi(Int_t iStation, Int_t iChannelNrX, Int_t iChannelNrY, Float_t charge,
                       Float_t pixelSizeX, Float_t pixelSizeY, Float_t dominatorX, Float_t dominatorY, Short_t contributors, Float_t maxChargeContribution, Int_t pointID, Int_t trackID)
  : CbmDigi(),
    CbmMvdDetectorId(),
    fDetectorId(0),
    fChannelNr(0),
    fCharge(charge),
    fChannelNrX(iChannelNrX),
    fChannelNrY(iChannelNrY),
    fTrackID(trackID),
    fPointID(pointID),
    fPixelSizeX(pixelSizeX),
    fPixelSizeY(pixelSizeY),
    fDigiFlag(-1),
    fMaxChargeContribution(maxChargeContribution),
    fDominatingPointX(dominatorX),
    fDominatingPointY(dominatorY),
    fContributors(contributors)
{
    // Check range for station
    if ( ! ( iStation >= 0 && iStation <= 255 ) ) {
	cout << "-E- CbmMvdDigi: Illegal station number " << iStation << endl;
	Fatal("", "Illegal station number");
    }
    fDetectorId = DetectorId(iStation);
    //    fChannelNrY = iChannelNrY;
}
// -------------------------------------------------------------------------

Int_t CbmMvdDigi::GetAdcCharge(Int_t adcDynamic, Int_t adcOffset, Int_t adcBits)
{
    /**
     adcOffset  is the minimum value of the analogue signal
     adcDynamic is the difference between the max and min values of the full scale measurement range
     adcBits    is the number of bits used to encode the analogue signal

     Exemple:
     * If full scale measurement range is from 15 to 20 Volts
     * adcDynamic is the difference 20 - 15 = 5
     * adcOffset  = 15

     */

     Int_t adcCharge;

    if(fCharge<adcOffset){return 0;};

   
    Double_t stepSize;
    Int_t adcMax = adcOffset + adcDynamic;

    stepSize  = adcDynamic/TMath::Power(2,adcBits);
    adcCharge = int( (fCharge-adcOffset)/stepSize );


    if ( adcCharge>int( TMath::Power(2,adcBits)-1 ) )
    {
	adcCharge = (int)TMath::Power(2,adcBits)-1;

    }

    if(gDebug>0){
	cout << "CbmMvdDigi::GetAdcCharge() "<< adcCharge<< endl;
    }

    return adcCharge;

}


// -------------------------------------------------------------------------
Int_t CbmMvdDigi::GetPixelX(){
 return  fChannelNrX;

}
// -------------------------------------------------------------------------
Int_t CbmMvdDigi::GetPixelY(){
  return  fChannelNrY;

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdDigi::~CbmMvdDigi(){}
// -------------------------------------------------------------------------

ClassImp(CbmMvdDigi)
