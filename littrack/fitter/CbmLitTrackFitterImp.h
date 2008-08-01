#ifndef CBMLITTRACKFITTERIMP_H_
#define CBMLITTRACKFITTERIMP_H_

#include "CbmLitTrackFitter.h"

class CbmLitTrackPropagator;
class CbmLitTrackUpdate;
class CbmLitTrack;
class CbmLitHit;
class CbmLitTrackParam;

class CbmLitTrackFitterImp : public CbmLitTrackFitter
{
public:
	CbmLitTrackFitterImp(
			CbmLitTrackPropagator* propagator,
			CbmLitTrackUpdate* update);
	
	virtual ~CbmLitTrackFitterImp();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus Fit(
			CbmLitTrack *pTrack);
	
	Bool_t IsDownstream() { return fDownstream;}
	
	void IsDownstream(Bool_t isDownstream) { fDownstream = isDownstream; }
	
private:
	
	CbmLitTrackPropagator* fPropagator;
	CbmLitTrackUpdate* fUpdate;
	Bool_t fDownstream;
	
	Double_t CalcChi2(						
			const CbmLitTrackParam *pParam, 						
			const CbmLitHit* pHit);
	
	ClassDef(CbmLitTrackFitterImp, 1)
};

#endif /*CBMLITTRACKFITTERIMP_H_*/
