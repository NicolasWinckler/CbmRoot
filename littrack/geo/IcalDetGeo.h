#ifndef ICALDETGEO_H
#define ICALDETGEO_H

#include "FairGeoSet.h"

class IcalDetGeo: public FairGeoSet {
protected:
	char modName[20]; // name of module
	char eleName[20]; // substring for elements in module
public:
	IcalDetGeo();
	~IcalDetGeo() {
	}
	const char* getModuleName(Int_t);
	const char* getEleName(Int_t);
	inline Int_t getModNumInMod(const TString&);
ClassDef(IcalDetGeo,1);
};

inline Int_t IcalDetGeo::getModNumInMod(const TString& name) {
	/** returns the module index from module name
	 ?? in name[??] has to be the length of the detector name in the
	 .geo file. For example if all nodes in this file starts with
	 tutdet ?? has to be 6.
	 */
	TString tmp(name);
	tmp.Remove(0, 4); // for "ical"
	return tmp.Atoi() - 1;
//	return (Int_t)(name[6] - '0') - 1; //
}

#endif  /*  IcalDetGeo_H */
