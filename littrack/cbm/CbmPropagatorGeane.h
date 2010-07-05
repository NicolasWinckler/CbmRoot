#ifndef CBMPROPAGATORGEANE_H
#define CBMPROPAGATORGEANE_H

#include "CbmPropagator.h"

#include "FairField.h"
#include "FairGeanePro.h"

//#include "TMatrixD.h"

#include <vector>

class TMatrixD;

class CbmPropagatorGeane: public CbmPropagator {
public:
   CbmPropagatorGeane();
   virtual ~CbmPropagatorGeane();

   virtual StatusCode Propagate(
		   const FairTrackParam *parIn,
           FairTrackParam *parOut,
           Double_t zOut,
           Int_t pdg);

   virtual StatusCode Propagate(
		   FairTrackParam *par,
           Double_t zOut,
           Int_t pdg);

   virtual void TransportMatrix(
		   std::vector<Double_t>& F);

   virtual void TransportMatrix(
		   TMatrixD& F);

private:
	void ToGeaneCovMatrix(
			const std::vector<Double_t>& cov,
			std::vector<Double_t>& gCov) const;

	void FromGeaneCovMatrix(
			const std::vector<Double_t>& gCov,
			std::vector<Double_t>& cov) const;

	Bool_t IsInParCorrect(
			const FairTrackParam* par) const;

   FairGeanePro *fPropagator;

   Double_t fMinSlope;
   Double_t fMaxSlope;
};

#endif //CbmPropagatorGeane
