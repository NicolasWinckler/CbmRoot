#include "CbmLitReconstructionQa.h"
#include "CbmLitEnvironment.h"

#include "CbmGlobalTrack.h"
#include "CbmStsTrackMatch.h"
#include "CbmMuchTrackMatch.h"
#include "CbmTrdTrackMatch.h"
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
}

CbmLitReconstructionQa::~CbmLitReconstructionQa()
{
	fHistoList->Delete();
	delete fHistoList;
}

void CbmLitReconstructionQa::SetParContainers()
{

}

InitStatus CbmLitReconstructionQa::Init()
{
    DetermineSetup();
    ReadDataBranches();
    CreateHistos();

    return kSUCCESS;
}

InitStatus CbmLitReconstructionQa::ReInit()
{
	return kSUCCESS;
}

void CbmLitReconstructionQa::Exec(
		Option_t* opt)
{
	ProcessGlobalTracks();
	ProcessMcTracks();
	PrintEventStatistics();

	std::cout << "Event: " << fEventNo++ << std::endl;
}

void CbmLitReconstructionQa::Finish()
{
	CalculateEfficiencyHistos();
	WriteToFile();
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
		CbmStsTrackMatch* stsTrackMatch;
		if (isStsOk) {
			stsTrackMatch = (CbmStsTrackMatch*) fStsMatches->At(stsId);
			isStsOk = CheckStsTrackQuality(stsTrackMatch);
		}
		CbmTrdTrackMatch* trdTrackMatch;
		if (isTrdOk) {
			trdTrackMatch = (CbmTrdTrackMatch*) fTrdMatches->At(trdId);
			isTrdOk = CheckTrdTrackQuality(trdTrackMatch);
		}
		CbmMuchTrackMatch* muchTrackMatch;
		if (isMuchOk) {
			muchTrackMatch = (CbmMuchTrackMatch*) fMuchMatches->At(muchId);
			isMuchOk = CheckMuchTrackQuality(muchTrackMatch);
		}

		// Get MC indices of track segments
		Int_t stsMCId = -1, trdMCId = -1, muchMCId = -1, tofMCId = -1;
		if (isStsOk) stsMCId = stsTrackMatch->GetMCTrackId();
		if (isTrdOk) trdMCId = trdTrackMatch->GetMCTrackID();
		if (isMuchOk) muchMCId = muchTrackMatch->GetMCTrackId();
		if (isTofOk) {
			CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofId);
			CbmTofPoint* tofPoint = (CbmTofPoint*) fTofPoints->At(tofHit->GetRefIndex());
			tofMCId = tofPoint->GetTrackID();
		}

//		std::cout << "stsId=" << stsId << ", stsMCId=" << stsMCId << " | "
//			<< "trdId=" << trdId << ", trdMCId=" << trdMCId << " | "
//			<< "muchId=" << muchId << ", muchMCId=" << muchMCId << " | "
//			<< "tofId=" << tofId << ", tofMCId=" << tofMCId << std::endl;

		// select STS tracks only
		if (isStsOk && stsMCId != -1) {
			fMcStsMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
//			std::cout << "STS" << std::endl;
		}
		// select STS+TRD(MUCH) tracks
		if ((isStsOk && stsMCId != -1) &&
				((isTrdOk && stsMCId == trdMCId) || (isMuchOk && stsMCId == muchMCId))) {
			fMcHalfGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
//			std::cout << "STS+TRD(MUCH)" << std::endl;
		}
		// select the longest tracks STS+TRD(MUCH)+TOF
		if ( (isStsOk && stsMCId != -1) &&
				((isTrdOk && stsMCId == trdMCId) || (isMuchOk && stsMCId == muchMCId)) &&
				(isTofOk && stsMCId == tofMCId) ) {
			fMcGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
//			std::cout << "STS+TRD(MUCH)+TOF" << std::endl;
		}
	}
//	std::cout << "-I- number of all/STS/STS+TRD(MUCH)/STS+TRD(MUCH)+TOF tracks: "
//		<< nofGlobalTracks << " / " << fMcStsMap.size() << " / "
//		<< fMcHalfGlobalMap.size() << " / " << fMcGlobalMap.size() << std::endl;
}

