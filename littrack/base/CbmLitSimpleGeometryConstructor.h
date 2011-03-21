/** CbmLitSimpleGeometryConstructor.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Class constructs simplified geometry.
 **/

#ifndef CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_
#define CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_

#include "propagation/CbmLitMaterialInfo.h"

#include <string>
#include <map>
#include <vector>

class TGeoManager;
class TGeoMedium;
class TGeoVolume;

class CbmLitSimpleGeometryConstructor
{
public:
   /* Returns pointer to the singleton object instance. */
   static CbmLitSimpleGeometryConstructor* Instance();

   /* Destructor */
   virtual ~CbmLitSimpleGeometryConstructor();

   /* Returns array of materials for simplified geometry. */
   const std::vector<CbmLitMaterialInfo>& GetMyGeoNodes() const {return fMyGeoNodes;}

   /* Returns array of materials for MUCH geometry. */
   const std::vector<CbmLitMaterialInfo>& GetMyMuchGeoNodes() const {return fMyMuchGeoNodes;}

   /* Returns array of materials for TRD geometry. */
   const std::vector<CbmLitMaterialInfo>& GetMyTrdGeoNodes() const {return fMyTrdGeoNodes;}

   /* Draws geometry. */
   void Draw();

protected:
   /* Constructor is protected since singleton pattern is used.
    * Pointer to the object is returned by static Instance() method. */
   CbmLitSimpleGeometryConstructor();

private:
   /* Constructs simplified geometry. */
   void ConstructGeometry();

   /* */
   TGeoMedium* CreateMedium(
      const std::string& matName);

   /* */
   void CreateMediumList();

   /* Converts TGeoMedium to CbmLitMaterialInfo. */
   void GeoMediumToMaterialInfo(
      const TGeoMedium* med,
      CbmLitMaterialInfo& mat);

   /* Constructs simplified STS geometry. */
   void ConstructSts();

   /* Constructs simplified MUCH geometry. */
   void ConstructMuch();

   /* Constructs simplified TOF geometry. */
   void ConstructTof();

   /* Constructs simplified TRD geometry. */
   void ConstructTrd();

   static CbmLitSimpleGeometryConstructor* fInstance; // static instance

   TGeoManager* fGeo; // Pointer to full geometry
   TGeoManager* fSimpleGeo; // Pointer to simplified geometry

   std::map<std::string, TGeoMedium*> fMedium; // Maps name of the medium to TGeoMedium pointer

   std::vector<CbmLitMaterialInfo> fMyGeoNodes; // Vector of materials for the simplified geometry
   std::vector<CbmLitMaterialInfo> fMyMuchGeoNodes; // Vector of materials for the MUCH simplified geometry
   std::vector<CbmLitMaterialInfo> fMyTrdGeoNodes; // Vector of materials for the TRD simplified geometry

   bool fIsTrd; // true if TRD in TGeo
   bool fIsMuch; // true if MUCH in TGeo
   bool fIsTof; // true if TOF in TGeo
};

#endif /* CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_ */
