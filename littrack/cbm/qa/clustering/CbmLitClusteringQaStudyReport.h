/**
 * \file CbmLitClusteringQaStudyReport.h
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQASTUDYREPORT_H_
#define CBMLITCLUSTERINGQASTUDYREPORT_H_

#include "../report/CbmLitStudyReport.h"

/**
 * \class CbmLitClusteringQaStudyReport
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitClusteringQaStudyReport : public CbmLitStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQaStudyReport(LitReportType reportType);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaStudyReport();

   /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create(
         ostream& out,
         const vector<string>& studyNames,
         const vector<boost::property_tree::ptree*>& qa,
         boost::property_tree::ptree* ideal,
         const vector<boost::property_tree::ptree*>& check);

private:

   string PrintRow(
         const string& property,
         const string& name);
};

#endif /* CBMLITCLUSTERINGQASTUDYREPORT_H_ */
