/**
 * \file CbmLitQaReconstructionReportStudy.h
 * \brief Creates study report for reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITQARECONSTRUCTIONREPORTSTUDY_H_
#define CBMLITQARECONSTRUCTIONREPORTSTUDY_H_

#include "../report/CbmLitStudyReport.h"
#include <string>
using std::string;

/**
 * \class CbmLitQaReconstructionReportStudy
 * \brief Creates study report for reconstruction.
 *
 * Generates report from a number of results of different
 * simulations and reconstructions. Creates comparison tables.
 * Very useful for studies since all numbers are automatically
 * put in the comparison tables.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaReconstructionReportStudy : public CbmLitStudyReport
{
public:
   /**
    * \brief Constructor with report type.
    * \param[in] reportType Type of report to be produced.
    */
   CbmLitQaReconstructionReportStudy(LitReportType reportType);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaReconstructionReportStudy();

   /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create(
         ostream& out,
         const vector<string>& studyNames,
         const vector<boost::property_tree::ptree*>& qa,
         boost::property_tree::ptree* ideal,
         const vector<boost::property_tree::ptree*>& check);

protected:

   /**
    * \brief Print one row in a table.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return string with table row in HTML format.
    */
   virtual string PrintRow(
      const string& property,
      const string& name);

   /**
    * \brief Print one row with efficiencies in a table.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return string with table row in HTML format.
    */
   virtual string PrintRowEff(
         const string& property,
         const string& name);

   /**
    * \brief Print images with reconstruction performance.
    * \param[in] title Title of the image.
    * \param[in] file Name of the image file.
    * \return string with table which contains images in HTML format.
    */
   virtual string PrintImage(
         const string& title,
         const string& file);

   /**
    * \brief
    */
   virtual string PrintValue(
         int studyId,
         const string& valueTitle);
};

#endif /* CBMLITQARECONSTRUCTIONREPORTSTUDY_H_ */
