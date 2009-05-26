/** CbmLitReconstructionQa.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **/

#include "CbmLitReconstructionQa.h"
#include "CbmLitEnvironment.h"

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

#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>

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

CbmLitReconstructionQa::CbmLitReconstructionQa():
  FairTask("LitReconstructionQA", 1)
{
	fEventNo = 0;
	fVerbose = 1;
	fMinNofPointsSts = 4;
	fMinNofPointsTrd = 8;
	fMinNofPointsMuch = 10;
	fMinNofPointsTof = 1;
	fQuota = 0.7;
	fRefMomentum = 2.;
	fNofTypes = 3;
	fNofCategories = 6;

	fNofGlobalTracks = 0;
	fNofStsTracks = 0;
	fNofTrdTracks = 0;
	fNofMuchTracks = 0;
	fNofTofHits = 0;
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
	PrintFinalStatistics();
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

    fStsMatches = (TClonesArray*) ioman->GetObject("STSTrackMatch");
    if (NULL == fStsMatches) Fatal("Init",": No STSTrackMatch array!");

    if (fIsMuch) {
    	fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
    	if (NULL == fMuchMatches) Fatal("Init","No MuchTrackMatch array!");
    }

    if (fIsTrd) {
    	fTrdMatches = (TClonesArray*) ioman->GetObject("TRDTrackMatch");
    	if (NULL == fTrdMatches) Fatal("Init","No TRDTrackMatch array!");
    }

    if (fIsTof) {
    	fTofPoints = (TClonesArray*) ioman->GetObject("TOFPoint");
    	if (NULL == fTofPoints) Fatal("Init", "No TofPoint array!");
    	fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
    	if (NULL == fTofHits) Fatal("Init", "No TofHit array!");
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
		Int_t muchId = globalTrack->GetRichRingIndex(); //TODO:change to MUCH
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
			isStsOk = CheckStsTrackQuality(stsTrackMatch);
		}
		CbmTrackMatch* trdTrackMatch;
		if (isTrdOk) {
			trdTrackMatch = (CbmTrackMatch*) fTrdMatches->At(trdId);
			isTrdOk = CheckTrdTrackQuality(trdTrackMatch);
		}
		CbmTrackMatch* muchTrackMatch;
		if (isMuchOk) {
			muchTrackMatch = (CbmTrackMatch*) fMuchMatches->At(muchId);
			isMuchOk = CheckMuchTrackQuality(muchTrackMatch);
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
		// select STS+TRD(MUCH) tracks
		if ((isStsOk && stsMCId != -1) &&
				((isTrdOk && stsMCId == trdMCId) || (isMuchOk && stsMCId == muchMCId))) {
			fMcHalfGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
		}
		// select the longest tracks STS+TRD(MUCH)+TOF
		if ( (isStsOk && stsMCId != -1) &&
				((isTrdOk && stsMCId == trdMCId) || (isMuchOk && stsMCId == muchMCId)) &&
				(isTofOk && stsMCId == tofMCId) ) {
			fMcGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
		}
	}
}

Bool_t CbmLitReconstructionQa::CheckStsTrackQuality(
		CbmTrackMatch* trackMatch)
{
	Int_t mcId = trackMatch->GetMCTrackId();
	if(mcId < 0) return false;

	Int_t nofTrue = trackMatch->GetNofTrueHits();
	Int_t nofWrong = trackMatch->GetNofWrongHits();
	Int_t nofFake = trackMatch->GetNofFakeHits();
	Int_t nofHits = nofTrue + nofWrong + nofFake;
	Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
	if (quali < fQuota) return false;
	return true;
}

Bool_t CbmLitReconstructionQa::CheckTrdTrackQuality(
		CbmTrackMatch* trackMatch)
{
	Int_t mcId = trackMatch->GetMCTrackId();
	if(mcId < 0) return false;

	Int_t nofTrue = trackMatch->GetNofTrueHits();
	Int_t nofWrong = trackMatch->GetNofWrongHits();
	Int_t nofFake = trackMatch->GetNofFakeHits();
	Int_t nofHits = nofTrue + nofWrong + nofFake;
	Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
	if (quali < fQuota) return false;
	return true;
}

