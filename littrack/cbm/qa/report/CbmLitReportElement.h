/**
 * \file CbmLitReportElement.h
 * \brief Abstract class for basic report elements (headers, tables, images etc.).
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITREPORTELEMENT_H_
#define CBMLITREPORTELEMENT_H_

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

using namespace std;
/**
 * \class CbmLitReportElement
 *
 * \brief Abstract class for basic report elements (headers, tables, images etc.).
 *
 * Each concrete implementation for report elements has to implement
 * this interface (e.g. Latex, text, HTML). Report has to be written
 * using functionality of this interface class in order to be able
 * to automatically produce reports in different representations
 * (e.g. Latex, text, HTML).
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitReportElement
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitReportElement(){}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitReportElement(){}

   /**
    * \brief Return string with table open tag.
    * \param[in] caption Table caption.
    * \param[in] colNames Names of the columns in table.
    * \return String with table open tag.
    */
   virtual string TableBegin(
         const string& caption,
         const vector<string>& colNames) const = 0;

   /**
    * \brief Return string with table close tag.
    * \return String with table close tag.
    */
   virtual string TableEnd() const = 0;

   /**
    * \brief Return string with table row which spans over all columns.
    * \param[in] nofCols number of columns in table.
    * \param[in] name Name of the row.
    * \return string with table row which spans over all columns.
    */
   virtual string TableEmptyRow(
         int nofCols,
         const string& name) const = 0;

   /**
    * \brief Return string with table row tags.
    * \param[in] name1 The first column text.
    * \param[in] name2 The second column text etc.
    * \return string with table row tags.
    */
   virtual string TableRow(
         const string& name1,
         const string& name2,
         const string& name3 = "",
         const string& name4 = "",
         const string& name5 = "",
         const string& name6 = "",
         const string& name7 = "",
         const string& name8 = "") const = 0;

   /**
    * \brief Return string with image tags.
    * \param[in] title Title of the image.
    * \param[in] file Name of the image file.
    * \return string with image tags.
    */
   virtual string Image(
         const string& title,
         const string& file) const = 0;

   /**
    * \brief Return string with open tags for document.
    * \return string with open tags for document.
    */
   virtual string DocumentBegin() const = 0;

   /**
    * \brief Return string with close tags of the document.
    * \return string with close tags of the document.
    */
   virtual string DocumentEnd() const = 0;

   /**
    * \brief Return string with title.
    * \param[in] size Size of the title. [0-5]. 0 is the largest size.
    * \param[in] title Title string.
    * \return string with subtitle.
    */
   virtual string Title(
         int size,
         const string& title) const = 0;
};

#endif /* CBMLITREPORTELEMENT_H_ */
