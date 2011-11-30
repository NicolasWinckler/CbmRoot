/**
 * \file CbmLitQaReconstructionReportStudy.h
 *
 * \brief Creates report for study of the reconstruction performance.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CbmLitQaReconstructionReportStudy_H_
#define CbmLitQaReconstructionReportStudy_H_

#include "../CbmLitStudyReport.h"

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

using namespace std;
/**
 * \class CbmLitQaReconstructionReportStudy
 *
 * \brief Creates report for study of the reconstruction performance.
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
    * \brief Constructor.
    * \param[in] type type of the report. "latex" or "html"
    */
   CbmLitQaReconstructionReportStudy(
         const string& type);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaReconstructionReportStudy();

protected:

   /**
    * \brief Creates report and writes output to ostream.
    * \param[out] out ostream for output.
    */
   virtual void Create(
      ostream& out);

   /**
    * \brief Prints one row in a table.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return string with table row in HTML format.
    */
   virtual string PrintRow(
      const string& property,
      const string& name);

   /**
    * \brief Prints one row with efficiencies in a table.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return string with table row in HTML format.
    */
   virtual string PrintRowEff(
         const string& property,
         const string& name);

   /**
    * \brief Prints images with reconstruction performance.
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

#endif /* CbmLitQaReconstructionReportStudy_H_ */
