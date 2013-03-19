/**
 * \file CbmLitConverterParallel.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include "CbmLitConverterParallel.h"
#include "parallel/LitTrackParam.h"
#include "FairTrackParam.h"


CbmLitConverterParallel::CbmLitConverterParallel()
{

}

CbmLitConverterParallel::~CbmLitConverterParallel()
{

}

void CbmLitConverterParallel::FairTrackParamToLitTrackParamScal(
   const FairTrackParam* par,
   lit::parallel::LitTrackParamScal* lpar)
{
   lpar->X = par->GetX();
   lpar->Y = par->GetY();
   lpar->Tx = par->GetTx();
   lpar->Ty = par->GetTy();
   lpar->Qp = par->GetQp();
   lpar->Z = par->GetZ();
   Double_t cov[15];
   par->CovMatrix(cov);
   lpar->C0 = cov[0];
   lpar->C1 = cov[1];
   lpar->C2 = cov[2];
   lpar->C3 = cov[3];
   lpar->C4 = cov[4];
   lpar->C5 = cov[5];
   lpar->C6 = cov[6];
   lpar->C7 = cov[7];
   lpar->C8 = cov[8];
   lpar->C9 = cov[9];
   lpar->C10 = cov[10];
   lpar->C11 = cov[11];
   lpar->C12 = cov[12];
   lpar->C13 = cov[13];
   lpar->C14 = cov[14];
}

void CbmLitConverterParallel::LitTrackParamScalToFairTrackParam(
   const lit::parallel::LitTrackParamScal* lpar,
   FairTrackParam* par)
{
   par->SetX(lpar->X);
   par->SetY(lpar->Y);
   par->SetTx(lpar->Tx);
   par->SetTy(lpar->Ty);
   par->SetQp(lpar->Qp);
   par->SetZ(lpar->Z);
   Double_t cov[15];
   cov[0] = lpar->C0;
   cov[1] = lpar->C1;
   cov[2] = lpar->C2;
   cov[3] = lpar->C3;
   cov[4] = lpar->C4;
   cov[5] = lpar->C5;
   cov[6] = lpar->C6;
   cov[7] = lpar->C7;
   cov[8] = lpar->C8;
   cov[9] = lpar->C9;
   cov[10] = lpar->C10;
   cov[11] = lpar->C11;
   cov[12] = lpar->C12;
   cov[13] = lpar->C13;
   cov[14] = lpar->C14;
   par->SetCovMatrix(cov);
}

