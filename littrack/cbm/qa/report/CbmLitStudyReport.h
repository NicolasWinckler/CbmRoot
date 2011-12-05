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
   virtual void Create(
         ostream& out,
         const vector<string>& studyNames,
         const vector<boost::property_tree::ptree*>& qa,
         boost::property_tree::ptree* ideal,
         const vector<boost::property_tree::ptree*>& check) = 0;

protected:
   // Property tree of Qa results for each study
   vector<boost::property_tree::ptree*> fQa;
   // Property with ideal values
   boost::property_tree::ptree* fIdeal;
   // Property tree with checked results for each study
   vector<boost::property_tree::ptree*> fCheck;
   // Names of studies
   vector<string> fStudyNames;
//   // Names of directories with study results
//   vector<string> fStudyResults;
};

#endif /* CBMLITSTUDYREPORT_H_ */
