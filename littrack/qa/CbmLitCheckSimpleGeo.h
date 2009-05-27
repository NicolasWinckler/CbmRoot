#ifndef CBMLITCHECKSIMPLEGEO_H_
#define CBMLITCHECKSIMPLEGEO_H_

#include "CbmLitSimpleGeometryConstructor.h"

#include "FairTask.h"

class CbmLitCheckSimpleGeo : public FairTask
{
public:
	CbmLitCheckSimpleGeo();
	virtual ~CbmLitCheckSimpleGeo();

	virtual InitStatus Init();
	virtual void Exec(
			Option_t* opt);
	virtual void Finish();
	virtual void SetParContainers();

private:
	CbmLitSimpleGeometryConstructor fGeoConstructor;

	ClassDef(CbmLitCheckSimpleGeo, 1);
};

#endif /* CBMLITCHECKSIMPLEGEO_H_ */
