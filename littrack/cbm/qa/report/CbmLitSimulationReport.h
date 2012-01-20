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
using boost::property_tree::ptree;

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
         ptree* qa,
         ptree* ideal,
         ptree* check) = 0;

   /**
    * \brief Inherited from CbmLitReport.
    */
   bool PropertyExists(
         const string& name) const;

protected:
   // Property tree of Qa results for each study
   ptree* fQa;
   // Property with ideal values
   ptree* fIdeal;
   // Property tree with checked results for each study
   ptree* fCheck;
};

#endif /* CBMLITSIMULATIONREPORT_H_ */