Bool_t CbmLitReconstructionQa::CheckMuchTrackQuality(
		CbmTrackMatch* trackMatch)
{
	Int_t mcId = trackMatch->GetMCTrackId();
	if(mcId < 0) return false;

	Int_t nofTrue = trackMatch->GetNofTrueHits();
	Int_t nofWrong = trackMatch->GetNofWrongHits();
	Int_t nofFake = trackMatch->GetNofFakeHits();
	Int_t nofHits = nofTrue + nofWrong + nofFake;
	Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
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

		// Check accepted tracks cutting on minimal number of MC points
		// acceptance: STS tracks only
		if (isStsOk){
			// momentum dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsMom, mcTrack->GetP());
			// number of points dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsNp, nofPointsSts);
		}
		// acceptance: STS+TRD(MUCH)
		if (isStsOk && (isTrdOk || isMuchOk)){
			// momentum dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhHalfGlobalMom, mcTrack->GetP());
		}
		// acceptance: STS+TRD(MUCH)+TOF
		if (isStsOk && (isTrdOk || isMuchOk) && isTofOk) {
			// momentum dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhGlobalMom, mcTrack->GetP());
		}

		// acceptance: STS as 100% + local TRD(MUCH) track cutting on number of points
		if ( isStsReconstructed && isStsOk && (isTrdOk || isMuchOk) ){
			// momentum dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhRecMom, mcTrack->GetP());
			// number of points dependence histograms
			Int_t np = fIsElectronSetup ? nofPointsTrd : nofPointsMuch;
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhRecNp, np);
		}

		if (isHalfGlobalReconstructed && isStsOk && (isTrdOk || isMuchOk) && isTofOk) {
			// momentum dependence histograms
			FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhTofMom, mcTrack->GetP());
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
	fhHalfGlobalMom.resize(fNofCategories);
	fhGlobalMom.resize(fNofCategories);
	fhRecMom.resize(fNofCategories);
	fhRecNp.resize(fNofCategories);
	fhTofMom.resize(fNofCategories);
	for (Int_t i = 0; i < fNofCategories; i++) {
		fhStsMom[i].resize(fNofTypes);
		fhStsNp[i].resize(fNofTypes);
		fhHalfGlobalMom[i].resize(fNofTypes);
		fhGlobalMom[i].resize(fNofTypes);
		fhRecMom[i].resize(fNofTypes);
		fhRecNp[i].resize(fNofTypes);
		fhTofMom[i].resize(fNofTypes);
	}

	// Momentum distributions
	Double_t minMom   =  0.;
	Double_t maxMom   = 25.;
	Int_t    nBinsMom = 50;
	// Number of points distributions
	Double_t minNofPoints   =  0.;
	Double_t maxNofPoints   = 25.;
	Int_t    nBinsNofPoints = 25;

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

			// TOF momentum dependence histograms
			histName = "hTofMom" + type[j] + cat[i];
			histTitle = "TOF matching: " + type[j] + cat[i] + " " + " tracks";
			fhTofMom[i][j] = new TH1F(histName.c_str(), histTitle.c_str(), nBinsMom, minMom, maxMom);
			fHistoList->Add(fhTofMom[i][j]);
		}
	}
}

void CbmLitReconstructionQa::DivideHistos(
		TH1* histo1,
		TH1* histo2,
		TH1* histo3)
{
	histo1->Sumw2();
	histo2->Sumw2();
	histo3->Divide(histo1, histo2, 1, 1, "B");
}

