/** CbmLitRK4TrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 **
 **/

#ifndef CBMLITRK4TRACKEXTRAPOLATOR_H_
#define CBMLITRK4TRACKEXTRAPOLATOR_H_

#include "CbmLitTrackExtrapolator.h"

#include "CbmLitTrackParam.h"
#include "FairField.h"

#include <vector>

class CbmLitRK4TrackExtrapolator: public CbmLitTrackExtrapolator {
public:
   CbmLitRK4TrackExtrapolator();
   virtual ~CbmLitRK4TrackExtrapolator();

   // derived from CbmTool
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

protected:

   void RK4Order(
		   const std::vector<myf>& xIn,
		   myf zIn,
		   std::vector<myf>& xOut,
           myf zOut,
           std::vector<myf>& derivs) const;

   myf CalcOut(
   		myf in,
   		const myf k[4]) const;

   void TransportC(
   		const std::vector<myf>& cIn,
   		const std::vector<myf>& F,
   		std::vector<myf>& cOut) const;

private:
	std::vector<myf> fF;
	FairField *fMagneticField;
};

#endif //CBMLITRK4TRACKEXTRAPOLATOR_H_

