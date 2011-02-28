/** CbmLitReconstructionQa.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **/

#include "qa/CbmLitReconstructionQa.h"

#include "base/CbmLitEnvironment.h"
#include "utils/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"

#include "CbmGlobalTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "CbmDetectorList.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TList.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPad.h"

#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

// histogram types
const Int_t ACC=0; // accepted tracks histogram
const Int_t REC=1; // reconstructed tracks histogram
const Int_t EFF=2; // efficiency histogram
// track categories
const Int_t ALL=0; // all tracks
const Int_t REF=1; // reference tracks
const Int_t PRIM=2; // primary tracks
const Int_t SEC=3; // secondary tracks
const Int_t MU=4; // primary muon tracks
const Int_t EL=5; // primary electron tracks
// for hits in the track distribution
const Int_t ALLHITS = 0;
const Int_t TRUEHITS = 1;
const Int_t FAKEHITS = 2;
const Int_t TRUEALL = 3;
const Int_t FAKEALL = 4;

CbmLitReconstructionQa::CbmLitReconstructionQa():
	FairTask("LitReconstructionQA", 1),

	fMinNofPointsSts(4),
	fMinNofPointsTrd(8),
	fMinNofPointsMuch(10),
	fMinNofPointsTof(1),
	fQuota(0.7),

	fMinNofHitsTrd(8),
	fMinNofHitsMuch(10),

	fIsElectronSetup(false),
	fIsSts(false),
	fIsTrd(false),
	fIsMuch(false),
	fIsTof(false),

	fRefMomentum(2.),
	fMinMom(0.),
	fMaxMom(25.),
	fNofBinsMom(25),
	fMinAngle(0.),
	fMaxAngle(30.),
    fNofBinsAngle(3),

    fMcStsMap(),
    fMcHalfGlobalMap(),
    fMcGlobalMap(),

	fMCTracks(NULL),
	fGlobalTracks(NULL),
	fStsMatches(NULL),
	fMuchPixelHits(NULL),
	fMuchStrawHits(NULL),
	fMuchMatches(NULL),
	fTrdMatches(NULL),
	fTrdHits(NULL),
	fTofPoints(NULL),
	fTofHits(NULL),

	fNofCategories(6),
	fNofTypes(3),

	fhStsMom(),
	fhStsNp(),
	fhStsAngle(),
	fhHalfGlobalMom(),
	fhGlobalMom(),
	fhRecMom(),
	fhRecNp(),
	fhRecAngle(),
	fhTofMom(),

	fhStsGhostNh(NULL),
	fhRecGhostNh(NULL),

	fhTrdNofHitsInStation(NULL),
	fhMuchNofHitsInStation(NULL),
	fhTofNofHitsInStation(NULL),

	fhStsTrackHits(),
	fhTrdTrackHits(),
	fhMuchTrackHits(),

	fHistoList(NULL),

	fNofGlobalTracks(0),
	fNofStsTracks(0),
	fNofTrdTracks(0),
	fNofMuchTracks(0),
	fNofTofHits(0),
	fEventNo(0),
	fOutputDir("")
{
}

CbmLitReconstructionQa::~CbmLitReconstructionQa()
{
	fHistoList->Delete();
	delete fHistoList;
}

InitStatus CbmLitReconstructionQa::Init()
{
    DetermineSetup();
    ReadDataBranches();
    CreateHistos();

    return kSUCCESS;
}

void CbmLitReconstructionQa::Exec(
		Option_t* opt)
{
	ProcessHits();
	ProcessGlobalTracks();
	ProcessMcTracks();
	PrintEventStatistics();
	IncreaseCounters();

	std::cout << "Event: " << fEventNo++ << std::endl;
}

void CbmLitReconstructionQa::Finish()
{
	CalculateEfficiencyHistos();
	WriteToFile();
	PrintFinalStatistics(std::cout);
	Draw();
}

void CbmLitReconstructionQa::DetermineSetup()
{
    CbmLitEnvironment* env = CbmLitEnvironment::Instance();
    fIsElectronSetup = env->IsElectronSetup();
	fIsSts = true;
    fIsTrd = env->IsTrd();
    fIsMuch = env->IsMuch();
    fIsTof = env->IsTof();

    if (fIsElectronSetup) std::cout << "-I- CBM electron setup detected" << std::endl;
    else std::cout << "-I- CBM muon setup detected" << std::endl;
    std::cout << "-I- The following detectors were found in the CBM setup:" << std::endl;
    if (fIsTrd) std::cout << "TRD" << std::endl;
    if (fIsMuch) std::cout << "MUCH" << std::endl;
    if (fIsTof) std::cout << "TOF" << std::endl;
}

void CbmLitReconstructionQa::ReadDataBranches()
{
	FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init","CbmRootManager is not instantiated");

    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    if (NULL == fMCTracks) Fatal("Init","No MCTrack array!");

    fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
    if (NULL == fGlobalTracks) Fatal("Init","No GlobalTrack array!");

    fStsMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
    if (NULL == fStsMatches) Fatal("Init",": No StsTrackMatch array!");

    if (fIsMuch) {
		fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
		fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
		if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) Fatal("CbmLitReconstructionQa::Init", "No MuchPixelHit AND MuchStrawHit arrays!");
    	fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
    	if (NULL == fMuchMatches) Fatal("Init","No MuchTrackMatch array!");
    }

    if (fIsTrd) {
    	fTrdMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
    	if (NULL == fTrdMatches) Fatal("Init","No TrdTrackMatch array!");
    	fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
    	if (NULL == fTrdHits) Fatal("Init","No TrdHit array!");
    }

    if (fIsTof) {
    	fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
    	if (NULL == fTofPoints) Fatal("Init", "No TofPoint array!");
    	fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
    	if (NULL == fTofHits) Fatal("Init", "No TofHit array!");
    }
}

