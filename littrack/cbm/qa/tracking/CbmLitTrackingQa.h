/**
 * \file CbmLitTrackingQa.h
 * \brief FairTask for tracking performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */

#ifndef CBMLITTRACKINGQA_H_
#define CBMLITTRACKINGQA_H_

#include "FairTask.h"
#include "CbmDetectorList.h"
#include "../report/CbmLitReport.h"
#include "cbm/base/CbmLitDetectorSetup.h"

class CbmHistManager;
class CbmLitTrackingQaCalculator;

using std::string;
using std::vector;

/**
 * \class CbmLitTrackingQa.h
 * \brief FairTask for tracking performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */
class CbmLitTrackingQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackingQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQa();

   /**
    * \brief Derived from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Derived from FairTask.
    */
   virtual void Exec(
      Option_t* opt);

   /**
    * \brief Derived from FairTask.
    */
   virtual void Finish();

   /** Setters **/
   void SetMinNofPointsSts(Int_t minNofPointsSts) { fMinNofPointsSts = minNofPointsSts; }
   void SetMinNofPointsTrd(Int_t minNofPointsTrd) { fMinNofPointsTrd = minNofPointsTrd; }
   void SetMinNofPointsMuch(Int_t minNofPointsMuch) { fMinNofPointsMuch = minNofPointsMuch; }
   void SetMinNofPointsTof(Int_t minNofPointsTof) { fMinNofPointsTof = minNofPointsTof; }
   void SetMinNofHitsRich(Int_t minNofHitsRich) { fMinNofHitsRich = minNofHitsRich; }
   void SetQuota(Double_t quota) { fQuota = quota; }
   void SetQuotaRich(Double_t quotaRich) { fQuotaRich = quotaRich; }
   void SetMinNofHitsTrd(Int_t minNofHitsTrd) { fMinNofHitsTrd = minNofHitsTrd; }
   void SetMinNofHitsMuch(Int_t minNofHitsMuch) { fMinNofHitsMuch = minNofHitsMuch; }
   void SetUseConsecutivePointsInSts(Bool_t useConsecutivePointsInSts) { fUseConsecutivePointsInSts = useConsecutivePointsInSts; }
   void SetOutputDir(const std::string& dir) { fOutputDir = dir; }

   /**
    * \brief Generate summary report out of several different simulation results.
    * \param[in] title Title of report.
    * \param[in] resultDirectories Paths to directories with resuls.
    * \param[in] studyNames Study names which are displayed in report.
    */
   void CreateStudyReport(
         const string& title,
         const vector<string>& resultDirectories,
         const vector<string>& studyNames);

   /**
    * \brief Create final simulation report for this tracking QA run.
    * \param[in] title Report title.
    * \param[in] resultDirectory Directory with results.
    */
   void CreateSimulationReport(
         const string& title,
         const string& resultDirectory);

   /**
    * \brief Draw histograms in macro from the specified file.
    * \param[in] fileName Name of the file.
    */
   void DrawHistosFromFile(
         const std::string& fileName);

private:
   CbmHistManager* fHM; // Histogram manager
   CbmLitTrackingQaCalculator* fTrackingQa; // Tracking performance calculator
   string fOutputDir; // Output directory for results
   CbmLitDetectorSetup fDet; // Detector presence information

   // Variables for temporary storage of QA parameters.
   // These parameters later used in CbmLitTrackingQaCalculator.
   Int_t fMinNofPointsSts;
   Int_t fMinNofPointsTrd;
   Int_t fMinNofPointsMuch;
   Int_t fMinNofPointsTof;
   Int_t fMinNofHitsRich;
   Double_t fQuota;
   Double_t fQuotaRich;
   Int_t fMinNofHitsTrd;
   Int_t fMinNofHitsMuch;
   Bool_t fUseConsecutivePointsInSts;

   ClassDef(CbmLitTrackingQa, 1);
};

#endif /* CBMLITTRACKINGQA_H_ */
