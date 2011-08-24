/**
 * \file CbmLitQaHTMLGeneratorStudy.h
 *
 * \brief Creates HTML page for sudy of the reconstruction performance.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITRECOQASTUDYGENERATORHTML_H_
#define CBMLITRECOQASTUDYGENERATORHTML_H_

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
class CbmLitQaHTMLGeneratorStudy {
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
    * \param[in] results List of directories with results.
    * \param[in] names Name of the studies.
    */
   void Create(
         const std::vector<std::string>& results,
         const std::vector<std::string>& names);

private:

   void Create(
      std::ostream& out);

   std::string PrintTableHeader();

   std::string PrintRow(
      int row,
      const std::string& property,
      const std::string& name);

   std::string PrintEmptyRow(
         int row,
         const std::string& name);

   std::string PrintRowEff(
         int row,
         const std::string& property,
         const std::string& name);

   std::string PrintImage(
         const std::string& title,
         const std::string& file);

   std::string PrintValue(
         int studyId,
         const std::string& valueTitle,
         const std::string& valueName,
         const std::string& tag);

   std::string HtmlHeadString();

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
   // Background colors for error highlighting
   std::string fErrorColor; // error
   std::string fWarningColor; // warning
   std::string fNormalColor; // normal
   // Names of directories with study results
   std::vector<std::string> fStudyResults;
};

#endif /* CBMLITRECOQASTUDYGENERATORHTML_H_ */
