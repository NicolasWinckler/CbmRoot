/**
 * \file CbmLitClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQAREPORT_H_
#define CBMLITCLUSTERINGQAREPORT_H_

#include "../report/CbmLitSimulationReport.h"
#include "TSystem.h"
#include <string>
using std::string;

/**
 * \class CbmLitClusteringQaReport
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitClusteringQaReport : public CbmLitSimulationReport
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
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual void Create(
      ostream& out);

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetQaFileName() const {
      return "clustering_qa.json";
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetIdealFileName() const {
      return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/clustering/clustering_qa_ideal.json");
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetCheckFileName() const {
      return "clustering_qa_check.json";
   }

   /**
    *
    */
   string PrintNofStatistics(
           const string& name,
           const string& mvd,
           const string& sts,
           const string& rich,
           const string& trd,
           const string& muchP,
           const string& muchS,
           const string& tof);
};

#endif /* CBMLITCLUSTERINGQAREPORT_H_ */
