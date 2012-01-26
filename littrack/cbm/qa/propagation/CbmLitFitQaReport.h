/**
 * \file CbmLitFitQaReport.h
 * \brief Create report for fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITFITQAREPORT_H_
#define CBMLITFITQAREPORT_H_

#include "../report/CbmLitSimulationReport.h"

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

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   void Create(
      LitReportType reportType,
      ostream& out,
      const string& resultDirectory);

protected:
   /**
    * \brief
    */
   void Create(
      ostream& out);

   string PrintResAndPullRow(
           const string& rowName,
           const string& histName,
           const string& propertyName);

};

#endif /* CBMLITFITQAREPORT_H_ */
