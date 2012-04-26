/**
 * \file CbmLitTrackingQaReport.h
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITTRACKINGQAREPORT_H_
#define CBMLITTRACKINGQAREPORT_H_

#include "../report/CbmLitSimulationReport.h"
#include "TSystem.h"
#include <string>
#include <sstream>
using std::string;
using std::stringstream;
class CbmLitPropertyTree;

/**
 * \class CbmLitTrackingQaReport
 * \brief Creates report for reconstruction performance.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitTrackingQaReport : public CbmLitSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackingQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQaReport();

protected:
   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual void Create(
      ostream& out);

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetQaFileName() const {
      return "tracking_qa.json";
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetIdealFileName() const {
      return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/tracking/tracking_qa_ideal.json");
   }

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   virtual string GetCheckFileName() const {
      return "tracking_qa_check.json";
   }

   string PrintNofObjects();

   /**
    * \brief Print specified value.
    * \param[in] hist full name of the property in property tree.
    * \return string with table row in specific format.
    */
   string PrintValue(
         const string& hist) {
      stringstream ss;
      ss << fQa.get(hist, -1.);
      return ss.str();
   }

   /**
    * \brief Print hits histogram statistics (nof all, true, fake hits in track/ring).
    */
   string PrintTrackHits();

   string PrintTrackingEfficiency(
		   Bool_t includeRich);

   string PrintNofGhosts();

//
//   /**
//    * \brief Print integrated efficiencies and forms string with electron identification statistic information.
//    * \param[in] name string of the first column in the table.
//    * \param[in] hist histogram name.
//    */
//   virtual string PrintEfficiencyElId(
//         const string& name,
//         const string& hist);
//
//   /**
//    * \brief Print detector acceptance efficiencies.
//    * \param[in] name string of the first column in the table.
//    * \param[in] hist histogram name.
//    */
//   virtual string PrintDetAccEl(
//         const string& name,
//         const string& hist);
//
//   /**
//    * \brief Calculate integrated efficiencies for different polar angles and returns a string with statistics.
//    * \param[in] name string of the first column in the table.
//    * \param[in] hist histogram name.
//    */
//   virtual string PrintPolarAngle(
//         const string& hist);

   CbmLitPropertyTree* fPT;
};

#endif /* CBMLITTRACKINGQAREPORT_H_ */
