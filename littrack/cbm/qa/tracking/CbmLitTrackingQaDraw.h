/**
 * \file CbmLitTrackingQaDraw.h
 * \brief Draw histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITTRACKINGQADRAW_H_
#define CBMLITTRACKINGQADRAW_H_

#include <string>

class CbmLitHistManager;
class TH1;
class TH1F;
class TH2F;
class TH3F;

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
    *
    */
   void Draw(
         CbmLitHistManager* histManager);

   /**
    *
    */
   void SetRebinForDraw(int rebin = 1) {
      fRebin = (rebin >= 1) ? rebin : 1;
   }

   bool fIsElectronSetup; // If "electron" setup detected than true
   bool fIsMvd; // If MVD detected than true
   bool fIsSts; // If STS detected than true
   bool fIsRich; // If RICH detected than true
   bool fIsTrd; // If TRD detected than true
   bool fIsMuch; // If MUCH detected than true
   bool fIsTof; // If TOF detected than true
   string fOutputDir; // Output directory for images

private:
   /**
    * \brief Draw efficiency histograms.
    */
   void DrawEfficiencyHistos();

   /**
    * \brief Draw efficiency plots. TODO ADD COMMENTS
    *
    * \param[in] opt Can be "pisupp" for pion suppression drawing
    * \param[in] projName Can be "p", "y", "pt".
    */
   void DrawEfficiency(
      const string& canvasName,
      const string& hist1,
      const string& hist2,
      const string& hist3,
      const string& hist4,
      const string& name1,
      const string& name2,
      const string& name3,
      const string& name4,
      const string& opt = "",
      const string& proj = "");

   /**
    * \brief Calculate efficiency for two histograms.
    */
   float CalcEfficiency(
      TH1* histRec,
      TH1* histAcc,
      const std::string& opt);

   /**
    * \brief Draw mean efficiency lines (up to 4) on the histogramm.
    */
   void DrawMeanEfficiencyLines(
      TH1* h,
      float eff1 = -1.,
      float eff2 = -1.,
      float eff3 = -1.,
      float eff4 = -1.);

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
   int fRebin;
};

#endif /* CBMLITTRACKINGQADRAW_H_ */