void CbmLitReconstructionQa::ProcessHits()
{
	if (fIsTrd) {
		for (Int_t i = 0; i < fTrdHits->GetEntriesFast(); i++) {
			CbmBaseHit* hit = (CbmBaseHit*) fTrdHits->At(i);
			fhTrdNofHitsInStation->Fill(hit->GetPlaneId());
		}
	}
	if (fIsMuch) {
		for (Int_t i = 0; i < fMuchPixelHits->GetEntriesFast(); i++) {
			CbmBaseHit* hit = (CbmBaseHit*) fMuchPixelHits->At(i);
			fhMuchNofHitsInStation->Fill(hit->GetPlaneId());
		}
		for (Int_t i = 0; i < fMuchStrawHits->GetEntriesFast(); i++) {
			CbmBaseHit* hit = (CbmBaseHit*) fMuchStrawHits->At(i);
			fhMuchNofHitsInStation->Fill(hit->GetPlaneId());
		}
	}
	if (fIsTof) {
		for (Int_t i = 0; i < fTofHits->GetEntriesFast(); i++) {
			CbmBaseHit* hit = (CbmBaseHit*) fTofHits->At(i);
			fhTofNofHitsInStation->Fill(hit->GetPlaneId());
		}
	}
}

void CbmLitReconstructionQa::ProcessGlobalTracks()
{
	fMcStsMap.clear();
	fMcHalfGlobalMap.clear();
	fMcGlobalMap.clear();
	Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
	for(Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
		CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
		if (globalTrack == NULL) continue;

		// get track segments indices
		Int_t stsId = globalTrack->GetStsTrackIndex();
		Int_t trdId = globalTrack->GetTrdTrackIndex();
		Int_t muchId = globalTrack->GetMuchTrackIndex();
		Int_t tofId = globalTrack->GetTofHitIndex();

		// check track segments
		Bool_t isStsOk = stsId > -1 && fIsSts;
		Bool_t isTrdOk = trdId > -1 && fIsTrd;
		Bool_t isMuchOk = muchId > -1 && fIsMuch;
		Bool_t isTofOk = tofId > -1 && fIsTof;

		// check the quality of track segments
		CbmTrackMatch* stsTrackMatch;
		if (isStsOk) {
			stsTrackMatch = (CbmTrackMatch*) fStsMatches->At(stsId);
			isStsOk = CheckTrackQuality(stsTrackMatch, kSTS);
			if (!isStsOk) { // ghost track
				Int_t nofHits = stsTrackMatch->GetNofTrueHits() + stsTrackMatch->GetNofWrongHits() + stsTrackMatch->GetNofFakeHits();
				fhStsGhostNh->Fill(nofHits);
			}
		}
		CbmTrackMatch* trdTrackMatch;
		if (isTrdOk) {
			trdTrackMatch = (CbmTrackMatch*) fTrdMatches->At(trdId);
			Int_t nofHits = trdTrackMatch->GetNofTrueHits() + trdTrackMatch->GetNofWrongHits() + trdTrackMatch->GetNofFakeHits();
			if (nofHits >= fMinNofHitsTrd) {
				isTrdOk = CheckTrackQuality(trdTrackMatch, kTRD);
				if (!isTrdOk) { // ghost track
					fhRecGhostNh->Fill(nofHits);
				}
			} else {
				isTrdOk = false;
			}
		}
		CbmTrackMatch* muchTrackMatch;
		if (isMuchOk) {
			muchTrackMatch = (CbmTrackMatch*) fMuchMatches->At(muchId);
			Int_t nofHits = muchTrackMatch->GetNofTrueHits() + muchTrackMatch->GetNofWrongHits() + muchTrackMatch->GetNofFakeHits();
			if (nofHits >= fMinNofHitsMuch) {
				isMuchOk = CheckTrackQuality(muchTrackMatch, kMUCH);
				if (!isMuchOk) { // ghost track
					fhRecGhostNh->Fill(nofHits);
				}
			} else {
				isMuchOk = false;
			}
		}

		// Get MC indices of track segments
		Int_t stsMCId = -1, trdMCId = -1, muchMCId = -1, tofMCId = -1;
		if (isStsOk) stsMCId = stsTrackMatch->GetMCTrackId();
		if (isTrdOk) trdMCId = trdTrackMatch->GetMCTrackId();
		if (isMuchOk) muchMCId = muchTrackMatch->GetMCTrackId();
		if (isTofOk) {
			CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofId);
			CbmTofPoint* tofPoint = (CbmTofPoint*) fTofPoints->At(tofHit->GetRefId());
			tofMCId = tofPoint->GetTrackID();
		}

		// select STS tracks only
		if (isStsOk && stsMCId != -1) {
			fMcStsMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
		}
		//FIXME
		if (fIsMuch && fIsTrd) { // if MUCH and TRD together
			// select STS+MUCH+TRD tracks
			if ((isStsOk && stsMCId != -1) &&
					((isTrdOk && stsMCId == trdMCId) && (isMuchOk && stsMCId == muchMCId))) {
				fMcHalfGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
			}
			// select the longest tracks STS+MUCH+TRD+TOF
			if ( (isStsOk && stsMCId != -1) &&
					((isTrdOk && stsMCId == trdMCId) && (isMuchOk && stsMCId == muchMCId)) &&
					(isTofOk && stsMCId == tofMCId) ) {
				fMcGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
			}
		} else {
			// select STS+TRD(MUCH) tracks
			if ((isStsOk && stsMCId != -1) &&
					((isTrdOk && stsMCId == trdMCId) || (isMuchOk && stsMCId == muchMCId))) {
				fMcHalfGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
			}
			// select the longest tracks STS+TRD(MUCH)+TOF
			if (fIsTrd || fIsMuch) {
				if ( (isStsOk && stsMCId != -1) &&
						((isTrdOk && stsMCId == trdMCId) || (isMuchOk && stsMCId == muchMCId)) &&
						(isTofOk && stsMCId == tofMCId) ) {
					fMcGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
				}
			} else { // for STS+TOF setup
				if ( (isStsOk && stsMCId != -1) &&
						(isTofOk && stsMCId == tofMCId) ) {
					fMcGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
				}
			}
		}
	}
}

