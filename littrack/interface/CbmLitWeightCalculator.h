/** CbmLitWeightCalculator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 *
 * Interface for weight calculation algorithms
 **/

#ifndef CBMLITWEIGHTCALCULATOR_H_
#define CBMLITWEIGHTCALCULATOR_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitTypes.h"

class CbmLitTrackParam;

class CbmLitWeightCalculator : public CbmLitTool
{
public:
   /* Constructor */
   CbmLitWeightCalculator() {};

   /* Destructor */
   virtual ~CbmLitWeightCalculator() {};

   /* Main function to be implemented for the concrete weight calculation algorithm
    * @param par Pointer to the track parameter
    * @param itBegin First hit iterator
    * @param itEnd Last hit iterator
    * @param T Temperature parameter
    * @return Status code */
   virtual LitStatus DoCalculate(
      const CbmLitTrackParam* par,
      HitPtrIterator itBegin,
      HitPtrIterator itEnd,
      myf T) = 0;

   /* Main function to be implemented for the concrete weight calculation algorithm
    * @param par Pointer to the track parameter
    * @param hits Array of hits
    * @param T Temperature parameter
    * @return Status code */
   virtual LitStatus DoCalculate(
      const CbmLitTrackParam* par,
      HitPtrVector& hits,
      myf T) = 0;
};

#endif /*CBMLITWEIGHTCALCULATOR_H_*/
