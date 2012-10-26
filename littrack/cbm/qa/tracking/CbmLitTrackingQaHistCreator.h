/**
 * \file CbmLitTrackingQaHistCreator.h
 * \brief Create histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITTRACKINGQAHISTCREATOR_H_
#define CBMLITTRACKINGQAHISTCREATOR_H_

#include "base/CbmLitDetectorSetup.h"
#include "TObject.h"
#include <string>
#include <boost/function.hpp>
class CbmHistManager;
using std::string;
using std::vector;

typedef boost::function<Bool_t (TClonesArray* mcTracks, Int_t index)> LitTrackAcceptanceFunction;
typedef boost::function<Bool_t (TClonesArray* mcTracks, Int_t index, Int_t nofHitsInRing)> LitRingAcceptanceFunction;

/**
 * \class CbmLitTrackingQaHistCreator
 * \brief Create histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitTrackingQaHistCreator
{
public:

   /**
    * \brief Return pointer to singleton instance.
    * \return Pointer to singleton instance.
    */
   static CbmLitTrackingQaHistCreator* Instance();

   /**
    * \brief Create histograms for tracking QA.
    * \param[in] histManager CbmLitQaHistManager to be filled with created histograms.
    */
   void Create(
         CbmHistManager* histManager);

   /**
    * \brief Set properties of momentum axis in histograms.
    * \param[in] bins Number of bins.
    * \param[in] min Minimum momentum.
    * \param[in] max Maximum momentum.
    */
   void SetPRange(
         Int_t bins,
         Double_t min,
         Double_t max) {
      fPRangeBins = bins;
      fPRangeMin = min;
      fPRangeMax = max;
   }

   /**
    * \brief Set properties of Pt axis in histograms.
    * \param[in] min Minimum Pt.
    * \param[in] max Maximum Pt.
    * \param[in] nofBins Number of bins.
    */
   void SetPtRange(
         Int_t bins,
         Double_t min,
         Double_t max) {
      fPtRangeBins = bins;
      fPtRangeMin = min;
      fPtRangeMax = max;
   }

   /**
    * \brief Set properties of rapidity axis in histograms.
    * \param[in] min Minimum rapidity.
    * \param[in] max Maximum rapidity.
    * \param[in] nofBins Number of bins.
    */
   void SetYRange(
         Int_t bins,
         Double_t min,
         Double_t max) {
      fYRangeBins = bins;
      fYRangeMin = min;
      fYRangeMax = max;
   }

   /**
    * \brief Set properties of angle axis in histograms.
    * \param[in] min Minimum angle.
    * \param[in] max Maximum angle.
    * \param[in] nofBins Number of bins.
    */
   void SetAngleAxis(
         Double_t min,
         Double_t max,
         Int_t nofBins) {
      fMinAngle = min;
      fMaxAngle = max;
      fNofBinsAngle = nofBins;
   }

   static vector<string> GlobalTrackVariants();

   /* Getters */
   const vector<string>& GetTrackCategories() { return fTrackCategories; }
   const map<string, LitTrackAcceptanceFunction>& GetTrackAcceptanceFunctions() { return fTrackAcceptanceFunctions; }
   const vector<string>& GetRingCategories() { return fRingCategories; }
   const map<string, LitRingAcceptanceFunction>& GetRingAcceptanceFunctions() { return fRingAcceptanceFunctions; }

private:

   /**
    * \brief Constructor.
    */
   CbmLitTrackingQaHistCreator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQaHistCreator();

   void CreateH1Efficiency(
         const string& name,
         const string& parameter,
         const string& xTitle,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin,
         const string& opt);

   void CreateH2Efficiency(
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
         const string& opt);

   void CreateH1(
         const string& name,
         const string& xTitle,
         const string& yTitle,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin);

   void CreateH2(
         const string& name,
         const string& xTitle,
         const string& yTitle,
         const string& zTitle,
         Int_t nofBinsX,
         Double_t minBinX,
         Double_t maxBinX,
         Int_t nofBinsY,
         Double_t minBinY,
         Double_t maxBinY);

   void CreateTrackHitsHistogram(
   		const string& detName);

   vector<string> CreateGlobalTrackingHistogramNames(
   		const vector<string>& detectors);

   vector<string> CreateGlobalTrackingHistogramNames();

   vector<string> GetDefaultTrackCategories();
   map<string, LitTrackAcceptanceFunction> GetDefaultTrackAcceptanceFunctions();
   vector<string> GetDefaultRingCategories();
   map<string, LitRingAcceptanceFunction> GetDefaultRingAcceptanceFunctions();

   string LocalEfficiencyNormalization(
   		const string& detName);

private:

   Double_t fPRangeMin; // Minimum momentum for tracks for efficiency calculation [GeV/c]
   Double_t fPRangeMax; // Maximum momentum for tracks for efficiency calculation [GeV/c]
   Int_t fPRangeBins; // Number of bins for efficiency vs. momentum histogram
   Double_t fPtRangeMin; // Minimum Pt for tracks for efficiency calculation [GeV/c]
   Double_t fPtRangeMax; // Maximum Pt for tracks for efficiency calculation [GeV/c]
   Int_t fPtRangeBins; // Number of bins for efficiency vs. Pt histogram
   Double_t fYRangeMin; // Minimum rapidity for tracks for efficiency calculation [GeV/c]
   Double_t fYRangeMax; // Maximum rapidity for tracks for efficiency calculation [GeV/c]
   Int_t fYRangeBins; // Number of bins for efficiency vs. rapidity histogram
   Double_t fMinAngle; // Minimum polar angle [grad]
   Double_t fMaxAngle; // Maximum polar angle [grad]
   Int_t fNofBinsAngle; // Number of bins for efficiency vs. polar angle histogram

   CbmHistManager* fHM; // Histogram manager for created histograms

   vector<string> fTrackCategories; // Vector of track category names
   vector<string> fRingCategories; // Vector of ring category names
   map<string, LitTrackAcceptanceFunction> fTrackAcceptanceFunctions; // maps track category name to track acceptance function
   map<string, LitRingAcceptanceFunction> fRingAcceptanceFunctions; // maps ring category name to ring acceptance function

   CbmLitDetectorSetup fDet;
};

#endif /* CBMLITTRACKINGQAHISTCREATOR_H_ */
