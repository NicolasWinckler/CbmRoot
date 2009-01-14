/**********************************************************************************
*    Class: CbmRichRingFitterEllipse                                              *
*    Description: This is the header of a particular ellipse fitting              *
*                 Here the ring is fitted with Taubin algorithm from              *
*                 Alexander Ayriyan, G. Ososkov, N. Chernov                       *
*                                                                                 *
*    Author : Alexander Ayriyan and Semen Lebedev                                *
*    E-mail : A.Ayriyan@gsi.de                                                    *
**********************************************************************************/

#ifndef CBM_RICH_RING_FITTER_ELLIPSE_TAU_H
#define CBM_RICH_RING_FITTER_ELLIPSE_TAU_H 1

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
//#define fN 6
#include "TH1D.h"
#include "TTime.h"
#include "TMath.h"
#include "TEllipse.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TMatrixDEigen.h"
#include "CbmRichRingFitter.h"

using namespace std;

class CbmRichRingFitterEllipseTau : public CbmRichRingFitter
{
public:

/***** Methods *****/

   /** Default constructor **/
   CbmRichRingFitterEllipseTau();

   /** Standard constructor **/
   CbmRichRingFitterEllipseTau(Int_t verbose,
                               Double_t correction,
                               TString fieldName);

   /** Destructor **/
   virtual ~CbmRichRingFitterEllipseTau();

   void DoFit1(CbmRichRing *pRing, vector<Double_t> hitX, vector<Double_t> hitY);

   /** Run Ring Fitting algorithm **/
   void DoFit(CbmRichRing *pRing);

   /** Using Ring Fitting algorithm **/
   void FittingEllipse(CbmRichRing *pRing);

   /** Transpose ellipse to canonical system of coordinates **/
   void TransposeEllipse(CbmRichRing *pRing);

   /** Initialization of Matrices **/
   void InitMatrices(void);

   /** Taubin algorithm **/
   TVectorD Taubin(void);

protected:
   virtual void CalcChi2(CbmRichRing* pRing);
   void MakeRadiusCorrection(CbmRichRing* pRing);
   void InitHistForRadiusCorrection();

private:

   Int_t fVerbose;
   Double_t fCorrection;

   TString fFieldName; ///specify correction map for different field
   TH1D* fh_mapaxisAXY;
   TH1D* fh_mapaxisBXY;

   vector<Double_t> fX;
   vector<Double_t> fY;
   TVectorD fAlgPar;
   TMatrixD fM;
   TMatrixD fB;
   TMatrixD fMB;

   ClassDef(CbmRichRingFitterEllipseTau,1);

};

#endif
