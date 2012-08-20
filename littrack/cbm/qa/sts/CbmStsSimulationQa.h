/**
 * \file CbmStsSimulationQa.h
 * \author Volker Friese <v.friese@gsi.de>
 * \brief STS simulation QA.
 * \date 2012
 */

#ifndef CBMSTSSIMULATIONQA_H_
#define CBMSTSSIMULATIONQA_H_

#include "FairTask.h"
#include "TClonesArray.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

class CbmHistManager;
class CbmStsSimulationQaCalculator;

/**
 * \class CbmStsSimulationQa
 * \author Volker Friese <v.friese@gsi.de>
 * \brief STS simulation QA.
 * \date 2012
 */
class CbmStsSimulationQa: public FairTask
{
public:

    /**
     * \brief Constructor.
     */
    CbmStsSimulationQa();

    /**
     * \brief Destructor.
     */
    virtual ~CbmStsSimulationQa();

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Exec(Option_t* opt);

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Finish();

    /**
     * \brief Inherited from FairTask.
     */
    virtual InitStatus Init();

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

    /**
     * \brief Get the geometry from TGeo.
     */
//    Bool_t GetGeometry();

    CbmHistManager* fHM; // Histogram manager
    CbmStsSimulationQaCalculator* fStsSimulationQa; // STS simulation performance calculator
    string fOutputDir; // Output directory for results

    ClassDef(CbmStsSimulationQa, 1);
};

#endif /* CBMSTSSIMULATIONQA_H_ */
