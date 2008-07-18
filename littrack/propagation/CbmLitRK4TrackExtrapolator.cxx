 
// -------------------------------------------------------------------------
// -----                  CbmLitRK4TrackExtrapolator source file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
 


#include "CbmLitRK4TrackExtrapolator.h"
#include "CbmLitEnvironment.h"

#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TMath.h"

//constructors
CbmLitRK4TrackExtrapolator::CbmLitRK4TrackExtrapolator():
   CbmLitTrackExtrapolator("CbmLitRK4TrackExtrapolator"),
   fStepSizeMin(20.), // in cm
   fAccError(0.0005)  // in cm
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fMagneticField = env->GetField();
}

//Destructor
CbmLitRK4TrackExtrapolator::~CbmLitRK4TrackExtrapolator() 
{
   //
}

// Initialization
LitStatus CbmLitRK4TrackExtrapolator::Initialize()
{
	return kLITSUCCESS;
}

// Finalization
LitStatus CbmLitRK4TrackExtrapolator::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitRK4TrackExtrapolator::Extrapolate(
		const CbmLitTrackParam *pParamIn,
        CbmLitTrackParam *pParamOut,
        Double_t zOut)
{
   *pParamOut = *pParamIn;
   return Extrapolate(pParamOut, zOut);
}


