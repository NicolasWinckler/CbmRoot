#ifndef CBMLITROBUSTFITTERANALYSIS_H_
#define CBMLITROBUSTFITTERANALYSIS_H_

#include "CbmLitDetectorLayout.h"

#include "FairTask.h"
#include "CbmDetectorList.h"
#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrack;
class CbmTrackMatch;
//class CbmTrdTrackMatch;
class TList;
class TH1D;
class TH2D;

class CbmLitRobustFitterAnalysis : public FairTask
{
public:
	CbmLitRobustFitterAnalysis(
			DetectorId detId);
	virtual ~CbmLitRobustFitterAnalysis();

	virtual InitStatus Init();
	virtual void Exec(
			Option_t* opt);
	virtual void FinishTask();
	virtual void SetParContainers();

protected:
	void CreateHistograms();
	void ReadDataBranches();
	void CreateTrackArrays();
	void DeleteTrackArrays();
	void InitTrackParamFromStsTrackParam(
			Int_t trackIndex,
			CbmLitTrack* track);
	void FillHistograms(
			Int_t id);
	void TestFitter(
			Int_t id);
	void MatchMuchTracks(
			TrackPtrVector& tracks,
			std::vector<CbmTrackMatch*>& matches);
	void MatchTrdTracks(
			TrackPtrVector& tracks,
			std::vector<CbmTrackMatch*>& matches);
private:
	CbmLitDetectorLayout fLayout;
	DetectorId fDetId;
	Int_t fNofVars;
	std::vector<TrackPtrVector> fLitTracks;
	std::vector<std::vector<CbmTrackMatch*> > fLitMuchTrackMatches;
	std::vector<std::vector<CbmTrackMatch*> > fLitTrdTrackMatches;

	Int_t fNofPlanes;

	TList* fHistoList;

	TrackFitterPtr fFitter;

//	TClonesArray* fMCTracks;
	TClonesArray* fPoints;
	TClonesArray* fHits;
	TClonesArray* fTracks;
	TClonesArray* fTrackMatches;
	TClonesArray* fDigiMatches;
	TClonesArray* fStsTracks;
//	TClonesArray* fStsTrackMatches;

//	Int_t fNof1DParams;
//	Int_t fNof2DParams;
	//Histograms
//	std::vector<std::vector<TH1D*> > fh1DHistos;
//	std::vector<std::vector<TH2D*> > fh2DHistos;
	Int_t fNofParams;
	std::vector<TH2D*> fhPlaneNofHits;
	std::vector<TH2D*> fhLastPlaneIdNofHits;
	std::vector<TH1D*> fhNofHitsInPlane;

	std::vector<TH1D*> fhTrueHitsRatioInTrack;

	std::vector<TH1D*> fhNofHitsInTrack;
	std::vector<TH1D*> fhNofTrueHitsInTrack;
	std::vector<TH1D*> fhNofFalseHitsInTrack;
	std::vector<TH1D*> fhNofHitsInTrueTrack;
	std::vector<TH1D*> fhNofHitsInGhostTrack;

	std::vector<TH1D*> fhTrackLastPlaneId;
	std::vector<TH1D*> fhTrueTrackLastPlaneId;
	std::vector<TH1D*> fhGhostTrackLastPlaneId;

	std::vector<TH1D*> fhTrackChiSq;
	std::vector<TH1D*> fhTrueTrackChiSq;
	std::vector<TH1D*> fhGhostTrackChiSq;



	Int_t fEvents;
	Int_t fVerbose;

	ClassDef(CbmLitRobustFitterAnalysis, 1);
};

#endif /*CBMLITROBUSTFITTERANALYSIS_H_*/
