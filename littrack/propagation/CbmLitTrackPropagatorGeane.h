/** CbmTrackPropagatorGeane.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 **
 **/

#ifndef CBMLITTRACKPROPAGATORGEANE_H
#define CBMLITTRACKPROPAGATORGEANE_H

#include "CbmLitTrackPropagator.h"

#include "TMatrixD.h"

#include <vector>

class CbmPropagator;

class CbmLitTrackPropagatorGeane: public CbmLitTrackPropagator {
public:
   CbmLitTrackPropagatorGeane();
   virtual ~CbmLitTrackPropagatorGeane();

   // derived from CbmTool
   virtual LitStatus Initialize();
   virtual LitStatus Finalize();

   // Propagator
   virtual LitStatus Propagate(
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           Double_t zOut,
           Int_t pdg);

   virtual LitStatus Propagate(
		   CbmLitTrackParam *pParam,
           Double_t zOut,
           Int_t pdg);

   virtual void TransportMatrix(
		   std::vector<Double_t>& F){
	   //TODO: implement transport matrix for geane
   }
   virtual void TransportMatrix(
		   TMatrixD& F){
	   //TODO: implement transport matrix for geane
   }

private:
   CbmPropagator *fPropagator;
};

#endif //CbmLitTrackPropagatorGeane
