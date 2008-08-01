#ifndef CBMLITGEONAVIGATORIMP_H_
#define CBMLITGEONAVIGATORIMP_H_

#include "CbmLitTool.h"
#include "CbmLitMaterialInfo.h"

#include <vector>

class CbmLitTrackParam;

class CbmLitGeoNavigatorImp : public CbmLitTool
{
public:
	CbmLitGeoNavigatorImp();
	virtual ~CbmLitGeoNavigatorImp();
	
	// derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	LitStatus FindIntersections(
			const CbmLitTrackParam* par,
			Double_t zOut,
			std::vector<CbmLitMaterialInfo>& inter);
		
private:
	void InitTrack(
			const CbmLitTrackParam* par) const;
	
	CbmLitMaterialInfo MakeStep(
			Double_t step = 0.) const;
	
	Double_t CalcLength(
			Double_t zOut) const;
	
	ClassDef(CbmLitGeoNavigatorImp,1);
};

#endif /*CBMLITGEONAVIGATORIMP_H_*/
