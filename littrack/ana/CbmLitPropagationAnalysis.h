#ifndef CBMLITPROPAGATIONANALYSIS_H
#define CBMLITPROPAGATIONANALYSIS_H 1

#include "FairTask.h"
#include "CbmDetectorList.h"
#include "CbmLitDetectorLayout.h"
#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

#include <vector>

class CbmLitTrackParam;
class CbmLitTrack;
class CbmLitHit;
class FairMCPoint;
class CbmMCTrack;
class CbmStsTrack;
class CbmTrdTrack;
class CbmMuchTrack;
class TClonesArray;
class TH1F;

class CbmLitPropagationAnalysis : public FairTask
{
public:
	CbmLitPropagationAnalysis(
		   DetectorId detId);
	virtual ~CbmLitPropagationAnalysis();
	virtual InitStatus Init();
	virtual void Exec(
			Option_t* opt);
	virtual void Finish();
	virtual void SetParContainers();

private:
	void CreateHistograms();
	void ReadDataBranches();
	void RunTest();
	void TestPropagation(
			CbmLitTrack* track,
			CbmLitTrack* mcTrack);
	void TestFitter(
			CbmLitTrack* track,
			CbmLitTrack* mcTrack);
	void FillHistosPropagation(
			const CbmLitTrackParam* par,
			const CbmLitTrackParam* mcPar,
			const CbmLitHit* hit,
			Int_t plane);
	void FillHistosFitter(
			const CbmLitTrack* track,
			const CbmLitTrack* mcTrack);
	void CreateTrackArrays();
	void DeleteTrackArrays();
	void McMuchTrackToLitTrack(
			CbmMuchTrack* muchTrack,
			CbmLitTrack* track);
	void McTrdTrackToLitTrack(
			const CbmTrdTrack* trdTrack,
			CbmLitTrack* track);
	void McPointToLitFitNode(
			FairMCPoint* point,
			CbmLitFitNode* node);
	std::vector<Double_t> CalcResidualsAndPulls(
			const CbmLitTrackParam* par,
			const CbmLitTrackParam* mcPar);
	Bool_t InitTrackParamFromStsTrackParam(
			Int_t trackIndex,
			CbmLitTrack* track);
	CbmStsTrack* FindStsTrackByMc(
			Int_t mcId);

	CbmLitDetectorLayout fLayout;
	TrackPtrVector fLitTracks;
	TrackPtrVector fLitMcTracks;
	Int_t fNofPlanes;

	TClonesArray* fMCTracks;
	TClonesArray* fMCPoints;
	TClonesArray* fHits;
	TClonesArray* fTracks;
	TClonesArray* fTrackMatches;
	TClonesArray* fDigiMatches;
	TClonesArray* fStsTracks;
	TClonesArray* fStsTrackMatches;

	TrackPropagatorPtr fPropagator;
	TrackUpdatePtr fFilter;
	TrackFitterPtr fFitter;
	TrackFitterPtr fSmoother;

	DetectorId fDetId; // kTRD or kMUCH

	std::vector<std::vector<TH1F*> > fPropagationHistos;
	std::vector<std::vector<TH1F*> > fFilterHistos;
	std::vector<std::vector<TH1F*> > fSmootherHistos;
	Int_t fNofParams;

	Int_t fEvents;
	Int_t fVerbose;

	ClassDef(CbmLitPropagationAnalysis,1);
};

#endif
