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
   CbmLitClusteringQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaStudyReport();

   /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create(
         LitReportType reportType,
         ostream& out,
         const vector<string>& resultDirectories,
         const vector<string>& studyNames);

private:

   void Create(
         ostream& out);

   string PrintRow(
         const string& property,
         const string& name);
};

#endif /* CBMLITCLUSTERINGQASTUDYREPORT_H_ */
