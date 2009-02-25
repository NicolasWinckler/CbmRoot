#ifndef CBMLITMATERIALEFFECTS_H_
#define CBMLITMATERIALEFFECTS_H_

#include "CbmLitTool.h"

class CbmLitTrackParam;
class CbmLitMaterialInfo;

class CbmLitMaterialEffects : public CbmLitTool
{
public:
	CbmLitMaterialEffects();
	virtual ~CbmLitMaterialEffects();

	virtual LitStatus Update(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat,
	        Int_t pdg,
	        Bool_t downstream) = 0;

	ClassDef(CbmLitMaterialEffects, 1);
};

#endif /*CBMLITMATERIALEFFECTS_H_*/
