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

   /**
    * \brief Return string with number of objects statistics.
    * \return String with number of objects statistics.
    */
   string PrintNofObjects() const;

   /**
    * \brief Return string with hits histogram statistics (nof all, true, fake hits in track/ring).
    * \return String with hits histogram statistics (nof all, true, fake hits in track/ring).
    */
   string PrintTrackHits() const;

   /**
    * \brief Return string with tracking efficiency.
    * \param[in] includeRich True if RICH detector is included in the tracking efficiency table.
    * \return String with tracking efficiency.
    */
   string PrintTrackingEfficiency(
		   Bool_t includeRich) const;

   /**
    * \brief Return string with number of ghosts statistics.
    * \return String with number of ghosts statistics.
    */
   string PrintNofGhosts() const;

   CbmLitPropertyTree* fPT;
};

#endif /* CBMLITTRACKINGQAREPORT_H_ */
