/**
 * \file CbmLitClusteringQa.h
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQA_H_
#define CBMLITCLUSTERINGQA_H_

#include "FairTask.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
class CbmLitHistManager;
class CbmLitClusteringQaCalculator;

class CbmLitClusteringQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQa();

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

    /** Setters */
    void SetOutputDir(const string& outputDir) { fOutputDir = outputDir; }

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
     * TODO: Add comments
     */
    void CreateSimulationReport(
          const string& title,
          const string& resultDirectory);
private:
    CbmLitHistManager* fHM; // Histogram manager
    CbmLitClusteringQaCalculator* fClusteringQa; // Clustering performance calculator
    string fOutputDir; // Output directory for results

    ClassDef(CbmLitClusteringQa, 1);
};

#endif /* CBMLITCLUSTERINGQA_H_ */
