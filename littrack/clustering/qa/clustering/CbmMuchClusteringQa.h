/**
 * \file CbmLitClusteringQa.h
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMMUCHCLUSTERINGQA_H_
#define CBMMUCHCLUSTERINGQA_H_

#include "FairTask.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
class CbmHistManager;
class CbmMuchClusteringQaCalculator;

class CbmMuchClusteringQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmMuchClusteringQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMuchClusteringQa();

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
    CbmHistManager* fHM; // Histogram manager
    CbmMuchClusteringQaCalculator* fClusteringQa; // Clustering performance calculator
    string fOutputDir; // Output directory for results
    Int_t fNofEvents;

    ClassDef(CbmMuchClusteringQa, 1);
};

#endif /* CBMMUCHCLUSTERINGQA_H_ */
