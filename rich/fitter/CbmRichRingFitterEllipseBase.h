/*
 *
 *  Description: Base class for concrete ellipse fitting algorithms.
 *               Each derived class must implement the method DoFit.
 *
 *  Author: Semeon Lebedev
 *  E-mail: S.Lebedev@gsi.de
 */

#ifndef CBMRICHRINGFITTERELLIPSEBASE_H_
#define CBMRICHRINGFITTERELLIPSEBASE_H_

#include "CbmRichRingFitterImpl.h"
#include "TH2D.h"

class CbmRichRing;


class CbmRichRingFitterEllipseBase: public CbmRichRingFitterImpl{
public:
	/** Default constructor **/
	CbmRichRingFitterEllipseBase(){};

	/** Destructor **/
	virtual ~CbmRichRingFitterEllipseBase(){};

protected:
	virtual void CalcChi2(CbmRichRing* pRing);
	virtual void CalcChi2(Double_t A, Double_t B, Double_t C,
			Double_t D, Double_t E, Double_t F, CbmRichRing* pRing);
	virtual void MakeRadiusCorrection(CbmRichRing* pRing);
	virtual void InitHistForRadiusCorrection();

	TString fFieldName; ///specify correction map for different field
	TH2D* fh_mapaxisAXY;
	TH2D* fh_mapaxisBXY;

	ClassDef(CbmRichRingFitterEllipseBase,1);

};

#endif /* CBMRICHRINGFITTERELLIPSEBASE_H_ */
