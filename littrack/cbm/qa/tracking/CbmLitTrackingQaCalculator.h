/**
 * \file CbmLitTrackingQaCalculator.h
 * \brief Implementation of global track reconstruction QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */

#ifndef CBMLITTRACKINGQACALCULATOR_H_
#define CBMLITTRACKINGQACALCULATOR_H_

#include "FairTask.h"
#include "CbmDetectorList.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmLitTrackingQaHistNames.h"

#include <map>
#include <string>
#include <vector>
#include <set>

#include <boost/property_tree/ptree.hpp>

class CbmTrackMatch;
class CbmRichRingMatch;
class CbmVertex;
class CbmStsKFTrackFitter;
class CbmMCTrack;
class TClonesArray;
class TH1;
class TH2F;
class TH3F;
class TH1F;
class CbmLitGlobalElectronId;
class CbmLitHistManager;
class CbmRichRingFitterEllipseTau;
class CbmLitMCTrackCreator;

using std::multimap;
using std::string;
using std::map;

/**
 * \class CbmLitTrackingQaCalculator
 * \brief Implementation of global track reconstruction QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */
class CbmLitTrackingQaCalculator : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackingQaCalculator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQaCalculator();

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
    * \brief Derived from FairTask.
    */
   virtual void Finish();

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
    * \brief Set if consecute MC points are used for efficiency normalization.
    * \param[in] param quota Quota value.
    */
   void SetUseConsecutivePointsInSts(Bool_t useConsecutivePointsInSts) { fUseConsecutivePointsInSts = useConsecutivePointsInSts; }

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
    * \brief Fill histogram with number of crossed stations by MC track.
    */
   void FillNofCrossedStationsHistos();

   /**
    * \brief Return maximum number of consecutive numbers in std:set.
    * \param[in] numbers Set with numbers.
    * \return Maximum number.
    */
   Int_t MaxConsecutiveNumbers(
         const std::set<Int_t>& numbers);

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
    * \brief Loop over the reconstructed RICH rings.
    *
    * Check if ring is correct and fill
    * std::multimap <MC track index, reconstructed ring index>.
    */
   void ProcessRichRings();

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
    * \param[in] p Momentum of MC track.
    * \param[in] y Rapidity of Mc track.
    * \param[in] pt Transverse momentum of MC track.
    */
   void FillMcHistoForDetAcc(
         Double_t p,
         Double_t y,
         Double_t pt);

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
    * \param[in] hist main name of the histograms to be filled.
    * \param[in] par Value that will be added in histogram (momentum or number of points).
    */
   void FillGlobalReconstructionHistos(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const multimap<Int_t, Int_t>& mcMap,
      const string& hist,
      Double_t par = -1.);

   /**
    * \brief ADD COMMENTS.
    */
   void FillGlobalElIdHistos3D(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const multimap<Int_t, Int_t>& mcMap,
      const string& hist,
      const string& opt);

    /**
     * \brief Fill histograms of accepted and reconstructed rings tracks.
     * \param[in] mcTrack MC track pointer.
     * \param[in] mcId MC track index in the array.
     * \param[in] mcMap Map from MC track index to reconstructed track index. Map is filled in the ProcessGlobalTrack function.
     * \param[in] hist main name of the histograms to be filled.
     * \param[in] par Value that will be added in histogram (momentum or number of points)
     */
   void FillGlobalReconstructionHistosRich(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const multimap<Int_t, Int_t>& mcMap,
      const string& hist,
      Double_t par = -1.);

   /**
    * \brief Fill pion suppression histogramms.
    */
   void PionSuppression3D();

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
    * \brief Create x,y,z projections from 3D histogram. New histograms
    * will be added to histogram manager (fHM) with names hist+"px", "py", "pz"
    * \hist[in] Name of the histogram.
    * \opt Options.
    */
   void CreateProjections3D(
         const string& hist,
         LitQaNameOption opt);

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

   void DivideHistos(
      const string& hist,
      LitQaNameOption opt,
      Bool_t addProjections = false);

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
    * \brief Draw histograms.
    */
   void Draw();

   /**
    * \brief Calculate electron Identification.
    */
   void ElectronIdentification();

   Int_t fMinNofPointsSts; // Minimal number of MCPoints in STS
   Int_t fMinNofPointsTrd; // Minimal number of MCPoints in TRD
   Int_t fMinNofPointsMuch; // Minimal number of MCPoints in MUCH
   Int_t fMinNofPointsTof; // Minimal number of MCPoints in TOF
   Double_t fQuota;  // True/all hits for track to be considered correctly reconstructed

   Bool_t fUseConsecutivePointsInSts; // Use consecutive MC points for STS normalization

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

   // Maps for reconstructed tracks
   // <MC track index, reconstructed track index>
   std::multimap<Int_t, Int_t> fMcStsMap; // STS
   std::multimap<Int_t, Int_t> fMcHalfGlobalMap; // STS+TRD(MUCH)
   std::multimap<Int_t, Int_t> fMcGlobalMap; // STS+TRD(MUCH)+TOF

   std::multimap<Int_t, Int_t> fMcRichMap; // RICH
   std::multimap<Int_t, Int_t> fMcStsRichMap; // STS+RICH
   std::multimap<Int_t, Int_t> fMcStsRichNoMatchingMap; // STS+RICH
   std::multimap<Int_t, Int_t> fMcStsRichTrdMap; // STS+RICH+TRD
   std::multimap<Int_t, Int_t> fMcStsRichTrdTofMap; // STS+RICH+TRD+TOF

   // Number of hits in the MC RICH ring
   std::map<Int_t, Int_t> fNofHitsInRingMap;

   // Number of crossed stations
   // <MC track index, number of crossed stations>
   std::map<Int_t, Int_t> fNofStsStationsMap;
   std::map<Int_t, Int_t> fNofStsConsecutiveStationsMap;
   std::map<Int_t, Int_t> fNofTrdStationsMap;
   std::map<Int_t, Int_t> fNofMuchStationsMap;

   // Pointers to data arrays
   TClonesArray* fMCTracks; // CbmMCTrack array
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array

   TClonesArray* fMvdPoints; // CbmMvdPoint array
   TClonesArray* fMvdHitMatches; // CbmMvdHitMatch array

   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsMatches; // CbmStsTrackMatch array
   TClonesArray* fStsPoints; // CbmStsPoint array

   TClonesArray* fRichHits; // CbmRichHits array
   TClonesArray* fRichRings; // CbmRichRing array
   TClonesArray* fRichProjections; // CbmRichProjection array
   TClonesArray* fRichRingMatches; // CbmRichProjection array
   TClonesArray* fRichPoints; // CbmRichPoint array

   TClonesArray* fMuchMatches; // CbmTrackMatch array
   TClonesArray* fMuchPoints; // CbmMuchPoint array

   TClonesArray* fTrdMatches; // CbmTrackMatch array
   TClonesArray* fTrdPoints; // CbmTrdPoint array

   TClonesArray* fTofPoints; // CbmTofPoint array
   TClonesArray* fTofHits; // CbmTofHit array

   string fOutputDir; // Output directory for images

   CbmVertex *fPrimVertex; // Pointer to the primary vertex
   CbmStsKFTrackFitter* fKFFitter; // Pointer to the Kalman Filter Fitter algorithm

   CbmLitGlobalElectronId* fElectronId; // Electron identification tool
   CbmRichRingFitterEllipseTau* fRichEllipseFitter; // ellipse fitter

   CbmLitMCTrackCreator* fMCTrackCreator;
   CbmLitHistManager* fHM; // histogram manager
};

#endif /* CBMLITTRACKINGQACALCULATOR_H_ */
