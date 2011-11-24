/**
 * \file CbmLitQaLatexReportElementStudy.h
 *
 * \brief Base class.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CbmLitQaLatexReportElementStudy_H_
#define CbmLitQaLatexReportElementStudy_H_

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "CbmLitQaBaseReportElementStudy.h"


using namespace std;
/**
 * \class CbmLitQaLatexReportElementStudy
 *
 * \brief .
 *
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaLatexReportElementStudy: public CbmLitQaBaseReportElementStudy
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaLatexReportElementStudy();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaLatexReportElementStudy();


   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintTableBegin(
         const string& caption = "");

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintTableEnd();

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintSubtitle(
         const string& name = "");

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintRow(
      int row,
      const string& property,
      const string& name);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintEmptyRow(
         int row,
         const string& name);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintRowEff(
         int row,
         const string& property,
         const string& name);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintImage(
         const string& title,
         const string& file);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintValue(
         int studyId,
         const string& valueName);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintHead();

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintCloseDocument();

};

#endif /* CbmLitQaLatexReportElementStudy_H_ */
