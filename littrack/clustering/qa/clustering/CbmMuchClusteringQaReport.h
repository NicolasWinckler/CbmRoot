/**
 * \file CbmMuchClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMMUCHCLUSTERINGQAREPORT_H_
#define CBMMUCHCLUSTERINGQAREPORT_H_

#include "CbmSimulationReport.h"
#include "TSystem.h"
#include <string>
#include <sstream>
using std::string;
using std::stringstream;

class CbmLitPropertyTree;

/**
 * \class CbmLitClusteringQaReport
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmMuchClusteringQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmMuchClusteringQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMuchClusteringQaReport();

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
      return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/clustering/qa/clustering/clustering_qa_ideal.json");
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
   string PrintNofObjects() const;

   /**
    * \brief Print specified value.
    * \param[in] hist full name of the property in property tree.
    * \return string with table row in specific format.
    */
   string PrintValue(
         const string& hist) const {
      stringstream ss;
      ss << fQa.get(hist, -1.);
      return ss.str();
   }

   CbmLitPropertyTree* fPT;
};

#endif /* CBMMUCHCLUSTERINGQAREPORT_H_ */
