/**
 * \file CbmMuchClusteringQaStudyReport.h
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMMUCHCLUSTERINGQASTUDYREPORT_H_
#define CBMMUCHCLUSTERINGQASTUDYREPORT_H_

#include "../report/CbmStudyReport.h"
#include "TSystem.h"

/**
 * \class CbmLitClusteringQaStudyReport
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmMuchClusteringQaStudyReport : public CbmStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmMuchClusteringQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMuchClusteringQaStudyReport();

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
      return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/clustering/qa/clustering/clustering_qa_ideal.json");
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

#endif /* CBMMUCHCLUSTERINGQASTUDYREPORT_H_ */