LitStatus CbmLitRK4TrackExtrapolator::Extrapolate(
		CbmLitTrackParam *pParam, 
        Double_t zOut)
{
   Double_t zIn = pParam->GetZ();
   
   Double_t dz = TMath::Abs(zOut - zIn);
   if (dz < 0.01) {
      return kLITSUCCESS;
   }
   
   Double_t xIn[5] = { pParam->GetX(), pParam->GetY(),
                        pParam->GetTx(), pParam->GetTy(),
                        pParam->GetQp()};
      
   Double_t derivs[25];
   for (UInt_t i = 0; i < 25; i++) derivs[i] = 0.;
      
   Double_t xOut[5] = {0., 0., 0., 0., 0.};
     
   RK4Step(zIn , xIn , zOut, xOut, derivs);

   // protection from uncorrect values
   if (TMath::Abs(xOut[0]) > 1000.) xOut[0] = 1000.; 
   if (TMath::Abs(xOut[1]) > 1000.) xOut[1] = 1000.;
   if (TMath::Abs(xOut[2]) > 1000.) xOut[2] = 1000.;
   if (TMath::Abs(xOut[3]) > 1000.) xOut[3] = 1000.;
           
   pParam->SetX(xOut[0]);
   pParam->SetY(xOut[1]);
   pParam->SetTx(xOut[2]);
   pParam->SetTy(xOut[3]);
   pParam->SetQp(xOut[4]);
   
   std::vector<Double_t> covMat15 = pParam->GetCovMatrix();
   Double_t covMat25[25];
   Int_t index = 0;
   for (Int_t i = 0; i < 5; i++) {
      for (Int_t j = i; j < 5; j++) {
         covMat25[5*i + j] = covMat15[index];
         covMat25[5*j + i] = covMat15[index];
         index++;
      }
   }
   
   //transport the covariance matrix F*C*F.T()         
   TMatrixD F(5,5);
   F.SetMatrixArray(derivs, "F");
   TMatrixDSym C;
   C.Use(5, covMat25);
   C.Similarity(F);
   
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


//==============================================================================
// Forth order Runge-Kutta method for solution of the 
// equation of motion of the particle in the magnetic field.
// This function calculates calculates output state vector
// parameters and derivatives.
//==============================================================================
void CbmLitRK4TrackExtrapolator::RK4Order( 
                                  Double_t zIn ,
                                  Double_t xIn[] ,
                                  Double_t zOut,
                                  Double_t xOut[],
                                  Double_t derivs[])
{
   const Double_t fC = 0.000299792458;
   
   Double_t zCoef[4] = { 0.0, 0.5, 0.5, 1.0 };
   Double_t c[4] = { 1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0 };
   Double_t estCoef[4] = { 0.0, 0.5, 0.5, 1.0 };
   
   Double_t Ax[4], Ay[4];
   Double_t dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
   Double_t k[4][4];   
   
   Double_t h = zOut - zIn;
   Double_t hC   = h * fC;
   Double_t hCqp = h * fC * xIn[4];
   Double_t x0[4];
      
   Double_t x[4] = { xIn[0], xIn[1], xIn[2], xIn[3] };

   for (UInt_t iStep = 0; iStep < 4; iStep++) { // 1
      
      if (iStep > 0) {
         for(UInt_t i = 0; i < 4; i++) {
            x[i] = xIn[i] + estCoef[iStep] * k[iStep - 1][i];
         }
      }
      
      Double_t pos[3] = { x[0], x[1], zIn + zCoef[iStep] * h };
      Double_t B[3];     
      fMagneticField->GetFieldValue(pos, B);
   
      Double_t tx = x[2];
      Double_t ty = x[3]; 
      Double_t txtx = tx * tx;
      Double_t tyty = ty * ty;
      Double_t txty = tx * ty;
      Double_t txtxtyty1= 1.0 + txtx + tyty;
      Double_t t1 = TMath::Sqrt(txtxtyty1);
      Double_t t2 = 1.0 / txtxtyty1;
     
      Ax[iStep] = ( txty * B[0] + ty * B[2] - ( 1.0 + txtx ) * B[1] ) * t1;
      Ay[iStep] = (-txty * B[1] - tx * B[2] + ( 1.0 + tyty ) * B[0] ) * t1;

      dAx_dtx[iStep] = Ax[iStep] * tx * t2 + ( ty * B[0] - 2.0 * tx * B[1] ) * t1;
      dAx_dty[iStep] = Ax[iStep] * ty * t2 + ( tx * B[0] + B[2] ) * t1;
      dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * B[1] - B[2] ) * t1;
      dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * B[1] + 2.0 * ty * B[0] ) * t1;

      k[iStep][0] = tx * h;
      k[iStep][1] = ty * h;
      k[iStep][2] = Ax[iStep] * hCqp;
      k[iStep][3] = Ay[iStep] * hCqp;

   } // 1

   for (UInt_t i = 0; i < 4; i++) {
      xOut[i] = xIn[i] + c[0] * k[0][i] + c[1] * k[1][i] + 
                 c[2] * k[2][i] + c[3] * k[3][i];
   }
   xOut[4] = xIn[4];
   
   // Calculation of the derivatives 
   
   // derivatives dx/dx and dx/dy
   for (UInt_t i = 0; i < 10; i++){ 
      derivs[i] = 0.;
   }
   derivs[0] = 1.; 
   derivs[6] = 1.;
   // end of derivatives dx/dx and dx/dy
      
   // Derivatives dx/tx
   x[0] = x0[0] = 0.0; 
   x[1] = x0[1] = 0.0; 
   x[2] = x0[2] = 1.0; 
   x[3] = x0[3] = 0.0; 

   for (UInt_t iStep = 0; iStep < 4; iStep++) { // 2
      
      if (iStep > 0) {
         for (UInt_t i = 0; i < 4; i++) {
            if (i != 2) {
               x[i] = x0[i] + estCoef[iStep] * k[iStep - 1][i];
            }
         }
      }
      
      k[iStep][0] = x[2] * h;
      k[iStep][1] = x[3] * h;
      //k[iStep][2] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      k[iStep][3] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;

   } // 2

   for (UInt_t i = 0; i < 4; i++) {
      if(i != 2) {
         derivs[10 + i] = x0[i] + c[0] * k[0][i] + c[1] * k[1][i] + 
                           c[2] * k[2][i] + c[3] * k[3][i];
      }
   }
   derivs[12] = 1.0;
   derivs[14] = 0.0;
   // end of derivatives dx/dtx
   
   // Derivatives    dx/ty
   x[0] = x0[0] = 0.0; 
   x[1] = x0[1] = 0.0; 
   x[2] = x0[2] = 0.0; 
   x[3] = x0[3] = 1.0; 
   
   for (UInt_t iStep = 0; iStep < 4; iStep++) { // 4
      
      if (iStep > 0) {
         for(UInt_t i = 0; i < 4; i++) {
            if(i!=3) {
               x[i] = x0[i] + estCoef[iStep] * k[iStep - 1][i];
            }
         }
      }
      
      k[iStep][0] = x[2] * h;
      k[iStep][1] = x[3] * h;
      k[iStep][2] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      //k[iStep][3] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   }  // 4

   for (UInt_t i = 0; i < 3; i++ ) { 
      derivs[15 + i] = x0[i] + c[0] * k[0][i] + c[1] * k[1][i] + 
                        c[2] * k[2][i] + c[3] * k[3][i];
   }
   derivs[18] = 1.;
   derivs[19] = 0.;
   // end of derivatives dx/dty
   
   // Derivatives dx/dqp
   x[0] = x0[0] = 0.0; 
   x[1] = x0[1] = 0.0; 
   x[2] = x0[2] = 0.0; 
   x[3] = x0[3] = 0.0;   

   for (UInt_t iStep = 0; iStep < 4; iStep++) { // 4
      
      if (iStep > 0) {
         for(UInt_t i = 0; i < 4; i++) {
            x[i] = x0[i] + estCoef[iStep] * k[iStep - 1][i];
         }
      }      
      
      k[iStep][0] = x[2] * h;
      k[iStep][1] = x[3] * h;
      k[iStep][2] = Ax[iStep] * hC + 
                     hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
      k[iStep][3] = Ay[iStep] * hC + 
                     hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
   }  // 4
      
   for (UInt_t i = 0; i < 4; i++ ) {
      derivs[20 + i] = x0[i] + c[0] * k[0][i] + c[1] * k[1][i] + 
                        c[2] * k[2][i] + c[3] * k[3][i];
   }
   derivs[24] = 1.;
   // end of derivatives dx/dqp
    
   // end calculation of the derivatives
}