Bool_t CbmLitReconstructionQa::CheckTrackQuality(
		CbmTrackMatch* trackMatch,
		DetectorId detId)
{
	Int_t mcId = trackMatch->GetMCTrackId();
	if(mcId < 0) return false;

	Int_t nofTrue = trackMatch->GetNofTrueHits();
	Int_t nofWrong = trackMatch->GetNofWrongHits();
	Int_t nofFake = trackMatch->GetNofFakeHits();
	Int_t nofHits = nofTrue + nofWrong + nofFake;
	Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
	Double_t fakequali = Double_t(nofFake + nofWrong) / Double_t(nofHits);

	if(detId == kSTS) {
		fhStsTrackHits[ALLHITS]->Fill(nofHits);
		fhStsTrackHits[TRUEHITS]->Fill(nofTrue);
		fhStsTrackHits[FAKEHITS]->Fill(nofFake + nofWrong);
		fhStsTrackHits[TRUEALL]->Fill(quali);
		fhStsTrackHits[FAKEALL]->Fill(fakequali);
	}
	if(detId == kTRD) {
		fhTrdTrackHits[ALLHITS]->Fill(nofHits);
		fhTrdTrackHits[TRUEHITS]->Fill(nofTrue);
		fhTrdTrackHits[FAKEHITS]->Fill(nofFake + nofWrong);
		fhTrdTrackHits[TRUEALL]->Fill(quali);
		fhTrdTrackHits[FAKEALL]->Fill(fakequali);
	}
	if(detId == kMUCH) {
		fhMuchTrackHits[ALLHITS]->Fill(nofHits);
		fhMuchTrackHits[TRUEHITS]->Fill(nofTrue);
		fhMuchTrackHits[FAKEHITS]->Fill(nofFake + nofWrong);
		fhMuchTrackHits[TRUEALL]->Fill(quali);
		fhMuchTrackHits[FAKEALL]->Fill(fakequali);
	}

	if (quali < fQuota) return false;
	return true;
}

void CbmLitReconstructionQa::ProcessMcTracks()
{
	Int_t nofMcTracks = fMCTracks->GetEntriesFast();
	for (Int_t iMCTrack = 0; iMCTrack < nofMcTracks; iMCTrack++) {
		CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
		if (mcTrack == NULL) continue;

		// Check accepted tracks cutting on minimal number of MC points
		Int_t nofPointsSts = mcTrack->GetNPoints(kSTS);
		Int_t nofPointsTrd = mcTrack->GetNPoints(kTRD);
		Int_t nofPointsMuch = mcTrack->GetNPoints(kMUCH);
		Int_t nofPointsTof = mcTrack->GetNPoints(kTOF);

		// Check local tracks
		Bool_t isStsOk = nofPointsSts >= fMinNofPointsSts && fIsSts;
		Bool_t isTrdOk = nofPointsTrd >= fMinNofPointsTrd && fIsTrd;
		Bool_t isMuchOk = nofPointsMuch >= fMinNofPointsMuch && fIsMuch;
		Bool_t isTofOk = nofPointsTof >= fMinNofPointsTof && fIsTof;
		Bool_t isStsReconstructed = fMcStsMap.find(iMCTrack) != fMcStsMap.end();
		Bool_t isHalfGlobalReconstructed = fMcHalfGlobalMap.find(iMCTrack) != fMcHalfGlobalMap.end();
		Bool_t isRecOk = (fIsMuch && fIsTrd) ? (isTrdOk && isMuchOk) : (isTrdOk || isMuchOk); // MUCH+TRD

		// calculate polar angle
		TVector3 mom;
		mcTrack->GetMomentum(mom);
		Double_t angle = std::abs(mom.Theta() * 180 / TMath::Pi());

		// Check accepted tracks cutting on minimal number of MC points
		// acceptance: STS tracks only
		if (isStsOk){
			// momentum dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsMom, mcTrack->GetP());
			// number of points dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsNp, nofPointsSts);
			// number of points dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsAngle, angle);
		}
		// acceptance: STS+TRD(MUCH)
		if (isStsOk && isRecOk){
			// momentum dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhHalfGlobalMom, mcTrack->GetP());
		}
		// acceptance: STS+TRD(MUCH)+TOF
		if (isStsOk && isRecOk && isTofOk) {
			// momentum dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhGlobalMom, mcTrack->GetP());
		}

		// acceptance: STS as 100% + local TRD(MUCH) track cutting on number of points
		if ( isStsReconstructed && isStsOk && isRecOk ){
			// momentum dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhRecMom, mcTrack->GetP());
			// number of points dependence histograms
			Int_t np = fIsElectronSetup ? nofPointsTrd : nofPointsMuch;
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhRecNp, np);
			// polar angle dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhRecAngle, angle);
		}

		if (fIsTrd || fIsMuch) {
			if (isHalfGlobalReconstructed && isStsOk && isRecOk && isTofOk) {
				// momentum dependence histograms
				FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhTofMom, mcTrack->GetP());
			}
		} else { // for STS+TOF setup
			if (isStsOk && isTofOk) {
				// momentum dependence histograms
				FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhTofMom, mcTrack->GetP());
			}
		}
	} // Loop over MCTracks
}

