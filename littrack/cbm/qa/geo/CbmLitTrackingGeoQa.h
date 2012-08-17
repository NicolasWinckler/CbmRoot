/**
 * \file CbmLitTrackingGeoQa.h
 * \brief Tracking geometry creator QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/

#ifndef CBMLITSIMPLEGEOQA_H_
#define CBMLITSIMPLEGEOQA_H_

#include "FairTask.h"

class CbmLitTrackingGeometryConstructor;

/**
 * \class CbmLitTrackingGeoQa
 * \brief Tracking geometry creator QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
class CbmLitTrackingGeoQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackingGeoQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingGeoQa();

   /**
    * Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * Inherited from FairTask.
    */
   virtual void Exec(
      Option_t* opt);

   /**
    * Inherited from FairTask.
    */
   virtual void Finish();

   /**
    * Inherited from FairTask.
    */
   virtual void SetParContainers();

private:
   // Simplified geometry constructor tool
   CbmLitTrackingGeometryConstructor* fGeoConstructor;

   ClassDef(CbmLitTrackingGeoQa, 1);
};

#endif /* CBMLITSIMPLEGEOQA_H_ */
