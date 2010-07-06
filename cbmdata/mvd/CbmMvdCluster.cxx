// -------------------------------------------------------------------------
// -----                CbmMvdCluster source file                  -----

// -------------------------------------------------------------------------

#include "CbmMvdCluster.h"

#include "TVector3.h"

// -----   Default constructor   -------------------------------------------
CbmMvdCluster::CbmMvdCluster() {

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdCluster::CbmMvdCluster(Int_t statNr, TVector3& pos, TVector3& dpos, Int_t flag, 
                             Short_t chargeArray[49], Double_t pixelSizeX, Double_t pixelSizeY)
:CbmMvdHit(statNr, pos, dpos, flag) {

    fContributors=-1;

    for (Int_t i=0;i<49;i++){
	fChargeArray[i] = chargeArray[i];
	fPointInfo[i]   = 0;
    }

    for(Int_t i=0;i<5;i++){

	fPointX[i]=0;
	fPointY[i]=0;
    }

    fPixelSizeX = pixelSizeX;
    fPixelSizeY = pixelSizeY;


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
