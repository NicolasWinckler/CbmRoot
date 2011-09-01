/**
 * \file CbmLitQaHTMLGeneratorStudy.h
 *
 * \brief Creates HTML page for sudy of the reconstruction performance.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITQAHTMLGENERATORSTUDY_H_
#define CBMLITQAHTMLGENERATORSTUDY_H_

#include "CbmLitQaBaseGenerator.h"

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

/**
 * \class CbmLitQaHTMLGeneratorStudy
 *
 * \brief Creates HTML page for study of the reconstruction performance.
 *
 * Generates HTML from a number of results of different
 * simulations and reconstructions. Creates comparison tables.
 * Very useful for studies since all numbers are automatically
 * put in the comparison tables.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaHTMLGeneratorStudy : public CbmLitQaBaseGenerator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaHTMLGeneratorStudy();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaHTMLGeneratorStudy();

   /**
    * \brief Main function which creates HTML.
    *
    * \param[in] out Output stream for HTML.
    * \param[in] title Title of the page.
    * \param[in] results List of directories with results.
    * \param[in] names Name of the studies.
    */
   void Create(
         std::ostream& out,
         const std::string& title,
         const std::vector<std::string>& results,
         const std::vector<std::string>& names);



private:

   /**
    * \brief Creates HTML page and writes output to ostream.
    * \param[out] out std::ostream for output.
    */
   void Create(
      std::ostream& out);

   /**
    * \brief Return string with HTML table header.
    * \return String with HTML table header.
    */
   std::string PrintTableHeader();

   /**
    * \brief Prints one row in a table.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return std::string with table row in HTML format.
    */
   std::string PrintRow(
      int row,
      const std::string& property,
      const std::string& name);

   /**
    * \brief Prints table row which spans over all columns.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] name Name of the row.
    * \return std::string with table row in HTML format.
    */
   std::string PrintEmptyRow(
         int row,
         const std::string& name);

   /**
    * \brief Prints one row with efficiencies in a table.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return std::string with table row in HTML format.
    */
   std::string PrintRowEff(
         int row,
         const std::string& property,
         const std::string& name);

   /**
    * \brief Prints images with reconstruction performance.
    * \param[in] title Title of the image.
    * \param[in] file Name of the image file.
    * \return std::string with table which contains images in HTML format.
    */
   std::string PrintImage(
         const std::string& title,
         const std::string& file);

   /**
    * \brief Print property value from property tree to std::string.
    *
    * Uniforms output style of properties. Colors background
    * in dependence on the correctness of the property.
    *
    * \param[in] studyId Index of the study from which to take the value.
    * \param[in] valueTitle Property title which is printed before the property value.
    * \param[in] valueName Property name in the property tree.
    * \param[in] tag HTML tag which is used to print the property i.e. "p", "b", "td".
    * \return std::string with property in HTML format.
    */
   std::string PrintValue(
         int studyId,
         const std::string& valueTitle,
         const std::string& valueName,
         const std::string& tag);

   // Number of studies
   int fNofStudies;
   // Property tree of Qa results for each study
   std::vector<boost::property_tree::ptree> fQa;
   // Property with ideal values
   boost::property_tree::ptree fIdeal;
   // Property tree with checked results for each study
   std::vector<boost::property_tree::ptree> fCheck;
   // Names of studies
   std::vector<std::string> fStudyNames;
   // Names of directories with study results
   std::vector<std::string> fStudyResults;
};

#endif /* CBMLITQAHTMLGENERATORSTUDY_H_ */
