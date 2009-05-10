#ifndef CBMLITFINDGLOBALTRACKS_H_
#define CBMLITFINDGLOBALTRACKS_H_

#include "FairTask.h"

#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

#include <string>

class TClonesArray;

class CbmLitFindGlobalTracks : public FairTask
{
public:
	CbmLitFindGlobalTracks();
	virtual ~CbmLitFindGlobalTracks();

	virtual InitStatus Init();
	virtual void Exec(Option_t* opt);
	virtual void SetParContainers();

	void SetTrackingType(const std::string& trackingType) { fTrackingType = trackingType;}
	void SetMergerType(const std::string& mergerType) { fMergerType = mergerType;}

private:
	virtual void Finish();
	void DetermineSetup();
	void ReadAndCreateDataBranches();
	void InitTrackReconstruction();
	void ConvertInputData();
	void ConvertOutputData();
	void ClearArrays();
	void InitStsTrackSeeds();
	void RunTrackReconstruction();

	Bool_t fIsElectronSetup;
	Bool_t fIsTrd;
	Bool_t fIsMuch;
	Bool_t fIsTof;

	TClonesArray* fStsTracks;
	TClonesArray* fMuchHits;
	TClonesArray* fMuchTracks; //output
	TClonesArray* fTrdHits;
	TClonesArray* fTrdTracks; //output
	TClonesArray* fTofHits;
	TClonesArray* fGlobalTracks;   //output

	TrackPtrVector fLitStsTracks;
	HitPtrVector fLitMuchHits;
	HitPtrVector fLitTrdHits;
	HitPtrVector fLitTofHits;
	TrackPtrVector fLitOutputTracks; // output Lit tracks

	// Tools
	TrackFinderPtr fFinder;
	HitToTrackMergerPtr fMerger;

	// Settings
	// Tracking method to be used
	// "branch" - branching method
	// "nn" - nearest neighbor method
	// "weight" - weighting method
	std::string fTrackingType;
	// Merger method to be used
	// "nearest_hit" - assigns nearest hit to the track
	std::string fMergerType;

	Int_t fEventNo;

	ClassDef(CbmLitFindGlobalTracks, 1);
};

#endif /* CBMLITFINDGLOBALTRACKS_H_ */
