/**
 * \file CbmLitQaTxtReportElement.h
 *
 * \brief Base class.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CbmLitQaTxtReportElement_H_
#define CbmLitQaTxtReportElement_H_

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "CbmLitQaBaseReportElement.h"


using namespace std;
/**
 * \class CbmLitQaTxtReportElement
 *
 * \brief .
 *
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaTxtReportElement: public CbmLitQaBaseReportElement
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaTxtReportElement();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaTxtReportElement();


   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintTableBegin(
         const string& caption,
         const vector<string>& colNames);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintTableEnd();

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintEmptyRow(
         int nofCols,
         const string& name);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintRow(
         const string& name1,
         const string& name2,
         const string& name3 = "",
         const string& name4 = "",
         const string& name5 = "",
         const string& name6 = "");

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintValue(
         const string& hist);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintNofStatistics(
         const string& name,
         const string& mvd,
         const string& sts,
         const string& rich,
         const string& trd,
         const string& muchP,
         const string& muchS,
         const string& tof);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintHits(
         const string& name,
         const string& hist);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintEfficiency(
         const string& name,
         const string& hist);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintEfficiencyRich(
         const string& name,
         const string& hist);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintEfficiencyElId(
         const string& name,
         const string& hist);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintDetAccEl(
         const string& name,
         const string& hist);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintPolarAngle(
         const string& hist);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintImage(
         const string& title,
         const string& file);

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintHead();

   /**
    * \brief Inherited from CbmLitQaBaseReportElement.
    */
   virtual string PrintCloseDocument();

private:
   int fColW; // column width

};

#endif /* CbmLitQaTxtReportElement_H_ */
