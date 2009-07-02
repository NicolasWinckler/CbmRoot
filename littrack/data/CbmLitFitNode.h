#ifndef CBMLITFITNODE_H_
#define CBMLITFITNODE_H_

#include "CbmLitFloat.h"
#include "CbmLitTrackParam.h"

class CbmLitFitNode
{
public:
	CbmLitFitNode();
	virtual ~CbmLitFitNode();

	const std::vector<myf>& GetF() const { return fF; }
	const CbmLitTrackParam* GetPredictedParam() const { return &fPredictedParam; }
	const CbmLitTrackParam* GetUpdatedParam() const { return &fUpdatedParam; }
	const CbmLitTrackParam* GetSmoothedParam() const { return &fSmoothedParam; }
	myf GetChiSqFiltered() const { return fChiSqFiltered; }
	myf GetChiSqSmoothed() const { return fChiSqSmoothed; }

	void SetF(const std::vector<myf>& F);
	void SetPredictedParam(const CbmLitTrackParam* par) { fPredictedParam = *par;}
	void SetUpdatedParam(const CbmLitTrackParam* par) { fUpdatedParam = *par;}
	void SetSmoothedParam(const CbmLitTrackParam* par) { fSmoothedParam = *par;}
	void SetChiSqFiltered(myf chiSq) { fChiSqFiltered = chiSq; }
	void SetChiSqSmoothed(myf chiSq) { fChiSqSmoothed = chiSq; }

private:
	// transport matrix
	std::vector<myf> fF;

	CbmLitTrackParam fPredictedParam;
	CbmLitTrackParam fUpdatedParam;
	CbmLitTrackParam fSmoothedParam;

	myf fChiSqFiltered;
	myf fChiSqSmoothed;
};

#endif /*CBMLITFITNODE_H_*/
