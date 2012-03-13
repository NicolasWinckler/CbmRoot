/** CbmAnaDielectronTaskDrawAll.h
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#ifndef CBM_ANA_DIELECTRON_TASK_DRAW_ALL_H
#define CBM_ANA_DIELECTRON_TASK_DRAW_ALL_H

#include <string>
#include <map>

#include "TObject.h"

class TH1;
class TH2D;
class TH1D;
class TFile;
class TCanvas;

using namespace std;

class CbmAnaDielectronTaskDrawAll: public TObject {

public:

   /**
   * \brief Default constructor.
   */
   CbmAnaDielectronTaskDrawAll(){;}

   /**
    * \brief Destructor.
    */
   virtual ~CbmAnaDielectronTaskDrawAll(){;}

   /**
   * \brief Implement functionality of drawing histograms in the macro
   * from the specified files, this function should be called from macro.
   * \param[in] fileNameRho0 Name of the file with rho0 histograms.
   * \param[in] fileNameOmega Name of the file with omega histograms.
   * \param[in] fileNamePhi Name of the file with phi histograms.
   * \param[in] fileNameOmegaDalitz Name of the file with omegaDalitz histograms.
   * \param useMvd draw histograms related to the MVD detector?
   **/
   void DrawHistosFromFile(
         const string& fileNameRho0,
         const string& fileNameOmega,
         const string& fileNamePhi,
         const string& fileNameOmegaDalitz,
         Bool_t useMvd = false);

private:
   static const int fNofCuts = 7;
   static const int fNofSignals = 4;

   Bool_t fUseMvd; // do you want to draw histograms related to the MVD detector?

   //[0]=rho0, [1]=omega, [2]=phi, [3]=omegaDalitz
   vector<string> fFileNames; // path to files with histograms
   vector<TFile*> fFile; // TFile with histograms
   vector<string> fNames; // Names of the signals
   vector<string> fCutNames; // Names of the cuts

   // first index: [0]=rho0, [1]=omega, [2]=phi, [3]=omegaDalitz
   // second index: [0]=signal, [1]=background, [2]=eta, [3]=pi0
   // third index: [0]=el_id, [1]=gammacut, [2]=dstscut, [3]=dsts2cut, [4]=stcut, [5]=ttcut, [6]=ptcut
   vector<vector<vector<TH1D*> > > fh_minv; // minv histograms after each cut (third index)

   // first index: [0]=signal(only for convenience), [1]=background, [2]=eta, [3]=pi0
   // second index: [0]=el_id, [1]=gammacut, [2]=dstscut, [3]=dsts2cut, [4]=stcut, [5]=ttcut, [6]=ptcut
   vector<vector<TH1D*> > fh_mean_minv;//mean histograms from 4 files

   // index: [0]=el_id, [1]=gammacut, [2]=dstscut, [3]=dsts2cut, [4]=stcut, [5]=ttcut, [6]=ptcut
   vector<TH1D*> fh_sum_signals_minv; // sum of all signals

   // first index: [0]=rho0, [1]=omega, [2]=phi, [3]=omegaDalitz
   // second index: [0]=el_id, [1]=gammacut, [2]=dstscut, [3]=dsts2cut, [4]=stcut, [5]=ttcut, [6]=ptcut
   vector<vector<TH1D*> > fh_pty; // pty distribution of signal for efficiency calculation

   // first index: [0]=rho0, [1]=omega, [2]=phi, [3]=omegaDalitz
   vector<TH1D*> fh_mc_pty; // MC pty distribution of signal for efficiency calculation

   /**
    * \brief Scale all histograms from the file to one event.
    * \param[in] file Pointer to TFile.
    * \param[in] nofEvents Number of events.
    */
   void ScaleAllHistogramms(
         TFile* file,
         Int_t nofEvents);

   /**
    * \brief Draw histograms and plots.
    */
   void Draw();

   /**
    * \brief Draw invariant mass spectra for all signals for specified cut.
    * \param[in] cutType [0]=el_id, [1]=gammacut, [2]=dstscut, [3]=dsts2cut, [4]=stcut, [5]=ttcut, [6]=ptcut
    */
   void DrawMinv(
         int cutType);

   /**
    * \brief It creates a mean histogram from 4 files.
    */
   void FillMeanHist();

   /**
    * \brief Fill sum signals.
    */
   void FillSumSignalsHist();

   /**
    * \brief Calculate cut efficiency in specified invariant mass region.
    * \param[in] min Minimum invariant mass.
    * \param[in] max Maximum invariant mass.
    */
   void CalcCutEff(
         Double_t min,
         Double_t max);

   /**
    * \brief Create S/BG vs cuts for specified invariant mass region.
    * \param[in] min Minimum invariant mass.
    * \param[in] max Maximum invariant mass.
    */
   TH1D* CreateSBGRegionHist(
         Double_t min,
         Double_t max);

   /**
    * \brief Draw S/BG vs plots for different mass regions.
    */
   void DrawSBGRegion();

   /**
    * \brief Draw S/BG vs plots for different signals.
    */
   void DrawSBGSignals();

   ClassDef(CbmAnaDielectronTaskDrawAll, 1);
};

#endif
