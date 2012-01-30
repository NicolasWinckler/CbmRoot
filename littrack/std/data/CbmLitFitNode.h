/** CbmLitFitNode.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Data class stores fitted track parameters, transport matrix and
 ** chi-square on each detector station.
 **/

#ifndef CBMLITFITNODE_H_
#define CBMLITFITNODE_H_

#include "base/CbmLitFloat.h"
#include "data/CbmLitTrackParam.h"

class CbmLitFitNode
{
public:
   /* Constructor */
   CbmLitFitNode():
      fF(25, 0.),
      fPredictedParam(),
      fUpdatedParam(),
      fSmoothedParam(),
      fChiSqFiltered(0.),
      fChiSqSmoothed(0.) {
   }

   /* Destructor */
   virtual ~CbmLitFitNode() {};

   /* @return Pointer to transport matrix.*/
   const std::vector<litfloat>& GetF() const { return fF; }

   /* @return Pointer to predicted track parameters. */
   const CbmLitTrackParam* GetPredictedParam() const { return &fPredictedParam; }

   /* @return Pointer to updated with KF track parameters. */
   const CbmLitTrackParam* GetUpdatedParam() const { return &fUpdatedParam; }

   /* @return Pointer to smoothed track parameters. */
   const CbmLitTrackParam* GetSmoothedParam() const { return &fSmoothedParam; }

   /* @return Contribution to chi-square of the fitted track parameters and the hit. */
   litfloat GetChiSqFiltered() const { return fChiSqFiltered; }

   /* @return Contribution to chi-square of the smoothed track parameters and the hit. */
   litfloat GetChiSqSmoothed() const { return fChiSqSmoothed; }

   /* Sets the transport matrix
    * @param F Reference to transport matrix to be set.
    */
   void SetF(const std::vector<litfloat>& F) {
      fF.assign(F.begin(), F.end());
   }

   /* Sets the predicted track parameters.
    * @param par Pointer to predicted track parameters.
    */
   void SetPredictedParam(const CbmLitTrackParam* par) { fPredictedParam = *par;}

   /* Sets the updated with KF track parameters.
    * @param par Pointer to updated with KF track parameters.
    */
   void SetUpdatedParam(const CbmLitTrackParam* par) { fUpdatedParam = *par;}

   /* Sets the smoothed track parameters.
    * @param par Pointer to smoothed track parameters.
    */
   void SetSmoothedParam(const CbmLitTrackParam* par) { fSmoothedParam = *par;}

   /* Sets the contribution to the chi-square of the updated track parameters and the hit.
    * @param chiSq Contribution to chi-square.
    */
   void SetChiSqFiltered(litfloat chiSq) { fChiSqFiltered = chiSq; }

   /* Sets the contribution to the chi-square of the smoothed track parameters and the hit.
    * @param chiSq Contribution to chi-square.
    */
   void SetChiSqSmoothed(litfloat chiSq) { fChiSqSmoothed = chiSq; }

private:
   std::vector<litfloat> fF; // transport matrix

   CbmLitTrackParam fPredictedParam; // predicted track parameters
   CbmLitTrackParam fUpdatedParam; // updated with KF track parameters
   CbmLitTrackParam fSmoothedParam; // smoothed track parameters

   litfloat fChiSqFiltered; // Contribution to the chi-square of the updated track parameters and the hit
   litfloat fChiSqSmoothed; // Contribution to the chi-square of the smoothed track parameters and the hit
};

#endif /*CBMLITFITNODE_H_*/
