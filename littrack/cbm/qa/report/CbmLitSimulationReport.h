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
    */
   void CreateReport(
         ostream& out,
         boost::property_tree::ptree* qa);

   /* Setters */
   void SetPtreeQa(boost::property_tree::ptree* qa){ fQa = qa; }
   void SetPtreeIdeal(boost::property_tree::ptree* ideal){ fIdeal = ideal; }
   void SetPtreeCheck(boost::property_tree::ptree* check){ fCheck = check; }
   void SetOutputDir(const string& dir){ fOutputDir = dir; }

protected:
   /**
    * \brief Creates report and writes output to ostream.
    * \param[out] out ostream for output.
    */
   virtual void Create(
      ostream& out) = 0;

   // Property tree of Qa results for each study
   boost::property_tree::ptree* fQa;
   // Property with ideal values
   boost::property_tree::ptree* fIdeal;
   // Property tree with checked results for each study
   boost::property_tree::ptree* fCheck;
   // Name of directory with results
   string fOutputDir;
};

#endif /* CBMLITSIMULATIONREPORT_H_ */
