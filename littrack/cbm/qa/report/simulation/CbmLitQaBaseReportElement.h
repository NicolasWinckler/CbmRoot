/**
 * \file CbmLitQaBaseReportElement.h
 *
 * \brief CbmLitQaBaseReportElement.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CbmLitQaBaseReportElement_H_
#define CbmLitQaBaseReportElement_H_

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

using namespace std;
/**
 * \class CbmLitQaBaseReportElement
 *
 * \brief CbmLitQaBaseReportElement.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaBaseReportElement
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaBaseReportElement(){;}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaBaseReportElement(){;}

   /**
    * \brief Return string with table open tag.
    * \param[in] caption Table caption.
    * \param[in] colNames Names of the columns in table.
    * \return String with table open tag.
    */
   virtual string PrintTableBegin(
         const string& caption,
         const vector<string>& colNames) = 0;

   /**
    * \brief Return string with table close tag.
    * \return String with table close tag.
    */
   virtual string PrintTableEnd() = 0;

   /**
    * \brief Prints table row which spans over all columns.
    * \param[in] nofCols number of columns in table.
    * \param[in] name Name of the row.
    * \return string with table row in specific format.
    */
   virtual string PrintEmptyRow(
         int nofCols,
         const string& name) = 0;

   /**
    * \brief Prints table row which contains two columns.
    * \param[in] name1 The first column text.
    * \param[in] name2 The second column text etc.
    * \return string with table row in specific format.
    */
   virtual string PrintRow(
         const string& name1,
         const string& name2,
         const string& name3 = "",
         const string& name4 = "",
         const string& name5 = "",
         const string& name6 = "") = 0;

   /**
    * \brief Prints specified value.
    * \param[in] hist full name of the property in property tree.
    * \return string with table row in specific format.
    */
   virtual string PrintValue(
         const string& hist) = 0;

   /**
    * \brief Print number of points or hits or tracks or rings etc. to string.
    */
   virtual string PrintNofStatistics(
         const string& name,
         const string& mvd,
         const string& sts,
         const string& rich,
         const string& trd,
         const string& muchP,
         const string& muchS,
         const string& tof) = 0;

   /**
    * \brief Print hits histogram statistics (nof all, true, fake hits in track/ring).
    * \param[in] name String of the first column in the table.
    * \param[in] hist Histogram name.
    */
   virtual string PrintHits(
         const string& name,
         const string& hist) = 0;

   /**
    * \brief Calculate integrated efficiencies and forms string with statistic information.
    * \param[in] name String of the first column in the table.
    * \param[in] hist Histogram name.
    */
   virtual string PrintEfficiency(
         const string& name,
         const string& hist) = 0;

   /**
    * \brief Calculate integrated efficiencies and forms string with statistic information for the RICH detector.
    * \param[in] name String of the first column in the table.
    * \param[in] hist Histogram name.
    */
   virtual string PrintEfficiencyRich(
         const string& name,
         const string& hist) = 0;

   /**
    * \brief Print integrated efficiencies and forms string with electron identification statistic information.
    * \param[in] name string of the first column in the table.
    * \param[in] hist histogram name.
    */
   virtual string PrintEfficiencyElId(
         const string& name,
         const string& hist) = 0;

   /**
    * \brief Print detector acceptance efficiencies.
    * \param[in] name string of the first column in the table.
    * \param[in] hist histogram name.
    */
   virtual string PrintDetAccEl(
         const string& name,
         const string& hist) = 0;

   /**
    * \brief Calculate integrated efficiencies for different polar angles and returns a string with statistics.
    * \param[in] name string of the first column in the table.
    * \param[in] hist histogram name.
    */
   virtual string PrintPolarAngle(
         const string& hist) = 0;

   /**
    * \brief Print images with reconstruction performance.
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

   void SetPtreeQa(boost::property_tree::ptree* qa){fQa = qa;}

   void SetPtreeIdeal(boost::property_tree::ptree* ideal){fIdeal = ideal;}

   void SetPtreeCheck(boost::property_tree::ptree* check){fCheck = check;}

   void SetOutputDir(const string& dir){fOutputDir = dir;}

   void SetErrorColor(const string& color){fErrorColor = color;}

   void SetWarningColor(const string& color){fWarningColor = color;}

   void SetNormalColor(const string& color){fNormalColor = color;}

   void SetIsUseChecking(bool isCheck){fIsUseChecking = isCheck;}

protected:

   // Property tree of Qa results for each study
   boost::property_tree::ptree* fQa;
   // Property with ideal values
   boost::property_tree::ptree* fIdeal;
   // Property tree with checked results for each study
   boost::property_tree::ptree* fCheck;
   // Name of directory with results
   string fOutputDir;

   // Background colors for error highlighting
   string fErrorColor; // error
   string fWarningColor; // warning
   string fNormalColor; // normal

   // If TRUE than results are highlighted depending on the results of checking procedure
   bool fIsUseChecking;
};

#endif /* CbmLitQaBaseReportElement_H_ */
