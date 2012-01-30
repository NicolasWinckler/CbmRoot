/**
 * \file CbmLitClusteringQaStudyReport.h
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQASTUDYREPORT_H_
#define CBMLITCLUSTERINGQASTUDYREPORT_H_

#include "../report/CbmLitStudyReport.h"
#include "TSystem.h"

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

private:
   /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create(
         ostream& out);

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetQaFileName() const {
      return "clustering_qa.json";
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetIdealFileName() const {
      return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/clustering/clustering_qa_ideal.json");
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetCheckFileName() const {
      return "clustering_qa_check.json";
   }

   string PrintRow(
         const string& property,
         const string& name);
};

#endif /* CBMLITCLUSTERINGQASTUDYREPORT_H_ */
