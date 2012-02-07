// -------------------------------------------------------------------------
// -----                CbmMvdCluster source file                  -----

// -------------------------------------------------------------------------

#include "CbmMvdCluster.h"
#include "TVector3.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdCluster::CbmMvdCluster() 
  : CbmMvdHit(), 
    fChargeArray(),
    fPixelSizeX(0.),
    fPixelSizeY(0.),
    fPointInfo(),
    fPointX(),
    fPointY(),
    fContributors(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdCluster::CbmMvdCluster(Int_t statNr, TVector3& pos, TVector3& dpos, Int_t flag, 
                             Short_t chargeArray[49], Double_t pixelSizeX, Double_t pixelSizeY)
  : CbmMvdHit(statNr, pos, dpos, flag), 
    fChargeArray(),
    fPixelSizeX(pixelSizeX),
    fPixelSizeY(pixelSizeY),
    fPointInfo(),
    fPointX(),
    fPointY(),
    fContributors(-1)
{

  //    fContributors=-1;

    for (Int_t i=0;i<49;i++){
	fChargeArray[i] = chargeArray[i];
	fPointInfo[i]   = 0;
    }

    for(Int_t i=0;i<5;i++){

	fPointX[i]=0;
	fPointY[i]=0;
    }

    //    fPixelSizeX = ;
    //    fPixelSizeY = ;


}
// -------------------------------------------------------------------------

// ----- SetDebuggingInfo --------------------------------------------------

void CbmMvdCluster::SetDebuggingInfo(Short_t pointInfo[49], Float_t pointX[5],Float_t pointY[5])
{
    for( Int_t i=0; i<49; i++ ){
	fPointInfo[i]=pointInfo[i];
    };

    for( Int_t i=0; i<5; i++ ){
	fPointX[i] = pointX[i];
	fPointY[i] = pointY[i];
    }

}

// -----   Destructor   ----------------------------------------------------
CbmMvdCluster::~CbmMvdCluster() {}
// -------------------------------------------------------------------------



ClassImp(CbmMvdCluster)
