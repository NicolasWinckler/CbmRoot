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
           double zOut);

   virtual LitStatus Extrapolate(
		   CbmLitTrackParam *par,
           double zOut );

   virtual void TransportMatrix(
		   std::vector<double>& F);
   virtual void TransportMatrix(
		   TMatrixD& F);

protected:

   void RK4Order(
		   const std::vector<double>& xIn,
		   double zIn,
		   std::vector<double>& xOut,
           double zOut,
           std::vector<double>& derivs) const;

   double CalcOut(
   		double in,
   		const double k[4]) const;

   void TransportC(
   		const std::vector<double>& cIn,
   		const std::vector<double>& F,
   		std::vector<double>& cOut) const;

private:
	std::vector<double> fF;
	FairField *fMagneticField;
};

#endif //CBMLITRK4TRACKEXTRAPOLATOR_H_

