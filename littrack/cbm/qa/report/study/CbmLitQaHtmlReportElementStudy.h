/**
 * \file CbmLitQaHtmlReportElementStudy.h
 *
 * \brief Base class.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CbmLitQaHtmlReportElementStudy_H_
#define CbmLitQaHtmlReportElementStudy_H_

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "CbmLitQaBaseReportElementStudy.h"


using namespace std;
/**
 * \class CbmLitQaHtmlReportElementStudy
 *
 * \brief .
 *
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaHtmlReportElementStudy: public CbmLitQaBaseReportElementStudy
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaHtmlReportElementStudy();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaHtmlReportElementStudy();


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
         const string& valueTitle,
         const string& valueName,
         const string& tag);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintHead();

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintCloseDocument();

};

#endif /* CbmLitQaHtmlReportElementStudy_H_ */
