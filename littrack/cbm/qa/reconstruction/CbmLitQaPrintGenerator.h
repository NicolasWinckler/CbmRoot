/** CbmLitQaPrintGenerator.h
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **
 **/

#ifndef CBMLITQAPRINTGENERATOR_H_
#define CBMLITQAPRINTGENARATOR_H_

#include "TObject.h"
#include <string>

#include <boost/property_tree/ptree.hpp>

using namespace std;

class CbmLitQaPrintGenerator
{
public:
   /* Constructor */
   CbmLitQaPrintGenerator(){;}

   /* Destructor */
   virtual ~CbmLitQaPrintGenerator(){;}

   //boost::property_tree::ptree* fPt;

   /* Prints event statistics to the out stream from property tree
    * @param out output stream
    * @param pt property tree*/
   static void PrintEventStatistics(
      ostream& out,
      boost::property_tree::ptree* pt);

   /* Prints final statistics to the out stream from property tree
    * @param out output stream
    * @param pt property tree*/
   static void PrintFinalStatistics(
      ostream& out,
      boost::property_tree::ptree* pt);

private:

   /* Prints nof points or hits or tracks or rings etc. statistics to string
    * @param  */
   static string PrintNofStatisticsToString(
      boost::property_tree::ptree* pt,
      const string& name,
      const string& mvd,
      const string& sts,
      const string& rich,
      const string& trd,
      const string& muchP,
      const string& muchS,
      const string& tof);

   /* Print hits histos statistics (nof all, true, fake hits in track/ring)
    * @param hist histogram name
    * @param name string of the first column in the table */
   static string PrintHitsStatisticsToString(
      boost::property_tree::ptree* pt,
      const string& hist,
      const string& name);

   /* Calculates integrated efficiencies and forms string with statistic information.
    * @param name histogram name
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   static string EventEfficiencyStatisticsToString(
      boost::property_tree::ptree* pt,
      const string& name,
      const string& opt = "final");

   /* Calculates integrated efficiencies and forms string with statistic information for the RICH detector.
    * @param name histogram name
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   static string EventEfficiencyStatisticsRichToString(
      boost::property_tree::ptree* pt,
      const string& name,
      const string& opt = "final");

   /* Calculates integrated efficiencies and forms string with electron identification statistic information.
    * @param name histogram name
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   static string EventEfficiencyStatisticsElIdToString(
      boost::property_tree::ptree* pt,
      const string& name,
      const string& opt = "final");

   static string EventDetAccElStatisticsToString(
      boost::property_tree::ptree* pt,
      const string& effName,
      const string& name);

   /* Calculates integrated efficiencies for different polar angles
    * and returns a string with statistics.
    * @param name histogram name */
   static string PolarAngleEfficiencyToString(
      boost::property_tree::ptree* pt,
      const string& name);
};

#endif /* CBMLITQAPRINTGENARATOR_H_ */
