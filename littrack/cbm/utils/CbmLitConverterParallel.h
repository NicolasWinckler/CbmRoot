/**
 * \file CbmLitConverterParallel.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief Class for conversion between CBMROOT data classes and littrack parallel data classes.
 */

#ifndef CBMLITCONVERTERPARALLEL_H_
#define CBMLITCONVERTERPARALLEL_H_

#include "parallel/LitTrackParam.h"
class FairTrackParam;

class CbmLitConverterParallel {
public:
   /**
    * \brief Constructor.
    */
   CbmLitConverterParallel();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitConverterParallel();

   /**
    * \brief Convert FairTrackParam to LitTrackParamScal.
    * \param[in] par Pointer to FairTrackParam.
    * \param[out] lpar Pointer to LitTrackParamScal.
    */
   static void FairTrackParamToLitTrackParamScal(
      const FairTrackParam* par,
      lit::parallel::LitTrackParamScal* lpar);

   /**
    * \brief Convert LitTrackParamScal to FairTrackParam.
    * \param[in] lpar Pointer to LitTrackParamScal.
    * \param[out] par Pointer to FairTrackParam.
    */
   static void LitTrackParamScalToFairTrackParam(
      const lit::parallel::LitTrackParamScal* lpar,
      FairTrackParam* par);
};

#endif /* CBMLITCONVERTERPARALLEL_H_ */
