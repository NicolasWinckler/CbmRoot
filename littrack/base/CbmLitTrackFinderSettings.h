/** CbmLitTrackFinderSettings.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 *
 * Class stores settings for track finder algorithms.
 **/
#ifndef CBMLITTRACKFINDERSETTINGS_H_
#define CBMLITTRACKFINDERSETTINGS_H_

#include <string>

class CbmLitTrackFinderSettings
{
public:
   /* Constructor */
   CbmLitTrackFinderSettings():
      fNofIter(0),
      fPropagator(),
      fSeedSelection(),
      fFinalSelection(),
      fFilter(),
      fIsUseFastSearch(),
      fPDG(),
      fIsProcessSubstationsTogether(),
      fMaxNofMissingHits(),
      fSigmaCoef(),
      fChiSqPixelHitCut(),
      fChiSqStripHitCut(),
      fStationGroupSelection(),
      fIsAlwaysCreateMissingHit() {};

   /* Destructor */
   virtual ~CbmLitTrackFinderSettings() {};

   /* Sets number of iterations */
   void SetNofIter(unsigned int nofIter) {
      fNofIter = nofIter;
      fPropagator.resize(fNofIter);
      fSeedSelection.resize(fNofIter);
      fFinalSelection.resize(fNofIter);
      fFilter.resize(fNofIter);
      fIsUseFastSearch.resize(fNofIter);
      fPDG.resize(fNofIter);
      fIsProcessSubstationsTogether.resize(fNofIter);
      fMaxNofMissingHits.resize(fNofIter);
      fSigmaCoef.resize(fNofIter);
      fChiSqPixelHitCut.resize(fNofIter);
      fChiSqStripHitCut.resize(fNofIter);
      fStationGroupSelection.resize(fNofIter);
      fIsAlwaysCreateMissingHit.resize(fNofIter);
   }

   /* Sets propagator for iteration iter */
   void SetPropagator(unsigned int iter, const TrackPropagatorPtr& propagator) {
      fPropagator[iter] = propagator;
   }

