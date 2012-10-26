/**
 * \file CbmLitTrackingQaHistCreator.cxx
 * \brief Create histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaHistCreator.h"
#include "CbmHistManager.h"
#include "CbmMCTrack.h"

#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TClonesArray.h"

#include <boost/assign/list_of.hpp>

#include <cassert>
#include <set>

using std::cout;
using std::set;
using boost::assign::list_of;

Bool_t AllTrackAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index)
{
	return true;
}

Bool_t PrimaryTrackAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
	return (mcTrack->GetMotherId() == -1);
}

Bool_t ReferenceTrackAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
	return (mcTrack->GetMotherId() == -1) && (mcTrack->GetP() > 1.);
}

Bool_t SecondaryTrackAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
	return (mcTrack->GetMotherId() != -1);
}

Bool_t PrimaryElectronTrackAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
	return (std::abs(mcTrack->GetPdgCode()) == 11) && (mcTrack->GetMotherId() == -1);
}

Bool_t PrimaryMuonTrackAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
	return (std::abs(mcTrack->GetPdgCode()) == 13) && (mcTrack->GetMotherId() == -1);
}

Bool_t ProtonTrackAcceptanceFunction(
      const TClonesArray* mcTracks,
      Int_t index)
{
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return (std::abs(mcTrack->GetPdgCode()) == 2212);
}

Bool_t PionTrackAcceptanceFunction(
      const TClonesArray* mcTracks,
      Int_t index)
{
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return (std::abs(mcTrack->GetPdgCode()) == 211);
}

Bool_t KaonTrackAcceptanceFunction(
      const TClonesArray* mcTracks,
      Int_t index)
{
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return (std::abs(mcTrack->GetPdgCode()) == 321);
}

Bool_t AllRingAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index,
		Int_t nofHitsInRing)
{
	return true;
}

Bool_t AllReferenceRingAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index,
		Int_t nofHitsInRing)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
    return (mcTrack->GetMotherId() == -1) && (mcTrack->GetP() > 1.) && (nofHitsInRing >= 15);
}

Bool_t PrimaryElectronRingAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index,
		Int_t nofHitsInRing)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
    return (mcTrack->GetMotherId() == -1) && (std::abs(mcTrack->GetPdgCode()) == 11);
}

Bool_t PrimaryElectronReferenceRingAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index,
		Int_t nofHitsInRing)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
    return (mcTrack->GetMotherId() == -1) && (std::abs(mcTrack->GetPdgCode()) == 11) && (mcTrack->GetP() > 1.) && (nofHitsInRing >= 15);
}

Bool_t PionRingAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index,
		Int_t nofHitsInRing)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
    return (std::abs(mcTrack->GetPdgCode()) == 211);
}

Bool_t PionReferenceRingAcceptanceFunction(
		const TClonesArray* mcTracks,
		Int_t index,
		Int_t nofHitsInRing)
{
	const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
    return (std::abs(mcTrack->GetPdgCode()) == 211) && (mcTrack->GetP() > 1.) && (nofHitsInRing >= 15);
}

CbmLitTrackingQaHistCreator::CbmLitTrackingQaHistCreator():
   fMinMom(0.),
   fMaxMom(10.),
   fNofBinsMom(40),
   fMinPt(0.),
   fMaxPt(3.),
   fNofBinsPt(12),
   fMinY(0.),
   fMaxY(4.),
   fNofBinsY(16),
   fMinAngle(0.),
   fMaxAngle(25.),
   fNofBinsAngle(5),
   fHM(NULL),
   fTrackCategories(),
   fRingCategories(),
   fTrackAcceptanceFunctions(),
   fRingAcceptanceFunctions(),
   fDet()
{
	fTrackCategories = GetDefaultTrackCategories();
	fTrackAcceptanceFunctions = GetDefaultTrackAcceptanceFunctions();
	fRingCategories = GetDefaultRingCategories();
	fRingAcceptanceFunctions = GetDefaultRingAcceptanceFunctions();
}

CbmLitTrackingQaHistCreator::~CbmLitTrackingQaHistCreator()
{
}

CbmLitTrackingQaHistCreator* CbmLitTrackingQaHistCreator::Instance()
{
   static CbmLitTrackingQaHistCreator instance;
   return &instance;
}

vector<string> CbmLitTrackingQaHistCreator::GetDefaultTrackCategories()
{
	CbmLitDetectorSetup det;
	det.DetermineSetup();
	return list_of("All")("Primary")("Secondary")("Reference")(det.GetElectronSetup() ? "Electron" : "Muon")("Proton")("Pion")("Kaon");
}

map<string, LitTrackAcceptanceFunction> CbmLitTrackingQaHistCreator::GetDefaultTrackAcceptanceFunctions()
{
	map<string, LitTrackAcceptanceFunction> cat;
	cat["All"] = AllTrackAcceptanceFunction;
	cat["Primary"] = PrimaryTrackAcceptanceFunction;
	cat["Secondary"] = SecondaryTrackAcceptanceFunction;
	cat["Reference"] = ReferenceTrackAcceptanceFunction;
	CbmLitDetectorSetup det;
	det.DetermineSetup();
	if (det.GetElectronSetup()) cat["Electron"] = PrimaryElectronTrackAcceptanceFunction;
	else cat["Muon"] = PrimaryMuonTrackAcceptanceFunction;
	cat["Proton"] = ProtonTrackAcceptanceFunction;
	cat["Pion"] = PionTrackAcceptanceFunction;
	cat["Kaon"] = KaonTrackAcceptanceFunction;
	return cat;
}

vector<string> CbmLitTrackingQaHistCreator::GetDefaultRingCategories()
{
	return list_of("All")("AllReference")("Electron")("ElectronReference")("Pion")("PionReference");
}

map<string, LitRingAcceptanceFunction> CbmLitTrackingQaHistCreator::GetDefaultRingAcceptanceFunctions()
{
	map<string, LitRingAcceptanceFunction> cat;
	cat["All"] = AllRingAcceptanceFunction;
	cat["AllReference"] = AllReferenceRingAcceptanceFunction;
	cat["Electron"] = PrimaryElectronRingAcceptanceFunction;
	cat["ElectronReference"] = PrimaryElectronReferenceRingAcceptanceFunction;
	cat["Pion"] = PionRingAcceptanceFunction;
	cat["PionReference"] = PionReferenceRingAcceptanceFunction;
	return cat;
}

void CbmLitTrackingQaHistCreator::CreateH1Efficiency(
      const string& name,
      const string& parameter,
      const string& xTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin,
      const string& opt)
{
   assert(opt == "track" || opt == "ring");
   string types[] = { "Acc", "Rec", "Eff" };
   vector<string> cat = (opt == "track") ? GetTrackCategories() : GetRingCategories();

   for (Int_t iCat = 0; iCat < cat.size(); iCat++) {
      for (Int_t iType = 0; iType < 3; iType++) {
    	   string yTitle = (types[iType] == "Eff") ? "Efficiency [%]" : "Counter";
         string histName = name + "_" + cat[iCat] + "_" + types[iType] + "_" + parameter;
         string histTitle = histName + ";" + xTitle + ";" + yTitle;
         fHM->Add(histName, new TH1F(histName.c_str(), histTitle.c_str(), nofBins, minBin, maxBin));
      }
   }
}

void CbmLitTrackingQaHistCreator::CreateH2Efficiency(
      const string& name,
      const string& parameter,
      const string& xTitle,
      const string& yTitle,
      Int_t nofBinsX,
      Double_t minBinX,
      Double_t maxBinX,
      Int_t nofBinsY,
      Double_t minBinY,
      Double_t maxBinY,
      const string& opt)
{
   assert(opt == "track" || opt == "ring");
   string types[] = { "Acc", "Rec", "Eff" };
   vector<string> cat = (opt == "track") ? GetTrackCategories() : GetRingCategories();

   for (Int_t iCat = 0; iCat < cat.size(); iCat++) {
      for (Int_t iType = 0; iType < 3; iType++) {
         string zTitle = (types[iType] == "Eff") ? "Efficiency [%]" : "Counter";
         string histName = name + "_" + cat[iCat] + "_" + types[iType] + "_" + parameter;
         string histTitle = histName + ";" + xTitle + ";" + yTitle + ";" + zTitle;
         fHM->Add(histName, new TH2F(histName.c_str(), histTitle.c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY));
      }
   }
}

void CbmLitTrackingQaHistCreator::CreateH1(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin)
{
   TH1F* h = new TH1F(name.c_str(), string(name + ";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
   fHM->Add(name, h);
}

void CbmLitTrackingQaHistCreator::CreateH2(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      const string& zTitle,
      Int_t nofBinsX,
      Double_t minBinX,
      Double_t maxBinX,
      Int_t nofBinsY,
      Double_t minBinY,
      Double_t maxBinY)
{
   TH2F* h = new TH2F(name.c_str(), (name + ";" + xTitle + ";" + yTitle + ";" + zTitle).c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
   fHM->Add(name, h);
}

void CbmLitTrackingQaHistCreator::CreateTrackHitsHistogram(
		const string& detName)
{
	string type[] = { "All", "True", "Fake", "TrueOverAll", "FakeOverAll" };
	Double_t min[] = { 0, 0, 0, -0.1, -0.1 };
	Double_t max[] = { 100, 100, 100, 1.1, 1.1 };
	Int_t bins[] = { 100, 100, 100, 12, 12 };
	for(Int_t i = 0; i < 5; i++) {
	   string xTitle = (i == 3 || i == 4) ? "Ratio" : "Number of hits";
	   string histName = "hth_" + detName + "_TrackHits_" + type[i];
	   CreateH1(histName.c_str(), xTitle, "Yeild", bins[i], min[i], max[i]);
	}
}

vector<string> CbmLitTrackingQaHistCreator::CreateGlobalTrackingHistogramNames(
		const vector<string>& detectors)
{
   vector<string> histos;
   Int_t nofDetectors = detectors.size();
   for (Int_t iDet = 0; iDet < nofDetectors; iDet++) {
	   string histEff;
	   for (Int_t i = 0; i <= iDet; i++) { histEff += detectors[i]; }
	   string histNorm = histEff;
	   histos.push_back("hte_" + histEff + "_" + histNorm);
	   for (Int_t i = iDet + 1; i < nofDetectors; i++) {
		   histNorm += detectors[i];
		   histos.push_back("hte_" + histEff + "_" + histNorm);
	   }
   }
   return histos;
}

vector<string> CbmLitTrackingQaHistCreator::CreateGlobalTrackingHistogramNames()
{
	// Histograms w/o RICH detector
	vector<string> detectors;
	if (fDet.GetDet(kSTS)) detectors.push_back("Sts");
	if (fDet.GetDet(kMUCH)) detectors.push_back("Much");
	if (fDet.GetDet(kTRD)) detectors.push_back("Trd");
	if (fDet.GetDet(kTOF)) detectors.push_back("Tof");
	vector<string> names1 = CreateGlobalTrackingHistogramNames(detectors);

	// Histograms with RICH detector
	vector<string> names2;
	if (fDet.GetDet(kRICH)) {
	   detectors.clear();
	   if (fDet.GetDet(kSTS)) detectors.push_back("Sts");
	   if (fDet.GetDet(kRICH)) detectors.push_back("Rich");
	   if (fDet.GetDet(kTRD)) detectors.push_back("Trd");
	   if (fDet.GetDet(kTOF)) detectors.push_back("Tof");
	   names2 = CreateGlobalTrackingHistogramNames(detectors);
	}

	set<string> names;
	names.insert(names1.begin(), names1.end());
	names.insert(names2.begin(), names2.end());
	vector<string> nameVector(names.begin(), names.end());
	return nameVector;
}

vector<string> CbmLitTrackingQaHistCreator::GlobalTrackVariants()
{
	CbmLitDetectorSetup det;
	det.DetermineSetup();

	set<string> trackVariants;
	// Histograms w/o RICH detector
	vector<string> detectors;
	if (det.GetDet(kSTS)) detectors.push_back("Sts");
	if (det.GetDet(kMUCH)) detectors.push_back("Much");
	if (det.GetDet(kTRD)) detectors.push_back("Trd");
	if (det.GetDet(kTOF)) detectors.push_back("Tof");
	string name("");
	for (Int_t i = 0; i < detectors.size(); i++) {
		name += detectors[i];
		trackVariants.insert(name);
	}

	// Histograms with RICH detector
	if (det.GetDet(kRICH)) {
	   detectors.clear();
	   if (det.GetDet(kSTS)) detectors.push_back("Sts");
	   if (det.GetDet(kRICH)) detectors.push_back("Rich");
	   if (det.GetDet(kTRD)) detectors.push_back("Trd");
	   if (det.GetDet(kTOF)) detectors.push_back("Tof");
	   name = "";
	   for (Int_t i = 0; i < detectors.size(); i++) {
          name += detectors[i];
		  trackVariants.insert(name);
	   }
	}
	vector<string> trackVariantsVector(trackVariants.begin(), trackVariants.end());

	trackVariantsVector.push_back("Rich");

	return trackVariantsVector;
}

string CbmLitTrackingQaHistCreator::LocalEfficiencyNormalization(
		const string& detName)
{
	CbmLitDetectorSetup det;
	det.DetermineSetup();

	set<string> trackVariants;
	// Histograms w/o RICH detector
	vector<string> detectors;
	if (det.GetDet(kSTS)) detectors.push_back("Sts");
	if (det.GetDet(kMUCH)) detectors.push_back("Much");
	if (det.GetDet(kTRD)) detectors.push_back("Trd");
	if (det.GetDet(kTOF)) detectors.push_back("Tof");
	string name("");
	for (Int_t i = 0; i < detectors.size(); i++) {
		name += detectors[i];
		if (detectors[i] == detName) break;
	}
	return name;
}

void CbmLitTrackingQaHistCreator::Create(
      CbmHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   fDet.DetermineSetup();

   // Number of points distributions
   Double_t minNofPoints =  0.;
   Double_t maxNofPoints = 100.;
   Int_t nofBinsPoints = 100;

   // Reconstruction efficiency histograms
   // Local efficiency histograms
   // STS
 //  CreateEffHist3D("hSts3D", "track");
   CreateH1Efficiency("hte_Sts_Sts", "Np", "Number of points", nofBinsPoints, minNofPoints, maxNofPoints, "track");
   CreateH1Efficiency("hte_Sts_Sts", "Angle", "Polar angle [grad]", fNofBinsAngle, fMinAngle, fMaxAngle, "track");
   // MUCH
   if (fDet.GetDet(kMUCH)) {
	   string norm = LocalEfficiencyNormalization("Much");
	   string histName = "hte_Much_" + norm;
	   CreateH1Efficiency(histName, "p", "P [GeV/c]", fNofBinsMom, fMinMom, fMaxMom, "track");
	   CreateH1Efficiency(histName, "y", "Rapidity", fNofBinsY, fMinY, fMaxY, "track");
	   CreateH1Efficiency(histName, "pt", "P_{t} [GeV/c]", fNofBinsPt, fMinPt, fMaxPt, "track");
	   CreateH1Efficiency(histName, "Np", "Number of points", nofBinsPoints, minNofPoints, maxNofPoints, "track");
	   CreateH1Efficiency(histName, "Angle", "Polar angle [grad]", fNofBinsAngle, fMinAngle, fMaxAngle, "track");
	   CreateH2Efficiency(histName, "YPt", "Rapidity", "P_{t} [GeV/c]", fNofBinsY, fMinY, fMaxY, fNofBinsPt, fMinPt, fMaxPt, "track");
   }
   // TRD
   if (fDet.GetDet(kTRD)) {
	   string norm = LocalEfficiencyNormalization("Trd");
	   string histName = "hte_Trd_" + norm;
	   CreateH1Efficiency(histName, "p", "P [GeV/c]", fNofBinsMom, fMinMom, fMaxMom, "track");
	   CreateH1Efficiency(histName, "y", "Rapidity", fNofBinsY, fMinY, fMaxY, "track");
	   CreateH1Efficiency(histName, "pt", "P_{t} [GeV/c]", fNofBinsPt, fMinPt, fMaxPt, "track");
	   CreateH1Efficiency(histName, "Np", "Number of points", nofBinsPoints, minNofPoints, maxNofPoints, "track");
	   CreateH1Efficiency(histName, "Angle", "Polar angle [grad]", fNofBinsAngle, fMinAngle, fMaxAngle, "track");
      CreateH2Efficiency(histName, "YPt", "Rapidity", "P_{t} [GeV/c]", fNofBinsY, fMinY, fMaxY, fNofBinsPt, fMinPt, fMaxPt, "track");
   }
   // TOF
   if (fDet.GetDet(kTOF)) {
	   string norm = LocalEfficiencyNormalization("Tof");
	   string histName = "hte_Tof_" + norm;
	   CreateH1Efficiency(histName, "p", "P [GeV/c]", fNofBinsMom, fMinMom, fMaxMom, "track");
	   CreateH1Efficiency(histName, "y", "Rapidity", fNofBinsY, fMinY, fMaxY, "track");
	   CreateH1Efficiency(histName, "pt", "P_{t} [GeV/c]", fNofBinsPt, fMinPt, fMaxPt, "track");
//	   CreateEfficiencyHistogram(histName, "Np", "Number of points", nofBinsPoints, minNofPoints, maxNofPoints, "track");
	   CreateH1Efficiency(histName, "Angle", "Polar angle [grad]", fNofBinsAngle, fMinAngle, fMaxAngle, "track");
      CreateH2Efficiency(histName, "YPt", "Rapidity", "P_{t} [GeV/c]", fNofBinsY, fMinY, fMaxY, fNofBinsPt, fMinPt, fMaxPt, "track");
   }
   // RICH
   if (fDet.GetDet(kRICH)) {
	   CreateH1Efficiency("hte_Rich_Rich", "p", "P [GeV/c]", fNofBinsMom, fMinMom, fMaxMom, "ring");
	   CreateH1Efficiency("hte_Rich_Rich", "y", "Rapidity", fNofBinsY, fMinY, fMaxY, "ring");
	   CreateH1Efficiency("hte_Rich_Rich", "pt", "P_{t} [GeV/c]", fNofBinsPt, fMinPt, fMaxPt, "ring");
	   CreateH1Efficiency("hte_Rich_Rich", "Nh", "Number of hits", nofBinsPoints, minNofPoints, maxNofPoints, "ring");
	   CreateH1Efficiency("hte_Rich_Rich", "BoA", "B/A", 50, 0.0, 1.0, "ring");
	   CreateH1Efficiency("hte_Rich_Rich", "RadPos", "Radial position [cm]", 50, 0., 150., "ring");
      CreateH2Efficiency("hte_Rich_Rich", "YPt", "Rapidity", "P_{t} [GeV/c]", fNofBinsY, fMinY, fMaxY, fNofBinsPt, fMinPt, fMaxPt, "ring");
   }

   // Global efficiency histograms
   vector<string> histoNames = CreateGlobalTrackingHistogramNames();
   for (Int_t iHist = 0; iHist < histoNames.size(); iHist++) {
	   string name = histoNames[iHist];
	   string opt = (name.find("Rich") == string::npos) ? "track" : "ring";
	   CreateH1Efficiency(name, "p", "P [GeV/c]", fNofBinsMom, fMinMom, fMaxMom, opt);
	   CreateH1Efficiency(name, "y", "Rapidity", fNofBinsY, fMinY, fMaxY, opt);
	   CreateH1Efficiency(name, "pt", "P_{t} [GeV/c]", fNofBinsPt, fMinPt, fMaxPt, opt);
      CreateH2Efficiency(name, "YPt", "Rapidity", "P_{t} [GeV/c]", fNofBinsY, fMinY, fMaxY, fNofBinsPt, fMinPt, fMaxPt, opt);
   }

//   // Electron identification efficiency histograms
//   CreateEfficiencyHistogramPYPt("hStsTrd3DElId", kElid);
//   CreateEfficiencyHistogramPYPt("hStsTrd3DElIdNormStsTrdTof", kElid);
//   CreateEfficiencyHistogramPYPt("hStsTrdTof3DElId", kElid);
//   CreateEfficiencyHistogramPYPt("hStsRich3DElId", kElid);
//   CreateEfficiencyHistogramPYPt("hStsRich3DElIdNormStsRichTrd", kElid);
//   CreateEfficiencyHistogramPYPt("hStsRichTrd3DElId", kElid);
//   CreateEfficiencyHistogramPYPt("hStsRich3DElIdNormStsRichTrdTof", kElid);
//   CreateEfficiencyHistogramPYPt("hStsRichTrd3DElIdNormStsRichTrdTof", kElid);
//   CreateEfficiencyHistogramPYPt("hStsRichTrdTof3DElId", kElid);
//
//   // Detector acceptance histogramm
//   CreateEfficiencyHistogramPYPt("hStsDetAcc3D", kDetAcc);
//   CreateEfficiencyHistogramPYPt("hStsRichDetAcc3D", kDetAcc);
//   CreateEfficiencyHistogramPYPt("hStsTrdDetAcc3D", kDetAcc);
//   CreateEfficiencyHistogramPYPt("hStsTofDetAcc3D", kDetAcc);
//   CreateEfficiencyHistogramPYPt("hStsRichTrdDetAcc3D", kDetAcc);
//   CreateEfficiencyHistogramPYPt("hStsRichTrdTofDetAcc3D", kDetAcc);
//   CreateEfficiencyHistogramPYPt("hStsTrdTofDetAcc3D", kDetAcc);

   // Create histograms for ghost tracks
   if (fDet.GetDet(kSTS)) CreateH1("hng_NofGhosts_Sts_Nh", "Number of hits", "Yield", nofBinsPoints, minNofPoints, maxNofPoints);
   if (fDet.GetDet(kTRD)) CreateH1("hng_NofGhosts_Trd_Nh", "Number of hits", "Yield", nofBinsPoints, minNofPoints, maxNofPoints);
   if (fDet.GetDet(kMUCH)) CreateH1("hng_NofGhosts_Much_Nh", "Number of hits", "Yield", nofBinsPoints, minNofPoints, maxNofPoints);
   if (fDet.GetDet(kRICH)) {
	   CreateH1("hng_NofGhosts_Rich_Nh", "Number of hits", "Yield", nofBinsPoints, minNofPoints, maxNofPoints);
	   CreateH1("hng_NofGhosts_RichStsMatching_Nh", "Number of hits", "Yield", nofBinsPoints, minNofPoints, maxNofPoints);
	   CreateH1("hng_NofGhosts_RichElId_Nh", "Number of hits", "Yield", nofBinsPoints, minNofPoints, maxNofPoints);
	   CreateH1("hng_NofGhosts_StsRichMatching_Nh", "Number of hits", "Yield", nofBinsPoints, minNofPoints, maxNofPoints);
   }

   // Create track hits histograms
   if (fDet.GetDet(kMVD)) CreateTrackHitsHistogram("Mvd");
   if (fDet.GetDet(kSTS)) CreateTrackHitsHistogram("Sts");
   if (fDet.GetDet(kTRD)) CreateTrackHitsHistogram("Trd");
   if (fDet.GetDet(kMUCH)) CreateTrackHitsHistogram("Much");
   if (fDet.GetDet(kRICH)) CreateTrackHitsHistogram("Rich");


   // Create number of object histograms
   Int_t nofBinsC = 100000;
   Double_t maxXC = 100000.;
   CreateH1("hno_NofObjects_GlobalTracks", "Tracks per event", "Yield", nofBinsC, 1., maxXC);
   if (fDet.GetDet(kSTS)) CreateH1("hno_NofObjects_StsTracks", "Tracks per event", "Yield", nofBinsC, 1., maxXC);
   if (fDet.GetDet(kTRD)) CreateH1("hno_NofObjects_TrdTracks", "Tracks per event", "Yield", nofBinsC, 1., maxXC);
   if (fDet.GetDet(kMUCH)) CreateH1("hno_NofObjects_MuchTracks", "Tracks per event", "Yield", nofBinsC, 1., maxXC);
   if (fDet.GetDet(kRICH)) {
	   CreateH1("hno_NofObjects_RichRings", "Rings per event", "Yield", nofBinsC, 1., maxXC);
	   CreateH1("hno_NofObjects_RichProjections", "Projections per event", "Yield", nofBinsC, 1., maxXC);
   }

//   // MC momentum vs. polar angle histograms
//   string cat[6];
//   cat[0] = "All"; cat[1] = "Ref"; cat[2] = "Prim";
//   cat[3] = "Sec"; cat[4] = "Mu"; cat[5] = "El";
//   for (Int_t i = 0; i < 6; i++) {
//      string histName = "hMCMomVsAngle_" + cat[i];
//      Create2DHist(histName.c_str(), "P [GeV/c]", "Polar angle [grad]", "Counter",
//            fNofBinsMom, fMinMom, fMaxMom, 10, 0., 35.);
//   }

   // Histogram stores number of events
   CreateH1("hen_EventNo_TrackingQa", "", "", 1, 0, 1.);

//   cout << fHM->ToString();

//   vector<TH1*> histos = fHM->H1Vector("hte_.*_All_.*");
//   for (Int_t i = 0; i < histos.size(); i++) {
//	   TH1* hist = histos[i];
//	   string name = hist->GetName();
//	   vector<string> parse = ParseHistoName(name);
//	   cout << name;
//	   for (Int_t j = 0; j < parse.size(); j++) {
//		   cout << " [" << j << "]=" << parse[j];
//	   }
//	   cout << std::endl;
//   }
}
