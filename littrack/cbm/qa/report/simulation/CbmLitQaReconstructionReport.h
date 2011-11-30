/**
 * \file CbmLitQaReconstructionReport.h
 *
 * \brief Creates HTML page for sudy of the reconstruction performance.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CbmLitQaReconstructionReport_H_
#define CbmLitQaReconstructionReport_H_

#include "CbmLitQaBaseReport.h"

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

using namespace std;
/**
 * \class CbmLitQaReconstructionReport
 *
 * \brief Creates HTML page for study of the reconstruction performance.
 *
 * Generates report from a number of results of different
 * simulations and reconstructions. Creates comparison tables.
 * Very useful for studies since all numbers are automatically
 * put in the comparison tables.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaReconstructionReport : public CbmLitQaBaseReport
{
public:
   /**
    * \brief Constructor.
    * \param[in] type type of the report. "latex" or "html"
    */
   CbmLitQaReconstructionReport(
         const string& type);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaReconstructionReport();

protected:

   /**
    * \brief Creates HTML page and writes output to ostream.
    * \param[out] out ostream for output.
    */
   void Create(
      ostream& out);

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

#endif /* CbmLitQaReconstructionReport_H_ */
