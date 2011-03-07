/** CbmLitKalmanFilter.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 **
 **/

#ifndef CBMLITKALMANFILTER_H_
#define CBMLITKALMANFILTER_H_

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
           const CbmLitHit *hit);

   virtual LitStatus Update(
		   CbmLitTrackParam *par,
		   const CbmLitHit *hit);

private:
	LitStatus Update(
		   CbmLitTrackParam *par,
		   const CbmLitPixelHit *hit);

	LitStatus UpdateWMF(
			CbmLitTrackParam *par,
	        const CbmLitPixelHit *hit);

	LitStatus Update(
		   CbmLitTrackParam *par,
		   const CbmLitStripHit *hit);

	LitStatus UpdateWMF(
			CbmLitTrackParam *par,
	        const CbmLitStripHit *hit);
};

#endif //CbmLitKalmanFilter

