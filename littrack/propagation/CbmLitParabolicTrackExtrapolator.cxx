 
 
// -------------------------------------------------------------------------
// -----                  CbmLitParabolicTrackExtrapolator source file               -----
// -----                  Created 30/10/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
 


#include "CbmLitParabolicTrackExtrapolator.h"
#include "CbmLitEnvironment.h"

#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TMath.h"

//constructors
CbmLitParabolicTrackExtrapolator::CbmLitParabolicTrackExtrapolator():
   CbmLitTrackExtrapolator("CbmLitParabolicTrackExtrapolator")
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fMagneticField = env->GetField();
}

//Destructor
CbmLitParabolicTrackExtrapolator::~CbmLitParabolicTrackExtrapolator() 
{
   //
}

// Initialization
LitStatus CbmLitParabolicTrackExtrapolator::Initialize()
{
	return kLITSUCCESS;
}

// Finalization
LitStatus CbmLitParabolicTrackExtrapolator::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitParabolicTrackExtrapolator::Extrapolate(
		const CbmLitTrackParam *pParamIn,
        CbmLitTrackParam *pParamOut,
        Double_t zOut)
{
   *pParamOut = *pParamIn;
   return Extrapolate(pParamOut, zOut);
}


LitStatus CbmLitParabolicTrackExtrapolator::Extrapolate(
		CbmLitTrackParam *pParam, 
        Double_t zOut)
{
    
   const Double_t fC = 0.000299792458;
   
   Double_t zIn = pParam->GetZ();
   
   Double_t dz = zOut - zIn;
   if (TMath::Abs(dz) < 0.01) {
      return kLITSUCCESS;
   }
     
   Double_t xIn[5] = { pParam->GetX(), pParam->GetY(),
                       pParam->GetTx(), pParam->GetTy(),
                       pParam->GetQp()};
                       
   Double_t pos[3] = { xIn[0], xIn[1], zIn };
   Double_t B[3];     
   fMagneticField->GetFieldValue(pos, B);
   
   Double_t tx = xIn[2];
   Double_t ty = xIn[3]; 
   Double_t txtx = tx * tx;
   Double_t tyty = ty * ty;
   Double_t txty = tx * ty;
   Double_t norm = TMath::Sqrt(1.0 + txtx + tyty);
 
   Double_t Ax = ( txty * B[0] + ty * B[2] - ( 1.0 + txtx ) * B[1] ) * norm;
   Double_t Ay = (-txty * B[1] - tx * B[2] + ( 1.0 + tyty ) * B[0] ) * norm;

   Double_t xOut[5] = {0., 0., 0., 0., 0.};
   
   // Extrapolate state  
   Double_t t1 = xIn[4] * fC * dz;
   Double_t xt = Ax * t1;
   Double_t yt = Ay * t1;
   xOut[0] = xIn[0] + dz * ( tx + 0.5 * xt);
   xOut[1] = xIn[1] + dz * ( ty + 0.5 * yt);
   xOut[2] = xIn[2] + xt;
   xOut[3] = xIn[3] + yt;  
   xOut[4] = xIn[4];
   
   
   // calculate the transport matrix
   Double_t F[25] = { 1, 0, 0, 0, 0,
                      0, 1, 0, 0, 0,
                      0, 0, 1, 0, 0,
                      0, 0, 0, 1, 0,
                      0, 0, 0, 0, 1 };
   Double_t norm2 = norm * norm;
   
   Double_t dAx_dtx = Ax * tx / norm2 + ( ty * B[0] - 2.0 * tx * B[1] ) * norm;
   Double_t dAx_dty = Ax * ty / norm2 + ( tx * B[0] + B[2] ) * norm;
   Double_t dAy_dtx = Ay * tx / norm2 + (-ty * B[1] - B[2] ) * norm;
   Double_t dAy_dty = Ay * ty / norm2 + (-tx * B[1] + 2.0 * ty * B[0] ) * norm;

   F[2] = dz + 0.5 * dAx_dtx * xIn[4] * fC * dz * dz;
   F[3] = 0.5 * dAx_dty * xIn[4] * fC * dz * dz;
   F[4] = 0.5 * Ax * fC * dz * dz;
   
   F[7] = 0.5 * dAy_dtx * xIn[4] * fC * dz * dz;
   F[8] = dz + 0.5 * dAy_dty * xIn[4] * fC * dz * dz;
   F[9] = 0.5 * Ay * fC * dz * dz;
   
   F[12] = 1.0 + dAx_dtx * xIn[4] * fC * dz;
   F[13] = dAx_dty * xIn[4] * fC * dz;
   F[14] = Ax * fC * dz;
   
   F[17] = dAy_dtx * xIn[4] * fC * dz;
   F[18] = 1.0 + dAy_dty * xIn[4] * fC * dz;
   F[19] = Ay * fC * dz;
   
   // end calculation of the transport matrix
   
   
   pParam->SetX(xOut[0]);
   pParam->SetY(xOut[1]);
   pParam->SetTx(xOut[2]);
   pParam->SetTy(xOut[3]);
   pParam->SetQp(xOut[4]);
   
   std::vector <Double_t> covMat15 = pParam->GetCovMatrix();
   std::vector <Double_t> covMat25(25);
   Int_t index = 0;
   for (Int_t i = 0; i < 5; i++) {
      for (Int_t j = i; j < 5; j++) {
         covMat25[5*i + j] = covMat15[index];
         covMat25[5*j + i] = covMat15[index];
         index++;
      }
   }
      
   //transport the covariance matrix F*C*F.T()         
   TMatrixD Fm(5,5);
   Fm.SetMatrixArray(F);//, "F");
   TMatrixDSym C;
   C.Use(5, &covMat25[0]);
   C.Similarity(Fm);
   
   index = 0;
   for (Int_t i = 0; i < 5; i++) {
      for (Int_t j = i; j < 5; j++) {
         covMat15[index]= covMat25[5*i + j];
         index++;
      }
   }
   pParam->SetCovMatrix(covMat15);
   pParam->SetZ(zOut);  
   
   
   return kLITSUCCESS;
}

ClassImp(CbmLitParabolicTrackExtrapolator)


