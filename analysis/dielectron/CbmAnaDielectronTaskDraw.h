/** CbmAnaDielectronTaskDraw.h
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#ifndef CBM_ANA_DIELECTRON_TASK_DRAW_H
#define CBM_ANA_DIELECTRON_TASK_DRAW_H

#include <string>
#include <map>

#include "TObject.h"

class TH1;
class TH2D;
class TH1D;
class TFile;
class TCanvas;

using namespace std;

class CbmAnaDielectronTaskDraw: public TObject {

public:

    CbmAnaDielectronTaskDraw(){;}
    
    virtual ~CbmAnaDielectronTaskDraw(){;}

    /**
     * Implement functionality of drawing histograms in the macro
     * from the specified file, this function should be called from macro.
     * \param fileName Name of the file
     * \param useMvd draw histograms related to the MVD detector?
     **/
    void DrawHistosFromFile(
          const string& fileName,
          Bool_t useMvd = true,
          Bool_t drawSig = true,
          Bool_t drawCumProb = true);

private:
    Bool_t fUseMvd; // do you want to draw histograms related to the MVD detector?
    Bool_t fDrawSignificance; // do you want to draw significance histograms of 1D cuts?
    Bool_t fDrawCumProb; // do you want to draw cumulative probability histograms of 1D cuts?

    string fFileName; // path to file with histograms
    TFile* fFile; // TFile with histograms
    map<string, TH1*> fHistoMap; // map which connects histogram's name and pointer to the histogram

    /**
     * \brief Read all histograms from file to map fHistoMap.
     */
    void ReadAllHistosToMap(
          TFile* file);

    /**
     * \brief Scale all histograms on the value 1./nofEvents.
     */
    void ScaleAllHistos(
          Int_t nofEvents);

    /**
     * \brief Rebin minv histograms for better drawing. Should be called after
     * calculation of S/BG ratios.
     */
    void RebinMinvHistos();

    /**
     * \brief Return TH1D* pointer to the specified histogram.
     * \param name Histogram name.
     */
    TH1D* H1(
          const string& name);

    /**
     * \brief Return TH2D* pointer to the specified histogram.
     * \param[in] name Histogram name
     */
    TH2D* H2(
          const string& name);

    /**
     * \brief Draw an integrated efficiency on a histogram (100.*h1->GetEntries()/h2->GetEntries()).
     * Histogram must be drawn in advance.
     * \param[in] h1 Pointer to the first histogram.
     * \param[in] h2 Pointer to the second histogram.
     * \param[in] xPos X position of the text in absolute coordinates.
     * \param[in] yPos Y position of the text in absolute coordinates.
     */
    void DrawEfficiencyOnHist(
          TH1* h1,
          TH1* h2,
          Double_t xPos,
          Double_t yPos);

    /**
     * Draw text on the histogram. Histogram must be drawn in advance.
     * \param text Text you want to draw
     * \param x1
     * \param y1
     * \param x2
     * \param y2
     */
    void DrawTextOnHist(
          const string& text,
          Double_t x1,
          Double_t y1,
          Double_t x2,
          Double_t y2);

    /**
     * Divide two 2D histograms and return 2D result histogram.
     * \param h1 Pointer to the first histogram
     * \param h2 Pointer to the second histogram
     */
    TH2D* DivideHisto2D(
          TH2D* h1,
          TH2D* h2);

    /**
     * Divide two 1D histograms and returns 1D result histogram.
     * \param h1 Pointer to the first histogram
     * \param h2 Pointer to the second histogram
     */
    TH1D* DivideHisto1D(
          TH1D* h1,
          TH1D* h2);

    /**
     * Produce 1D significance histogram Significance=S/sqrt(S+BG).
     * \param option Could be "forward" or "back"
     */
    TH1D* CreateSignificanceHisto1D(
          TH1D* signal,
          TH1D* bg,
          const string& name,
          const string& option);

    /**
     * Produce 2D significance histogram Significance=S/sqrt(S+BG).
     */
    TH2D* CreateSignificanceHisto2D(
          TH2D* signal,
          TH2D* bg,
          const string& name,
          const string& title);

    /**
     * Fit signal histogram using Fit("gaus").
     * Calculate S/BG ratio in 2 sigma region.
     * Print summary table of the efficiency, S/BG, sigma etc
     * for each step in cout.
     */
    void CalculateSignalOverBg(
          TH1D* s,
          TH1D* bg,
          TH2D* pty1,
          TH2D* pty2,
          const string& stepName);

    /**
     * Calculates S/BG ratio for each step of the analysis
     * using CalculateSignalOverBg method.
     */
    void SignalOverBgAll();

    /**
     * Draw Pt vs. Y distribution of signal for one step.
     * Print integrated efficiency using DrawEfficiencyOnHist method.
     */
    void DrawPtYDistribution(
          TH2D* h1,
          TH2D* h2,
          const string& text);

    /**
     * Draw Pt vs. Y distributions of signal for all steps
     * using DrawPtYDistribution method.
     */
    void DrawPtYDistributionAll();

    //pty efficiency of signal for one step
    void DrawPtYEfficiency(
          TH2D* h1,
          TH2D* h2,
          const string& text);

    /**
     * Draw efficiency in dependence on Pt and Rapidity of signal for all steps.
     */
    void DrawPtYEfficiencyAll();

    /**
     * Draw momentum distribution of signal for all steps.
     */
    void DrawMomentumDistributionAll();

    /**
     * Draw efficiency vs. momentum of pair for all steps.
     */
    void DrawMomentumEfficiencyAll();

    /**
     * Draw Mother PDG
     */
    void DrawMotherPdg();

    // Draw distribution and significance of 1D analysis cut
    void Draw1DCutsForSandBg(
          TH1D* s,
          TH1D* bg);

    // Draw significance of 1D analysis cut
     void Draw1DSignificance(
          TH1D* s,
          TH1D* bg,
          const string& sigHistoName,
          const string& sigHistoOption);

     void DrawCutsAndSignificance(
           const string& canvasName,
           const string& sName,
           const string& bgName,
           const string& significanceName,
           const string& sigOption);

    //Draw identification cuts distributions
    void DrawIdCutsAll();

    // Draw distributions and significances of 1D analysis cuts for all cuts
    void DrawAnalysisCutsAll();

    void DrawSourcesBgPairs(
          TH2D* h,
          const string& text);

    /**
     * Draw sources of BG pairs for all steps.
     */
    void DrawSourcesBgPairsAll();


    //Draw AP CUT distribution
    void DrawAPCut();

    //Draw APM cut distribution
    void DrawAPMCut();

    //Draw ST cut distribution (segment tracks)
    void DrawSTCut();

    //Draw TT cut distribution (full reco tracks)
    void DrawTTCut();

    void DrawGammaVertex();

    //Draw Dsts cut distribution for the first and the second MVD stations
    void DrawDstsCut();

    void Draw1DHistoAfterEachCut(
          TH1D* mc,
          TH1D* acc,
          TH1D* rec,
          TH1D* chi_prim,
          TH1D* elid,
          TH1D* gamma,
          TH1D* dsts,
          TH1D* dsts2,
          TH1D* st,
          TH1D* tt,
          TH1D* pt,
          TH1D* angle,
          TH1D* apm,
          Bool_t logy = false);

    //Draw Invariant mass distributions after each cut
    void DrawInvariantMassAfterEachCut();

    void DrawMinvBothSandBG(
          TH1* s,
          TH1* bg,
          const string& text);

    // Invariant mass distribution after each cut for BG and signal
    void DrawInvariantMassSandBG();

    // Invariant mass distribution after each cut for Pi0 and Etal
    void DrawInvariantMassPi0andEta();

    //SOURCE TRACKS
    void DrawBGSourceTracks();

    //MINV vs. PT
    void DrawMinvVsPt();

    void DrawBgSourcesVsMomentum();

    TH1D* CreateCumulativeProbabilityHisto(
          TH1D* histo,
          TString name,
          TString title);

    void DrawCumulativeProbabilities(
          TH1D* s,
          TH1D* bg);

    void DrawCumulativeProbabilitiesAll();

   ClassDef(CbmAnaDielectronTaskDraw, 1);
};

#endif
