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

class CbmLitHistManager;
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
   void SetMinNofPointsSts(Int_t minNofPointsSts);
   void SetMinNofPointsTrd(Int_t minNofPointsTrd);
   void SetMinNofPointsMuch(Int_t minNofPointsMuch);
   void SetMinNofPointsTof(Int_t minNofPointsTof);
   void SetMinNofHitsRich(Int_t minNofHits);
   void SetQuota(Double_t quota);
   void SetQuotaRich(Double_t quota);
   void SetMinNofHitsTrd(Int_t minNofHitsTrd);
   void SetMinNofHitsMuch(Int_t minNofHitsMuch);
   void SetOutputDir(const std::string& dir) { fOutputDir = dir; }
   void SetUseConsecutivePointsInSts(Bool_t useConsecutivePointsInSts);

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
   CbmLitHistManager* fHM; // Histogram manager
   CbmLitTrackingQaCalculator* fTrackingQa; // Tracking performance calculator
   string fOutputDir; // Output directory for results
   CbmLitDetectorSetup fDet; // Detector presence information

   ClassDef(CbmLitTrackingQa, 1);
};

#endif /* CBMLITTRACKINGQA_H_ */
