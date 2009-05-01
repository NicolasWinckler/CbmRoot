#ifndef CBMLITFINDGLOBALTRACKS_H_
#define CBMLITFINDGLOBALTRACKS_H_

#include "FairTask.h"

#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

class TClonesArray;

class CbmLitFindGlobalTracks : public FairTask
{
public:
	CbmLitFindGlobalTracks();
	virtual ~CbmLitFindGlobalTracks();

	virtual void SetParContainers();
	virtual InitStatus Init();
	virtual InitStatus ReInit();
	virtual void Exec(Option_t* opt);

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
	TrackPtrVector fLitMuchTracks;
	HitPtrVector fLitTrdHits;
	TrackPtrVector fLitTrdTracks;
	HitPtrVector fLitTofHits;

	// Tools
	TrackFinderPtr fFinder;
	HitToTrackMergerPtr fMerger;

	Int_t fEventNo;

	ClassDef(CbmLitFindGlobalTracks, 1);
};

#endif /* CBMLITFINDGLOBALTRACKS_H_ */