void CbmLitReconstructionQa::FillGlobalReconstructionHistos(
		const CbmMCTrack* mcTrack,
		Int_t mcId,
		const std::multimap<Int_t, Int_t>& mcMap,
		std::vector<std::vector<TH1F*> >& hist,
		Double_t par)
{
	// Get MC track properties (vertex, momentum, primary/secondary, pdg, etc...)
	TVector3 vertex;
	mcTrack->GetStartVertex(vertex);
	Bool_t isPrim = mcTrack->GetMotherId() == -1;
	Double_t mom = mcTrack->GetP();
	Bool_t isRef = isPrim && mom > fRefMomentum;
	Bool_t isMuon = std::abs(mcTrack->GetPdgCode()) == 13;
	Bool_t isElectron = std::abs(mcTrack->GetPdgCode()) == 11;

	// Fill histograms for accepted tracks
	hist[ALL][ACC]->Fill(par);
	if (isPrim) hist[PRIM][ACC]->Fill(par);
	if (isRef) hist[REF][ACC]->Fill(par);
	if (!isPrim) hist[SEC][ACC]->Fill(par);
	if (isPrim && isMuon) hist[MU][ACC]->Fill(par);
	if (isPrim && isElectron) hist[EL][ACC]->Fill(par);

	// Fill histograms for reconstructed tracks which are accepted
	if (mcMap.find(mcId) != mcMap.end() ) {
		hist[ALL][REC]->Fill(par);
		if (isPrim) hist[PRIM][REC]->Fill(par);
		if (isRef) hist[REF][REC]->Fill(par);
		if (!isPrim) hist[SEC][REC]->Fill(par);
		if (isPrim && isMuon) hist[MU][REC]->Fill(par);
		if (isPrim && isElectron) hist[EL][REC]->Fill(par);
	}
}

void CbmLitReconstructionQa::CreateHistos()
{
	// Histogram list
	fHistoList = new TList();

	// resize histogram vectors
	fhStsMom.resize(fNofCategories);
	fhStsNp.resize(fNofCategories);
	fhStsAngle.resize(fNofCategories);
	fhHalfGlobalMom.resize(fNofCategories);
	fhGlobalMom.resize(fNofCategories);
	fhRecMom.resize(fNofCategories);
	fhRecNp.resize(fNofCategories);
	fhRecAngle.resize(fNofCategories);
	fhTofMom.resize(fNofCategories);
	for (Int_t i = 0; i < fNofCategories; i++) {
		fhStsMom[i].resize(fNofTypes);
		fhStsNp[i].resize(fNofTypes);
		fhStsAngle[i].resize(fNofTypes);
		fhHalfGlobalMom[i].resize(fNofTypes);
		fhGlobalMom[i].resize(fNofTypes);
		fhRecMom[i].resize(fNofTypes);
		fhRecNp[i].resize(fNofTypes);
		fhRecAngle[i].resize(fNofTypes);
		fhTofMom[i].resize(fNofTypes);
	}

	// Momentum distributions
	Double_t minMom   =  fMinMom;
	Double_t maxMom   = fMaxMom;
	Int_t    nBinsMom = fNofBinsMom;
	// Number of points distributions
	Double_t minNofPoints   =  0.;
	Double_t maxNofPoints   = 25.;
	Int_t    nBinsNofPoints = 25;
	// Polar angle distributions
	Double_t minAngle   =  fMinAngle; //grad
	Double_t maxAngle   = fMaxAngle; //grad
	Int_t    nBinsAngle = fNofBinsAngle; // grad

	std::string cat[] = { "All", "Ref", "Prim", "Sec", "Muon", "Electron"};
	std::string type[] = { "Acc", "Rec", "Eff" };

	// Create histograms
	for (Int_t i = 0; i < fNofCategories; i++) {
		for (Int_t j = 0; j < fNofTypes; j++) {
			// STS momentum dependence histograms
			std::string histName = "hStsMom" + type[j] + cat[i];
			std::string histTitle = "STS: " + type[j] + cat[i] + " " + " tracks";
			fhStsMom[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsMom, minMom, maxMom);
			fHistoList->Add(fhStsMom[i][j]);

			// STS number of points dependence histograms
			histName = "hStsNp" + type[j] + cat[i];
			histTitle = "STS: " + type[j] + cat[i] + " " + " tracks";
			fhStsNp[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsNofPoints, minNofPoints, maxNofPoints);
			fHistoList->Add(fhStsNp[i][j]);

			// STS polar angle dependence histograms
			histName = "hStsAngle" + type[j] + cat[i];
			histTitle = "STS: " + type[j] + cat[i] + " " + " tracks";
			fhStsAngle[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsAngle, minAngle, maxAngle);
			fHistoList->Add(fhStsAngle[i][j]);

			// STS+TRD(MUCH) momentum dependence histograms
			histName = "hHalfGlobalMom" + type[j] + cat[i];
			histTitle = "STS+TRD(MUCH): " + type[j] + cat[i] + " " + " tracks";
			fhHalfGlobalMom[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsMom, minMom, maxMom);
			fHistoList->Add(fhHalfGlobalMom[i][j]);

			// STS+TRD(MUCH)+TOF momentum dependence histograms
			histName = "hGlobalMom" + type[j] + cat[i];
			histTitle = "STS+TRD(MUCH)+TOF: " + type[j] + cat[i] + " " + " tracks";
			fhGlobalMom[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsMom, minMom, maxMom);
			fHistoList->Add(fhGlobalMom[i][j]);

			// TRD(MUCH) momentum dependence histograms
			histName = "hRecMom" + type[j] + cat[i];
			histTitle = "TRD(MUCH): " + type[j] + cat[i] + " " + " tracks";
			fhRecMom[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsMom, minMom, maxMom);
			fHistoList->Add(fhRecMom[i][j]);

			// TRD(MUCH) number of points dependence histograms
			histName = "hRecNp" + type[j] + cat[i];
			histTitle = "TRD(MUCH): " + type[j] + cat[i] + " " + " tracks";
			fhRecNp[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsNofPoints, minNofPoints, maxNofPoints);
			fHistoList->Add(fhRecNp[i][j]);

			// TRD(MUCH) polar angle dependence histograms
			histName = "hRecAngle" + type[j] + cat[i];
			histTitle = "TRD(MUCH): " + type[j] + cat[i] + " " + " tracks";
			fhRecAngle[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsAngle, minAngle, maxAngle);
			fHistoList->Add(fhRecAngle[i][j]);

			// TOF momentum dependence histograms
			histName = "hTofMom" + type[j] + cat[i];
			histTitle = "TOF matching: " + type[j] + cat[i] + " " + " tracks";
			fhTofMom[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsMom, minMom, maxMom);
			fHistoList->Add(fhTofMom[i][j]);
		}
	}

	//Create histograms for ghost tracks
	fhStsGhostNh = new TH1F("hStsGhostNh", "STS: ghost tracks", nBinsNofPoints, minNofPoints, maxNofPoints);
	fHistoList->Add(fhStsGhostNh);
	fhRecGhostNh = new TH1F("hRecGhostNh", "TRD(MUCH): ghost tracks", nBinsNofPoints, minNofPoints, maxNofPoints);
	fHistoList->Add(fhRecGhostNh);

	const UInt_t maxNofStations = 30;
	fhTrdNofHitsInStation = new TH1F("hTrdNofHitsInStation", "TRD: number of hits", maxNofStations, 0, maxNofStations);
	fHistoList->Add(fhTrdNofHitsInStation);
	fhMuchNofHitsInStation = new TH1F("hMuchNofHitsInStation", "MUCH: number of hits", maxNofStations, 0, maxNofStations);
	fHistoList->Add(fhMuchNofHitsInStation);
	fhTofNofHitsInStation = new TH1F("hTofNofHitsInStation", "TOF: number of hits", maxNofStations, 0, maxNofStations);
	fHistoList->Add(fhTofNofHitsInStation);

	const UInt_t nofHitsHistos = 5;
	std::string hittype[] = { "All", "True", "Fake", "TrueOverAll", "FakeOverAll" };
	Double_t hitmin[] = {0, 0, 0, -0.1, -0.1};
	Double_t hitmax[] = {20, 20, 20, 1.1, 1.1};
	Int_t hitbins[] = {20, 20, 20, 12, 12};
	fhStsTrackHits.resize(nofHitsHistos);
	fhTrdTrackHits.resize(nofHitsHistos);
	fhMuchTrackHits.resize(nofHitsHistos);
	for(UInt_t i = 0; i < nofHitsHistos; i++){
		std::string histName = "hStsTrackHits" + hittype[i];
		std::string histTitle = "STS hits in track: " + hittype[i];
		fhStsTrackHits[i] = new TH1F(histName.c_str(), histTitle.c_str(), hitbins[i], hitmin[i], hitmax[i]);
		fHistoList->Add(fhStsTrackHits[i]);

		histName = "hTrdTrackHits" + hittype[i];
		histTitle = "TRD hits in track: " + hittype[i];
		fhTrdTrackHits[i] = new TH1F(histName.c_str(), histTitle.c_str(), hitbins[i], hitmin[i], hitmax[i]);
		fHistoList->Add(fhTrdTrackHits[i]);

		histName = "hMuchTrackHits" + hittype[i];
		histTitle = "MUCH hits in track: " + hittype[i];
		fhMuchTrackHits[i] = new TH1F(histName.c_str(), histTitle.c_str(), hitbins[i], hitmin[i], hitmax[i]);
		fHistoList->Add(fhMuchTrackHits[i]);
	}
}

