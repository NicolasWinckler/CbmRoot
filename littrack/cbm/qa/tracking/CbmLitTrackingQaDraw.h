/**
 * \file CbmLitTrackingQaDraw.h
 * \brief Draw histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITTRACKINGQADRAW_H_
#define CBMLITTRACKINGQADRAW_H_

#include "cbm/base/CbmLitDetectorSetup.h"
#include <string>
#include <vector>

class CbmLitHistManager;
class TH1;
class TH1F;
class TH2F;
class TH3F;

using std::vector;
using std::string;

/**
 * \class CbmLitTrackingQaDraw
 * \brief Draw histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitTrackingQaDraw
{
public:

   /**
    * \brief Constructor.
    */
   CbmLitTrackingQaDraw();

   /**
    * \brief Destructor.
    */
   ~CbmLitTrackingQaDraw();

   /**
    * \brief Draw histograms.\
    * \param[in] histManager Pointer to histogram manager.
    * \param[in] outputDir Output directory for images. If not specified images are not saved.
    */
   void Draw(
         CbmLitHistManager* histManager,
         const std::string& outputDir = "");

   /**
    * \brief Set rebining factor for histograms.
    */
   void SetRebin(int rebin = 1) {
      fRebin = (rebin >= 1) ? rebin : 1;
   }

   void SetDetectorSetup(
         const CbmLitDetectorSetup& detectorSetup) {
      fDet = detectorSetup;
   }

private:
   /**
    * \brief Draw efficiency histograms.
    */
   void DrawEfficiencyHistos();

   /**
    * \brief Draw efficiency plots. TODO ADD COMMENTS
    * \param[in] canvasName Name of canvas.
    * \param[in] histNames Vector of histogram names.
    * \param[in] histLabels Vector of histogram labels.
    * \param[in] opt Additional options. "pisupp" for pion suppression drawing.
    * \param[in] projName Projection name: "p", "y", "pt".
    */
   void DrawEfficiency(
         const string& canvasName,
         const vector<string>& histNames,
         const vector<string>& histLabels,
         const string& opt = "",
         const string& proj = "");

   /**
    * \brief Calculate efficiency for two histograms.
    * TODO: Add comments
    */
   float CalcEfficiency(
      TH1* histRec,
      TH1* histAcc,
      const std::string& opt);

   /**
    * \brief Draw mean efficiency lines on histogramm.
    * \param[in] histos Vector of histograms.
    * \param[in] efficiencies Vector of efficiency numbers.
    */
   void DrawMeanEfficiencyLines(
      const vector<TH1*>& histos,
      const vector<Double_t>& efficiencies);

   /**
    * \brief Draw ACC and REC efficiency graphs normalized to MC.
    */
   void DrawMcEfficiencyGraph();

   /**
    * \brief Draw histograms for hits.
    */
   void DrawHitsHistos();

   /**
    * \brief Draw histograms for hits.
    * \param[in] canvasName Name of canvas.
    * \param[in] hist main name of hits histograms.
    */
   void DrawHitsHistos(
      const string& canvasName,
      const string& hist);

   /**
    * \brief Draw histograms of Sts tracks Qa.
    */
   void DrawStsTracksQaHistos();

   /**
    * \brief Draw MC momentum vs. angle histogram.
    */
   void DrawMCMomVsAngle();

   CbmLitHistManager* fHM; // histogram manager
   int fRebin; // Rebining factor for histograms
   CbmLitDetectorSetup fDet; // Detector setup
   string fOutputDir; // Output directory for images
};

#endif /* CBMLITTRACKINGQADRAW_H_ */
