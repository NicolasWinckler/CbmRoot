/** CbmLitKalmanFilter.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 **
 **/

#ifndef CBMLITKALMANFILTER_H_
#define CBMLITKALMANFILTER_H_

#include "base/CbmLitTypes.h"
#include "interface/CbmLitTrackUpdate.h"

class CbmLitHit;
class CbmLitStripHit;
class CbmLitPixelHit;
class CbmLitTrackParam;

class CbmLitKalmanFilter: public CbmLitTrackUpdate
{
public:
   CbmLitKalmanFilter();

   virtual ~CbmLitKalmanFilter();

   // derived from CbmTool
   virtual LitStatus Initialize();
   virtual LitStatus Finalize();

   // Filter
   virtual LitStatus Update(
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           const CbmLitHit *hit,
           myf &chiSq);

   virtual LitStatus Update(
		   CbmLitTrackParam *par,
		   const CbmLitHit *hit,
           myf &chiSq);

private:
	LitStatus Update(
		   CbmLitTrackParam *par,
		   const CbmLitPixelHit *hit,
           myf &chiSq);

	LitStatus UpdateWMF(
			CbmLitTrackParam *par,
	        const CbmLitPixelHit *hit,
	        myf &chiSq);

	LitStatus Update(
		   CbmLitTrackParam *par,
		   const CbmLitStripHit *hit,
           myf &chiSq);

	LitStatus UpdateWMF(
			CbmLitTrackParam *par,
	        const CbmLitStripHit *hit,
	        myf &chiSq);
};

#endif //CbmLitKalmanFilter

