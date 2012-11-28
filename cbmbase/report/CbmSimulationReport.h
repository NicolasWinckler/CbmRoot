/**
 * \file CbmSimulationReport.h
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMSIMULATIONREPORT_H_
#define CBMSIMULATIONREPORT_H_

#include "CbmReport.h"
#include <string>

using std::ostream;
using std::string;

class CbmHistManager;

/**
 * \class CbmSimulationReport
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmSimulationReport : public CbmReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmSimulationReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmSimulationReport();

   /**
    * \brief Main function which creates report data.
    *
    * Non virtual interface pattern is used here.
    * User always creates simulation report using this public non virtual method.
    * In order to configure concrete report generation class
    * user has to implement protected Create() method.
    *
    * \param[in] histManager Pointer to histogram manager for which report is created.
    * \param[in] outputDir Path to directory for output results.
    */
   void Create(
		   CbmHistManager* histManager,
		   const string& outputDir);

   /**
    * \brief Main function which creates report data.
    *
    * Same pattern is used here.
    *
    * \param[in] fileName Name of the file with histograms.
    * \param[in] outputDir Path to directory for output results.
    */
   void Create(
		   const string& fileName,
		   const string& outputDir);

   /**
    * \brief Return pointer to Histogram manager.
    */
   CbmHistManager* HM() const { return fHM; };

private:
   CbmHistManager* fHM; // Histogram manager

   ClassDef(CbmSimulationReport, 1)
};

#endif /* CBMSIMULATIONREPORT_H_ */
