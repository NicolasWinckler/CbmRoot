/**
 * \file CbmLitSimulationReport.h
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITSIMULATIONREPORT_H_
#define CBMLITSIMULATIONREPORT_H_

#include "CbmLitReport.h"
#include <boost/property_tree/ptree.hpp>
#include <string>

using std::ostream;
using std::string;

/**
 * \class CbmLitSimulationReport
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitSimulationReport : public CbmLitReport
{
public:
   /**
    * \brief Constructor with report type.
    * \param[in] reportType Type of the report to be produced.
    */
   CbmLitSimulationReport(LitReportType reportType);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitSimulationReport();

   /**
    * \brief Main function which creates report file.
    * \param[in] out Output stream for report file.
    * \param[in] qa Property tree with results.
    * \param[in] ideal Property tree with ideal results.
    * \param[in] check Property tree with checked results.
    */
   virtual void Create(
         ostream& out,
         boost::property_tree::ptree* qa,
         boost::property_tree::ptree* ideal,
         boost::property_tree::ptree* check) = 0;

   /* Setters */
//   void SetOutputDir(const string& dir){ fOutputDir = dir; }

protected:
   // Property tree of Qa results for each study
   boost::property_tree::ptree* fQa;
   // Property with ideal values
   boost::property_tree::ptree* fIdeal;
   // Property tree with checked results for each study
   boost::property_tree::ptree* fCheck;
   // Name of directory with results
//   string fOutputDir;
};

#endif /* CBMLITSIMULATIONREPORT_H_ */
