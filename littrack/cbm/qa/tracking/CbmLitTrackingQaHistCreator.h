/**
 * \file CbmLitTrackingQaHistCreator.h
 * \brief Create histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITTRACKINGQAHISTCREATOR_H_
#define CBMLITTRACKINGQAHISTCREATOR_H_

#include "qa/tracking/CbmLitTrackingQaHistNames.h"
#include <string>
class CbmLitQaHistManager;
class TFile;
using std::string;

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
    * \brief Constructor.
    */
   CbmLitTrackingQaHistCreator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQaHistCreator();

   /**
    * \brief Read histograms from file and return CbmLitQaHistManager.
    * \param[in] file Pointer to file with histograms.
    * \return CbmLitQaHistManager for histograms in specified file.
    */
   CbmLitQaHistManager* ReadFromFile(
         TFile* file);

   /**
    * \brief Create histograms for tracking QA.
    * \return CbmLitQaHistManager for created histograms.
    */
   CbmLitQaHistManager* Create();

   /**
    * \brief Set properties of momentum axis in histograms.
    * \param[in] min Minimum momentum.
    * \param[in] max Maximum momentum.
    * \param[in] nofBins Number of bins.
    */
   void SetMomAxis(
         float min,
         float max,
         int nofBins) {
      fMinMom = min;
      fMaxMom = max;
      fNofBinsMom = nofBins;
   }

   /**
    * \brief Set properties of Pt axis in histograms.
    * \param[in] min Minimum Pt.
    * \param[in] max Maximum Pt.
    * \param[in] nofBins Number of bins.
    */
   void SetPtAxis(
         float min,
         float max,
         int nofBins) {
      fMinPt = min;
      fMaxPt = max;
      fNofBinsPt = nofBins;
   }

   /**
    * \brief Set properties of rapidity axis in histograms.
    * \param[in] min Minimum rapidity.
    * \param[in] max Maximum rapidity.
    * \param[in] nofBins Number of bins.
    */
   void SetRapidityAxis(
         float min,
         float max,
         int nofBins) {
      fMinY = min;
      fMaxY = max;
      fNofBinsY = nofBins;
   }

   /**
    * \brief Set properties of angle axis in histograms.
    * \param[in] min Minimum angle.
    * \param[in] max Maximum angle.
    * \param[in] nofBins Number of bins.
    */
   void SetAngleAxis(
         float min,
         float max,
         int nofBins) {
      fMinAngle = min;
      fMaxAngle = max;
      fNofBinsAngle = nofBins;
   }

private:

   void CreateEffHist(
         const string& name,
         const string& xTitle,
         int nofBins,
         float minBin,
         float maxBin,
         LitQaNameOption opt);

   void CreateEffHist3D(
         const string& name,
         LitQaNameOption opt);

   void Create1DHist(
         const string& name,
         const string& xTitle,
         const string& yTitle,
         int nofBins,
         float minBin,
         float maxBin);

   void Create2DHist(
         const string& name,
         const string& xTitle,
         const string& yTitle,
         const string& zTitle,
         int nofBinsX,
         float minBinX,
         float maxBinX,
         int nofBinsY,
         float minBinY,
         float maxBinY);

private:
   float fMinMom; // Minimum momentum for tracks for efficiency calculation [GeV/c]
   float fMaxMom; // Maximum momentum for tracks for efficiency calculation [GeV/c]
   int fNofBinsMom; // Number of bins for efficiency vs. momentum histogram
   float fMinPt; // Minimum Pt for tracks for efficiency calculation [GeV/c]
   float fMaxPt; // Maximum Pt for tracks for efficiency calculation [GeV/c]
   int fNofBinsPt; // Number of bins for efficiency vs. Pt histogram
   float fMinY; // Minimum rapidity for tracks for efficiency calculation [GeV/c]
   float fMaxY; // Maximum rapidity for tracks for efficiency calculation [GeV/c]
   int fNofBinsY; // Number of bins for efficiency vs. rapidity histogram
   float fMinAngle; // Minimum polar angle [grad]
   float fMaxAngle; // Maximum polar angle [grad]
   int fNofBinsAngle; // Number of bins for efficiency vs. polar angle histogram

   CbmLitQaHistManager* fHM; // Histogram manager for created histograms
};

#endif /* CBMLITQAHISTCREATOR_H_ */