void CbmLitReconstructionQa::DivideHistos(
		TH1* histo1,
		TH1* histo2,
		TH1* histo3)
{
	histo1->Sumw2();
	histo2->Sumw2();
	histo3->Divide(histo1, histo2, 1, 1, "");
}

void CbmLitReconstructionQa::CalculateEfficiencyHistos()
{
	// Divide histograms for efficiency calculation
	for (Int_t i = 0; i < fNofCategories; i++) {
		DivideHistos(fhStsMom[i][REC], fhStsMom[i][ACC], fhStsMom[i][EFF]);
		DivideHistos(fhStsNp[i][REC], fhStsNp[i][ACC], fhStsNp[i][EFF]);
		DivideHistos(fhStsAngle[i][REC], fhStsAngle[i][ACC], fhStsAngle[i][EFF]);
		DivideHistos(fhHalfGlobalMom[i][REC], fhHalfGlobalMom[i][ACC], fhHalfGlobalMom[i][EFF]);
		DivideHistos(fhGlobalMom[i][REC], fhGlobalMom[i][ACC], fhGlobalMom[i][EFF]);
		DivideHistos(fhRecMom[i][REC], fhRecMom[i][ACC], fhRecMom[i][EFF]);
		DivideHistos(fhRecNp[i][REC], fhRecNp[i][ACC], fhRecNp[i][EFF]);
		DivideHistos(fhRecAngle[i][REC], fhRecAngle[i][ACC], fhRecAngle[i][EFF]);
		DivideHistos(fhTofMom[i][REC], fhTofMom[i][ACC], fhTofMom[i][EFF]);
	}
	fhTrdNofHitsInStation->Scale(1./fEventNo);
	fhMuchNofHitsInStation->Scale(1./fEventNo);
	fhTofNofHitsInStation->Scale(1./fEventNo);
}

void CbmLitReconstructionQa::WriteToFile()
{
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
}

void CbmLitReconstructionQa::IncreaseCounters()
{
	fNofGlobalTracks += fGlobalTracks->GetEntriesFast();
	if (fIsSts) fNofStsTracks += fStsMatches->GetEntriesFast();
	if (fIsTrd) fNofTrdTracks += fTrdMatches->GetEntriesFast();
	if (fIsMuch) fNofMuchTracks += fMuchMatches->GetEntriesFast();
	if (fIsTof) fNofTofHits += fTofHits->GetEntriesFast();
}

std::string CbmLitReconstructionQa::RecDetector()
{
	std::string recDet = "";
	if (fIsTrd && !fIsMuch) recDet = "TRD     ";
	else if (fIsMuch && !fIsTrd) recDet = "MUCH    ";
	else if (fIsMuch && fIsTrd) recDet = "MUCH+TRD";
	return recDet;
}

