/** CbmAnaDielectronTaskDrawAll.h
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 2.0
 **/

#ifndef CBM_ANA_DIELECTRON_TASK_DRAW_ALL
#define CBM_ANA_DIELECTRON_TASK_DRAW_ALL

#include <vector>
#include <string>
#include <map>

#include "CbmAnaLmvmNames.h"

#include "TObject.h"

class TH1;
class TH2D;
class TH1D;
class TFile;
class TCanvas;
class CbmHistManager;

using namespace std;

enum SignalType {
   kRho0 = 0,
   kOmega = 1,
   kPhi = 2,
   kOmegaD = 3
};

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
   static const int fNofSignals = 4;

   vector<TCanvas*> fCanvas; // store all pointers to TCanvas -> save to images

   Bool_t fUseMvd; // do you want to draw histograms related to the MVD detector?

   //[0]=rho0, [1]=omega, [2]=phi, [3]=omegaDalitz
   vector<CbmHistManager*> fHM;

   vector<string> fNames; // Names of the signals

   // index: AnalysisSteps
   vector<TH1D*> fh_mean_bg_minv; //mean histograms from 4 files
   vector<TH1D*> fh_mean_eta_minv;
   vector<TH1D*> fh_mean_pi0_minv;

   // index: AnalysisSteps
   vector<TH1D*> fh_sum_s_minv; // sum of all signals


   TCanvas* CreateCanvas(
         const string& name,
         const string& title,
         int width,
         int height);

   TH1D* H1(
         int signalType,
         const string& name);

   TH2D* H2(
         int signalType,
         const string& name);


   /**
    * \brief Draw invariant mass histograms.
    */
   void DrawMinvAll();

   /**
    * \brief Draw invariant mass spectra for all signal types for specified analysis step.
    * \param[in] step Analysis step.
    */
   void DrawMinv(
         AnalysisSteps step);

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
   TH1D* SBgRegion(
         Double_t min,
         Double_t max);

   /**
    * \brief Draw S/BG vs plots for different mass regions.
    */
   void SBgRegionAll();

   /**
    * \brief Draw S/BG vs plots for different signals.
    */
   void DrawSBGSignals();

   ClassDef(CbmAnaDielectronTaskDrawAll, 1);
};

#endif
