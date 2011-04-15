/** CbmLitWeightedHitCalculator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 *
 * Interface for weighted hit calculation algorithms
 **/

#ifndef CBMLITWEIGHTEDHITCALCULATOR_H_
#define CBMLITWEIGHTEDHITCALCULATOR_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitTypes.h"

class CbmLitHit;

class CbmLitWeightedHitCalculator : public CbmLitTool
{
public:
   /* Constructor */
   CbmLitWeightedHitCalculator() {};

   /* Destructor */
   virtual ~CbmLitWeightedHitCalculator() {};

   /* Main function to be implemented for the concrete weighted hit calculation algorithms
    * @param itBegin Iterator of the first hit
    * @param itEnd Iterator of the last hit
    * @param hit Pointer to the output hit
    * @return Status code */
   virtual void DoCalculate(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd,
      CbmLitHit* hit) = 0;

   /* Main function to be implemented for the concrete weighted hit calculation algorithms
    * @param hits Array of hits
    * @param hit Pointer to the output hit
    * @return Status code */
   virtual void DoCalculate(
      HitPtrVector& hits,
      CbmLitHit* hit) = 0;
};

#endif /*CBMLITWEIGHTEDHITCALCULATOR_H_*/
