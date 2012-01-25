/**
 * \file CbmLitFieldQaReport.h
 * \brief Creates field QA report.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITFIELDQAREPORT_H_
#define CBMLITFIELDQAREPORT_H_

#include "../report/CbmLitSimulationReport.h"
#include <string>

/**
 * \class CbmLitFieldQaReport
 * \brief Creates field QA report.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitFieldQaReport : public CbmLitSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFieldQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldQaReport();

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   void Create(
      LitReportType reportType,
      ostream& out,
      const string& resultDirectory);

private:
   /**
    * \brief
    */
   void Create(
      ostream& out);

   /**
    * \brief Return  string with summary table for field QA.
    * \return String with summary table for field QA.
    */
   std::string PrintSummaryTable();

   /**
    * \brief Return string with images for field QA.
    * \return  Return string with images for field QA.
    */
   std::string PrintListOfImages();
};

#endif /* CBMLITFIELDQAREPORT_H_ */
