#ifndef CBMLITPROPANA_H
#define CBMLITPROPANA_H 1

#include "FairTask.h"
#include "CbmDetectorList.h"

#include <vector>

class CbmLitTrackUpdate;
class CbmLitTrackExtrapolator;
class CbmLitTrackPropagator;
class CbmLitTrackFitter;
class CbmLitTrackParam;
class CbmLitTrack;
class FairMCPoint;
class CbmMCTrack;
class TClonesArray;
class TH1F;

class CbmLitPropAna : public FairTask
{

public:

   CbmLitPropAna(
		   DetectorId detId,
		   Int_t nofPlanes);
   virtual ~CbmLitPropAna();

   virtual InitStatus Init();

   virtual void Exec(Option_t* opt);

   virtual void Finish();

   virtual void SetParContainers();

private:
   TClonesArray* fMCTracks;
   TClonesArray* fMCPoints;
   TClonesArray* fHits;
   TClonesArray* fTracks;
   TClonesArray* fDigiMatches;

   CbmLitTrackExtrapolator *fExtrapolator;
   CbmLitTrackPropagator *fPropagator;
   CbmLitTrackExtrapolator *fExtrapolatorDet;
   CbmLitTrackPropagator *fPropagatorDet;
   CbmLitTrackUpdate *fFilter;
   CbmLitTrackFitter *fFitter;
   CbmLitTrackFitter *fSmoother;

   Int_t fNofPlanes;
   DetectorId fDetId; // TRD or MUCH

   std::vector<TH1F *> fh_srx;
   std::vector<TH1F *> fh_sry;

   std::vector<TH1F *> fh_resx;
   std::vector<TH1F *> fh_resy;
   std::vector<TH1F *> fh_restx;
   std::vector<TH1F *> fh_resty;
   std::vector<TH1F *> fh_resqp;

   std::vector<TH1F *> fh_pullx;
   std::vector<TH1F *> fh_pully;
   std::vector<TH1F *> fh_pulltx;
   std::vector<TH1F *> fh_pullty;
   std::vector<TH1F *> fh_pullqp;

   std::vector<TH1F *> fh_resp;

   TH1F* fh_pullx_last, *fh_pullx_first;
   TH1F* fh_pully_last, *fh_pully_first;
   TH1F* fh_pulltx_last, *fh_pulltx_first;
   TH1F* fh_pullty_last, *fh_pullty_first;
   TH1F* fh_pullqp_last, *fh_pullqp_first;

   TH1F* fh_resx_last, *fh_resx_first;
   TH1F* fh_resy_last, *fh_resy_first;
   TH1F* fh_restx_last, *fh_restx_first;
   TH1F* fh_resty_last, *fh_resty_first;
   TH1F* fh_resqp_last, *fh_resqp_first;

   TH1F* fh_resp_last, *fh_resp_first;

   Int_t fEvents;

   Int_t fVerbose;

   Bool_t CreateTrackMuch(
   		Int_t trackId,
   		CbmLitTrack* track,
   		std::vector<FairMCPoint*>& points);

   Bool_t CreateTrackTrd(
	   	Int_t trackId,
	   	CbmLitTrack* track,
	   	std::vector<FairMCPoint*>& points);

   void FillParam(
		   const FairMCPoint* point,
		   CbmLitTrackParam* par);

   void CreateHistograms();

   void CalcResAndPull(
   		const FairMCPoint* point,
   		const CbmMCTrack* mcTrack,
   		const CbmLitTrackParam* par,
   		std::vector<Double_t>& res,
   		std::vector<Double_t>& pull,
   		Double_t& resp);

   ClassDef(CbmLitPropAna,1);

};

#endif
