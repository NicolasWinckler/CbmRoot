/**
 * \file CbmLitTrackingQaStudyReport.h
 * \brief Creates study report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITTRACKINGQASTUDYREPORT_H_
#define CBMLITTRACKINGQASTUDYREPORT_H_

#include "../report/CbmStudyReport.h"
#include <string>
#include <vector>
#include "TSystem.h"
#include <boost/function.hpp>
using std::string;
using std::vector;
class CbmLitPropertyTree;

/**
 * \class CbmLitTrackingQaStudyReport
 * \brief Creates study report for tracking QA.
 *
 * Generates tracking QA report from a number of results of different
 * simulations and reconstructions. Creates comparison tables.
 * Very useful for studies since all numbers are automatically
 * put in the comparison tables.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitTrackingQaStudyReport : public CbmStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackingQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQaStudyReport();

protected:
    /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create(
         ostream& out);

   /**
     * \brief Inherited from CbmLitSimulationReport.
     */
    virtual string GetQaFileName() const {
       return "tracking_qa.json";
    }

    /**
     * \brief Inherited from CbmLitSimulationReport.
     */
    virtual string GetIdealFileName() const {
       return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/tracking/tracking_qa_ideal.json");
    }

    /**
     * \brief Inherited from CbmLitSimulationReport.
     */
    virtual string GetCheckFileName() const {
       return "tracking_qa_check.json";
    }

    /**
     * \brief Return formated string with table of numbers.
     * \param[in] tableName Table name.
     * \param[in] pattern Pattern of histogram name.
     * \param[in] rowNameFormatter Function which formats row name.
     * \return Formated string with table of numbers.
     */
    string PrintTable(
    		const string& tableName,
    		const string& pattern,
    		const boost::function<string (const string&)>& rowNameFormatter) const;

    /**
     * \brief Return formated string with table of efficiency numbers.
     * \param[in] tableName Table name.
     * \param[in] pattern Pattern of histogram name.
     * \return Formated string with table of efficiency numbers.
     */
    string PrintEfficiencyTable(
    		const string& tableName,
    		const string& pattern) const;

   vector<CbmLitPropertyTree*> fPT;
};

#endif /* CBMLITTRACKINGQASTUDYREPORT_H_ */
