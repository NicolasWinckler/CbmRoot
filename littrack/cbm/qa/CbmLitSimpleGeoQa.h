/**
 * \file CbmLitSimpleGeoQa.h
 *
 * \brief Task for checking of simplified geometry creator.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 **/

#ifndef CBMLITSIMPLEGEOQA_H_
#define CBMLITSIMPLEGEOQA_H_

#include "FairTask.h"

class CbmLitSimpleGeometryConstructor;

/**
 * \class CbmLitSimpleGeoQa
 *
 * \brief Task for checking of simplified geometry creator.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 **/
class CbmLitSimpleGeoQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitSimpleGeoQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitSimpleGeoQa();

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
   CbmLitSimpleGeometryConstructor* fGeoConstructor;

   ClassDef(CbmLitSimpleGeoQa, 1);
};

#endif /* CBMLITSIMPLEGEOQA_H_ */
