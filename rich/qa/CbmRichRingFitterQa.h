#ifndef CbmRichRingFitterQa_h
#define CbmRichRingFitterQa_h

#include "TObject.h"
#include <vector>
#include "TMatrixD.h"
class TH1D;
class CbmRichRing;
//class TMatrixD;

using std::vector;

class CbmRichRingFitterQa: public TObject {
public:

 	CbmRichRingFitterQa();

	void GenerateEllipse();

	void Draw();

private:

	void CalculateFitErrors(
	      CbmRichRing* ring,
	      const vector<Double_t>& xh,
	      const vector<Double_t>& yh,
	      Double_t sigma,
	      TMatrixD& cov);

	// ellipse fitting algorithm, errors
	TH1D* fhErrorA;
	TH1D* fhErrorB;
	TH1D* fhErrorX;
	TH1D* fhErrorY;
	TH1D* fhErrorPhi;
	// ellipse fitting algorithm, parameters
	TH1D* fhA;
	TH1D* fhB;
	TH1D* fhX;
	TH1D* fhY;
	TH1D* fhPhi;

   // circle fitting algorithm, errors
   TH1D* fhRadiusErr;
   TH1D* fhCircleXcErr;
   TH1D* fhCircleYcErr;
   // circle fitting algorithm, parameters
   TH1D* fhRadius;
   TH1D* fhCircleXc;
   TH1D* fhCircleYc;
   // circle fitting algorithm, pools
   TH1D* fhRadiusPool;
   TH1D* fhCircleXcPool;
   TH1D* fhCircleYcPool;

	ClassDef(CbmRichRingFitterQa, 1);
};
#endif
