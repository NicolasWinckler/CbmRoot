/** CbmLitLineTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 **
 **/

#ifndef CBMLITLINETRACKEXTRAPOLATOR_H_
#define CBMLITLINETRACKEXTRAPOLATOR_H_

#include "CbmLitTrackExtrapolator.h"

#include "TMatrixD.h"

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
           double zOut);

   virtual LitStatus Extrapolate(
		   CbmLitTrackParam *par,
           double zOut );

   virtual void TransportMatrix(
		   std::vector<double>& F);
   virtual void TransportMatrix(
		   TMatrixD& F);

private:
	double fDz;
};

#endif //CbmLitLineTrackExtrapolator

