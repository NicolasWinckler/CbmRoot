#ifndef CBMLITROBUSTFITTERANALYSIS_H_
#define CBMLITROBUSTFITTERANALYSIS_H_

#include "CbmLitDetectorLayout.h"

#include "FairTask.h"
#include "CbmDetectorList.h"
#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

//class CbmLitTrackFitter;
class CbmLitTrack;
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
	virtual void Finish();
	virtual void SetParContainers();

protected:
	void CreateHistograms();
	void ReadDataBranches();
	void CreateTrackArrays();
	void DeleteTrackArrays();
	void InitTrackParamFromStsTrackParam(
			Int_t trackIndex,
			CbmLitTrack* track);
	void AnalyseTracks();
	void TestFitter();

private:
	CbmLitDetectorLayout fLayout;
	DetectorId fDetId;
	TrackPtrVector fLitTracks;
	Int_t fNofPlanes;

	TList* fHistoList;

	TrackFitterPtr fFitter;

//	TClonesArray* fMCTracks;
//	TClonesArray* fMCPoints;
	TClonesArray* fHits;
	TClonesArray* fTracks;
	TClonesArray* fTrackMatches;
//	TClonesArray* fDigiMatches;
	TClonesArray* fStsTracks;
//	TClonesArray* fStsTrackMatches;

	//Histograms
	TH2D* fhPlaneNofHits;
	TH2D* fhLastPlaneIdNofHits;
	TH1D* fhNofHitsInTrack;
	TH1D* fhNofTrueHitsInTrack;
	TH1D* fhNofFalseHitsInTrack;
	TH1D* fhLastPlaneIdInTrack;
	TH1D* fhNofHitsInPlane;

	Int_t fEvents;
	Int_t fVerbose;

	ClassDef(CbmLitRobustFitterAnalysis, 1);
};

#endif /*CBMLITROBUSTFITTERANALYSIS_H_*/
