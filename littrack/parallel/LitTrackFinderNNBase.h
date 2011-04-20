/** LitTrackFinderNNBase.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **
 ** Base class for fast implementation of the nearest
 ** neighbor tracking algorithms. Implements some common
 ** functionality.
 **/

#ifndef LITTRACKFINDERNNBASE_H_
#define LITTRACKFINDERNNBASE_H_

#include "LitHit.h"
#include "LitTrackParam.h"
#include "LitTrack.h"

namespace lit {
namespace parallel {

//template<class T>
class LitTrackFinderNNBase
{
public:
   LitTrackFinderNNBase();
   virtual ~LitTrackFinderNNBase();

   void SetSigmaCoef(fscal sigmaCoef) {
      fSigmaCoef = sigmaCoef;
   }

   void SetMaxCovSq(fscal maxCovSq) {
      fMaxCovSq = maxCovSq;
   }

protected:
   void MinMaxIndex(
      const LitTrackParamScal* par,
      LitScalPixelHit** hits,
      unsigned int nofHits,
      fscal maxErr,
      unsigned int& first,
      unsigned int& last);

private:
   fscal fSigmaCoef;
   fscal fMaxCovSq;
};

//typedef LitTrackFinderNNBase<fscal> LitTrackFinderNNBaseScal;
//typedef LitTrackFinderNNBase<fvec> LitTrackFinderNNBaseVec;

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFINDERNNBASE_H_ */
