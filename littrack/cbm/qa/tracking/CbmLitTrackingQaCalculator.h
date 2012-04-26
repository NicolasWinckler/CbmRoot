/**
 * \file CbmLitTrackingQaCalculator.h
 * \brief Implementation of global track reconstruction QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */

#ifndef CBMLITTRACKINGQACALCULATOR_H_
#define CBMLITTRACKINGQACALCULATOR_H_

#include "CbmDetectorList.h"
#include "CbmStsKFTrackFitter.h"
#include "cbm/base/CbmLitDetectorSetup.h"

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
class CbmLitGlobalElectronId;
class CbmLitHistManager;
class CbmRichRingFitterEllipseTau;
class CbmLitMCTrackCreator;
class TFile;

using std::multimap;
using std::string;
using std::map;

/**
 * \class CbmLitTrackingQaCalculator
 * \brief Implementation of global track reconstruction QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */
class CbmLitTrackingQaCalculator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackingQaCalculator(
         CbmLitHistManager* histManager);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQaCalculator();

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

   /**
    * \brief Set minimum number of MC points in STS.
    * \param[in] minNofPointsSts Minimum number of MC points in STS.
    */
   void SetMinNofPointsSts(Int_t minNofPointsSts) { fMinNofPointsSts = minNofPointsSts; }

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
    * \brief Calculate efficiency histograms.
    */
   void CalculateEfficiencyHistos();


private:

   /**
    * \brief Read data branches from input data files.
    */
   void ReadDataBranches();

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
      const CbmTrackMatch* trackMatch,
      DetectorId detId);

   /**
    * \brief Check ring quality based on fQuota value.
    * \param[in] ringMatch Ring match.
    * \return True if ring is correctly reconstructed.
    */
   Bool_t CheckRingQuality(
      const CbmRichRingMatch* ringMatch);

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
      Int_t mcId,
      const multimap<Int_t, Int_t>& mcMap,
      const string& histName,
      const string& accName,
      Double_t par);

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
      Int_t mcId,
      const multimap<Int_t, Int_t>& mcMap,
      const string& histName,
      const string& accName,
      Double_t par);

//   /**
//    * \brief Fill pion suppression histogramms.
//    */
//   void PionSuppression3D();

//   /**
//    * \brief Fill histograms: momentum resolution vs. momentum and chi2Vertex.
//    */
//   void StsTracksQa();

//   /**
//    * \brief Fill MC momentum vs. polar angle histogram.
//    */
//   void FillMCMomVsAngle(
//         const CbmMCTrack* mcTrack);

   /**
    * \brief Divide two histograms.
    * \param[in] histo1 Numerator.
    * \param[in] histo2 Denominator.
    * \param[out] histo3 Output histogram.
    * \param[in] scale Scaling factor.
    */
   void DivideHistos(
      TH1* histo1,
      TH1* histo2,
      TH1* histo3,
      Double_t scale);

   /**
    * \brief Increase number of objects counters.
    */
   void IncreaseCounters();

   /**
    * \brief Calculate electron Identification.
    */
   void ElectronIdentification();

   // Acceptance defined by MC points
   Int_t fMinNofPointsSts; // Minimal number of MCPoints in STS
   Int_t fMinNofPointsTrd; // Minimal number of MCPoints in TRD
   Int_t fMinNofPointsMuch; // Minimal number of MCPoints in MUCH
   Int_t fMinNofPointsTof; // Minimal number of MCPoints in TOF
   Double_t fQuota;  // True/all hits for track to be considered correctly reconstructed

   Bool_t fUseConsecutivePointsInSts; // Use consecutive MC points for STS normalization

   // Acceptance in RICH
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

   CbmLitDetectorSetup fDet; // For access of detector presence in setup

   Double_t fRefMomentum; // Momentum cut for reference tracks
   Int_t fRefMinNofHitsRich; // Minimum number of hits in RICH ring to be considered as reference

   // Global track segment name maps to multimap <MC track index, reconstructed track index>
   map<string, multimap<Int_t, Int_t> > fMcToRecoMap;

   // Pointers to data arrays
   TClonesArray* fMCTracks; // CbmMCTrack array
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array
   TClonesArray* fMvdPoints; // CbmMvdPoint array
   TClonesArray* fMvdHitMatches; // CbmMvdHitMatch array
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsMatches; // CbmStsTrackMatch array
   TClonesArray* fRichRings; // CbmRichRing array
   TClonesArray* fRichProjections; // CbmRichProjection array
   TClonesArray* fRichRingMatches; // CbmRichProjection array
   TClonesArray* fMuchMatches; // CbmTrackMatch array
   TClonesArray* fTrdMatches; // CbmTrackMatch array
   TClonesArray* fTofPoints; // CbmTofPoint array
   TClonesArray* fTofHits; // CbmTofHit array

   CbmVertex *fPrimVertex; // Pointer to the primary vertex
   CbmStsKFTrackFitter* fKFFitter; // Pointer to the Kalman Filter Fitter algorithm

   CbmLitGlobalElectronId* fElectronId; // Electron identification tool
   CbmRichRingFitterEllipseTau* fRichEllipseFitter; // ellipse fitter

   CbmLitMCTrackCreator* fMCTrackCreator;
   CbmLitHistManager* fHM; // histogram manager
};

#endif /* CBMLITTRACKINGQACALCULATOR_H_ */
