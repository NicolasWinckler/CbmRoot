/**
 * \file CbmLitReconstructionQaImpl.h
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 *
 * Implementation of global track reconstruction QA.
 **/

#ifndef CBMLITRECONSTRUCTIONQAIMPL_H_
#define CBMLITRECONSTRUCTIONQAIMPL_H_

#include "FairTask.h"
#include "CbmDetectorList.h"
#include "CbmStsKFTrackFitter.h"

#include <map>
#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>

class CbmTrackMatch;
class CbmRichRingMatch;
class CbmVertex;
class CbmStsKFTrackFitter;
class CbmMCTrack;
class TClonesArray;
class TH1;
class TH2F;
class TH1F;
class TList;
class TCanvas;
class CbmLitGlobalElectronId;

/**
 * \class CbmLitReconstructionQaImpl
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 *
 * Implementation of global track reconstruction QA.
 **/
class CbmLitReconstructionQaImpl : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitReconstructionQaImpl();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitReconstructionQaImpl();

   /**
    * \brief Derived from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Derived from FairTask.
    */
   virtual void Exec(
      Option_t* opt);

   /**
    * \brief Set minimum number of MC points in STS.
    * \param[in] minNofPointsSts Minimum number of MC points in STS.
    */
   void SetMinNofPointsSts(Int_t minNofPointsSts) {fMinNofPointsSts = minNofPointsSts; }

   /**
    * \brief Set minimum number of MC points in TRD.
    * \param[in] minNofPointsTrd Minimum number of MC points in TRD.
    */
   void SetMinNofPointsTrd(Int_t minNofPointsTrd) { fMinNofPointsTrd = minNofPointsTrd;}

   /**
    * \brief Set minimum number of MC points in MUCH.
    * \param[in] minNofPointsMuch Minimum number of MC points in MUCH.
    */
   void SetMinNofPointsMuch(Int_t minNofPointsMuch) { fMinNofPointsMuch = minNofPointsMuch;}

   /**
    * \brief Set minimum number of MC points in TOF.
    * \param[in] minNofPointsTof Minimum number of MC points in TOF.
    */
   void SetMinNofPointsTof(Int_t minNofPointsTof) { fMinNofPointsTof = minNofPointsTof;}

   /**
    * \brief Set quota value, which is true/all hits for track to be considered correctly reconstructed.
    * \param[in] param quota Quota value.
    */
   void SetQuota(Double_t quota) { fQuota = quota;}

   /**
    * \brief Set minimum number of hits in MC RICH ring.
    * \param minNofHits Minimum number of hits in MC RICH ring.
    */
   void SetMinNofHitsRich(Int_t minNofHits) { fMinNofHitsRich = minNofHits;}

   /**
    * \brief Set quota value for RICH.
    * \param[in] quota Quota value.
    */
   void SetQuotaRich(Double_t quota) { fQuotaRich = quota;}

   /**
    * \brief Set minimum number of hits in TRD track.
    * \param[in] minNofPointsTrd Minimum number of hits in TRD.
    */
   void SetMinNofHitsTrd(Int_t minNofHitsTrd) { fMinNofHitsTrd = minNofHitsTrd;}

   /**
    * \brief Set minimum number of hits in MUCH track.
    * \param[in] minNofHitsMuch Minimum number of hits in MUCH track.
    */
   void SetMinNofHitsMuch(Int_t minNofHitsMuch) { fMinNofHitsMuch = minNofHitsMuch;}

   /**
    * \brief Set verbose level for print.
    * \param[in] verbose Verbose value.
    */
   void SetVerbose(Int_t verbose) { fVerbose = verbose;}

   /**
    * \brief Set output directory for images.
    * \param[in] dir Directory name.
    */
   void SetOutputDir(const std::string& dir) { fOutputDir = dir;}

   /**
    * \brief Return output directory for images.
    * \return Output directory name.
    */
   const std::string& GetOutputDir() const { return fOutputDir;}

   /**
    * \brief Set momentum range for efficiency calculation.
    * \param[in] minMom Minimum momentum.
    * \param[in] maxMom Maximum momentum.
    */
   void SetMomentumRange(Double_t minMom, Double_t maxMom) {
      fMinMom = minMom;
      fMaxMom = maxMom;
   }

   /**
    * \brief Set number of bins for efficiency v. momentum histograms.
    * \param[in] nofBins Number of bins.
    */
   void SetNofBinsMom(Int_t nofBins) {
      fNofBinsMom = nofBins;
   }

   /**
    * \brief Implement functionality of drawing histograms in macro from the specified file.
    * \param[in] fileName Name of the file.
    */
   void DrawHistosFromFile(const std::string& fileName);

   /**
    * \brief Explicitly sets detectors in the setup.
    * \param[in] detId Id of the detector kMVD, kSTS...
    * \param[in] isDet TRUE if detector is in the setup.
    */
   void SetDetectorPresence(
		   DetectorId detId,
		   bool isDet);

   /**
    * \brief Return TRUE if detector presents in the setup.
    * \param[in] detId Id of the detector kMVD, kSTS...
    * \return TRUE if detector is in the setup.
    */
   bool GetDetectorPresence(
         DetectorId detId) const;

   /**
    * \brief Set explicitly electron setup of the detector.
    * \param[in] isElectronSetup TRUE if electron setup.
    */
   void SetIsElectronSetup(bool isElectronSetup) {fIsElectronSetup = isElectronSetup;}

   /**
    * \brief Return TRUE if electron setup of CBM is detected.
    * \return Return TRUE if electron setup of CBM is detected.
    */
   bool IsElectronSetup() const {return fIsElectronSetup;}

   /**
    * \brief Derived from FairTask.
    */
   virtual void Finish();

