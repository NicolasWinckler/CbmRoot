/**
 * \file CbmLitFitQaReport.h
 * \brief Create report for fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITFITQAREPORT_H_
#define CBMLITFITQAREPORT_H_

#include "../report/CbmLitSimulationReport.h"
#include "TSystem.h"

/**
 * \class CbmLitFitQaReport
 * \brief Create report for fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
class CbmLitFitQaReport : public CbmLitSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFitQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFitQaReport();

protected:
   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual void Create(
      ostream& out);

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetQaFileName() const {
      return "fit_qa.json";
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetIdealFileName() const {
      return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/fit/fit_qa_ideal.json");
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetCheckFileName() const {
      return "fit_qa_check.json";
   }

   string PrintResAndPullRow(
           const string& rowName,
           const string& histName,
           const string& propertyName);

};

#endif /* CBMLITFITQAREPORT_H_ */
