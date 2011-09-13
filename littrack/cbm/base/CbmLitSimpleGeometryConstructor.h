/**
 * \file CbmLitSimpleGeometryConstructor.h
 *
 * \brief Simplified geometry constructor.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
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
   /**
    * \brief Return pointer to singleton object.
    * \return Pointer to singleton object.
    */
   static CbmLitSimpleGeometryConstructor* Instance();

   /**
    * \brief Return array of materials for simplified geometry.
    * \return Array of materials for simplified geometry.
    */
   const std::vector<CbmLitMaterialInfo>& GetMyGeoNodes() const {return fMyGeoNodes;}

   /**
    * \brief Returs array of materials for MUCH geometry.
    * \return Array of materials for MUCH geometry.
    */
   const std::vector<CbmLitMaterialInfo>& GetMyMuchGeoNodes() const {return fMyMuchGeoNodes;}

   /**
    * \brief Return array of materials for TRD geometry.
    * \return Array of materials for TRD geometry.
    */
   const std::vector<CbmLitMaterialInfo>& GetMyTrdGeoNodes() const {return fMyTrdGeoNodes;}

   /**
    * \brief Return array of materials for RICH geometry.
    * \return Array of materials for RICH geometry.
    */
   const std::vector<CbmLitMaterialInfo>& GetMyRichGeoNodes() const {return fMyRichGeoNodes;}

   /**
    * \brief Draw simplified geometry.
    */
   void Draw();

private:
   /**
    * \brief Constructor.
    *
    * Constructor is protected since singleton pattern is used.
    * Pointer to object is returned by static Instance() method.
    */
   CbmLitSimpleGeometryConstructor();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitSimpleGeometryConstructor();

   /**
    * \brief Copy constructor.
    */
   CbmLitSimpleGeometryConstructor(const CbmLitSimpleGeometryConstructor&);

   /**
    * \brief Assignment operator.
    */
   const CbmLitSimpleGeometryConstructor& operator=(const CbmLitSimpleGeometryConstructor&);

   /**
    * \brief Main function for simplified geometry construction.
    */
   void ConstructGeometry();

   /* */
   TGeoMedium* CreateMedium(
      const std::string& matName);

   /* */
   void CreateMediumList();

   /**
    * \brief Convert TGeoMedium to CbmLitMaterialInfo.
    * \param[in] med Pointer to TGeoMedium.
    * \param[out] mat Output CbmLitMaterialInfo.
    */
   void GeoMediumToMaterialInfo(
      const TGeoMedium* med,
      CbmLitMaterialInfo& mat);

   /**
    * \brief Construct simplified STS geometry.
    */
   void ConstructSts();

   /**
    * \brief Construct simplified MUCH geometry.
    */
   void ConstructMuch();

   /**
    * \brief Construct simplified TOF geometry.
    */
   void ConstructTof();

   /**
    * \brief Construct simplified TRD geometry.
    */
   void ConstructTrd();

   /**
    * \brief Construct simplified RICH geometry.
    */
   void ConstructRich();

   TGeoManager* fGeo; // Pointer to full geometry
   TGeoManager* fSimpleGeo; // Pointer to simplified geometry

   std::map<std::string, TGeoMedium*> fMedium; // Maps name of the medium to TGeoMedium pointer

   std::vector<CbmLitMaterialInfo> fMyGeoNodes; // Vector of materials for the simplified geometry
   std::vector<CbmLitMaterialInfo> fMyMuchGeoNodes; // Vector of materials for the MUCH simplified geometry
   std::vector<CbmLitMaterialInfo> fMyTrdGeoNodes; // Vector of materials for the TRD simplified geometry
   std::vector<CbmLitMaterialInfo> fMyRichGeoNodes; // Vector of materials for the RICH simplified geometry

   bool fIsTrd; // true if TRD in TGeo
   bool fIsMuch; // true if MUCH in TGeo
   bool fIsTof; // true if TOF in TGeo
   bool fIsRich; // true if TOF in TGeo
};

#endif /* CBMLITSIMPLEGEOMETRYCONSTRUCTOR_H_ */
