/**
 * \file CbmLitTrackingGeometryConstructor.h
 * \brief Tracking geometry constructor.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

#ifndef CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_
#define CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_

#include "propagation/CbmLitMaterialInfo.h"
#include "base/CbmLitDetectorSetup.h"
#include "base/CbmLitDetectorLayout.h"
#include "parallel/muon/LitDetectorLayoutMuon.h"

#include <string>
#include <map>
#include <vector>

class TGeoManager;
class TGeoMedium;
class TGeoVolume;
class TGeoNode;

class CbmLitTrackingGeometryConstructor
{
public:
   /**
    * \brief Return pointer to singleton object.
    * \return Pointer to singleton object.
    */
   static CbmLitTrackingGeometryConstructor* Instance();

   /**
    * \brief Draw simplified geometry.
    */
   void Draw();

   /**
    * \brief Return reference to the MUCH detector layout in SIMD format.
    */
   void GetMuchLayoutVec(lit::parallel::LitDetectorLayoutMuonVec& layout);

   /**
    * \brief Return reference to the MUCH detector layout in scalar format.
    */
   void GetMuchLayoutScal(lit::parallel::LitDetectorLayoutMuonScal& layout);

   /**
    * \brief Template function that returns reference to the MUCH detector layout.
    */
   template<class T> void GetMuchLayout(lit::parallel::LitDetectorLayoutMuon<T>& layout);

   TGeoManager* GetTrdTrackingGeo() const { return fTrdTrackingGeo; }
   const CbmLitDetectorLayout& GetTrdLayout() const { return fTrdLayout; }

private:
   /**
    * \brief Constructor.
    *
    * Constructor is protected since singleton pattern is used.
    * Pointer to object is returned by static Instance() method.
    */
   CbmLitTrackingGeometryConstructor();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingGeometryConstructor();

   /**
    * \brief Copy constructor.
    */
   CbmLitTrackingGeometryConstructor(const CbmLitTrackingGeometryConstructor&);

   /**
    * \brief Assignment operator.
    */
   const CbmLitTrackingGeometryConstructor& operator=(const CbmLitTrackingGeometryConstructor&);

   /**
    * \brief Main function for simplified geometry construction.
    */
   void ConstructGeometry();

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

   void ConstructTrdWitDifferentKeepingVolumes();

   void ConstructTrdWithSameKeepingVolume();

   /**
    * \brief Construct simplified RICH geometry.
    */
   void ConstructRich();

   /**
    * \brief Construct RICH node with TRAP shape.
    */
   void ReadRichTRAP(
         TGeoNode* node,
         double startZ);

   TGeoManager* fGeo; // Pointer to full geometry
   std::map<std::string, TGeoMedium*> fMedium; // Maps name of the medium to TGeoMedium pointer
   CbmLitDetectorSetup fDet; // Used for detector layout determination
   TGeoManager* fTrdTrackingGeo; // This is needed for TRD geometries with rotated planes.
   CbmLitDetectorLayout fTrdLayout; // TRD tracking layout
};

#endif /* CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_ */
