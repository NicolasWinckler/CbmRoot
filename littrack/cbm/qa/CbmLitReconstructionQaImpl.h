/** CbmLitReconstructionQaImpl.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **
 ** Task for global track reconstruction performance calculation.
 ** Produces efficiency histograms. Calculates different statistics.
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

class CbmLitReconstructionQaImpl : public FairTask
{
public:
   /* Constructor */
   CbmLitReconstructionQaImpl();

   /* Destructor */
   virtual ~CbmLitReconstructionQaImpl();

   /* Derived from FairTask. Executed before starting event-by-event execution. */
   virtual InitStatus Init();

   /* Derived from FairTask. Executed on each event.
     * @param opt Options */
   virtual void Exec(
      Option_t* opt);

   /* Sets the minimum number of MC points in STS.
    * @param minNofPointsSts Minimum number of MC points in STS. */
   void SetMinNofPointsSts(Int_t minNofPointsSts) {fMinNofPointsSts = minNofPointsSts; }

   /* Sets the minimum number of MC points in TRD.
     * @param minNofPointsTrd Minimum number of MC points in TRD. */
   void SetMinNofPointsTrd(Int_t minNofPointsTrd) { fMinNofPointsTrd = minNofPointsTrd;}

   /* Sets the minimum number of MC points in MUCH.
    * @param minNofPointsMuch Minimum number of MC points in MUCH. */
   void SetMinNofPointsMuch(Int_t minNofPointsMuch) { fMinNofPointsMuch = minNofPointsMuch;}

   /* Sets the minimum number of MC points in TOF.
    * @param minNofPointsTof Minimum number of MC points in TOF. */
   void SetMinNofPointsTof(Int_t minNofPointsTof) { fMinNofPointsTof = minNofPointsTof;}

   /* Sets the quota value, which is true/all hits for track to be considered correctly reconstructed.
    * @param quota Quota value. */
   void SetQuota(Double_t quota) { fQuota = quota;}

   /* Sets the minimum number of hits in MC RICH ring.
    * @param minNofHits Minimum number of hits in MC RICH ring. */
   void SetMinNofHitsRich(Int_t minNofHits) { fMinNofHitsRich = minNofHits;}

   /* Sets the quota value for RICH
    * @param quota Quota value. */
   void SetQuotaRich(Double_t quota) { fQuotaRich = quota;}

   /* Sets the minimum number of hits in TRD track.
     * @param minNofPointsTrd Minimum number of hits in TRD. */
   void SetMinNofHitsTrd(Int_t minNofHitsTrd) { fMinNofHitsTrd = minNofHitsTrd;}

   /* Sets the minimum number of hits in MUCH track.
    * @param minNofHitsMuch Minimum number of hits in MUCH track. */
   void SetMinNofHitsMuch(Int_t minNofHitsMuch) { fMinNofHitsMuch = minNofHitsMuch;}

   /* Sets the verbose level for printout.
    * @param verbose Verbose value. */
   void SetVerbose(Int_t verbose) { fVerbose = verbose;}

   /* Sets the output directory for images.
    * @param dir Directory name. */
   void SetOutputDir(const std::string& dir) { fOutputDir = dir;}

   /* Sets the output json file name.
    * @param dir path. */
   void SetOutputJsonFileName(const std::string& path) { fOutputJsonFileName = path;}

   /* Sets momentum range for efficiency calculation */
   void SetMomentumRange(Double_t minMom, Double_t maxMom) {
      fMinMom = minMom;
      fMaxMom = maxMom;
   }

   /* Set number of bins for efficiency v. momentum histograms */
   void SetNofBinsMom(Int_t nofBins) {
      fNofBinsMom = nofBins;
   }

   /* Implement functionality of drawing histograms in the macro
    * from the specified file
    * @param fileName Name of the file */
   void DrawHistosFromFile(const std::string& fileName);

   /* Explicitly sets detectors in the setup
    * detId Id of the detector kMVD, kSTS...
    * isDet true if detector is in the setup */
   void SetDetectorPresence(
		   DetectorId detId,
		   bool isDet);

   /* Set explicitly electron setup of the detector
    * isElectronSetup true if electron setup*/
   void SetIsElectronSetup(bool isElectronSetup) {fIsElectronSetup = isElectronSetup;}

   /* Derived from FairTask. Executed after all events are processed. */
   virtual void Finish();
