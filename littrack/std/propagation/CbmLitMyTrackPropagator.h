/** CbmLitMyTrackPropagator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2009
 **
 ** The class performs track parameters propagation. The track propagation
 ** uses simplified detector geometry and simplified geometry navigation.
 **/
#ifndef CBMLITMYTRACKPROPAGATOR_H_
#define CBMLITMYTRACKPROPAGATOR_H_

#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackPropagator.h"

class CbmLitMyTrackPropagator : public CbmLitTrackPropagator
{
public:
   /* Constructor with track extrapolation tool.
    * @param extrapolator Pointer to the track extrapolation tool.
    */
   CbmLitMyTrackPropagator(
      TrackExtrapolatorPtr extrapolator);

   /* Destructor */
   virtual ~CbmLitMyTrackPropagator();

   /* Inherited from CbmLitTrackPropagator */
   virtual LitStatus Propagate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      litfloat zOut,
      int pdg,
      std::vector<litfloat>* F);

   /* Inherited from CbmLitTrackPropagator */
   virtual LitStatus Propagate(
      CbmLitTrackParam* par,
      litfloat zOut,
      int pdg,
      std::vector<litfloat>* F);

   /* Calculates updated transport matrix after each track extrapolation,
    * using formula F = newF * F.
    * @param F Input/Output current transport matrix.
    * @param newF addition to the transport matrix.
    */
   void UpdateF(
      std::vector<litfloat>& F,
      const std::vector<litfloat>& newF);

private:
   GeoNavigatorPtr fNavigator; // Geometry navigation tool
   TrackExtrapolatorPtr fExtrapolator; // Track extrapolator tool
   MaterialEffectsPtr fMaterial; // Material Effects tool
};

#endif /* CBMLITMYTRACKPROPAGATOR_H_ */
