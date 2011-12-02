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
    * \brief Constructor with report type.
    * \param[in] reportType Type of report to be created.
    */
   CbmLitFieldQaReport(LitReportType reportType);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldQaReport();

   /**
    * \brief Main function which creates HTML.
    *
    * \param[out] out Output stream.
    * \param[in] qa Property tree with field Qa results.
    * \param[in] ideal Property tree with ideal values.
    * \param[in] check Property tree with checked results.
    */
   void Create(
      std::ostream& out,
      boost::property_tree::ptree* qa,
      boost::property_tree::ptree* ideal,
      boost::property_tree::ptree* check);

private:
   /**
    * \brief Prints summary table for field QA in HTML format.
    * \return String with HTML.
    */
   std::string PrintSummaryTable();

//   /**
//    * \brief Prints table row which spans over all columns.
//    * \param[in] row Row counter. Used to highlight row.
//    * \param[in] ncols Number of columns to span.
//    * \param[in] name Name of the row.
//    * \return std::string with table row in HTML format.
//    */
//   std::string PrintEmptyRow(
//         int row,
//         int ncols,
//         const std::string& name);
};

#endif /* CBMLITFIELDQAREPORT_H_ */
