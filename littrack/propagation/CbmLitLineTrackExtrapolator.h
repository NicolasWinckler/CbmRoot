/** CbmLitLineTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 **
 **/

#ifndef CBMLITLINETRACKEXTRAPOLATOR_H_
#define CBMLITLINETRACKEXTRAPOLATOR_H_

#include "CbmLitTrackExtrapolator.h"

class CbmLitTrackParam;

class CbmLitLineTrackExtrapolator: public CbmLitTrackExtrapolator {

public:
   CbmLitLineTrackExtrapolator();
   virtual ~CbmLitLineTrackExtrapolator();

   // Derived from CbmTool
   virtual LitStatus Initialize();
   virtual LitStatus Finalize();

   virtual LitStatus Extrapolate(
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           myf zOut);

   virtual LitStatus Extrapolate(
		   CbmLitTrackParam *par,
           myf zOut );

   virtual void TransportMatrix(
		   std::vector<myf>& F);

private:
	myf fDz;
};

#endif //CbmLitLineTrackExtrapolator

