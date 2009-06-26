#ifndef CBMLITFITNODE_H_
#define CBMLITFITNODE_H_

#include "CbmLitTrackParam.h"

#include "TMatrixD.h"

class CbmLitFitNode
{
public:
	CbmLitFitNode();
	virtual ~CbmLitFitNode();

	const std::vector<double>& GetF() const { return fF; }
	void GetF(TMatrixD& F) const;
	const CbmLitTrackParam* GetPredictedParam() const { return &fPredictedParam; }
	const CbmLitTrackParam* GetUpdatedParam() const { return &fUpdatedParam; }
	const CbmLitTrackParam* GetSmoothedParam() const { return &fSmoothedParam; }
	double GetChiSqFiltered() const { return fChiSqFiltered; }
	double GetChiSqSmoothed() const { return fChiSqSmoothed; }

	void SetF(const std::vector<double>& F);
	void SetF(const TMatrixD& F);
	void SetPredictedParam(const CbmLitTrackParam* par) { fPredictedParam = *par;}
	void SetUpdatedParam(const CbmLitTrackParam* par) { fUpdatedParam = *par;}
	void SetSmoothedParam(const CbmLitTrackParam* par) { fSmoothedParam = *par;}
	void SetChiSqFiltered(double chiSq) { fChiSqFiltered = chiSq; }
	void SetChiSqSmoothed(double chiSq) { fChiSqSmoothed = chiSq; }

private:
	// transport matrix
	std::vector<double> fF;

	CbmLitTrackParam fPredictedParam;
	CbmLitTrackParam fUpdatedParam;
	CbmLitTrackParam fSmoothedParam;

	double fChiSqFiltered;
	double fChiSqSmoothed;
};

#endif /*CBMLITFITNODE_H_*/
