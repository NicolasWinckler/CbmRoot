#ifndef CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_
#define CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_

#include "CbmLitMaterialInfo.h"

#include <string>
#include <map>
#include <vector>

class TGeoManager;
class TGeoMedium;
class TGeoVolume;

class CbmLitSimpleGeometryConstructor {
public:
	static CbmLitSimpleGeometryConstructor* Instance();
	virtual ~CbmLitSimpleGeometryConstructor();

	const std::vector<CbmLitMaterialInfo>& GetMyGeoNodes() const{return fMyGeoNodes;}
	const std::vector<CbmLitMaterialInfo>& GetMyMuchGeoNodes() const{return fMyMuchGeoNodes;}
	const std::vector<CbmLitMaterialInfo>& GetMyTrdGeoNodes() const{return fMyTrdGeoNodes;}

	void Draw();

protected:
	CbmLitSimpleGeometryConstructor();

private:
	void ConstructGeometry();

	TGeoMedium* CreateMedium(
			const std::string& matName);

	void CreateMediumList();
	void GeoMediumToMaterialInfo(
			const TGeoMedium* med,
			CbmLitMaterialInfo& mat);

	void ConstructSts();
	void ConstructMuch();
	void ConstructTof();
	void ConstructTrd();

	static CbmLitSimpleGeometryConstructor* fInstance;

	TGeoManager* fGeo;
	TGeoManager* fSimpleGeo;

	std::map<std::string, TGeoMedium*> fMedium;

	std::vector<CbmLitMaterialInfo> fMyGeoNodes;
	std::vector<CbmLitMaterialInfo> fMyMuchGeoNodes;
	std::vector<CbmLitMaterialInfo> fMyTrdGeoNodes;

	bool fIsTrd;
	bool fIsMuch;
	bool fIsTof;
};

#endif /* CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_ */
