/**
 * \file CbmLitQaBaseReportElementStudy.h
 *
 * \brief CbmLitQaBaseReportElementStudy.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CbmLitQaBaseReportElementStudy_H_
#define CbmLitQaBaseReportElementStudy_H_


#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

using namespace std;
/**
 * \class CbmLitQaBaseReportElementStudy
 *
 * \brief CbmLitQaBaseReportElementStudy.

 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaBaseReportElementStudy
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaBaseReportElementStudy(){;}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaBaseReportElementStudy(){;}

   /**
    * \brief Return string with table open tag.
    * \param[in] caption Table caption.
    * \return String with HTML table open tag.
    */
   virtual string PrintTableBegin(
         const string& caption = "") = 0;

   /**
    * \brief Return string with table close tag.
    * \return String with HTML table close tag.
    */
   virtual string PrintTableEnd() = 0;

   /**
    * \brief Return string with Subtitle.
    * \return String with subtitle.
    */
   virtual string PrintSubtitle(
         const string& name = "") = 0;

   /**
    * \brief Prints one row in a table.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return string with table row in HTML format.
    */
   virtual string PrintRow(
      int row,
      const string& property,
      const string& name) = 0;

   /**
    * \brief Prints table row which spans over all columns.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] name Name of the row.
    * \return string with table row in HTML format.
    */
   virtual string PrintEmptyRow(
         int row,
         const string& name) = 0;

   /**
    * \brief Prints one row with efficiencies in a table.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return string with table row in HTML format.
    */
   virtual string PrintRowEff(
         int row,
         const string& property,
         const string& name) = 0;

   /**
    * \brief Prints images with reconstruction performance.
    * \param[in] title Title of the image.
    * \param[in] file Name of the image file.
    * \return string with table which contains images in HTML format.
    */
   virtual string PrintImage(
         const string& title,
         const string& file) = 0;

   /**
    * \brief Return string with CSS style for the HTML page.
    * \return std::string with CSS style for the HTML page.
    */
   virtual string PrintHead() = 0;

   /**
    * \brief Return string with close tags of the document.
    * \return string Return string with close tags of the document.
    */
   virtual string PrintCloseDocument() = 0;

   void SetPtreeQa(const vector<boost::property_tree::ptree>& qa){fQa = qa;}

   void SetPtreeIdeal(boost::property_tree::ptree* ideal){fIdeal = ideal;}

   void SetPtreeCheck(const vector<boost::property_tree::ptree>& check){fCheck = check;}

   void SetStudyNames(const vector<string>& names){fStudyNames = names;}

   void SetStudyResults(const vector<string>& results){fStudyResults = results;}

   void SetErrorColor(const string& color){fErrorColor = color;}

   void SetWarningColor(const string& color){fWarningColor = color;}

   void SetNormalColor(const string& color){fNormalColor = color;}

   void SetIsUseChecking(bool isCheck){fIsUseChecking = isCheck;}

protected:

   // Property tree of Qa results for each study
   vector<boost::property_tree::ptree> fQa;
   // Property with ideal values
   boost::property_tree::ptree* fIdeal;
   // Property tree with checked results for each study
   vector<boost::property_tree::ptree> fCheck;
   // Names of studies
   vector<string> fStudyNames;
   // Names of directories with study results
   vector<string> fStudyResults;

   // Background colors for error highlighting
   string fErrorColor; // error
   string fWarningColor; // warning
   string fNormalColor; // normal

   // If TRUE than results are highlighted depending on the results of checking procedure
   bool fIsUseChecking;
};

#endif /* CbmLitQaBaseReportElementStudy_H_ */
