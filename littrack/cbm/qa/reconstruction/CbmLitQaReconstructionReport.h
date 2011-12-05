/**
 * \file CbmLitQaReconstructionReport.h
 * \brief Creates report for reconstruction performance.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITQARECONSTRUCTIONREPORT_H_
#define CBMLITQARECONSTRUCTIONREPORT_H_

#include "../report/CbmLitSimulationReport.h"
#include <string>
#include <sstream>
using std::string;
using std::stringstream;

/**
 * \class CbmLitQaReconstructionReport
 * \brief Creates report for reconstruction performance.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitQaReconstructionReport : public CbmLitSimulationReport
{
public:
   /**
    * \brief Constructor with report type.
    * \param[in] reportType Type of report to be produced.
    */
   CbmLitQaReconstructionReport(LitReportType reportType);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaReconstructionReport();

   /**
    * \brief Inherited from CbmLitSimulationReport.
    */
   void Create(
      ostream& out,
      boost::property_tree::ptree* qa,
      boost::property_tree::ptree* ideal,
      boost::property_tree::ptree* check);

protected:

   /**
    * \brief Prints specified value.
    * \param[in] hist full name of the property in property tree.
    * \return string with table row in specific format.
    */
   string PrintValue(
         const string& hist)
   {
      stringstream ss;
      ss << fQa->get(hist, -1.);
      return ss.str();
   }

   /**
    * \brief Print number of points or hits or tracks or rings etc. to string.
    */
   virtual string PrintNofStatistics(
         const string& name,
         const string& mvd,
         const string& sts,
         const string& rich,
         const string& trd,
         const string& muchP,
         const string& muchS,
         const string& tof);

   /**
    * \brief Print hits histogram statistics (nof all, true, fake hits in track/ring).
    * \param[in] name String of the first column in the table.
    * \param[in] hist Histogram name.
    */
   virtual string PrintHits(
         const string& name,
         const string& hist);

   /**
    * \brief Calculate integrated efficiencies and forms string with statistic information.
    * \param[in] name String of the first column in the table.
    * \param[in] hist Histogram name.
    */
   virtual string PrintEfficiency(
         const string& name,
         const string& hist);

   /**
    * \brief Calculate integrated efficiencies and forms string with statistic information for the RICH detector.
    * \param[in] name String of the first column in the table.
    * \param[in] hist Histogram name.
    */
   virtual string PrintEfficiencyRich(
         const string& name,
         const string& hist);

   /**
    * \brief Print integrated efficiencies and forms string with electron identification statistic information.
    * \param[in] name string of the first column in the table.
    * \param[in] hist histogram name.
    */
   virtual string PrintEfficiencyElId(
         const string& name,
         const string& hist);

   /**
    * \brief Print detector acceptance efficiencies.
    * \param[in] name string of the first column in the table.
    * \param[in] hist histogram name.
    */
   virtual string PrintDetAccEl(
         const string& name,
         const string& hist);

   /**
    * \brief Calculate integrated efficiencies for different polar angles and returns a string with statistics.
    * \param[in] name string of the first column in the table.
    * \param[in] hist histogram name.
    */
   virtual string PrintPolarAngle(
         const string& hist);
};

#endif /* CBMLITQARECONSTRUCTIONREPORT_H_ */
