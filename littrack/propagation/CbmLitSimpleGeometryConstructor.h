#ifndef CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_
#define CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_

#include "TObject.h"

#include <string>
#include <map>
#include <vector>
#include <sstream>

class TGeoManager;
class TGeoMedium;
class TGeoVolume;


//class CbmLitGeoNode
//{
//public:
//	const std::string& GetName() const {return fName; }
//	const std::string& GetMediumName() const {return fMediumName; }
//	Double_t GetZ() const {return fZ;}
//	Double_t GetDz() const {return fDz;}
//	Double_t GetRmin() const {return fRmin;}
//	Double_t GetRmax() const {return fRmax;}
//
//	void SetName(const std::string& name) {fName = name;}
//	void SetMediumName(const std::string& mediumName) {fMediumName = mediumName;}
//	void SetZ(Double_t z) {fZ = z;}
//	void SetDz(Double_t dz) {fDz = dz;}
//	void SetRmin(Double_t rmin) {fRmin = rmin;}
//	void SetRmax(Double_t rmax) {fRmax = rmax;}
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
//	Double_t fZ;
//	Double_t fDz;
//	Double_t fRmin;
//	Double_t fRmax;
//
////	ClassDef(CbmLitGeoNode, 1);
//};
//
//class CbmLitGeoMaterial {
//public:
//	CbmLitGeoMaterial() {};
//	CbmLitGeoMaterial(
//			const std::string& name,
//			Double_t a,
//			Double_t z,
//			Double_t density,
//			Double_t radLen,
//			Double_t intLen):
//				fName(name),
//				fA(a),
//				fZ(z),
//				fDensity(density),
//				fRadLen(radLen),
//				fIntLen(intLen){;}
//	~CbmLitGeoMaterial();
//
//	const std::string& GetName() const {return fName;}
//	Double_t GetA() const {return fA;}
//	Double_t GetZ() const {return fZ;}
//	Double_t GetDensity() const {return fDensity;}
//	Double_t GetRadLen() const {return fRadLen;}
//	Double_t GetIntLen() const {return fIntLen;}
//
//	void SetName(const std::string& name) {fName = name;}
//	void SetA(Double_t a) {fA = a;}
//	void SetZ(Double_t z) {fZ = z;}
//	void SetDensity(Double_t density) {fDensity = density;}
//	void SetRadLen(Double_t radLen) {fRadLen = radLen;}
//	void SetIntLen(Double_t intLen) {fIntLen = intLen;}
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
//	Double_t fA;
//	Double_t fZ;
//	Double_t fDensity;
//	Double_t fRadLen;
//	Double_t fIntLen;
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
//			Int_t id,
//			const CbmLitGeoMaterial& material):
//				fName(name),
//				fId(id),
//				fMaterial(material) {;}
//	~CbmLitGeoMedium() {;}
//
//	const std::string& GetName() const {return fName;}
//	Int_t GetId() const {return fId;}
//	const CbmLitGeoMaterial& GetMaterial() const {return fMaterial;}
//
//	void SetName(const std::string& name) {fName = name;}
//	void SetId(Int_t id) {fId = id;}
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
//	Int_t fId;
//	CbmLitGeoMaterial fMaterial;
//
////	ClassDef(CbmLitGeoMedium, 1);
//};



class CbmLitSimpleGeometryConstructor {
public:
	CbmLitSimpleGeometryConstructor();
	virtual ~CbmLitSimpleGeometryConstructor();

	void ConstructGeometry();
	void Draw();

private:
	TGeoMedium* CreateMedium(
			const std::string& matName);

	void CreateMediumList();

	void ConstructSts();
	void ConstructMuch();
	void ConstructTof();

	TGeoManager* fGeo;
	TGeoManager* fSimpleGeo;

	std::map<std::string, TGeoMedium*> fMedium;

	ClassDef(CbmLitSimpleGeometryConstructor, 1);
};

#endif /* CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_ */
