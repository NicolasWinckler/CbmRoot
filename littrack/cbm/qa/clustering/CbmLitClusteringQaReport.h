/**
 * \file CbmLitClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQAREPORT_H_
#define CBMLITCLUSTERINGQAREPORT_H_

#include "../report/CbmLitSimulationReport.h"
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
   CbmLitClusteringQaReport(LitReportType reportType);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaReport();

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   void Create(
      ostream& out,
      boost::property_tree::ptree* qa,
      boost::property_tree::ptree* ideal,
      boost::property_tree::ptree* check);

private:
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
