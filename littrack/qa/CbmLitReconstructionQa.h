#ifndef CBMLITRECONSTRUCTIONQA_H_
#define CBMLITRECONSTRUCTIONQA_H_

#include "FairTask.h"

#include <map>
#include <string>
#include <vector>

class CbmStsTrackMatch;
class CbmTrdTrackMatch;
class CbmMuchTrackMatch;
class CbmMCTrack;
class TClonesArray;
class TH1;
class TH1F;
class TList;

class CbmLitReconstructionQa : public FairTask
{
public:
	CbmLitReconstructionQa();
	virtual ~CbmLitReconstructionQa();

	// Derived from FairTask
	virtual void SetParContainers();
	virtual InitStatus Init();
	virtual InitStatus ReInit();
	virtual void Exec(Option_t* opt);

	void SetMinNofPointsSts(Int_t minNofPointsSts) {fMinNofPointsSts = minNofPointsSts; }
	void SetMinNofPointsTrd(Int_t minNofPointsTrd) { fMinNofPointsTrd = minNofPointsTrd;}
	void SetMinNofPointsMuch(Int_t minNofPointsMuch) { fMinNofPointsMuch = minNofPointsMuch;}
	void SetMinNofPointsTof(Int_t minNofPointsTof) { fMinNofPointsTof = minNofPointsTof;}
	void SetQuota(Double_t quota) { fQuota = quota;}
	void SetVerbose(Int_t verbose) { fVerbose = verbose;}

private:
	virtual void Finish();

	void DetermineSetup();
	void ReadDataBranches();
	void ProcessGlobalTracks();
	Bool_t CheckStsTrackQuality(
			CbmStsTrackMatch* trackMatch);
	Bool_t CheckTrdTrackQuality(
			CbmTrdTrackMatch* trackMatch);
	Bool_t CheckMuchTrackQuality(
			CbmMuchTrackMatch* trackMatch);
	void ProcessMcTracks();
	void FillGlobalReconstructionHistos(
			CbmMCTrack* mcTrack,
			Int_t mcId,
			const std::multimap<Int_t, Int_t>& mcMap,
			std::vector<std::vector<TH1F*> >& hist,
			Double_t par);
	void CreateHistos();
	void DivideHistos(
		  TH1* histo1,
		  TH1* histo2,
		  TH1* histo3);
	void CalculateEfficiencyHistos();
	void WriteToFile();
	void PrintEventStatistics();
	std::string EventEfficiencyStatisticsToString(
			const std::vector<std::vector<TH1F*> >& hist);

	Int_t fMinNofPointsSts; // Minimal number of MCPoints in STS
	Int_t fMinNofPointsTrd; // Minimal number of MCPoints in TRD
	Int_t fMinNofPointsMuch; // Minimal number of MCPoints in MUCH
	Int_t fMinNofPointsTof; // Minimal number of MCPoints in TOF
	Double_t fQuota;  // True/all hits for track to be considered correctly reconstructed

	Bool_t fIsElectronSetup; // If "electron" setup detected than true
	Bool_t fIsSts; // If STS detected than true
	Bool_t fIsTrd; // If TRD detected than true
	Bool_t fIsMuch; // If MUCH detected than true
	Bool_t fIsTof; // If TOF detected than true

	Double_t fRefMomentum; // Momentum cut for reference tracks

	// Maps for reconstructed tracks
	// <MC track index, reconstructed track index>
	std::multimap<Int_t, Int_t> fMcStsMap; // STS
	std::multimap<Int_t, Int_t> fMcHalfGlobalMap; // STS+TRD(MUCH)
	std::multimap<Int_t, Int_t> fMcGlobalMap; // STS+TRD(MUCH)+TOF

	// Pointers to data arrays
	TClonesArray* fMCTracks; // CbmMCTrack array
	TClonesArray* fGlobalTracks; // CbmGlobalTrack array
	TClonesArray* fStsMatches; // CbmStsTrackMatch array
	TClonesArray* fMuchMatches; // CbmMuchTrackMatch array
	TClonesArray* fTrdMatches; // CbmTrdTrackMatch array
	TClonesArray* fTofPoints; // CbmTofPoint array
	TClonesArray* fTofHits; // CbmTofHit array

	// track categories: all, ref, prim, sec, muon, electron
	Int_t fNofCategories;
	// histogram types: acc, rec, eff
	Int_t fNofTypes;

	// Histograms
	// h[track category][histogram type]:
	// track category (all, ref, prim, sec, muon, electron)
	// histogram type (acc, rec, eff)
	std::vector<std::vector<TH1F*> > fhStsMom; // STS: momentum dependence
	std::vector<std::vector<TH1F*> > fhStsNp; // STS: number of points dependence
	std::vector<std::vector<TH1F*> > fhHalfGlobalMom; // STS+TRD(MUCH): momentum dependence
	std::vector<std::vector<TH1F*> > fhGlobalMom; // STS+TRD(MUCH)+TOF: momentum dependence
	std::vector<std::vector<TH1F*> > fhRecMom; // TRD(MUCH): momentum dependence
	std::vector<std::vector<TH1F*> > fhRecNp; // TRD(MUCH): number of points dependence
	std::vector<std::vector<TH1F*> > fhTofMom; // TOF: momentum dependence

//	TH1F* fhNhClones, *fhNhGhosts;

	TList* fHistoList; // List of histograms

	Int_t fEventNo; // Event counter

	ClassDef(CbmLitReconstructionQa,1);
};

#endif /* CBMLITRECONSTRUCTIONQA_H_ */
