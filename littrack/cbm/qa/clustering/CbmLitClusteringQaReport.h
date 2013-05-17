/**
 * \file CbmLitClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQAREPORT_H_
#define CBMLITCLUSTERINGQAREPORT_H_

#include "CbmSimulationReport.h"
#include <string>
using std::string;

/**
 * \class CbmLitClusteringQaReport
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitClusteringQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaReport();

private:
   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Create();

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Draw();

   /**
    * \brief Print number of objects table.
    */
   string PrintNofObjects() const;

   string PrintMuchAcuracy() const;

   void DrawLogYHistogramsByPattern(
            const string& histNamePattern);

   void DrawHistogramsByLayer(const string& histNamePattern, const string& histName);

   void DrawNofObjectsHistigrams(
         const string& histNamePatternPixelHits,
         const string& histNamePatternPoints,
         const string& histNamePatternClusters,
         const string& histNamePatternDigis,
         const string& histName);

   void DrawResidualHistogrms();

   void ScaleHistogramsByNofEvents();

   ClassDef(CbmLitClusteringQaReport, 1)
};

#endif /* CBMLITCLUSTERINGQAREPORT_H_ */
