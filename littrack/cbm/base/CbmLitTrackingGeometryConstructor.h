/**
- * \file CbmLitTrackingGeometryConstructor.h
 * \brief Tracking geometry constructor.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

#ifndef CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_
#define CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_

#include "TObject.h"

#include "parallel/muon/LitDetectorLayoutMuon.h"
#include "parallel/electron/LitDetectorGeometryElectron.h"

#include <string>

class TGeoManager;

class CbmLitTrackingGeometryConstructor
{
public:
   /**
    * \brief Return pointer to singleton object.
    * \return Pointer to singleton object.
    */
   static CbmLitTrackingGeometryConstructor* Instance();

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
    * \brief Return number of stations in TRD.
    * \return Number of stations in TRD.
    */
   Int_t GetNofTrdStations();

   /**
    * \brief Return number of stations in MUCH.
    * \return Number of stations in MUCH.
    */
   Int_t GetNofMuchStations();

   /**
    * \brief Return number of stations in MUCH + TRD.
    * \return Number of stations in MUCH + TRD.
    */
   Int_t GetNofMuchTrdStations();

   /**
    * \brief Return number of stations in MVD.
    * \return Number of stations in MVD.
    */
   Int_t GetNofMvdStations();

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

   TGeoManager* fGeo; // Pointer to full geometry
   Int_t fNofTrdStations; // Number of TRD stations
   Int_t fNofMuchStations; // Number of MUCH stations
   Int_t fNofMvdStations; // Number of MVD stations
};

#endif /* CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_ */
