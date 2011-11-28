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
#include "CbmRichRingFitterEllipseBase.h"

using namespace std;

#define MAX_NOF_HITS 400

class CbmRichRingFitterEllipseTau : public CbmRichRingFitterEllipseBase
{
public:

   /** Default constructor **/
   CbmRichRingFitterEllipseTau();

   /** Standard constructor **/
   CbmRichRingFitterEllipseTau(
         Int_t verbose,
         Double_t correction,
         TString fieldName);

   /** Destructor **/
   virtual ~CbmRichRingFitterEllipseTau();

   void DoFit1(
         CbmRichRing *pRing,
         const vector<Double_t>& hitX,
         const vector<Double_t>& hitY);

   void DoFit(
         CbmRichRing *pRing);

	void TransEllipse(
	      CbmRichRing *pRing);

	void InitMatrices();

	void Taubin();

	void Inv5x5();

	void AMultB(
	      const Double_t * const ap,
	      Int_t na, Int_t ncolsa,
	      const Double_t * const bp,
	      Int_t nb,
	      Int_t ncolsb,
	      Double_t *cp);

	void Jacobi(
	      Double_t a[5][5],
	      Double_t d[5],
	      Double_t v[5][5]);

	void Eigsrt(
	      Double_t d[5],
	      Double_t v[5][5]);

private:

	Double_t fM[36];
	Double_t fP[25];
	Double_t fQ[25];
	Double_t fZ[MAX_NOF_HITS*6];
	Double_t fZT[MAX_NOF_HITS*6];

	Double_t fAlgPar[6];
	vector<Double_t> fX;
	vector<Double_t> fY;

   ClassDef(CbmRichRingFitterEllipseTau,1);

};

#endif
