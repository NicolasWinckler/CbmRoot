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
//	double GetZ() const {return fZ;}
//	double GetDz() const {return fDz;}
//	double GetRmin() const {return fRmin;}
//	double GetRmax() const {return fRmax;}
//
//	void SetName(const std::string& name) {fName = name;}
//	void SetMediumName(const std::string& mediumName) {fMediumName = mediumName;}
//	void SetZ(double z) {fZ = z;}
//	void SetDz(double dz) {fDz = dz;}
//	void SetRmin(double rmin) {fRmin = rmin;}
//	void SetRmax(double rmax) {fRmax = rmax;}
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
//	double fZ;
//	double fDz;
//	double fRmin;
//	double fRmax;
//
////	ClassDef(CbmLitGeoNode, 1);
//};
//
//class CbmLitGeoMaterial {
//public:
//	CbmLitGeoMaterial() {};
//	CbmLitGeoMaterial(
//			const std::string& name,
//			double a,
//			double z,
//			double density,
//			double radLen,
//			double intLen):
//				fName(name),
//				fA(a),
//				fZ(z),
//				fDensity(density),
//				fRadLen(radLen),
//				fIntLen(intLen){;}
//	~CbmLitGeoMaterial();
//
//	const std::string& GetName() const {return fName;}
//	double GetA() const {return fA;}
//	double GetZ() const {return fZ;}
//	double GetDensity() const {return fDensity;}
//	double GetRadLen() const {return fRadLen;}
//	double GetIntLen() const {return fIntLen;}
//
//	void SetName(const std::string& name) {fName = name;}
//	void SetA(double a) {fA = a;}
//	void SetZ(double z) {fZ = z;}
//	void SetDensity(double density) {fDensity = density;}
//	void SetRadLen(double radLen) {fRadLen = radLen;}
//	void SetIntLen(double intLen) {fIntLen = intLen;}
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
//	double fA;
//	double fZ;
//	double fDensity;
//	double fRadLen;
//	double fIntLen;
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