void CbmLitReconstructionQa::CalculateEfficiencyHistos()
{
	// Divide histograms for efficiency calculation
	for (Int_t i = 0; i < fNofCategories; i++) {
		DivideHistos(fhStsMom[i][REC], fhStsMom[i][ACC], fhStsMom[i][EFF]);
		DivideHistos(fhStsNp[i][REC], fhStsNp[i][ACC], fhStsNp[i][EFF]);
		DivideHistos(fhHalfGlobalMom[i][REC], fhHalfGlobalMom[i][ACC], fhHalfGlobalMom[i][EFF]);
		DivideHistos(fhGlobalMom[i][REC], fhGlobalMom[i][ACC], fhGlobalMom[i][EFF]);
		DivideHistos(fhRecMom[i][REC], fhRecMom[i][ACC], fhRecMom[i][EFF]);
		DivideHistos(fhRecNp[i][REC], fhRecNp[i][ACC], fhRecNp[i][EFF]);
		DivideHistos(fhTofMom[i][REC], fhTofMom[i][ACC], fhTofMom[i][EFF]);
	}
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

void CbmLitReconstructionQa::PrintEventStatistics()
{
	std::cout << "Number of rec tracks: global=" << fGlobalTracks->GetEntriesFast();
	if (fIsSts) std::cout << ", STS=" << fStsMatches->GetEntriesFast();
	if (fIsTrd) std::cout << ", TRD=" << fTrdMatches->GetEntriesFast();
	if (fIsMuch) std::cout << ", MUCH=" << fMuchMatches->GetEntriesFast();
	if (fIsTof) std::cout << ", TOF=" << fTofHits->GetEntriesFast();
	std::cout << std::endl;

	std::cout << "Efficiency STS:               " << EventEfficiencyStatisticsToString(fhStsMom, "event");
	std::cout << "Efficiency STS+TRD(MUCH):     " << EventEfficiencyStatisticsToString(fhHalfGlobalMom, "event");
	std::cout << "Efficiency STS+TRD(MUCH)+TOF: " << EventEfficiencyStatisticsToString(fhGlobalMom, "event");
	std::cout << "Efficiency TRD(MUCH):         " << EventEfficiencyStatisticsToString(fhRecMom, "event");
	std::cout << "Efficiency TOF matching:      " << EventEfficiencyStatisticsToString(fhTofMom, "event");
}

void CbmLitReconstructionQa::PrintFinalStatistics()
{
	std::cout << "-I- CbmLitReconstructionQa final statistics:" << std::endl;
	std::cout << "Number of rec tracks (per event/total): " << std::endl;
	std::cout << "  global=(" << (Double_t)fNofGlobalTracks/(Double_t)fEventNo << "/" << fNofGlobalTracks << ")" << std::endl;
	if (fIsSts) std::cout << "  STS=(" << (Double_t)fNofStsTracks/(Double_t)fEventNo << "/" << fNofStsTracks << ")" << std::endl;
	if (fIsTrd) std::cout << "  TRD=(" << (Double_t)fNofTrdTracks/(Double_t)fEventNo << "/" << fNofTrdTracks << ")" << std::endl;
	if (fIsMuch) std::cout << "  MUCH=(" << (Double_t)fNofMuchTracks/(Double_t)fEventNo << "/" << fNofMuchTracks << ")" << std::endl;
	if (fIsTof) std::cout << "  TOF=(" << (Double_t)fNofTofHits/(Double_t)fEventNo << "/" << fNofTofHits << ")" << std::endl;

	std::cout << "Efficiency STS:" << std::endl << EventEfficiencyStatisticsToString(fhStsMom, "final");
	std::cout << "Efficiency STS+TRD(MUCH):" << std::endl << EventEfficiencyStatisticsToString(fhHalfGlobalMom, "final");
	std::cout << "Efficiency STS+TRD(MUCH)+TOF: " << std::endl << EventEfficiencyStatisticsToString(fhGlobalMom, "final");
	std::cout << "Efficiency TRD(MUCH):" << std::endl << EventEfficiencyStatisticsToString(fhRecMom, "final");
	std::cout << "Efficiency TOF matching:" << std::endl << EventEfficiencyStatisticsToString(fhTofMom, "final");

	std::cout << "-------------------------------------------------------------" << std::endl;
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

ClassImp(CbmLitReconstructionQa)