private:

   /**
    * \brief Determine CBM detector setup, based on TGeoManager stored in the input MC file.
    */
   void DetermineSetup();

   /**
    * \brief Read data branches from input data files.
    */
   void ReadDataBranches();

   /**
    * \brief Fill hit related histograms.
    */
   void ProcessHits();

   /**
    * \brief Fill histogram with number of crossed stations by MC track.
    */
   void FillNofCrossedStationsHistos();

   /**
    * \brief Fill map with number of hits in MC RICH ring.
    */
   void FillRichRingNofHits();

   /**
    * \brief Loop over the reconstructed global tracks.
    *
    * Check if track is correct and fill
    * std::multimap <MC track index, reconstructed track index>.
    */
   void ProcessGlobalTracks();

   /**
    * \brief Check correctness attached MVD hits.
    */
   void ProcessMvd(
      Int_t stsId);

   /**
    * \brief Check track quality based on fQuota value.
    * \param[in] trackMatch Track match.
    * \param[in] detId Detector identifier for this track match.
    * \return True if track is correctly reconstructed.
    */
   Bool_t CheckTrackQuality(
      CbmTrackMatch* trackMatch,
      DetectorId detId);

   /**
    * \brief Check ring quality based on fQuota value.
    * \param[in] ringMatch Ring match.
    * \return True if ring is correctly reconstructed.
    */
   Bool_t CheckRingQuality(
      CbmRichRingMatch* ringMatch);

   /**
    * \brief Add MC momentum to histograms for detector acceptance calculation.
    * \param[in] mom Momentum of MC track.
    */
   void FillMcHistoForDetAcc(
         Double_t mom);

   /**
    * \brief Loop over the MC tracks. Check track acceptance for different cases.
    * Fill histograms of accepted and reconstructed tracks.
    */
   void ProcessMcTracks();

   /**
    * \brief Fill histograms of accepted and reconstructed tracks.
    * \param[in] mcTrack MC track pointer.
    * \param[in] mcId MC track index in array.
    * \param[in] mcMap Map from MC track index to reconstructed track index. Map is filled in ProcessGlobalTrack function.
    * \param[in] hist Vector with histograms to be filled.
    * \param[in] par Value that will be added in histogram (momentum or number of points).
    */
   void FillGlobalReconstructionHistos(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const std::multimap<Int_t, Int_t>& mcMap,
      std::vector<std::vector<TH1F*> >& hist,
      Double_t par);

   /**
    * \brief ADD COMMENTS.
    */
   void FillGlobalElIdHistos(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const std::multimap<Int_t, Int_t>& mcMap,
      std::vector<TH1F*>& hist,
      Double_t par,
      const std::string& opt);

    /**
     * \brief Fill histograms of accepted and reconstructed rings tracks.
     * \param[in] mcTrack MC track pointer.
     * \param[in] mcId MC track index in the array.
     * \param[in] mcMap Map from MC track index to reconstructed track index. Map is filled in the ProcessGlobalTrack function.
     * \param[in] hist Vector with histograms to be filled.
     * \param[in] par Value that will be added in histogram (momentum or number of points)
     */
   void FillGlobalReconstructionHistosRich(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const std::multimap<Int_t, Int_t>& mcMap,
      std::vector<std::vector<TH1F*> >& hist,
      Double_t par);

   /**
    * \brief Fill pion suppression histogramms.
    */
   void PionSuppression();

   /**
    * \brief Fill histograms: momentum resolution vs. momentum and chi2Vertex.
    */
   void StsTracksQa();

   /**
    * \brief Fill MC momentum vs. polar angle histogram.
    */
   void FillMCMomVsAngle(
         const CbmMCTrack* mcTrack);

   /**
    * \brief Create efficiency histograms using specified parameters.
    * \param[out] hist 2D vector of histograms.
    * \param[in] name Histogram name.
    * \param[in] nofBins Number of bins in histograms.
    * \param[in] minBin Minimum value of histograms.
    * \param[in] maxBin Maximum value of histograms.
    * \param[in] opt "tracking" for STS, TRD and TOF or "rich" for RICH.
    * \param[in] file If file==NULL then create histograms else read histograms from file.
    */
   void CreateEffHisto(
   		std::vector<std::vector<TH1F*> >& hist,
   		const std::string& name,
   		Int_t nofBins,
   		Double_t minBin,
   		Double_t maxBin,
   		const std::string& opt,
   		TFile* file);

   /**
    * \brief Create electron identification histograms using specified parameters.
    * \param[out] hist 2D vector of histograms.
    * \param[in] name Histogram name.
    * \param[in] nofBins Number of bins in histograms.
    * \param[in] minBin Minimum value of histograms.
    * \param[in] maxBin Maximum value of histograms.
    * \param[in] file If file==NULL then create histograms else read histograms from file.
    */
   void CreateEffHistoElId(
         std::vector<std::vector<TH1F*> >& hist,
         const std::string& name,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin,
         TFile* file);

   /**
    * \brief Create histograms for detector acceptance using specified parameters.
    * \param[out] hist vector of histograms.
    * \param[in] name Histogram name.
    * \param[in] nofBins Number of bins in histograms.
    * \param[in] minBin Minimum value of histograms.
    * \param[in] maxBin Maximum value of histograms.
    * \param[in] file If file==NULL then create histograms else read histograms from file.
    */
   void CreateEffHistoDetAcc(
         std::vector<TH1F*>& hist,
         const std::string& name,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin,
         TFile* file);

   /**
    * \brief Create histograms.
    * \param[in] file If file==NULL then create histograms else read histograms from file.
    */
   void CreateHistos(
        TFile* file);

   /**
    * \brief Divide two histograms.
    * \param[in] histo1 Numerator.
    * \param[in] histo2 Denominator.
    * \param[out] histo3 Output histogram.
    * \param[in] c Coefficient.
    */
   void DivideHistos(
      TH1* histo1,
      TH1* histo2,
      TH1* histo3,
      Double_t c);

   /**
    * \brief Calculate efficiency histograms.
    */
   void CalculateEfficiencyHistos();

   /**
    * \brief Write histograms to output file.
    */
   void WriteToFile();

   /**
    * \brief Increase number of tracks counters.
    */
   void IncreaseCounters();

   /**
    * \brief Return std::string of detector used in global reconstruction.
    */
   std::string RecDetector();

   /**
    * \brief Print statistics to property tree and return property tree.
    */
   boost::property_tree::ptree PrintPTree();

   /**
    * \brief TODO ADD COMMENTS
    */
   void NofStatisticsToPTree(
      boost::property_tree::ptree* pt,
      TH1F* mvd,
      TH1F* sts,
      TH1F* rich,
      TH1F* trd,
      TH1F* muchP,
      TH1F* muchS,
      TH1F* tof);

   /**
    * \brief TODO ADD COMMENTS
    */
   void HitsHistosToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         std::vector<TH1F*>& histos);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventEfficiencyStatisticsToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         const std::vector<std::vector<TH1F*> >& hist);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventEfficiencyStatisticsRichToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         const std::vector<std::vector<TH1F*> >& hist);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventEfficiencyStatisticsElIdToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         const std::vector<std::vector<TH1F*> >& hist);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventDetAccElStatisticsToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         TH1F* hmc,
         TH1F* hacc,
         TH1F* hrec);

   /**
    * \brief TODO ADD COMMENTS
    */
   void PolarAngleEfficiencyToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         const std::vector<std::vector<TH1F*> >& hist);

   /**
    * \brief Draw histograms.
    */
   void Draw();

   /**
    * \brief Draw efficiency histograms.
    */
   void DrawEfficiencyHistos();

   /**
    * \brief Draw efficiency plots. TODO ADD COMMENTS
    */
   void DrawEfficiency(
      const std::string& canvasName,
      const std::vector<TH1F*>* hist1,
   	const std::vector<TH1F*>* hist2,
   	const std::vector<TH1F*>* hist3,
   	const std::vector<TH1F*>* hist4,
   	const std::string& name1,
   	const std::string& name2,
   	const std::string& name3,
   	const std::string& name4,
   	const std::string& opt);

   /**
    * \brief Draw ACC and REC efficiency graphs normalized to MC.
    */
   void DrawMcEfficiencyGraph();

   /**
    * \brief Calculate efficiency for two histograms.
    */
   Double_t CalcEfficiency(
      TH1* histRec,
      TH1* histAcc,
      const std::string& opt);

   /**
    * \brief Draw mean efficiency lines (up to 4) on the histogramm.
    */
   void DrawMeanEfficiencyLines(
      TH1* h,
      Double_t eff1,
      Double_t eff2 = -1.,
      Double_t eff3 = -1.,
      Double_t eff4 = -1.);

   /**
    * \brief Draw histograms for hits.
    */
   void DrawHitsHistos();

   /**
    * \brief Draw histograms for hits.
    * \param[in] canvasName Name of canvas.
    * \param[in] histos Vector with hits histograms.
    */
   void DrawHitsHistos(
	   const std::string& canvasName,
      std::vector<TH1F*>& histos);

   /**
    * \brief Draw histogram of number of hits in station.
    * \param[in] name Canvas name.
    * \param[in] hist Pointer to histogram.
    */
   void DrawHitsStationHisto(
      const std::string& name,
      TH1F* hist);

   /**
    * \brief Draw histograms of number of hits in station.
    */
   void DrawHitsStationHistos();

   /**
    * \brief Draw histograms of Sts tracks Qa.
    */
   void DrawStsTracksQaHistos();

   /**
    * \brief Draw MC momentum vs. angle histogram.
    */
   void DrawMCMomVsAngle();

   /**
    * \brief Calculate electron Identification.
    */
   void ElectronIdentification();

   Int_t fMinNofPointsSts; // Minimal number of MCPoints in STS
   Int_t fMinNofPointsTrd; // Minimal number of MCPoints in TRD
   Int_t fMinNofPointsMuch; // Minimal number of MCPoints in MUCH
   Int_t fMinNofPointsTof; // Minimal number of MCPoints in TOF
   Double_t fQuota;  // True/all hits for track to be considered correctly reconstructed

   Int_t fMinNofHitsRich; // Minimal number of hits in MC RICH ring
   Double_t fQuotaRich; // True/all hits for ring to be considered correctly reconstructed

   // Minimal number of hits in track to be considered as accepted.
   // This is needed because the definition of the correctly reconstructed track
   // is 70% of hits in the reconstructed track are from the same MC track.
   // But this can lead to the situation when a reconstructed track has 4 hits,
   // all of them from the same MC track, which means that the track is correctly
   // reconstructed. BUT the real MC track has 12 points(hits). This is especially
   // important for the MUCH detector. To avoid such situations one has to put
   // such cut.
   Int_t fMinNofHitsTrd; // for TRD track
   Int_t fMinNofHitsMuch; // for MUCH track

   Bool_t fIsElectronSetup; // If "electron" setup detected than true
   Bool_t fIsMvd; // If MVD detected than true
   Bool_t fIsSts; // If STS detected than true
   Bool_t fIsRich; // If RICH detected than true
   Bool_t fIsTrd; // If TRD detected than true
   Bool_t fIsMuch; // If MUCH detected than true
   Bool_t fIsTof; // If TOF detected than true

   Double_t fRefMomentum; // Momentum cut for reference tracks
   Int_t fRefMinNofHitsRich; // Minimum number of hits in RICH ring to be considered as reference

   Double_t fMinMom; // Minimum momentum for tracks for efficiency calculation [GeV/c]
   Double_t fMaxMom; // Maximum momentum for tracks for efficiency calculation [GeV/c]
   Int_t fNofBinsMom; // Number of bins for efficiency vs. momentum histogram
   Double_t fMinAngle; // Minimum polar angle [grad]
   Double_t fMaxAngle; // Maximum polar angle [grad]
   Int_t fNofBinsAngle; // Number of bins for efficiency vs. polar angle histogram


   // Maps for reconstructed tracks
   // <MC track index, reconstructed track index>
   std::multimap<Int_t, Int_t> fMcStsMap; // STS
   std::multimap<Int_t, Int_t> fMcHalfGlobalMap; // STS+TRD(MUCH)
   std::multimap<Int_t, Int_t> fMcGlobalMap; // STS+TRD(MUCH)+TOF

   std::multimap<Int_t, Int_t> fMcRichMap; // RICH
   std::multimap<Int_t, Int_t> fMcStsRichMap; // STS+RICH
   std::multimap<Int_t, Int_t> fMcStsRichTrdMap; // STS+RICH+TRD
   std::multimap<Int_t, Int_t> fMcStsRichTrdTofMap; // STS+RICH+TRD+TOF

   // Number of hits in the MC RICH ring
   std::map<Int_t, Int_t> fNofHitsInRingMap;

   // Number of crossed stations
   // <MC track index, number of crossed stations>
   std::map<Int_t, Int_t> fNofStsStationsMap;
   std::map<Int_t, Int_t> fNofTrdStationsMap;
   std::map<Int_t, Int_t> fNofMuchStationsMap;

   // Pointers to data arrays
   TClonesArray* fMCTracks; // CbmMCTrack array
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array

   TClonesArray* fMvdHits; // CbmMvdHit array
   TClonesArray* fMvdPoints; // CbmMvdPoint array
   TClonesArray* fMvdHitMatches; // CbmMvdHitMatch array
   TClonesArray* fMvdDigis; // CbmMvdDigi array
   TClonesArray* fMvdClusters; // CbmMvdClusters array

   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsMatches; // CbmStsTrackMatch array
   TClonesArray* fStsPoints; // CbmStsPoint array
   TClonesArray* fStsDigis; // CbmStsDigi array
   TClonesArray* fStsClusters; // CbmStsCluster array
   TClonesArray* fStsHits; // CbmStsHit array

   TClonesArray* fRichHits; // CbmRichHits array
   TClonesArray* fRichRings; // CbmRichRing array
   TClonesArray* fRichProjections; // CbmRichProjection array
   TClonesArray* fRichRingMatches; // CbmRichProjection array
   TClonesArray* fRichPoints; // CbmRichPoint array

   TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array
   TClonesArray* fMuchStrawHits; // CbmMuchStrawHits array
   TClonesArray* fMuchMatches; // CbmTrackMatch array
   TClonesArray* fMuchPoints; // CbmMuchPoint array
   TClonesArray* fMuchDigis; // CbmMuchDigi array
   TClonesArray* fMuchClusters; // CbmMuchCluster array

   TClonesArray* fTrdMatches; // CbmTrackMatch array
   TClonesArray* fTrdHits; // CbmTrdHit array
   TClonesArray* fTrdPoints; // CbmTrdPoint array
   TClonesArray* fTrdDigis; // CbmTrdDigi array
   TClonesArray* fTrdClusters; // CbmTrdCluster array

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
   std::vector<std::vector<TH1F*> > fhStsMomNormHalfGlobal; // STS: momentum dependence, normalized to STS+TRD(MUCH)
   std::vector<std::vector<TH1F*> > fhStsMomNormGlobal; // STS: momentum dependence, normalized to STS+TRD(MUCH)+TOF
   std::vector<std::vector<TH1F*> > fhStsNp; // STS: number of points dependence
   std::vector<std::vector<TH1F*> > fhStsAngle; // STS: polar angle dependence
   std::vector<std::vector<TH1F*> > fhHalfGlobalMom; // STS+TRD(MUCH): momentum dependence
   std::vector<std::vector<TH1F*> > fhHalfGlobalMomNormGlobal; // STS+TRD(MUCH): momentum dependence, normalized to STS+TRD(MUCH)+TOF
   std::vector<std::vector<TH1F*> > fhGlobalMom; // STS+TRD(MUCH)+TOF: momentum dependence
   std::vector<std::vector<TH1F*> > fhRecMom; // TRD(MUCH): momentum dependence
   std::vector<std::vector<TH1F*> > fhRecNp; // TRD(MUCH): number of points dependence
   std::vector<std::vector<TH1F*> > fhRecAngle; // TRD(MUCH): polar angle dependence
   std::vector<std::vector<TH1F*> > fhTofMom; // TOF: momentum dependence
   std::vector<std::vector<TH1F*> > fhTofAngle; // TOF: momentum dependence
   // histograms for ghost tracks
   // STS: ghost tracks (number of hits dependence)
   TH1F* fhStsGhostNh;
   // TRD(MUCH): ghost tracks (number of hits dependence)
   TH1F* fhRecGhostNh;
   // RICH: ghost rings (number of hits dependence)
   TH1F* fhRichGhostNh;
   // RICH: ghost rings after STS matching (number of hits dependence)
   TH1F* fhRichGhostStsMatchingNh;
   // RICH: ghost rings after STS matching and electron identification (number of hits dependence)
   TH1F* fhRichGhostElIdNh;
   // STS: ghost tracks after RICH matching (number of hits dependence)
   TH1F* fhStsGhostRichMatchingNh;


   // RICH performance histograms
   std::vector<std::vector<TH1F*> > fhRichMom;// RICH: momentum dependence
   std::vector<std::vector<TH1F*> > fhRichNh;// RICH: number of hits
   //Norm STS+RICH
   std::vector<std::vector<TH1F*> > fhStsRichMom;// STS+RICH: momentum dependence
   std::vector<std::vector<TH1F*> > fhStsMomNormStsRich;// STS: momentum dependence, normalized to STS+RICH
   //Norm STS+RICH+TRD
   std::vector<std::vector<TH1F*> > fhStsMomNormStsRichTrd;// STS: momentum dependence, normalized to STS+RICH+TRD
   std::vector<std::vector<TH1F*> > fhStsRichTrdMom;// STS+RICH+TRD: momentum dependence
   std::vector<std::vector<TH1F*> > fhStsRichMomNormStsRichTrd;// STS+RICH: momentum dependence, normalized to STS+RICH+TRD
   //Norm STS+RICH+TRD+TOF
   std::vector<std::vector<TH1F*> > fhStsMomNormStsRichTrdTof;// STS: momentum dependence, normalized to STS+RICH+TRD+TOF
   std::vector<std::vector<TH1F*> > fhStsRichMomNormStsRichTrdTof;// STS+RICH: momentum dependence, normalized to STS+RICH+TRD+TOF
   std::vector<std::vector<TH1F*> > fhStsRichTrdMomNormStsRichTrdTof;// STS+RICH+TRD: momentum dependence, normalized to STS+RICH+TRD+TOF
   std::vector<std::vector<TH1F*> > fhStsRichTrdTofMom;// STS+RICH+TRD+TOF: momentum dependence, normalized to STS+RICH+TRD+TOF

   TH1F* fhMvdNofHitsInStation; // mean number of hits in station in MVD
   TH1F* fhStsNofHitsInStation; // mean number of hits in station in STS
   TH1F* fhTrdNofHitsInStation; // mean number of hits in station in TRD
   TH1F* fhMuchNofHitsInStation; // mean number of hits in station in MUCH
   TH1F* fhTofNofHitsInStation; // mean number of hits in station in TOF

   // histograms of number of hits in track
   // all, true, fake, true/all, fake/all
   std::vector<TH1F*> fhMvdTrackHits;
   std::vector<TH1F*> fhStsTrackHits;
   std::vector<TH1F*> fhTrdTrackHits;
   std::vector<TH1F*> fhMuchTrackHits;
   std::vector<TH1F*> fhRichRingHits;

   TList* fHistoList; // List of histograms

   // Total number of tracks/rings/hits histograms
   TH1F* fhNofGlobalTracks; // global tracks
   TH1F* fhNofStsTracks; // STS tracks
   TH1F* fhNofTrdTracks; // TRD tracks
   TH1F* fhNofMuchTracks; // MUCH tracks
   TH1F* fhNofRichRings; // RICH rings
   TH1F* fhNofRichProjections; // RICH projections
   // hits
   TH1F* fhNofMvdHits; // MVD hits
   TH1F* fhNofStsHits; // STS hits
   TH1F* fhNofRichHits; // RICH hits
   TH1F* fhNofTrdHits; // TRD hits
   TH1F* fhNofMuchPixelHits; // MUCH pixel hits
   TH1F* fhNofMuchStrawHits; // MUCH straw hits
   TH1F* fhNofTofHits; // TOF hits
   // points
   TH1F* fhNofMvdPoints; // MVD points
   TH1F* fhNofStsPoints; // STS points
   TH1F* fhNofRichPoints; // RICH points
   TH1F* fhNofTrdPoints; // TRD points
   TH1F* fhNofMuchPoints; // MUCH points
   TH1F* fhNofTofPoints; // TOF points
   // digis
   TH1F* fhNofMvdDigis; // MVD digis
   TH1F* fhNofStsDigis; // STS digis
   TH1F* fhNofTrdDigis; // TRD digis
   TH1F* fhNofMuchDigis; // MUCH digis
   // clusters
   TH1F* fhNofMvdClusters; // MVD clusters
   TH1F* fhNofStsClusters; // STS clusters
   TH1F* fhNofTrdClusters; // TRD clusters
   TH1F* fhNofMuchClusters; // MUCH clusters

   TH1F* fhStsChiprim; //chi2Vertex
   TH2F* fhStsMomresVsMom; // momentum resolution vs. momentum


   // Electron identification
   // h[category][histogram type]:
   // track category (ElId, PiSupp)
   // histogram type (acc, rec, eff)
   std::vector<std::vector<TH1F*> > fhStsTrdMomElId; // STS+TRD: momentum dependence
   std::vector<std::vector<TH1F*> > fhStsTrdMomElIdNormStsTrdTof; // STS+TRD: momentum dependence, normalized to STS+TRD+TOF
   std::vector<std::vector<TH1F*> > fhStsTrdTofMomElId; // STS+TRD+TOF: momentum dependence
   //Norm STS+RICH
   std::vector<std::vector<TH1F*> > fhStsRichMomElId;// STS+RICH: momentum dependence
   //Norm STS+RICH+TRD
   std::vector<std::vector<TH1F*> > fhStsRichMomElIdNormStsRichTrd;// STS+RICH: momentum dependence, normalized to STS+RICH+TRD
   std::vector<std::vector<TH1F*> > fhStsRichTrdMomElId;// STS+RICH+TRD: momentum dependence
   //Norm STS+RICH+TRD+TOF
   std::vector<std::vector<TH1F*> > fhStsRichMomElIdNormStsRichTrdTof;// STS+RICH: momentum dependence, normalized to STS+RICH+TRD+TOF
   std::vector<std::vector<TH1F*> > fhStsRichTrdMomElIdNormStsRichTrdTof;// STS+RICH+TRD: momentum dependence, normalized to STS+RICH+TRD+TOF
   std::vector<std::vector<TH1F*> > fhStsRichTrdTofMomElId;// STS+RICH+TRD+TOF: momentum dependence, normalized to STS+RICH+TRD+TOF

   // Detector acceptance for primary electrons (signal)
   // h[histogram type]:
   // histogram type (mc, acc, eff)
   // local detector acceptance
   std::vector<TH1F*> fhStsDetAccEl; // STS detector acceptance
   // pair detector acceptance
   std::vector<TH1F*> fhStsRichDetAccEl; // STS-RICH detector acceptance
   std::vector<TH1F*> fhStsTrdDetAccEl; // STS-TRD detector acceptance
   std::vector<TH1F*> fhStsTofDetAccEl; // STS-TOF detector acceptance
   // global detector acceptance
   std::vector<TH1F*> fhStsRichTrdDetAccEl; // STS-RICH-TOF detector acceptance
   std::vector<TH1F*> fhStsRichTrdTofDetAccEl; // STS-RICH-TRD-TOF detector acceptance
   std::vector<TH1F*> fhStsTrdTofDetAccEl; // STS-TRD-TOF detector acceptance

   // MC mom vs polar angle at vertex
   // [track category]
   // track category (all, ref, prim, sec, muon, electron)
   std::vector<TH2F*> fhMCMomVsAngle;

   TH1F* fhEventNo; // Event counter

   std::string fOutputDir; // Output directory for images

   CbmVertex *fPrimVertex; // Pointer to the primary vertex
   CbmStsKFTrackFitter* fKFFitter; // Pointer to the Kalman Filter Fitter algorithm

   CbmLitGlobalElectronId* fElectronId; // Electron identification tool
};

#endif /* CBMLITRECONSTRUCTIONQAIMPL_H_ */
