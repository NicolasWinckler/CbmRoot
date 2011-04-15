/** CbmLitKalmanSmoother.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** Class implements Kalman smoother algorithm.
 **/

#ifndef CBMLITKALMANSMOOTHER_H_
#define CBMLITKALMANSMOOTHER_H_

#include "interface/CbmLitTrackFitter.h"

#include <vector>

class CbmLitFitNode;

class CbmLitKalmanSmoother : public CbmLitTrackFitter
{
public:
   /* Constructor */
   CbmLitKalmanSmoother();

   /* Destructor */
   virtual ~CbmLitKalmanSmoother();

   /* Inherited from CbmLitTool */
   virtual LitStatus Initialize();

   /* Inherited from CbmLitTool */
   virtual LitStatus Finalize();

   /* Inherited from CbmLitTrackFitter */
   virtual LitStatus Fit(
      CbmLitTrack* track,
      bool downstream = false);
private:
   /* Smoothes one fit node
    *@param thisNode Current fit node to be smoothed
    *@param prevNode Previous fit node */
   void Smooth(
      CbmLitFitNode* thisNode,
      const CbmLitFitNode* prevNode);
};

#endif /*CBMLITKALMANSMOOTHER_H_*/