   /* Sets the same propagator for all iterations */
   void SetPropagator(const TrackPropagatorPtr& propagator) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetPropagator(i, propagator); }
   }

   /* Sets seed selection for iteration iter */
   void SetSeedSelection(unsigned int iter, const TrackSelectionPtr& selection) {
      fSeedSelection[iter] = selection;
   }

   /* Sets the same seed selection for all iterations */
   void SetSeedSelection(const TrackSelectionPtr& selection) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetSeedSelection(i, selection); }
   }

   /* Sets final selection for iteration iter */
   void SetFinalSelection(unsigned int iter, const TrackSelectionPtr& selection) {
      fFinalSelection[iter] = selection;
   }

   /* Sets the same final selection for all iterations */
   void SetFinalSelection(const TrackSelectionPtr& selection) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetFinalSelection(i, selection); }
   }

   /* Sets track update for iteration iter */
   void SetFilter(unsigned int iter, const TrackUpdatePtr& filter) {
      fFilter[iter] = filter;
   }

   /* Sets the same track update for all iterations */
   void SetFilter(const TrackUpdatePtr& filter) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetFilter(i, filter); }
   }

   /* Sets use fast search for iteration iter */
   void IsUseFastSearch(unsigned int iter, bool isUseFastSearch) {
      fIsUseFastSearch[iter] = isUseFastSearch;
   }

   /* Sets the same use fast search for all iterations */
   void IsUseFastSearch(bool isUseFastSearch) {
      for (unsigned int i = 0; i < fNofIter; i++) { IsUseFastSearch(i, isUseFastSearch); }
   }

   /* Sets PDG for iteration iter */
   void SetPDG(unsigned int iter, int pdg) {
      fPDG[iter] = pdg;
   }

   /* Sets the same PDG for all iterations */
   void SetPDG(int pdg) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetPDG(i, pdg); }
   }

   /* Sets process substations together for iteration iter */
   void IsProcessSubstationsTogether(unsigned int iter, bool isProcessSubstationsTogether) {
      fIsProcessSubstationsTogether[iter] = isProcessSubstationsTogether;
   }

   /* Sets the same process substations together for all iterations */
   void IsProcessSubstationsTogether(bool isProcessSubstationsTogether) {
      for (unsigned int i = 0; i < fNofIter; i++) { IsProcessSubstationsTogether(i, isProcessSubstationsTogether); }
   }

   /* Sets maximum number of missing hits for iteration iter */
   void SetMaxNofMissingHits(unsigned int iter, int maxNofMissingHits) {
      fMaxNofMissingHits[iter] = maxNofMissingHits;
   }

   /* Sets the same maximum number of missing hits for all iterations */
   void SetMaxNofMissingHits(int maxNofMissingHits) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetMaxNofMissingHits(i, maxNofMissingHits); }
   }

   /* Sets sigma coefficient for iteration iter */
   void SetSigmaCoef(unsigned int iter, myf sigmaCoef) {
      fSigmaCoef[iter] = sigmaCoef;
   }

   /* Sets the same sigma coefficient for all iterations */
   void SetSigmaCoef(myf sigmaCoef) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetSigmaCoef(i, sigmaCoef); }
   }

   /* Sets chi square pixel hit cut for iteration iter */
   void SetChiSqPixelHitCut(unsigned int iter, myf chiSqPixelHitCut) {
      fChiSqPixelHitCut[iter] = chiSqPixelHitCut;
   }

   /* Sets the same chi square pixel hit cut for all iterations */
   void SetChiSqPixelHitCut(myf chiSqPixelHitCut) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetChiSqPixelHitCut(i, chiSqPixelHitCut); }
   }

   /* Sets chi square strip hit cut for iteration iter */
   void SetChiSqStripHitCut(unsigned int iter, myf chiSqStripHitCut) {
      fChiSqStripHitCut[iter] = chiSqStripHitCut;
   }

   /* Sets the same chi square strip hit cut for all iterations */
   void SetChiSqStripHitCut(myf chiSqStripHitCut) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetChiSqStripHitCut(i, chiSqStripHitCut); }
   }

   /* Sets station group selection for iteration iter */
   void SetStationGroupSelection(unsigned int iter, const TrackSelectionPtr& selection) {
      fStationGroupSelection[iter] = selection;
   }

   /* Sets the same station group selection for all iterations */
   void SetStationGroupSelection(const TrackSelectionPtr& selection) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetStationGroupSelection(i, selection); }
   }

   /* Sets always create missing hit for iteration iter */
   void IsAlwaysCreateMissingHit(unsigned int iter, bool isAlwaysCreateMissingHit) {
      fIsAlwaysCreateMissingHit[iter] = isAlwaysCreateMissingHit;
   }

   /* Sets the same always create missing hit for all iterations */
   void  IsAlwaysCreateMissingHit(bool isAlwaysCreateMissingHit) {
      for (unsigned int i = 0; i < fNofIter; i++) { IsAlwaysCreateMissingHit(i, isAlwaysCreateMissingHit); }
   }

   /* Sets track fitter for iteration iter */
   void SetFitter(unsigned int iter, const TrackFitterPtr& fitter) {
      fFitter[iter] = fitter;
   }

   /* Sets the same fitter for all iterations */
   void SetFitter(const TrackFitterPtr& fitter) {
      for (unsigned int i = 0; i < fNofIter; i++) { SetFitter(i, fitter); }
   }


   /* Returns number of iterations */
   unsigned int GetNofIter() const {
      return fNofIter;
   }

   /* Returns propagator for iteration iter */
   const TrackPropagatorPtr& GetPropagator(unsigned int iter) const {
      return fPropagator[iter];
   }

   /* Returns seed selection for iteration iter */
   const TrackSelectionPtr& GetSeedSelection(unsigned int iter) const {
      return fSeedSelection[iter];
   }

   /* Returns final selection for iteration iter */
   const TrackSelectionPtr& GetFinalSelection(unsigned int iter) const {
      return fFinalSelection[iter];
   }

   /* Returns track update for iteration iter */
   const TrackUpdatePtr& GetFilter(unsigned int iter) const {
      return fFilter[iter];
   }

   /* Returns use fast search for iteration iter */
   bool GetIsUseFastSearch(unsigned int iter) const {
      return fIsUseFastSearch[iter];
   }

   /* Returns PDG for iteration iter */
   int GetPDG(unsigned int iter) const {
      return fPDG[iter];
   }

   /* Returns process substations together for iteration iter */
   bool GetIsProcessSubstationsTogether(unsigned int iter) const {
      return fIsProcessSubstationsTogether[iter];
   }

   /* Returns maximum number of missing hits for iteration iter */
   int GetMaxNofMissingHits(unsigned int iter) const {
      return fMaxNofMissingHits[iter];
   }

   /* Returns sigma coefficient for iteration iter */
   myf GetSigmaCoef(unsigned int iter) const {
      return fSigmaCoef[iter];
   }

   /* Returns chi square pixel hit cut for iteration iter */
   myf GetChiSqPixelHitCut(unsigned int iter) const {
      return fChiSqPixelHitCut[iter];
   }

   /* Returns chi square strip hit cut for iteration iter */
   myf GetChiSqStripHitCut(unsigned int iter) const {
      return fChiSqStripHitCut[iter];
   }

   /* Returns station group selection for iteration iter */
   const TrackSelectionPtr& GetStationGroupSelection(unsigned int iter) const {
      return fStationGroupSelection[iter];
   }

   /* Returns always create missing hit for iteration iter */
   bool GetIsAlwaysCreateMissingHit(unsigned int iter) const {
      return fIsAlwaysCreateMissingHit[iter];
   }

   /* Returns track fitter for iteration iter */
   const TrackFitterPtr& GetFitter(unsigned int iter) {
      return fFitter[iter];
   }

private:
   /* General settings for all track finder algorithms */

   /* */
   unsigned int fNofIter;
   /* Track propagator tool name */
   std::vector<TrackPropagatorPtr> fPropagator;
   /* Track seed selection tool name */
   std::vector<TrackSelectionPtr> fSeedSelection;
   /* */
   std::vector<TrackSelectionPtr> fFinalSelection;
   /* */
   std::vector<TrackUpdatePtr> fFilter;
   /* */
   std::vector<bool> fIsUseFastSearch;
   /* */
   std::vector<int> fPDG;
   /* */
   std::vector<bool> fIsProcessSubstationsTogether;
   /* */
   std::vector<int> fMaxNofMissingHits;
   /* */
   std::vector<myf> fSigmaCoef;
   /* */
   std::vector<myf> fChiSqPixelHitCut;
   /* */
   std::vector<myf> fChiSqStripHitCut;


   /* Specific settings for branching tracking */

   /* Track selection for station group */
   std::vector<TrackSelectionPtr> fStationGroupSelection;
   /* */
   std::vector<bool> fIsAlwaysCreateMissingHit;


   /* Specific settings for weight tracking */

   /* track fitter tool */
   std::vector<TrackFitterPtr> fFitter;
};

#endif /* CBMLITTRACKFINDERSETTINGS_H_ */
