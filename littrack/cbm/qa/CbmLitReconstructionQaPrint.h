/** CbmLitReconstructionQaPrint.h
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **
 **/

#ifndef CBMLITRECONSTRUCTIONQAPRINT_H_
#define CBMLITRECONSTRUCTIONQAPRINT_H_

#include "TObject.h"
#include <string>


#include <boost/property_tree/ptree.hpp>


class CbmLitReconstructionQaPrint
{
public:
   /* Constructor */
   CbmLitReconstructionQaPrint(){;}

   /* Destructor */
   virtual ~CbmLitReconstructionQaPrint(){;}

   //boost::property_tree::ptree* fPt;

   /* Prints event statistics to the out stream from property tree
    * @param out output stream
    * @param pt property tree*/
   static void PrintEventStatistics(
      std::ostream& out,
      boost::property_tree::ptree* pt);

   /* Prints final statistics to the out stream from property tree
    * @param out output stream
    * @param pt property tree*/
   static void PrintFinalStatistics(
      std::ostream& out,
      boost::property_tree::ptree* pt);

private:


   /* Prints nof points or hits or tracks or rings etc. statistics to string
    * @param  */
   static std::string PrintNofStatisticsToString(
      boost::property_tree::ptree* pt,
      const std::string& name,
      const std::string& mvd,
      const std::string& sts,
      const std::string& rich,
      const std::string& trd,
      const std::string& muchP,
      const std::string& muchS,
      const std::string& tof);

   /* Calculates integrated efficiencies and forms string with statistic information.
    * @param name histogram name
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   static std::string EventEfficiencyStatisticsToString(
      boost::property_tree::ptree* pt,
      const std::string& name,
      const std::string& opt);

   /* Calculates integrated efficiencies and forms string with statistic information for the RICH detector.
    * @param name histogram name
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   static std::string EventEfficiencyStatisticsRichToString(
      boost::property_tree::ptree* pt,
      const std::string& name,
      const std::string& opt);

   /* Calculates integrated efficiencies and forms string with electron identification statistic information.
    * @param name histogram name
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   static std::string EventEfficiencyStatisticsElIdToString(
      boost::property_tree::ptree* pt,
      const std::string& name,
      const std::string& opt);

   static std::string EventDetAccElStatisticsToString(
      boost::property_tree::ptree* pt,
      const std::string& effName,
      const std::string& name);

   /* Calculates integrated efficiencies for different polar angles
    * and returns a string with statistics.
    * @param name histogram name */
   static std::string PolarAngleEfficiencyToString(
      boost::property_tree::ptree* pt,
      const std::string& name);

};

#endif /* CBMLITRECONSTRUCTIONQAPRINT_H_ */
