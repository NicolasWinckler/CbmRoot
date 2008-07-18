// -------------------------------------------------------------------------
// -----                  CbmLitLineTrackExtrapolator source file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


#include "CbmLitLineTrackExtrapolator.h"


//constructors
CbmLitLineTrackExtrapolator::CbmLitLineTrackExtrapolator():
   CbmLitTrackExtrapolator("CbmLitLineTrackExtrapolator")
{
     

}

//Destructor
CbmLitLineTrackExtrapolator::~CbmLitLineTrackExtrapolator() 
{
   //
}

// Initialization
LitStatus CbmLitLineTrackExtrapolator::Initialize()
{
	return kLITSUCCESS;
}

// Finalization
LitStatus CbmLitLineTrackExtrapolator::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitLineTrackExtrapolator::Extrapolate(
		const CbmLitTrackParam *pParamIn,
        CbmLitTrackParam *pParamOut,
        Double_t zOut)
{ 
   *pParamOut = *pParamIn;
   return Extrapolate(pParamOut, zOut);
}

LitStatus CbmLitLineTrackExtrapolator::Extrapolate(
		CbmLitTrackParam *pParam, 
        Double_t zOut)
{
      Double_t X[5] = { pParam->GetX(), pParam->GetY(),
                        pParam->GetTx(), pParam->GetTy(),
                        pParam->GetQp()};
      
      Double_t dz = zOut - pParam->GetZ();

      //transport state vector F*X*F.T()
      X[0] = X[0] + dz * X[2];
      X[1] = X[1] + dz * X[3];
      
      pParam->SetX(X[0]);
      pParam->SetY(X[1]);

      std::vector<Double_t> C = pParam->GetCovMatrix();
      //transport covariance matrix F*C*F.T()
      Double_t t3 = C[2] + dz * C[9];
      Double_t t7 = dz * C[10];
      Double_t t8 = C[3] + t7;
      Double_t t19 = C[7] + dz * C[12];
      C[0] = C[0] + dz * C[2] + t3 * dz;
      C[1] = C[1] + dz * C[6] + t8 * dz;
      C[2] = t3;
      C[3] = t8;
      C[4] = C[4] + dz * C[11];
      C[5] = C[5] + dz * C[7] + t19 * dz;
      C[6] = C[6] + t7;
      C[7] = t19;
      C[8] = C[8] + dz * C[13];
      
      pParam->SetCovMatrix(C);
      pParam->SetZ(zOut);
      
      return kLITSUCCESS;
}

ClassImp(CbmLitLineTrackExtrapolator)


