#ifndef CBMLITFITNODE_H_
#define CBMLITFITNODE_H_

#include "CbmLitTrackParam.h"

#include "TObject.h"
#include "TMatrixD.h"

class CbmLitFitNode
{
public:
	CbmLitFitNode();
	virtual ~CbmLitFitNode();
	
	const std::vector<Double_t>& GetF() const { return fF; }
	void GetF(TMatrixD& F) const { 
		const Double_t* a = &fF[0];
		// TODO why TMatrixD cannot call const Use ?!?!
		F.Use(5, 5, (Double_t*)a);		
	}
	const CbmLitTrackParam* GetPredictedParam() const { return &fPredictedParam; }
	const CbmLitTrackParam* GetUpdatedParam() const { return &fUpdatedParam; }
	const CbmLitTrackParam* GetSmoothedParam() const { return &fSmoothedParam; }

	void SetF(const std::vector<Double_t>& F) {
			fF.assign(F.begin(), F.end());
	}
	void SetF(const TMatrixD& F) {
			fF.assign(F.GetMatrixArray(), F.GetMatrixArray() + F.GetNoElements());
	}
	void SetPredictedParam(const CbmLitTrackParam* par) { fPredictedParam = *par;}
	void SetUpdatedParam(const CbmLitTrackParam* par) { fUpdatedParam = *par;}
	void SetSmoothedParam(const CbmLitTrackParam* par) { fSmoothedParam = *par;}
	
private:
	// transport matrix
	std::vector<Double_t> fF;

	CbmLitTrackParam fPredictedParam;
	CbmLitTrackParam fUpdatedParam;
	CbmLitTrackParam fSmoothedParam;

	ClassDef(CbmLitFitNode,1);
};

#endif /*CBMLITFITNODE_H_*/
