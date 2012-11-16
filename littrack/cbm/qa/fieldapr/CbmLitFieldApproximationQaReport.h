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
   virtual void Create();

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Draw();

   /**
    * \brief Return  string with summary table for field QA.
    * \return String with summary table for field QA.
    */
   string PrintSummaryTable();
};

#endif /* CBMLITFIELDAPPROXIMATIONQAREPORT_H_ */
