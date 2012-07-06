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

class CbmHistManager;
class TH1;

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
         CbmHistManager* histManager,
         const string& outputDir = "");

//   /**
//    * \brief Set rebining factor for histograms.
//    */
//   void SetRebin(Int_t rebin = 1) {
//      fRebin = (rebin >= 1) ? rebin : 1;
//   }

private:
   /**
    * \brief Draw efficiency histograms.
    */
   void DrawEfficiencyHistos();

   /**
    * \brief Calculate efficiency for two histograms.
    * TODO: Add comments
    */
   Double_t CalcEfficiency(
      TH1* histRec,
      TH1* histAcc,
      Double_t scale = 1.);

   /**
    * \brief Draw mean efficiency lines on histogramm.
    * \param[in] histos Vector of histograms.
    * \param[in] efficiencies Vector of efficiency numbers.
    */
   void DrawMeanEfficiencyLines(
      const vector<TH1*>& histos,
      const vector<Double_t>& efficiencies);

//   /**
//    * \brief Draw ACC and REC efficiency graphs normalized to MC.
//    */
//   void DrawMcEfficiencyGraph();

   /**
    * \brief Draw histograms for hits.
    */
   void DrawHitsHistos();

   void DrawEfficiency(
         const string& canvasName,
         const string& histNamePattern);

   /**
    * \brief Draw histograms for hits. This function automatically
    * check the existence of histograms.
    * \param[in] canvasName Name of canvas.
    * \param[in] hist main name of hits histograms.
    */
   void DrawHitsHistos(
      const string& canvasName,
      const string& hist);

   void DrawAccAndRec(
         const string& canvasName,
         const string& histNamePattern);

//   /**
//    * \brief Draw histograms of Sts tracks Qa.
//    */
//   void DrawStsTracksQaHistos();
//
//   /**
//    * \brief Draw MC momentum vs. angle histogram.
//    */
//   void DrawMCMomVsAngle();

   CbmHistManager* fHM; // histogram manager
//   Int_t fRebin; // Rebining factor for histograms
   string fOutputDir; // Output directory for images
};

#endif /* CBMLITTRACKINGQADRAW_H_ */
