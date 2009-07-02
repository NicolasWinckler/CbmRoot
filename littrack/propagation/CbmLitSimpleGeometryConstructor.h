#ifndef CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_
#define CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_

#include "CbmLitMaterialInfo.h"

#include <string>
#include <map>
#include <vector>
//#include <sstream>

class TGeoManager;
class TGeoMedium;
class TGeoVolume;


//class CbmLitGeoNode
//{
//public:
//	const std::string& GetName() const {return fName; }
//	const std::string& GetMediumName() const {return fMediumName; }
//	myf GetZ() const {return fZ;}
//	myf GetDz() const {return fDz;}
//	myf GetRmin() const {return fRmin;}
//	myf GetRmax() const {return fRmax;}
//
//	void SetName(const std::string& name) {fName = name;}
//	void SetMediumName(const std::string& mediumName) {fMediumName = mediumName;}
//	void SetZ(myf z) {fZ = z;}
//	void SetDz(myf dz) {fDz = dz;}
//	void SetRmin(myf rmin) {fRmin = rmin;}
//	void SetRmax(myf rmax) {fRmax = rmax;}
//
//	std::string ToString() const {
//		std::stringstream ss;
//		ss << "LitGeoNode: name=" << GetName() << ", mediumName=" << GetMediumName()
//			<< ", z=" << GetZ() << ", dz=" << GetDz() << ", rmin=" << GetRmin()
//			<< ", rmax=" << GetRmax() << std::endl;
//		return ss.str();
//	}
//
//private:
//	std::string fName;
//	std::string fMediumName;
//	myf fZ;
//	myf fDz;
//	myf fRmin;
//	myf fRmax;
//
////	ClassDef(CbmLitGeoNode, 1);
//};
//
//class CbmLitGeoMaterial {
//public:
//	CbmLitGeoMaterial() {};
//	CbmLitGeoMaterial(
//			const std::string& name,
//			myf a,
//			myf z,
//			myf density,
//			myf radLen,
//			myf intLen):
//				fName(name),
//				fA(a),
//				fZ(z),
//				fDensity(density),
//				fRadLen(radLen),
//				fIntLen(intLen){;}
//	~CbmLitGeoMaterial();
//
//	const std::string& GetName() const {return fName;}
//	myf GetA() const {return fA;}
//	myf GetZ() const {return fZ;}
//	myf GetDensity() const {return fDensity;}
//	myf GetRadLen() const {return fRadLen;}
//	myf GetIntLen() const {return fIntLen;}
//
//	void SetName(const std::string& name) {fName = name;}
//	void SetA(myf a) {fA = a;}
//	void SetZ(myf z) {fZ = z;}
//	void SetDensity(myf density) {fDensity = density;}
//	void SetRadLen(myf radLen) {fRadLen = radLen;}
//	void SetIntLen(myf intLen) {fIntLen = intLen;}
//
//	std::string ToString() const {
//		std::stringstream ss;
//		ss << "LitGeoMaterial: name=" << GetName() << ", A=" << GetA()
//			<< ", Z=" << GetZ() << ", density=" << GetDensity() << ", radLen=" << GetRadLen()
//			<< ", intLen=" << GetIntLen() << std::endl;
//		return ss.str();
//	}
//private:
//	std::string fName;
//	myf fA;
//	myf fZ;
//	myf fDensity;
//	myf fRadLen;
//	myf fIntLen;
//
////	ClassDef(CbmLitGeoMaterial, 1);
//};
//
//
//
//class CbmLitGeoMedium {
//public:
//	CbmLitGeoMedium(	) {;}
//	CbmLitGeoMedium(
//			const std::string& name,
//			int id,
//			const CbmLitGeoMaterial& material):
//				fName(name),
//				fId(id),
//				fMaterial(material) {;}
//	~CbmLitGeoMedium() {;}
//
//	const std::string& GetName() const {return fName;}
//	int GetId() const {return fId;}
//	const CbmLitGeoMaterial& GetMaterial() const {return fMaterial;}
//
//	void SetName(const std::string& name) {fName = name;}
//	void SetId(int id) {fId = id;}
//	void SetMaterial(const CbmLitGeoMaterial& material) {fMaterial = material;}
//
//	std::string ToString() const {
//		std::stringstream ss;
//		ss << "LitGeoMedium: name=" << GetName() << ", id=" << GetId()
//			<< ", " << GetMaterial().ToString();
//		return ss.str();
//	}
//
//private:
//	std::string fName;
//	int fId;
//	CbmLitGeoMaterial fMaterial;
//
////	ClassDef(CbmLitGeoMedium, 1);
//};



class CbmLitSimpleGeometryConstructor {
public:
	static CbmLitSimpleGeometryConstructor* Instance();
	virtual ~CbmLitSimpleGeometryConstructor();

	const std::vector<CbmLitMaterialInfo>& GetMyGeoNodes() const{return fMyGeoNodes;}

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

	static CbmLitSimpleGeometryConstructor* fInstance;

	TGeoManager* fGeo;
	TGeoManager* fSimpleGeo;

	std::map<std::string, TGeoMedium*> fMedium;

	std::vector<CbmLitMaterialInfo> fMyGeoNodes;
};

#endif /* CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_ */
