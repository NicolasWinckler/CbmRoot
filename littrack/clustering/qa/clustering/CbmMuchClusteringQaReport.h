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
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Create();

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Draw() {};

   /**
    *
    */
   string PrintNofObjects() const;
};

#endif /* CBMMUCHCLUSTERINGQAREPORT_H_ */
