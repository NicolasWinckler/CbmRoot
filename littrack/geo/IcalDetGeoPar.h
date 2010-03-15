#ifndef ICALDETGEOPAR_H
#define ICALDETGEOPAR_H

#include "FairParGenericSet.h"

class TObjArray;
class FairParamList;

class IcalDetGeoPar: public FairParGenericSet {
public:

	/** List of FairGeoNodes for sensitive  volumes */
	TObjArray *fGeoSensNodes;

	/** List of FairGeoNodes for sensitive  volumes */
	TObjArray *fGeoPassNodes;

	IcalDetGeoPar(const char* name = "IcalDetGeoPar", const char* title =
			"IcalDet Geometry Parameters", const char* context =
			"TestDefaultContext");
	~IcalDetGeoPar(void);
	void clear(void);
	void putParams(FairParamList*);
	Bool_t getParams(FairParamList*);
	TObjArray* GetGeoSensitiveNodes() {
		return fGeoSensNodes;
	}
	TObjArray* GetGeoPassiveNodes() {
		return fGeoPassNodes;
	}

ClassDef(IcalDetGeoPar,1);
};

#endif /* ICALDETGEOPAR_H */