private:

   /* Determines the CBM detector setup, based on TGeoManager stored in the input MC file. */
   void DetermineSetup();

   /* Reads necessary data branches from the input data files. */
   void ReadDataBranches();

   /* Fills hit related histograms */
   void ProcessHits();

   /* Fill map with number of hits in MC RICH ring */
   void FillRichRingNofHits();

   /* Loops over the reconstructed global tracks. Checks if the track is correct
    * than fills the multimaps <MC track index, reconstructed track index>. */
   void ProcessGlobalTracks();

   /* Checks correctness of MVD attached hits*/
   void ProcessMvd(
      Int_t stsId);

   /* Checks track quality based on fQuota value.
     * @param trackMatch Track match
     * @param detId Detector identifier for this track match
     * @return True if track is correctly reconstructed */
   Bool_t CheckTrackQuality(
      CbmTrackMatch* trackMatch,
      DetectorId detId);

   /* Checks ring quality based on fQuota value.
     * @param ringMatch Ring match
     * @return True if ring is correctly reconstructed */
   Bool_t CheckRingQuality(
      CbmRichRingMatch* ringMatch);

   /* Add MC momentum to the histograms for detector acc calculation
    * @param mom MC momentum of the MC track*/
   void FillMcHistoForDetAcc(
         Double_t mom);

   /* Loops over the MC tracks. Checks the track acceptance for different cases.
     * Fills the histograms of the accepted and reconstructed tracks. */
   void ProcessMcTracks();

   /* Fills the histograms of the accepted and reconstructed tracks.
    * @param mcTrack MC track pointer
    * @param mcId MC track index in the array
    * @param mcMap Map from MC track index to reconstructed track index. Map is filled in the ProcessGlobalTrack function.
    * @param hist vector with histograms to be filled
    * @param par value that will be added in the histos (momentum or number of points) */
   void FillGlobalReconstructionHistos(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const std::multimap<Int_t, Int_t>& mcMap,
      std::vector<std::vector<TH1F*> >& hist,
      Double_t par);

   /* @param opt option string*/
   void FillGlobalElIdHistos(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const std::multimap<Int_t, Int_t>& mcMap,
      std::vector<TH1F*>& hist,
      Double_t par,
      const std::string& opt);

    /* Fills the histograms of the accepted and reconstructed rings tracks.
    * @param mcTrack MC track pointer
    * @param mcId MC track index in the array
    * @param mcMap Map from MC track index to reconstructed track index. Map is filled in the ProcessGlobalTrack function.
    * @param hist vector with histograms to be filled
    * @param par value that will be added in the histos (momentum or number of points)*/
   void FillGlobalReconstructionHistosRich(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const std::multimap<Int_t, Int_t>& mcMap,
      std::vector<std::vector<TH1F*> >& hist,
      Double_t par);

   /* Fills pion suppression histogramms.*/
   void PionSuppression();

   /* Fills the histograms: momentum resolution vs. momentum and chi2Vertex.*/
   void StsTracksQa();

   /* Creates histograms - hist using specified parameters.
   * @param hist 2D vector of the histograms
   * @param name Histogram name
   * @param nofBins Number of bins in the histograms
   * @param minBin Minimum value of the histograms
   * @param maxBin Maximum value of the histograms
   * @param opt "tracking" for STS, TRD and TOF or "rich" for RICH
   * @param file if file==NULL then create histograms else read histograms from file*/
   void CreateEffHisto(
   		std::vector<std::vector<TH1F*> >& hist,
   		const std::string& name,
   		Int_t nofBins,
   		Double_t minBin,
   		Double_t maxBin,
   		const std::string& opt,
   		TFile* file);

   /* Creates histograms for electron identification- hist using specified parameters.
   * @param hist 2D vector of the histograms
   * @param name Histogram name
   * @param nofBins Number of bins in the histograms
   * @param minBin Minimum value of the histograms
   * @param maxBin Maximum value of the histograms
   * @param file if file==NULL then create histograms else read histograms from file*/
   void CreateEffHistoElId(
         std::vector<std::vector<TH1F*> >& hist,
         const std::string& name,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin,
         TFile* file);

   /* Creates histograms for detector acceptance - hist using specified parameters.
   * @param hist vector of the histograms
   * @param name Histogram name
   * @param nofBins Number of bins in the histograms
   * @param minBin Minimum value of the histograms
   * @param maxBin Maximum value of the histograms
   * @param file if file==NULL then create histograms else read histograms from file*/
   void CreateEffHistoDetAcc(
         std::vector<TH1F*>& hist,
         const std::string& name,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin,
         TFile* file);

   /* Creates the histograms.
    *@param file if file==NULL then create histograms else read histograms from file*/
   void CreateHistos(
        TFile* file);

   /* Divides two histograms.
    * @param histo1 numerator
    * @param histo2 denominator
    * @param histo3 output histogram
    * @param c coefficient*/
   void DivideHistos(
      TH1* histo1,
      TH1* histo2,
      TH1* histo3,
      Double_t c);

   /* Calculates efficiency histograms. */
   void CalculateEfficiencyHistos();

   /* Writes the histograms to output file. */
   void WriteToFile();

   /* Increase number of tracks counters. */
   void IncreaseCounters();

   /* Returns std::string of the detector used in the global reconstruction. */
   std::string RecDetector();

   /* Prints statistics to property tree and return property tree. */
   boost::property_tree::ptree PrintPTree();

   void NofStatisticsToPTree(
      boost::property_tree::ptree* pt,
      TH1F* mvd,
      TH1F* sts,
      TH1F* rich,
      TH1F* trd,
      TH1F* muchP,
      TH1F* muchS,
      TH1F* tof);

   void HitsHistosToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         std::vector<TH1F*>& histos);

   void EventEfficiencyStatisticsToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         const std::vector<std::vector<TH1F*> >& hist);

   void EventEfficiencyStatisticsRichToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         const std::vector<std::vector<TH1F*> >& hist);

   void EventEfficiencyStatisticsElIdToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         const std::vector<std::vector<TH1F*> >& hist);

   void EventDetAccElStatisticsToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         TH1F* hmc,
         TH1F* hacc,
         TH1F* hrec);

   void PolarAngleEfficiencyToPTree(
         boost::property_tree::ptree* pt,
         const std::string& name,
         const std::vector<std::vector<TH1F*> >& hist);

   /* Draws histograms. */
   void Draw();

   /* Draws efficiency histograms. */
   void DrawEfficiencyHistos();

   /* Draws efficiency plots */
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

   /* Draws ACC and REC efficiency graphs normalized to MC*/
   void DrawMcEfficiencyGraph();

   /* Calculate efficiency for two histograms */
   Double_t CalcEfficiency(
      TH1* histRec,
      TH1* histAcc,
      const std::string& opt);

   /* Draw mean efficiency lines (up to 4) on the histogramm */
   void DrawMeanEfficiencyLines(
      TH1* h,
      Double_t eff1,
      Double_t eff2 = -1.,
      Double_t eff3 = -1.,
      Double_t eff4 = -1.);

   /* Draws histograms for hits */
   void DrawHitsHistos();

   /* Draws histograms for hits
    * @param canvasName Name of the canvas.
    * @param histos Vector with hits histograms. */
   void DrawHitsHistos(
	   const std::string& canvasName,
      std::vector<TH1F*>& histos);

   /* Draws histogram of number of hits in station
    * @name Canvas name
    * @hist Pointer to histogram */
   void DrawHitsStationHisto(
      const std::string& name,
      TH1F* hist);

   /* Draws histograms of number of hits in station */
   void DrawHitsStationHistos();

   /* Draws histograms of Sts tracks Qa */
   void DrawStsTracksQaHistos();

   /* Calculate electron Identification */
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

   TH1F* fhEventNo; // Event counter

   std::string fOutputDir; // Output directory for images
   std::string fOutputJsonFileName; // Output directory for images

   CbmVertex *fPrimVertex; // Pointer to the primary vertex
   CbmStsKFTrackFitter* fKFFitter; // Pointer to the Kalman Filter Fitter algorithm

   CbmLitGlobalElectronId* fElectronId; // Electron identification tool

};

#endif /* CBMLITRECONSTRUCTIONQAIMPL_H_ */
