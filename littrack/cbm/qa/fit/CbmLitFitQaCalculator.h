/**
 * \file CbmLitFitQaCalculator.h
 * \brief Performance calculator for track fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */

#ifndef CBMLITFITQACALCULATOR_H_
#define CBMLITFITQACALCULATOR_H_

#include "CbmDetectorList.h"
#include <string>
#include "TObject.h"
using std::string;
class CbmHistManager;
class TClonesArray;
class FairTrackParam;
class CbmLitMCPoint;
class CbmLitMCTrackCreator;

class CbmLitFitQaCalculator
{
public:
	/**
	 * \brief Constructor.
	 */
	CbmLitFitQaCalculator(
			CbmHistManager* histManager);

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmLitFitQaCalculator();

	/**
	* \brief Initialization. Has to be called in FairTask::Init.
	*/
	void Init();

	/**
	* \brief Execution. Has to be called in FairTask::Exec.
	*/
	void Exec();

	/**
	* \brief Finish. Has to be called in FairTask::Finish.
	*/
	void Finish();

	/* Setters */
	void SetMvdMinNofHits(Int_t minNofHits) { fMvdMinNofHits = minNofHits; }
	void SetStsMinNofHits(Int_t minNofHits) { fStsMinNofHits = minNofHits; }
	void SetTrdMinNofHits(Int_t minNofHits) { fTrdMinNofHits = minNofHits; }
	void SetMuchMinNofHits(Int_t minNofHits) { fMuchMinNofHits = minNofHits; }

private:
	/**
	* \brief Reads data branches.
	*/
	void ReadDataBranches();

	void ProcessGlobalTracks();

	void ProcessStsTrack(
		 Int_t trackId);

	void ProcessTrdTrack(
		 Int_t trackId);

	void ProcessMuchTrack(
		 Int_t trackId);

	void FillResidualsAndPulls(
		const FairTrackParam* par,
		const CbmLitMCPoint* mcPoint,
		const string& histName,
		Float_t wrongPar,
		DetectorId detId);

	// Data branches
	TClonesArray* fGlobalTracks; // CbmGlobalTrack array
	TClonesArray* fStsTracks; // CbmStsTrack array
	TClonesArray* fStsTrackMatches; // CbmTrackMatch array
	TClonesArray* fStsHits; // CbmStsHit
	TClonesArray* fMvdHits; // CbmMvdHit
	TClonesArray* fTrdTracks; // CbmTrdTrack array
	TClonesArray* fTrdTrackMatches; // CbmTrackMatch array
	TClonesArray* fTrdHits; // CbmTrdHit array
	TClonesArray* fMuchTracks; // CbmStsTrack array
	TClonesArray* fMuchTrackMatches; // CbmTrackMatch array
	TClonesArray* fMuchPixelHits; // CbmMuchPixelHit array
	TClonesArray* fMuchStripHits; // CbmMuchStripHit array

	Int_t fMvdMinNofHits; // Cut on minimum number of hits in track in MVD
	Int_t fStsMinNofHits; // Cut on minimum number of hits in track in STS
	Int_t fTrdMinNofHits; // Cut on minimum number of hits in track in TRD
	Int_t fMuchMinNofHits; // Cut on minimum number of hits in track in MUCH

	Double_t fQuota; // percent of correctly attached hits

	CbmHistManager* fHM; // Histogram manager
	CbmLitMCTrackCreator* fMCTrackCreator; // MC track creator tool
};

#endif /* CBMLITFITQACALCULATOR_H_ */
