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
    *
    * Non virtual interface pattern is used here.
    * User always creates simulation report using this public non virtual method.
    * In order to configure concrete report generation class
    * user has to implement protected Create() method
    * and getters for the file names.
    *
    * \param[in] reportType Type of report to be produced.
    * \param[out] out Output stream for report file.
    * \param[in] resultDirectory Path to directory with results.
    */
   void Create(
         LitReportType reportType,
         ostream& out,
         const string& resultDirectory);

   /**
    * \brief Inherited from CbmLitReport.
    */
   bool PropertyExists(
         const string& name) const;

protected:

   /**
    * \brief Pure abstract function which is called from main Create() function.
    * \param[in] out Output stream.
    */
   virtual void Create(
         ostream& out) = 0;

   ptree fQa; // Property tree of Qa results for each study
   ptree fIdeal; // Property with ideal values
   ptree fCheck; // Property tree with checked results for each study
   string fResultDirectory; // Directory with simulation results
};

#endif /* CBMLITSIMULATIONREPORT_H_ */
