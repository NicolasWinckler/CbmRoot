#ifndef CBMRICHRINGFITTERQA_H_
#define CBMRICHRINGFITTERQA_H_

#include "TObject.h"
#include "TH1D.h"

class QaHit {
public:
	Double_t fX;
	Double_t fY;
};

class QaEllipse{
public:
	Double_t fR; // 2*a
	Double_t fV11; // xf1
	Double_t fV12; //yf1
	Double_t fV21; // xf2
	Double_t fV22; //yf2
};

class CbmRichRingFitterQa: public TObject {
public:
	void GenerateEllipse();
	CbmRichRingFitterQa();
	void DrawHist();
private:
	TH1D* fhErrorA;
	TH1D* fhErrorB;
	TH1D* fhErrorX;
	TH1D* fhErrorY;
	TH1D* fhErrorPhi;

	TH1D* fhA;
	TH1D* fhB;
	TH1D* fhX;
	TH1D* fhY;
	TH1D* fhPhi;
	ClassDef(CbmRichRingFitterQa, 1);
};
#endif
