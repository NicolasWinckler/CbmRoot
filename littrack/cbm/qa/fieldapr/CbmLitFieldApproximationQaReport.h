/**
 * \file CbmLitFieldApproximationQaReport.h
 * \brief Creates field QA report.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITFIELDAPPROXIMATIONQAREPORT_H_
#define CBMLITFIELDAPPROXIMATIONQAREPORT_H_

#include "CbmSimulationReport.h"
#include "TSystem.h"
#include <string>

/**
 * \class CbmLitFieldApproximationQaReport
 * \brief Creates field QA report.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitFieldApproximationQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFieldApproximationQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldApproximationQaReport();

private:
   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Create(
      ostream& out);

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual string GetQaFileName() const {
      return "fieldapr_qa.json";
   }

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual string GetIdealFileName() const {
      return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/fieldapr/fieldapr_qa_ideal.json");
   }

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual string GetCheckFileName() const {
      return "fieldapr_qa_check.json";
   }

   /**
    * \brief Return  string with summary table for field QA.
    * \return String with summary table for field QA.
    */
   string PrintSummaryTable();
};

#endif /* CBMLITFIELDAPPROXIMATIONQAREPORT_H_ */
