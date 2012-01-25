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
    * \brief Constructor.
    */
   CbmLitSimulationReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitSimulationReport();

   /**
    * \brief Main function which creates report file.
    * \param[in] reportType Type of report to be produced.
    * \param[out] out Output stream for report file.
    * \param[in] resultDirectory Path to directory eith results.
    */
   virtual void Create(
         LitReportType reportType,
         ostream& out,
         const string& resultDirectory) = 0;

   /**
    * \brief Inherited from CbmLitReport.
    */
   bool PropertyExists(
         const string& name) const;

protected:
   ptree fQa; // Property tree of Qa results for each study
   ptree fIdeal; // Property with ideal values
   ptree fCheck; // Property tree with checked results for each study
};

#endif /* CBMLITSIMULATIONREPORT_H_ */
