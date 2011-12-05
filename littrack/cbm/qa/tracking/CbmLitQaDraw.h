/*
 * CbmLitQaDraw.h
 *
 *  Created on: 17.10.2011
 *      Author: slebedev
 */

#ifndef CBMLITQADRAW_H_
#define CBMLITQADRAW_H_

#include "TObject.h"

#include <string>

class CbmLitQaHistManager;
class TH1;
class TH1F;
class TH2F;
class TH3F;

using namespace std;

class CbmLitQaDraw
{
public:

   /**
    *
    */
   CbmLitQaDraw(CbmLitQaHistManager* hm){fHM = hm; fRebin = 1;}

   /**
    *
    */
   ~CbmLitQaDraw(){;}

   /**
    *
    */
   void Draw();

   /**
    *
    */
   void SetRebinForDraw(Int_t rebin = 1) {if (rebin >=1) fRebin = rebin; else fRebin = 1;}

   Bool_t fIsElectronSetup; // If "electron" setup detected than true
   Bool_t fIsMvd; // If MVD detected than true
   Bool_t fIsSts; // If STS detected than true
   Bool_t fIsRich; // If RICH detected than true
   Bool_t fIsTrd; // If TRD detected than true
   Bool_t fIsMuch; // If MUCH detected than true
   Bool_t fIsTof; // If TOF detected than true
   string fOutputDir; // Output directory for images

private:
   CbmLitQaHistManager* fHM; // histogram manager
   Int_t fRebin;

   /**
    *
    */
   TH1F* H1(
      const string& name);

   /**
    *
    */
   TH2F* H2(
      const string& name);

   /**
    *
    */
   TH3F* H3(
      const string& name);

   /**
    *
    */
   TH1* H(
      const string& name);

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
      const string& opt = "");

   /**
    * \brief Calculate efficiency for two histograms.
    */
   Double_t CalcEfficiency(
      TH1* histRec,
      TH1* histAcc,
      const std::string& opt);

   /**
    * \brief Draw mean efficiency lines (up to 4) on the histogramm.
    */
   void DrawMeanEfficiencyLines(
      TH1* h,
      Double_t eff1 = -1.,
      Double_t eff2 = -1.,
      Double_t eff3 = -1.,
      Double_t eff4 = -1.);

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
    * \brief Draw histogram of number of hits in station.
    * \param[in] name Canvas name.
    * \param[in] hist Pointer to histogram.
    */
   void DrawHitsStationHisto(
      const string& name,
      TH1F* hist);

   /**
    * \brief Draw histograms of number of hits in station.
    */
   void DrawHitsStationHistos();

   /**
    * \brief Draw histograms of Sts tracks Qa.
    */
   void DrawStsTracksQaHistos();

   /**
    * \brief Draw MC momentum vs. angle histogram.
    */
   void DrawMCMomVsAngle();
};

#endif /* CBMLITQADRAW_H_ */
