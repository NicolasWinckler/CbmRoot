/**
 * \file CbmStsSimulationQaStudyReport.h
 * \brief Study summary report for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */

#ifndef CBMSTSSIMULATIONQASTUDYREPORT_H_
#define CBMSTSSIMULATIONQASTUDYREPORT_H_

#include "../report/CbmStudyReport.h"
#include "TSystem.h"

/**
 * \class CbmStsSimulationQaStudyReport
 * \brief Study summary report for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */
class CbmStsSimulationQaStudyReport : public CbmStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmStsSimulationQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmStsSimulationQaStudyReport();

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
      return "sts_simulation_qa.json";
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetIdealFileName() const {
      return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/sts/sts_simulation_qa_ideal.json");
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetCheckFileName() const {
      return "sts_simulation_qa_check.json";
   }
};

#endif /* CBMSTSSIMULATIONQASTUDYREPORT_H_ */
