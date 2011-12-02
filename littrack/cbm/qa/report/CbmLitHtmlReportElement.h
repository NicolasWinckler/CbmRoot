/**
 * \file CbmLitHtmlReportElement.h
 * \brief Implementation of CbmLitReportElement for HTML output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITHTMLREPORTELEMENT_H_
#define CBMLITHTMLREPORTELEMENT_H_

#include "CbmLitReportElement.h"
#include <string>

using std::string;
using std::vector;

/**
 * \class CbmLitHtmlReportElement
 * \brief Implementation of CbmLitReportElement for text output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitHtmlReportElement: public CbmLitReportElement
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitHtmlReportElement();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitHtmlReportElement();

   /**
    * \brief Inherited from CbmLitReportElement.
    */
   virtual string TableBegin(
         const string& caption,
         const vector<string>& colNames) const;

   /**
    * \brief Inherited from CbmLitReportElement.
    */
   virtual string TableEnd() const;

   /**
    * \brief Inherited from CbmLitReportElement.
    */
   virtual string TableEmptyRow(
         int nofCols,
         const string& name) const;

   /**
    * \brief Inherited from CbmLitReportElement.
    */
   virtual string TableRow(
         const vector<string>& row) const;

   /**
    * \brief Inherited from CbmLitReportElement.
    */
   virtual string Image(
         const string& title,
         const string& file) const;

   /**
    * \brief Inherited from CbmLitReportElement.
    */
   virtual string DocumentBegin() const;

   /**
    * \brief Inherited from CbmLitReportElement.
    */
   virtual string DocumentEnd() const;

   /**
    * \brief Inherited from CbmLitReportElement.
    */
   virtual string Title(
         int size,
         const string& title) const;
};

#endif /* CBMLITHTMLREPORTELEMENT_H_ */
