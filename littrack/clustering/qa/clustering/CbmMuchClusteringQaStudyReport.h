/**
 * \file CbmMuchClusteringQaStudyReport.h
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMMUCHCLUSTERINGQASTUDYREPORT_H_
#define CBMMUCHCLUSTERINGQASTUDYREPORT_H_

#include "../report/CbmStudyReport.h"
#include "TSystem.h"

/**
 * \class CbmLitClusteringQaStudyReport
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmMuchClusteringQaStudyReport : public CbmStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmMuchClusteringQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMuchClusteringQaStudyReport();

private:
   /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create();

   /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Draw() {};

   /**
    * \brief Print number of objects table.
    */
   string PrintNofObjects() const;
};

#endif /* CBMMUCHCLUSTERINGQASTUDYREPORT_H_ */
