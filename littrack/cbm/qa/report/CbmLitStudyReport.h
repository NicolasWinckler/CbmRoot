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
    * \param[in] title Title of the page.
    * \param[in] results List of directories with results.
    * \param[in] names Name of the studies.
    */
   void CreateReport(
         ostream& out,
         const string& title,
         const vector<string>& results,
         const vector<string>& names);

   /* Setters */
   void SetPtreeQa(const vector<boost::property_tree::ptree>& qa){ fQa = qa; }
   void SetPtreeIdeal(boost::property_tree::ptree* ideal){ fIdeal = ideal; }
   void SetPtreeCheck(const vector<boost::property_tree::ptree>& check){ fCheck = check; }
   void SetStudyNames(const vector<string>& names){ fStudyNames = names; }
   void SetStudyResults(const vector<string>& results){ fStudyResults = results; }

protected:

   /**
    * \brief Creates report and writes output to ostream.
    * \param[out] out ostream for output.
    */
   virtual void Create(
      ostream& out) = 0;

   // Property tree of Qa results for each study
   vector<boost::property_tree::ptree> fQa;
   // Property with ideal values
   boost::property_tree::ptree* fIdeal;
   // Property tree with checked results for each study
   vector<boost::property_tree::ptree> fCheck;
   // Names of studies
   vector<string> fStudyNames;
   // Names of directories with study results
   vector<string> fStudyResults;
};

#endif /* CBMLITSTUDYREPORT_H_ */
