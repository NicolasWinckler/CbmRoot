/**
 * \file CbmStsSimulationQaReport.h
 * \brief Simulation report for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */

#ifndef CBMSTSSIMULATIONQAREPORT_H_
#define CBMSTSSIMULATIONQAREPORT_H_

#include "CbmSimulationReport.h"
#include "TSystem.h"
#include <string>
#include <sstream>
using std::string;
using std::stringstream;

class CbmLitPropertyTree;

/**
 * \class CbmStsSimulationQaReport
 * \brief Simulation report for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */
class CbmStsSimulationQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmStsSimulationQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmStsSimulationQaReport();

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
      return "sts_simulation_qa.json";
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetIdealFileName() const {
      return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/sts/sts_simulation_qa_ideal.json");
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetCheckFileName() const {
      return "sts_simulation_qa_check.json";
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

#endif /* CBMSTSSIMULATIONQAREPORT_H_ */