Bool_t CbmLitReconstructionQa::CheckStsTrackQuality(
		CbmStsTrackMatch* trackMatch)
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
		CbmTrdTrackMatch* trackMatch)
{
	Int_t mcId = trackMatch->GetMCTrackID();
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
		CbmMuchTrackMatch* trackMatch)
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
//			std::cout << "STS+TRD(MUCH)+TOF: sts=" << nofPointsSts
//							<< " trd=" << nofPointsTrd << " much=" << nofPointsMuch << " tof="
//							<< nofPointsTof << std::endl;
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
		CbmMCTrack* mcTrack,
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

void CbmLitReconstructionQa::PrintEventStatistics()
{
	std::cout << "Number of rec tracks: global=" << fGlobalTracks->GetEntriesFast();
	if (fIsSts) std::cout << ", STS=" << fStsMatches->GetEntriesFast();
	if (fIsTrd) std::cout << ", TRD=" << fTrdMatches->GetEntriesFast();
	if (fIsMuch) std::cout << ", MUCH=" << fMuchMatches->GetEntriesFast();
	if (fIsTof) std::cout << ", TOF=" << fTofHits->GetEntriesFast();
	std::cout << std::endl;

	std::cout << "Efficiency STS:               " << EventEfficiencyStatisticsToString(fhStsMom);
	std::cout << "Efficiency STS+TRD(MUCH):     " << EventEfficiencyStatisticsToString(fhHalfGlobalMom);
	std::cout << "Efficiency STS+TRD(MUCH)+TOF: " << EventEfficiencyStatisticsToString(fhGlobalMom);
	std::cout << "Efficiency TRD(MUCH):         " << EventEfficiencyStatisticsToString(fhRecMom);
	std::cout << "Efficiency TOF matching:      " << EventEfficiencyStatisticsToString(fhTofMom);
}

std::string CbmLitReconstructionQa::EventEfficiencyStatisticsToString(
		const std::vector<std::vector<TH1F*> >& hist)
{
	Double_t allEff = 0., refEff = 0., primEff = 0., secEff = 0., muEff = 0., elEff = 0.;
	Int_t allRec = hist[ALL][REC]->GetEntries();
	Int_t allAcc = hist[ALL][ACC]->GetEntries();
	if (allAcc != 0) allEff = (Double_t) allRec / (Double_t) allAcc;
	Int_t refRec = hist[REF][REC]->GetEntries();
	Int_t refAcc = hist[REF][ACC]->GetEntries();
	if (refAcc != 0) refEff = (Double_t) refRec / (Double_t) refAcc;
	Int_t primRec = hist[PRIM][REC]->GetEntries();
	Int_t primAcc = hist[PRIM][ACC]->GetEntries();
	if (primAcc != 0) primEff = (Double_t) primRec / (Double_t) primAcc;
	Int_t secRec = hist[SEC][REC]->GetEntries();
	Int_t secAcc = hist[SEC][ACC]->GetEntries();
	if (secAcc != 0) secEff = (Double_t) secRec / (Double_t) secAcc;
	Int_t muRec = hist[MU][REC]->GetEntries();
	Int_t muAcc = hist[MU][ACC]->GetEntries();
	if (muAcc != 0) muEff = (Double_t) muRec / (Double_t) muAcc;
	Int_t elRec = hist[EL][REC]->GetEntries();
	Int_t elAcc = hist[EL][ACC]->GetEntries();
	if (elAcc != 0) elEff = (Double_t) elRec / (Double_t) elAcc;

	std::stringstream ss;
	ss.precision(3);
	ss << "all=" << allEff << "(" << allRec << "/" << allAcc << "), "
		<< "ref=" << refEff << "(" << refRec << "/" << refAcc << "), "
		<< "prim=" << primEff << "(" << primRec << "/" << primAcc << "), "
		<< "sec=" << secEff << "(" << secRec << "/" << secAcc << "), "
		<< "mu=" << muEff << "(" << muRec << "/" << muAcc << "), "
		<< "el=" << elEff << "(" << elRec << "/" << elAcc << "), " << std::endl;

	return ss.str();
}

ClassImp(CbmLitReconstructionQa)
