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
         const string& name6 = "",
         const string& name7 = "",
         const string& name8 = "") = 0;


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
};

#endif /* CbmLitQaBaseReportElement_H_ */
