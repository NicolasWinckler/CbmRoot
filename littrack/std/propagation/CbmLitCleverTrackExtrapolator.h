/** CbmLitCleverTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2009
 **
 ** Class performs track extrapolation. It uses either line track extrapolation
 ** in the field free regions or Runge-Kutta of the 4th order in the magnetic field.
 ** The start Z position for the line track extrapolation is defined by
 ** lit::LINE_EXTRAPOLATION_START_Z in the littrack/base/CbmLitDefaultSettings.h
 **/

#ifndef CBMLITCLEVERTRACKEXTRAPOLATOR_H_
#define CBMLITCLEVERTRACKEXTRAPOLATOR_H_

#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackExtrapolator.h"

class CbmLitTrackParam;

class CbmLitCleverTrackExtrapolator : public CbmLitTrackExtrapolator
{
public:
   /* Constructor
    * @param type Type of the field extrapolator. If type == "myfield" than
    * fitted magnetic field is used in the RK4.
    */
   CbmLitCleverTrackExtrapolator(
      const std::string& type);

   /* Destructor */
   virtual ~CbmLitCleverTrackExtrapolator();

   /* Inherited from CbmLitTrackExtrapolator */
   virtual LitStatus Extrapolate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      litfloat zOut,
      std::vector<litfloat>* F);

   /* Inherited from CbmLitTrackExtrapolator */
   virtual LitStatus Extrapolate(
      CbmLitTrackParam* par,
      litfloat zOut,
      std::vector<litfloat>* F);

private:
   TrackExtrapolatorPtr fLineExtrapolator; // line track extrapolation tool
   TrackExtrapolatorPtr fRK4Extrapolator; // RK4 track extrapolation tool
};

#endif /* CBMLITCLEVERTRACKEXTRAPOLATOR_H_ */
