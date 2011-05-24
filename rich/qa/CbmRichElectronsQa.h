/** CbmRichElectronsQa.h
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2008
 * @version 2.0
 **/

#ifndef CBMRICHELECTRONSQA_H
#define CBMRICHELECTRONSQA_H

#include "FairTask.h"

class TH1D;
class TH2D;
class TClonesArray;

#include <map>
#include <fstream>
#include <string>

using std::map;
using std::string;

class CbmRichElectronsQa : public FairTask{

public:

	CbmRichElectronsQa(
	      const char *name,
	      const char *title,
	      Int_t verbose);

	virtual ~CbmRichElectronsQa();

	virtual InitStatus Init();

	virtual void Exec(
	      Option_t* option);

	virtual void FinishTask();

private:
	Int_t fEventNum;

	TClonesArray* fRichHits;
	TClonesArray* fRichRings;
	TClonesArray* fRichPoints;
	TClonesArray* fMCTracks;
	TClonesArray* fRichRingMatches;
	TClonesArray* fRichProj;
	TClonesArray* fStsTrackMatches;
	TClonesArray* fGlobalTracks;
	TClonesArray* fStsTracks;

   Int_t fMinNofHitsInRichRing;
   Double_t fQuota;

   void DiffElandPi();

   std::ofstream  fOutElandPi;

// difference between electrons and pions
	TH1D* fhAaxisEl;
	TH1D* fhAaxisPi;
	TH1D* fhBAxisEl;
	TH1D* fhBAxisPi;
	TH1D* fhAaxisCorEl;
	TH1D* fhAaxisCorPi;
	TH1D* fhBAxisCorEl;
	TH1D* fhBAxisCorPi;
	TH1D* fhDistEl;
	TH1D* fhDistPi;
	TH1D* fhNofHitsEl;
	TH1D* fhNofHitsPi;
	TH1D* fhChi2El;
	TH1D* fhChi2Pi;
	TH1D* fhRadPosEl;
	TH1D* fhRadPosPi;
	TH2D* fhAaxisVsMomEl;
	TH2D* fhAaxisVsMomPi;
	TH2D* fhBAxisVsMomEl;
	TH2D* fhBAxisVsMomPi;
	TH2D* fhPhiVsRadAngEl;
	TH2D* fhPhiVsRadAngPi;

	ClassDef(CbmRichElectronsQa,1)
};

#endif

