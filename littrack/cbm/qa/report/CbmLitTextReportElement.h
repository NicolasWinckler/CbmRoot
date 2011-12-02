/**
 * \file CbmLitTextReportElement.h
 * \brief Implementation of CbmLitReportElement for text output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITTEXTREPORTELEMENT_H_
#define CBMLITTEXTREPORTELEMENT_H_

#include "CbmLitReportElement.h"
#include <string>

using std::string;
using std::vector;

/**
 * \class CbmLitTextReportElement
 * \brief Implementation of CbmLitReportElement for text output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitTextReportElement: public CbmLitReportElement
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTextReportElement();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTextReportElement();

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
         const string& name1,
         const string& name2,
         const string& name3 = "",
         const string& name4 = "",
         const string& name5 = "",
         const string& name6 = "",
         const string& name7 = "",
         const string& name8 = "") const;

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

private:
   int fColW; // column width
};

#endif /* CBMLITTEXTREPORTELEMENT_H_ */
