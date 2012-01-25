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
    * \brief Constructor with report type.
    * \param[in] reportType Report type to be generated.
    */
   CbmLitStudyReport(LitReportType reportType);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitStudyReport();

   /**
    * \brief Main function which creates report file.
    * \param[in] out Output stream for report file.
    * \param[in] studyNames Names of studies.
    * \param[in] qa Vector of property trees with results.
    * \param[in] ideal Vector of property trees with ideal results.
    * \param[in] check Vector of property trees with checked results.
    */
   virtual void Create( // TODO: check if we have to pass ptrees, since directory names are already passed
         ostream& out,
         const vector<string>& resultDirectories,
         const vector<string>& studyNames,
         const vector<ptree*>& qa,
         ptree* ideal,
         const vector<ptree*>& check) = 0;

   /**
    * \brief Inherited from CbmLitReport.
    */
   bool PropertyExists(
         const std::string& name) const;

protected:
   // Property tree of Qa results for each study
   vector<ptree*> fQa;
   // Property with ideal values
   ptree* fIdeal;
   // Property tree with checked results for each study
   vector<ptree*> fCheck;
   // Directory names of study results
   vector<string> fResultDirectories;
   // Names of studies
   vector<string> fStudyNames;
};

#endif /* CBMLITSTUDYREPORT_H_ */