//==============================================================================
// Forth order Runge-Kutta method for solution of the 
// equation of motion of the particle in the magnetic field.
// This function calculates only parameters of the state
// vector without derivatives.
//==============================================================================
void CbmLitRK4TrackExtrapolator::RK4Fast(                
                                  Double_t zIn ,    
                                  Double_t xIn[],   
                                  Double_t zOut,    
                                  Double_t xOut[])  
{
   const Double_t fC = 0.000299792458;
   
   Double_t zCoef[4] = { 0.0, 0.5, 0.5, 1.0 };
   Double_t c[4] = { 1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0 };
   Double_t estCoef[4] = { 0.0, 0.5, 0.5, 1. };
  
   Double_t h = zOut - zIn;
   Double_t hCqp = h * fC * xIn[4];
   Double_t k[4][4];
   Double_t x[4] = { xIn[0], xIn[1], xIn[2], xIn[3] };
   
   for (UInt_t iStep = 0; iStep < 4; iStep++) { // 1
      
      if(iStep > 0) {
         for(UInt_t i = 0; i < 4; i++) {
            x[i] = xIn[i] + estCoef[iStep] * k[iStep - 1][i];
         }
      }
   
      Double_t pos[3] = { x[0], x[1], zIn + zCoef[iStep] * h };
      Double_t B[3];     
      fMagneticField->GetFieldValue(pos, B);
   
      Double_t tx = x[2]; 
      Double_t ty = x[3]; 
      Double_t txtx = tx * tx;
      Double_t tyty = ty * ty;
      Double_t txty = tx * ty;
      Double_t t1 =  TMath::Sqrt( 1.0 + txtx + tyty ) *  hCqp;
        
      k[iStep][0] = tx * h;
      k[iStep][1] = ty * h;
      k[iStep][2] = ( txty * B[0] + ty * B[2] - ( 1.0 + txtx ) * B[1] ) * t1;
      k[iStep][3] = (-txty * B[1] - tx * B[2] + ( 1.0 + tyty ) * B[0] ) * t1;

   } // 1
   
   for(UInt_t i = 0; i < 4; i++) {
      xOut[i] = xIn[i] + c[0] * k[0][i] + c[1] * k[1][i] + 
                 c[2] * k[2][i] + c[3] * k[3][i];
   }
   xOut[4] = xIn[4];
}    

//==============================================================================
// Forth order Runge-Kutta method for solution of the 
// equation of motion of the particle in the magnetic field.
// This function uses adaptive step size control to 
// calculate parameters of the state vector and derivatives.
//==============================================================================
void CbmLitRK4TrackExtrapolator::RK4Step( 
                                  Double_t zIn,
                                  Double_t xIn[],
                                  Double_t zOut,
                                  Double_t xOut[],
                                  Double_t derivs[])
{
  
   RK4Order(zIn, xIn, zOut, xOut, derivs);
   
   Bool_t isErrorAcceptable;
   Bool_t isStepTooSmall = TMath::Abs(zOut - zIn) < fStepSizeMin;
   
   if (!isStepTooSmall) { // 1
         
      Double_t nofSteps = 2.0;
      
      do {
         Double_t h = (zOut - zIn) / nofSteps;
         
         Double_t x1[] = {xIn[0], xIn[1], xIn[2], xIn[3], xIn[4]};
         Double_t x2[5];
         Double_t z1, z2;
         for (Int_t iStep = 0; iStep < nofSteps; iStep++) {
            z1 = zIn + iStep * h;
            z2 = zIn + (iStep + 1) * h;
            
            RK4Fast(z1, x1, z2, x2);
            
            for (UInt_t i = 0; i < 5; i++) x1[i] = x2[i];
         }
         
         isErrorAcceptable = (TMath::Abs(x2[0] - xOut[0])  < fAccError)  ||
                           (TMath::Abs(x2[1] - xOut[1])  < fAccError) ;
         isStepTooSmall = (TMath::Abs(h) < fStepSizeMin);
         
         for (UInt_t i = 0; i < 5; i++) xOut[i] = x2[i];
         
         nofSteps = nofSteps * 2.; 
         
         //cout << "iter = " << iter++ << endl;
         
      } while (!isErrorAcceptable && !isStepTooSmall);
    } // 1
}

ClassImp(CbmLitRK4TrackExtrapolator)


