/**
 * \file CbmLitStudyReport.h
 * \brief Base class for study reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITSTUDYREPORT_H_
#define CBMLITSTUDYREPORT_H_

#include "CbmLitReport.h"
#include <boost/property_tree/ptree.hpp>
#include <string>

using std::vector;
using std::string;
using std::ostream;
using boost::property_tree::ptree;

/**
 * \class CbmLitStudyReport
 * \brief Base class for study reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitStudyReport : public CbmLitReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitStudyReport();

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
    * \param[in] resultDirectories Vector of directory names.
    * \param[in] studyNames Names of studies.
    */
   void Create(
         LitReportType reportType,
         ostream& out,
         const vector<string>& resultDirectories,
         const vector<string>& studyNames);

   /**
    * \brief Inherited from CbmLitReport.
    */
   bool PropertyExists(
         const std::string& name) const;

protected:
   /**
    * \brief Pure abstract function which is called from main Create() function.
    * \param[in] out Output stream.
    */
   virtual void Create(
         ostream& out) = 0;

   vector<ptree> fQa; // Property tree of Qa results for each study
   ptree fIdeal; // Property with ideal values
   vector<ptree> fCheck; // Property tree with checked results for each study
   vector<string> fResultDirectories; // Directory names of study results
   vector<string> fStudyNames; // Names of studies
};

#endif /* CBMLITSTUDYREPORT_H_ */
