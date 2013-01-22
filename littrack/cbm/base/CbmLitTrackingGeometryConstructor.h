/**
- * \file CbmLitTrackingGeometryConstructor.h
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
#include "parallel/electron/LitDetectorGeometryElectron.h"

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
    * \brief Return MUCH detector layout for parallel MUCH tracking in SIMD format.
    * \param[out] layout MUCH detector layout.
    */
   void GetMuchLayoutVec(lit::parallel::LitDetectorLayoutMuonVec& layout);

   /**
    * \brief Return MUCH detector layout for parallel MUCH tracking in scalar format.
    * \param[out] layout MUCH detector layout.
    */
   void GetMuchLayoutScal(lit::parallel::LitDetectorLayoutMuonScal& layout);

   /**
    * \brief Return MUCH detector layout for parallel MUCH tracking.
    * \param[out] layout MUCH detector layout.
    */
   template<class T> void GetMuchLayout(lit::parallel::LitDetectorLayoutMuon<T>& layout);

   /**
    * \brief Return TRD detector layout for TRD parallel tracking in SIMD format.
    * \param[out] layout TRD detector layout.
    */
   void GetTrdLayoutVec(lit::parallel::LitDetectorLayoutElectronVec& layout);

   /**
    * \brief Return TRD detector layout for TRD parallel tracking in scalar format.
    * \param[out] layout TRD detector layout.
    */
   void GetTrdLayoutScal(lit::parallel::LitDetectorLayoutElectronScal& layout);

   /**
    * \brief Return TRD detector layout for TRD parallel tracking.
    * \param[out] layout TRD detector layout.
    */
   template<class T> void GetTrdLayout(lit::parallel::LitDetectorLayoutElectron<T>& layout);

   /**
    * \brief Return MVD detector layout.
    * \return MVD detector layout.
    */
   const CbmLitDetectorLayout& GetMvdLayout();

   /**
    * \brief Return detector layout. Automatically determines TRD or MUCH.
    * \return Detector layout.
    */
   const CbmLitDetectorLayout& GetLayout();

   /**
    * \brief Return MUCH detector layout.
    * \return MUCH detector layout.
    */
   const CbmLitDetectorLayout& GetMuchLayout();

   /**
    * \brief Return TRD tracking geometry in TGeo. GetTrdLayout has to be called in advance.
    * \return TRD tracking geometry in TGeo.
    */
   TGeoManager* GetTrdTrackingGeo() const { return fTrdTrackingGeo; }

   /**
    * \brief Return TRD detector layout.
    * \return TRD detector layout.
    */
   const CbmLitDetectorLayout& GetTrdLayout();

   /**
    * \brief Return TOF detector layout.
    * \return TOF detector layout.
    */
   const CbmLitDetectorLayout& GetTofLayout();

   /**
    * \brief Return TOF detector as station.
    * \return TOF detector station.
    */
   const CbmLitStation& GetTofStation();

private:
   /**
    * \brief Constructor.
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

   /* */
   void CreateMediumList();

   /**
    * \brief Constructs MVD tracking geometry.
    */
   void ConstructMvd();

   /**
    * \brief Construct simplified STS geometry.
    */
   void ConstructSts();

   /**
    * \brief Construct simplified MUCH geometry.
    */
   void ConstructMuch();

   /**
    * \brief Construct simplified TRD geometry.
    */
   void ConstructTrd();

   void ConstructTrdWitDifferentKeepingVolumes();

   void ConstructTrdWithSameKeepingVolume();

   /**
    * \brief Construct tracking TOF geometry.
    */
   void ConstructTof();

   /**
    * \brief Construct TOF tracking station.
    */
   void ConstructTofStation();

   /**
    * \brief Construct TOF tracking station for v13X geometry version.
    */
   void ConstructTofStationV13a();

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
   CbmLitDetectorLayout fLayout; // Detector layout
   CbmLitDetectorLayout fMvdLayout; // MVD detector layout
   CbmLitDetectorLayout fMuchLayout; // MUCH detector layout
   TGeoManager* fTofTrackingGeo; // This is needed for TOF with rotated planes.
   CbmLitDetectorLayout fTofLayout; // TOf detector layout
   CbmLitStation fTofStation; // TOF station
   TGeoManager* fTrdTrackingGeo; // This is needed for TRD geometries with rotated planes.
   CbmLitDetectorLayout fTrdLayout; // TRD tracking layout
};

#endif /* CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_ */