void CbmLitReconstructionQa::PrintEventStatistics()
{
	std::cout << "Number of rec tracks: global=" << fGlobalTracks->GetEntriesFast();
	if (fIsSts) std::cout << ", STS=" << fStsMatches->GetEntriesFast();
	if (fIsTrd) std::cout << ", TRD=" << fTrdMatches->GetEntriesFast();
	if (fIsMuch) std::cout << ", MUCH=" << fMuchMatches->GetEntriesFast();
	if (fIsTof) std::cout << ", TOF=" << fTofHits->GetEntriesFast();
	std::cout << std::endl;

	std::string det = RecDetector();

	std::cout << "Efficiency STS:              " << EventEfficiencyStatisticsToString(fhStsMom, "event");
	std::cout << "Efficiency STS+" << det << ":     " << EventEfficiencyStatisticsToString(fhHalfGlobalMom, "event");
	std::cout << "Efficiency STS+" << det << "+TOF: " << EventEfficiencyStatisticsToString(fhGlobalMom, "event");
	std::cout << "Efficiency " << det << ":         " << EventEfficiencyStatisticsToString(fhRecMom, "event");
	std::cout << "Efficiency TOF matching:     " << EventEfficiencyStatisticsToString(fhTofMom, "event");
}

void CbmLitReconstructionQa::PrintFinalStatistics(
		std::ostream& out)
{
	out << "-I- CbmLitReconstructionQa final statistics:" << std::endl;
	out << "Number of rec tracks (per event/total): " << std::endl;
	out << "  global=(" << (Double_t)fNofGlobalTracks/(Double_t)fEventNo << "/" << fNofGlobalTracks << ")" << std::endl;
	if (fIsSts) out << "  STS=(" << (Double_t)fNofStsTracks/(Double_t)fEventNo << "/" << fNofStsTracks << ")" << std::endl;
	if (fIsTrd) out << "  TRD=(" << (Double_t)fNofTrdTracks/(Double_t)fEventNo << "/" << fNofTrdTracks << ")" << std::endl;
	if (fIsMuch) out << "  MUCH=(" << (Double_t)fNofMuchTracks/(Double_t)fEventNo << "/" << fNofMuchTracks << ")" << std::endl;
	if (fIsTof) out << "  TOF=(" << (Double_t)fNofTofHits/(Double_t)fEventNo << "/" << fNofTofHits << ")" << std::endl;

	std::string det = RecDetector();

	out << "Efficiency STS:" << std::endl << EventEfficiencyStatisticsToString(fhStsMom, "final");
	out << "Efficiency STS+" << det << ":" << std::endl << EventEfficiencyStatisticsToString(fhHalfGlobalMom, "final");
	out << "Efficiency STS+" << det << "+TOF: " << std::endl << EventEfficiencyStatisticsToString(fhGlobalMom, "final");
	out << "Efficiency " << det << ":" << std::endl << EventEfficiencyStatisticsToString(fhRecMom, "final");
	out << "Efficiency TOF matching:" << std::endl << EventEfficiencyStatisticsToString(fhTofMom, "final");

	Double_t stsGhosts, recGhosts;
	stsGhosts = fhStsGhostNh->GetEntries();
	recGhosts = fhRecGhostNh->GetEntries();
	if (fIsSts)
		out << "Ghosts STS (per event/total): " << stsGhosts/fEventNo << "/" << stsGhosts << std::endl;
	if (fIsMuch || fIsTrd)
		out << "Ghosts " << det << " (per event/total): " << recGhosts/fEventNo << "/" << recGhosts << std::endl;
	out << "Polar angle efficiency:" << std::endl;
	out << "STS:" << std::endl;
	out << PolarAngleEfficiency(fhStsAngle);
	out << det << ":" << std::endl;
        out << PolarAngleEfficiency(fhRecAngle);
	out << "-------------------------------------------------------------" << std::endl;
}

std::string CbmLitReconstructionQa::EventEfficiencyStatisticsToString(
		const std::vector<std::vector<TH1F*> >& hist,
		const std::string& opt)
{
	Double_t allEff = 0., refEff = 0., primEff = 0., secEff = 0., muEff = 0., elEff = 0.;
	Double_t allRec = hist[ALL][REC]->GetEntries();
	Double_t allAcc = hist[ALL][ACC]->GetEntries();
	if (allAcc != 0.) allEff = allRec / allAcc;
	Double_t refRec = hist[REF][REC]->GetEntries();
	Double_t refAcc = hist[REF][ACC]->GetEntries();
	if (refAcc != 0.) refEff = refRec / refAcc;
	Double_t primRec = hist[PRIM][REC]->GetEntries();
	Double_t primAcc = hist[PRIM][ACC]->GetEntries();
	if (primAcc != 0.) primEff = primRec / primAcc;
	Double_t secRec = hist[SEC][REC]->GetEntries();
	Double_t secAcc = hist[SEC][ACC]->GetEntries();
	if (secAcc != 0.) secEff = secRec / secAcc;
	Double_t muRec = hist[MU][REC]->GetEntries();
	Double_t muAcc = hist[MU][ACC]->GetEntries();
	if (muAcc != 0.) muEff = muRec / muAcc;
	Double_t elRec = hist[EL][REC]->GetEntries();
	Double_t elAcc = hist[EL][ACC]->GetEntries();
	if (elAcc != 0.) elEff = elRec / elAcc;

	std::stringstream ss;
	// For each event
	if (opt == "event") {
		ss << "all=" << allEff << "(" << allRec << "/" << allAcc << "), ";
	//		<< "ref=" << refEff << "(" << refRec << "/" << refAcc << "), "
	//		<< "prim=" << primEff << "(" << primRec << "/" << primAcc << "), "
	//		<< "sec=" << secEff << "(" << secRec << "/" << secAcc << "), "
		if (fIsElectronSetup) ss << "el=" << elEff << "(" << elRec << "/" << elAcc << "), " << std::endl;
		else ss << "mu=" << muEff << "(" << muRec << "/" << muAcc << "), " << std::endl;
	}
	// Final statistics
	if (opt == "final") {
		ss << "  all=" << allEff << "(" << allRec << "/" << allAcc << "), per event ("
			<< allRec/fEventNo << "/" << allAcc/fEventNo << ")" << std::endl;
		ss	<< "  ref=" << refEff << "(" << refRec << "/" << refAcc << "), per event ("
			<< refRec/fEventNo << "/" << refAcc/fEventNo << ")" << std::endl;
		ss	<< "  prim=" << primEff << "(" << primRec << "/" << primAcc << "), per event ("
			<< primRec/fEventNo << "/" << primAcc/fEventNo << ")" << std::endl;
		ss	<< "  sec=" << secEff << "(" << secRec << "/" << secAcc << "), per event ("
			<< secRec/fEventNo << "/" << secAcc/fEventNo << ")" << std::endl;
		if (fIsElectronSetup) ss << "  el=" << elEff << "(" << elRec << "/" << elAcc << "), per event ("
			<< elRec/fEventNo << "/" << elAcc/fEventNo << ")" << std::endl;
		else ss << "  mu=" << muEff << "(" << muRec << "/" << muAcc << "), per event ("
			<< muRec/fEventNo << "/" << muAcc/fEventNo << ")" << std::endl;
	}

	return ss.str();
}

