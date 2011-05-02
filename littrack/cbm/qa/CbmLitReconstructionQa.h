/** CbmLitReconstructionQa.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **
 ** Task for global track reconstruction performance calculation.
 ** Produces efficiency histograms. Calculates different statistics.
 **/

#ifndef CBMLITRECONSTRUCTIONQA_H_
#define CBMLITRECONSTRUCTIONQA_H_

#include "FairTask.h"
#include "CbmDetectorList.h"
#include "CbmStsKFTrackFitter.h"

#include <map>
#include <string>
#include <vector>

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

class CbmLitReconstructionQa : public FairTask
{
public:
   /* Constructor */
   CbmLitReconstructionQa();

   /* Destructor */
   virtual ~CbmLitReconstructionQa();

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

private:
   /* Derived from FairTask. Executed after all events are processed. */
   virtual void Finish();

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

   /* */
   void FillGlobalElIdHistos(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const std::multimap<Int_t, Int_t>& mcMap,
      std::vector<TH1F*>& hist,
      Double_t par);

    /* Fills the histograms of the accepted and reconstructed rings tracks.
    * @param mcTrack MC track pointer
    * @param mcId MC track index in the array
    * @param mcMap Map from MC track index to reconstructed track index. Map is filled in the ProcessGlobalTrack function.
    * @param hist vector with histograms to be filled
    * @param par value that will be added in the histos (momentum or number of points) */
   void FillGlobalReconstructionHistosRich(
      const CbmMCTrack* mcTrack,
      Int_t mcId,
      const std::multimap<Int_t, Int_t>& mcMap,
      std::vector<std::vector<TH1F*> >& hist,
      Double_t par);

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

   /* */
   void CreateEffHistoElId(
         std::vector<TH1F*>& hist,
         const std::string& name,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin,
         const std::string& opt,
         TFile* file);

   /* Creates the histograms.
    *@param file if file==NULL then create histograms else read histograms from file*/
   void CreateHistos(
        TFile* file);

   /* Divides two histograms.
    * @param histo1 numerator
    * @param histo2 denominator
    * @param histo3 output histogram */
   void DivideHistos(
      TH1* histo1,
      TH1* histo2,
      TH1* histo3);

   /* Calculates efficiency histograms. */
   void CalculateEfficiencyHistos();

   /* Writes the histograms to output file. */
   void WriteToFile();

   /* Increase number of tracks counters. */
   void IncreaseCounters();

   /* Returns std::string of the detector used in the global reconstruction. */
   std::string RecDetector();

   /* Prints event-by-event statistics. */
   void PrintEventStatistics();

   /* Prints final statistics.
    * @param out Output stream */
   void PrintFinalStatistics(
      std::ostream& out);

   /* Calculates integrated efficiencies and forms string with statistic information.
    * @param hist vector with histograms
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   std::string EventEfficiencyStatisticsToString(
      const std::vector<std::vector<TH1F*> >& hist,
      const std::string& opt);

   /* Calculates integrated efficiencies and forms string with statistic information for the RICH detector.
    * @param hist vector with histograms
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   std::string EventEfficiencyStatisticsRichToString(
      const std::vector<std::vector<TH1F*> >& hist,
      const std::string& opt);

   /* Prints ghosts statistics to the out stream
    * @param out Output stream */
   void PrintGhostStatistics(
      std::ostream& out);

   /* Calculates integrated efficiencies for different polar angles
     * and returns a string with statistics.
    * @param hist Vector with histograms */
   std::string PolarAngleEfficiency(
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
   		const std::string& name4);

   /* Calculate efficiency for two histograms */
   Double_t CalcEfficiency(
      TH1* histRec,
      TH1* histAcc);

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
   // important for the MUCH detector. To avoid such situations on has to put
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

   Double_t fMinMom; // Minimum momentum for tracks for efficiency calculation
   Double_t fMaxMom; // Maximum momentum for tracks for efficiency calculation
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
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsMatches; // CbmStsTrackMatch array
   TClonesArray* fStsHits; // CbmStsHit array
   TClonesArray* fRichHits; // CbmRichHits array
   TClonesArray* fRichRings; // CbmRichRing array
   TClonesArray* fRichProjections; // CbmRichProjection array
   TClonesArray* fRichRingMatches; // CbmRichProjection array
   TClonesArray* fRichPoints; // CbmRichPoint array
   TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array
   TClonesArray* fMuchStrawHits; // CbmMuchStrawHits array
   TClonesArray* fMuchMatches; // CbmTrackMatch array
   TClonesArray* fTrdMatches; // CbmTrackMatch array
   TClonesArray* fTrdHits; // CbmTrdHit array
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
   // histograms for ghost tracks
   // STS: ghost tracks (number of hits dependence)
   TH1F* fhStsGhostNh;
   // TRD(MUCH): ghost tracks (number of hits dependence)
   TH1F* fhRecGhostNh;
   // RICH: ghost rings (number of hits dependence)
   TH1F* fhRichGhostNh;

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
   TH1F* fhNofMvdHits; // MVD hits
   TH1F* fhNofStsHits; // STS hits
   TH1F* fhNofRichHits; // RICH tracks
   TH1F* fhNofTrdHits; // TRD tracks
   TH1F* fhNofMuchPixelHits; // MUCH pixel hits
   TH1F* fhNofMuchStrawHits; // MUCH straw hits
   TH1F* fhNofTofHits; // TOF hits
   TH1F* fhStsChiprim; //chi2Vertex
   TH2F* fhStsMomresVsMom; // momentum resolution vs. momentum


   // Electron identification
   TH1F* fhPiAcc;
   std::vector<TH1F*> fhStsTrdMomElId; // STS+TRD: momentum dependence
   std::vector<TH1F*> fhStsTrdMomElIdNormStsTrdTof; // STS+TRD: momentum dependence, normalized to STS+TRD+TOF
   std::vector<TH1F*> fhStsTrdTofMomElId; // STS+TRD+TOF: momentum dependence

   TH1F* fhEventNo; // Event counter

   std::string fOutputDir; // Output directory for images

   CbmVertex *fPrimVertex;
   CbmStsKFTrackFitter* fKFFitter;

   CbmLitGlobalElectronId* fElectronId; // Electron identification tool

   ClassDef(CbmLitReconstructionQa, 1);
};

#endif /* CBMLITRECONSTRUCTIONQA_H_ */
