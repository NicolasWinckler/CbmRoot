/**
 * \file CbmLitLatexReportElement.h
 * \brief Implementation of CbmLitReportElement for Latex output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITLATEXREPORTELEMENT_H_
#define CBMLITLATEXREPORTELEMENT_H_

#include "CbmLitReportElement.h"
#include <string>

using std::string;
using std::vector;

/**
 * \class CbmLitLatexReportElement
 * \brief Implementation of CbmLitReportElement for Latex output.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitLatexReportElement: public CbmLitReportElement
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitLatexReportElement();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitLatexReportElement();

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
};

#endif /* CBMLITLATEXREPORTELEMENT_H_ */