std::string CbmLitReconstructionQa::PolarAngleEfficiency(
	const std::vector<std::vector<TH1F*> >& hist)
{
    Double_t step = (fMaxAngle - fMinAngle) / fNofBinsAngle;
    Double_t allEff[fNofBinsAngle], refEff[fNofBinsAngle];
    Double_t allRec[fNofBinsAngle], refRec[fNofBinsAngle];
    Double_t allAcc[fNofBinsAngle], refAcc[fNofBinsAngle];
    for(Int_t i = 0; i < fNofBinsAngle; i++) {
        allRec[i] = hist[ALL][REC]->GetBinContent(i+1);
		allAcc[i] = hist[ALL][ACC]->GetBinContent(i+1);
		if (allAcc[i] != 0.) allEff[i] = allRec[i] / allAcc[i];
		refRec[i] = hist[REF][REC]->GetBinContent(i+1);
		refAcc[i] = hist[REF][ACC]->GetBinContent(i+1);
		if (refAcc[i] != 0.) refEff[i] = refRec[i] / refAcc[i];
    }

    std::stringstream ss;
    for (Int_t i = 0; i < fNofBinsAngle; i++) {
	Double_t angle0 = i*step;
        Double_t angle1 = angle0 + step;
	ss << "(" << angle0 << "-" << angle1 << ") "
	    << "all=" << allEff[i] << "(" << allRec[i] << "/" << allAcc[i] << "), per event ("
		      << allRec[i]/fEventNo << "/" << allAcc[i]/fEventNo << ")"
	    << "  ref=" << refEff[i] << "(" << refRec[i] << "/" << refAcc[i] << "), per event ("
	              << refRec[i]/fEventNo << "/" << refAcc[i]/fEventNo << ")" << std::endl;
    }
    return ss.str();
}

void CbmLitReconstructionQa::Draw()
{
	SetStyles();
	DrawEfficiencyHistos();
	DrawHitsHistos();
	DrawHitsStationHistos();
	std::ofstream fout(std::string(fOutputDir + "rec_qa.txt").c_str());
	PrintFinalStatistics(fout);
}

void CbmLitReconstructionQa::DrawEfficiencyHistos()
{
	TCanvas *c1 = new TCanvas("rec_qa_global_tracking_efficiency_all","rec_qa_global_tracking_efficiency_all", 600, 500);
	c1->SetGrid();

	TCanvas *c2 = new TCanvas("rec_qa_global_tracking_efficiency_signal","rec_qa_global_tracking_efficiency_signal", 600, 500);
	c2->SetGrid();

	TCanvas *c3 = new TCanvas("rec_qa_global_tracking_efficiency_rec","rec_qa_global_tracking_efficiency_rec", 600, 500);
	c3->SetGrid();

	TCanvas *c4 = new TCanvas("rec_qa_global_tracking_efficiency_tof","rec_qa_global_tracking_efficiency_tof", 600, 500);
	c4->SetGrid();

	std::string sname("STS");
	std::string rname;
	if (fIsMuch && !fIsTrd) rname = "MUCH";
	else if (fIsTrd && !fIsMuch) rname = "TRD";
	else if (fIsTrd && fIsMuch) rname = "MUCH+TRD";
	std::string hgname(sname + "+" + rname);
	std::string gname = hgname + "+TOF";

	std::string signal;
	if (fIsMuch) signal = "muons"; else signal = "electrons";

	std::string hname1(sname), hname2(hgname), hname3(gname);
	hname1 += "(" + ToString<Double_t>(CalcEfficiency(fhStsMom[ALL][REC], fhStsMom[ALL][ACC])) + ")";
	hname2 += "(" + ToString<Double_t>(CalcEfficiency(fhHalfGlobalMom[ALL][REC], fhHalfGlobalMom[ALL][ACC])) + ")";
	hname3 += "(" + ToString<Double_t>(CalcEfficiency(fhGlobalMom[ALL][REC], fhGlobalMom[ALL][ACC])) + ")";
	c1->cd();
	fhStsMom[ALL][EFF]->SetMinimum(0.);
	fhStsMom[ALL][EFF]->SetMaximum(1.);
	if (fIsTof) {
	   DrawHist1D(fhStsMom[ALL][EFF], fhHalfGlobalMom[ALL][EFF], fhGlobalMom[ALL][EFF], NULL,
			"Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, hname3, "",
			false, false, true, 0.3,0.3,0.85,0.6);
	} else {
	    DrawHist1D(fhStsMom[ALL][EFF], fhHalfGlobalMom[ALL][EFF], NULL, NULL,
			"Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, hname3, "",
			false, false, true, 0.3,0.3,0.85,0.6);
	}
	SaveCanvasAsImage(c1, fOutputDir);

	Int_t cat;
	if (fIsMuch) cat = MU; else cat = EL;
	hname1 = sname;
	hname2 = hgname;
	hname3 = gname;
	hname1 += "(" + ToString<Double_t>(CalcEfficiency(fhStsMom[cat][REC], fhStsMom[cat][ACC])) + ")";
	hname2 += "(" + ToString<Double_t>(CalcEfficiency(fhHalfGlobalMom[cat][REC], fhHalfGlobalMom[cat][ACC])) + ")";
	hname3 += "(" + ToString<Double_t>(CalcEfficiency(fhGlobalMom[cat][REC], fhGlobalMom[cat][ACC])) + ")";
	c2->cd();
	fhStsMom[cat][EFF]->SetMinimum(0.);
	fhStsMom[cat][EFF]->SetMaximum(1.);
	if (fIsTof) {
	   DrawHist1D(fhStsMom[cat][EFF], fhHalfGlobalMom[cat][EFF], fhGlobalMom[cat][EFF], NULL,
				"Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, hname3, "",
				false, false, true, 0.3,0.3,0.85,0.6);
	} else {
	   DrawHist1D(fhStsMom[cat][EFF], fhHalfGlobalMom[cat][EFF], NULL, NULL,
				"Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, hname3, "",
				false, false, true, 0.3,0.3,0.85,0.6);
	}
	SaveCanvasAsImage(c2, fOutputDir);


	hname1 = rname + ": all";
	hname2 = rname + ": " + signal;
	c3->cd();
	hname1 += "(" + ToString<Double_t>(CalcEfficiency(fhRecMom[ALL][REC], fhRecMom[ALL][ACC]))+ ")";
	hname2 += "(" + ToString<Double_t>(CalcEfficiency(fhRecMom[cat][REC], fhRecMom[cat][ACC])) + ")";
	fhRecMom[ALL][EFF]->SetMinimum(0.);
	fhRecMom[ALL][EFF]->SetMaximum(1.);
	DrawHist1D(fhRecMom[ALL][EFF], fhRecMom[cat][EFF], NULL, NULL,
			"Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, "", "",
			false, false, true, 0.3,0.3,0.85,0.6);
	SaveCanvasAsImage(c3, fOutputDir);

	hname1 = "TOF: all";
	hname2 = "TOF: " + signal;
	hname1 += "(" + ToString<Double_t>(CalcEfficiency(fhTofMom[ALL][REC], fhTofMom[ALL][ACC])) + ")";
	hname2 += "(" + ToString<Double_t>(CalcEfficiency(fhTofMom[cat][REC], fhTofMom[cat][ACC])) + ")";
	c4->cd();
	fhTofMom[ALL][EFF]->SetMinimum(0.);
	fhTofMom[ALL][EFF]->SetMaximum(1.);
	DrawHist1D(fhTofMom[ALL][EFF], fhTofMom[cat][EFF], NULL, NULL,
			"Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, "", "",
			false, false, true, 0.3,0.3,0.85,0.6);
	SaveCanvasAsImage(c4, fOutputDir);
}

