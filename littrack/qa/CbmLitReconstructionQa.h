/** CbmLitReconstructionQa.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **
 ** CBM task class for global track reconstruction quality.
 ** Produces efficiency histograms. Calculates different statistics.
 **/

#ifndef CBMLITRECONSTRUCTIONQA_H_
#define CBMLITRECONSTRUCTIONQA_H_

#include "FairTask.h"
#include "CbmDetectorList.h"

#include <map>
#include <string>
#include <vector>

class CbmTrackMatch;
class CbmMCTrack;
class TClonesArray;
class TH1;
class TH1F;
class TList;
class TCanvas;

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

private:
   /* Derived from FairTask. Executed after all events are processed. */
   virtual void Finish();

   /* Determines the CBM detector setup, based on TGeoManager stored in the input MC file. */
   void DetermineSetup();

   /* Reads necessary data branches from the input data files. */
   void ReadDataBranches();

   /* Fills hit related histograms */
   void ProcessHits();

   /* Loops over the reconstructed global tracks. Checks if the track is correct
    * than fills the multimaps <MC track index, reconstructed track index>. */
   void ProcessGlobalTracks();

   /* Checks correctness of MVD attached hits*/
   void ProcessMvd(
      Int_t stsId);

   /* Checks track quality based on fQuota value.
     * @param trackMatch Track match */
   Bool_t CheckTrackQuality(
      CbmTrackMatch* trackMatch,
      DetectorId detId);

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

   /* Creates the histograms. */
   void CreateHistos();

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


   /* Calculates integrated efficiencies for different polar angles
     * and returns a string with statistics.
    * @param hist Vector with histograms */
   std::string PolarAngleEfficiency(
      const std::vector<std::vector<TH1F*> >& hist);

   /* Draws histograms. */
   void Draw();

   /* Draws efficiency histograms. */
   void DrawEfficiencyHistos();

   /* Draws Global tracking efficiency */
   void DrawGlobalEfficiency(
   		TCanvas* canvas,
   		const std::vector<std::vector<TH1F*> >& hist1,
   		const std::vector<std::vector<TH1F*> >& hist2,
   		const std::vector<std::vector<TH1F*> >& hist3,
   		const std::string& name1,
   		const std::string& name2,
   		const std::string& name3,
   		Int_t category);

   /* Draw efficiency for global track segments STS, TRD, MUCH, TOF */
   void DrawSegmentEfficiency(
   		TCanvas* canvas,
   		const std::vector<std::vector<TH1F*> >& hist,
   		const std::string& detName,
   		const std::string& signal,
   		Int_t category);

   /* Calculate efficiency for two histograms */
   Double_t CalcEfficiency(
      TH1* histRec,
      TH1* histAcc);

   /* Draws histograms for hits */
   void DrawHitsHistos();

   /* Draws histograms for hits
    * @param c Pointer to canvas.
    * @param histos Vector with hits histograms. */
   void DrawHitsHistos(
      TCanvas* c,
      std::vector<TH1F*>& histos);

   /* Draws histograms of number of hits in station */
   void DrawHitsStationHistos();

   Int_t fMinNofPointsSts; // Minimal number of MCPoints in STS
   Int_t fMinNofPointsTrd; // Minimal number of MCPoints in TRD
   Int_t fMinNofPointsMuch; // Minimal number of MCPoints in MUCH
   Int_t fMinNofPointsTof; // Minimal number of MCPoints in TOF
   Double_t fQuota;  // True/all hits for track to be considered correctly reconstructed

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
   Bool_t fIsTrd; // If TRD detected than true
   Bool_t fIsMuch; // If MUCH detected than true
   Bool_t fIsTof; // If TOF detected than true

   Double_t fRefMomentum; // Momentum cut for reference tracks
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

   // Pointers to data arrays
   TClonesArray* fMCTracks; // CbmMCTrack array
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array
   TClonesArray* fMvdHits; // CbmMvdHit array
   TClonesArray* fMvdPoints; // CbmMvdPoint array
   TClonesArray* fMvdHitMatches; // CbmMvdHitMatch array
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsMatches; // CbmStsTrackMatch array
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

   TH1F* fhMvdNofHitsInStation; // mean number of hits in station in MVD
   TH1F* fhTrdNofHitsInStation; // mean number of hits in station in TRD
   TH1F* fhMuchNofHitsInStation; // mean number of hits in station in MUCH
   TH1F* fhTofNofHitsInStation; // mean number of hits in station in TOF

   // histograms of number of hits in track
   // all, true, fake, true/all, fake/all
   std::vector<TH1F*> fhMvdTrackHits;
   std::vector<TH1F*> fhStsTrackHits;
   std::vector<TH1F*> fhTrdTrackHits;
   std::vector<TH1F*> fhMuchTrackHits;

   TList* fHistoList; // List of histograms

   // Total number of tracks/hits counters
   Int_t fNofGlobalTracks; // global tracks
   Int_t fNofStsTracks; // STS tracks
   Int_t fNofTrdTracks; // TRD tracks
   Int_t fNofMuchTracks; // MUCH tracks
   Int_t fNofTofHits; // TOF hits

   Int_t fEventNo; // Event counter

   std::string fOutputDir; // Output directory for images

   ClassDef(CbmLitReconstructionQa, 1);
};

#endif /* CBMLITRECONSTRUCTIONQA_H_ */