Double_t CbmLitReconstructionQa::CalcEfficiency(
		TH1* histRec,
		TH1* histAcc)
{
	if (histAcc->GetEntries() == 0) return 0.;
	else return Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
}

void CbmLitReconstructionQa::DrawHitsHistos()
{
	if (fIsSts){
		TCanvas* cStsHits = new TCanvas("rec_qa_sts_hits","rec_qa_sts_hits", 1200, 600);
		DrawHitsHistos(cStsHits, fhStsTrackHits);
	}

	if (fIsTrd){
	   TCanvas* cTrdHits = new TCanvas("rec_qa_trd_hits","rec_qa_trd_hits", 1200, 600);
	   DrawHitsHistos(cTrdHits, fhTrdTrackHits);
   }

   if (fIsMuch) {
	   TCanvas* cMuchHits = new TCanvas("rec_qa_much_hits","rec_qa_much_hits", 1200, 600);
	   DrawHitsHistos(cMuchHits, fhMuchTrackHits);
   }
}

void CbmLitReconstructionQa::DrawHitsHistos(
		TCanvas* c,
		std::vector<TH1F*>& histos)
{
	c->Divide(2,1);
	c->SetGrid();

	c->cd(1);
	DrawHist1D(histos[0], histos[1], histos[2], NULL,
				"Number of hits", "Number of hits", "Counter",
				"all: " + ToString<Double_t>(histos[0]->GetMean()),
				"true: " + ToString<Double_t>(histos[1]->GetMean()),
				"fake: " + ToString<Double_t>(histos[2]->GetMean()),  "",
				false, true, true, 0.25,0.99,0.55,0.75);

	c->cd(2);
	DrawHist1D(histos[3], histos[4], NULL, NULL,
				"Ratio", "Ratio", "Counter",
				"true/all: " + ToString<Double_t>(histos[3]->GetMean()),
				"fake/all: " + ToString<Double_t>(histos[4]->GetMean()),
				"", "",
				false, true, true, 0.25,0.99,0.55,0.75);

	SaveCanvasAsImage(c, fOutputDir);
}

void CbmLitReconstructionQa::DrawHitsStationHistos()
{
	if (fIsTrd){
	   TCanvas* cTrdHits = new TCanvas("rec_qa_trd_hits_station","rec_qa_trd_hits_station", 1200, 600);
	   DrawHist1D(fhTrdNofHitsInStation, "Station number", "Number of hits",
	   			LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
	   			LIT_MARKER_STYLE1, false, false, "");
	   SaveCanvasAsImage(cTrdHits, fOutputDir);
   }

   if (fIsMuch){
	   TCanvas* cMuchHits = new TCanvas("rec_qa_much_hits_station","rec_qa_much_hits_station", 1200, 600);
	   DrawHist1D(fhMuchNofHitsInStation, "Station number", "Number of hits",
	   			LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
	   			LIT_MARKER_STYLE1, false, false, "");
	   SaveCanvasAsImage(cMuchHits, fOutputDir);
   }

   if (fIsTof){
	   TCanvas* cTofHits = new TCanvas("rec_qa_tof_hits_station","rec_qa_tof_hits_station", 1200, 600);
	   DrawHist1D(fhTofNofHitsInStation, "Station number", "Number of hits",
	   			LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
	   			LIT_MARKER_STYLE1, false, false, "");
	   SaveCanvasAsImage(cTofHits, fOutputDir);
   }
}

ClassImp(CbmLitReconstructionQa);
