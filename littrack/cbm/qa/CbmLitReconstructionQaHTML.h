/** CbmLitReconstructionQaHtml.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **
 **/

#ifndef CBMLITRECONSTRUCTIONQAHTML_H_
#define CBMLITRECONSTRUCTIONQAHTML_H_

#include <string>
#include <boost/property_tree/ptree.hpp>

class CbmLitReconstructionQaHTML
{
public:
   /* Constructor */
   CbmLitReconstructionQaHTML(){;}

   /* Destructor */
   virtual ~CbmLitReconstructionQaHTML(){;}

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
      int row,
      boost::property_tree::ptree* pt,
      const std::string& name,
      const std::string& mvd,
      const std::string& sts,
      const std::string& rich,
      const std::string& trd,
      const std::string& muchP,
      const std::string& muchS,
      const std::string& tof);

   /* Print hits histos statistics (nof all, true, fake hits in track/ring)
    * @param row Row number in the table
    * @param pt Pointer to the property_tree
    * @param hist histogram name
    * @param name string of the first column in the table */
   static std::string PrintHitsStatisticsToString(
      int row,
      boost::property_tree::ptree* pt,
      const std::string& hist,
      const std::string& name);

   /* Calculates integrated efficiencies and forms string with statistic information.
    * @param name histogram name
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   static std::string EventEfficiencyStatisticsToString(
      boost::property_tree::ptree* pt,
      const std::string& name);

   /* Calculates integrated efficiencies and forms string with statistic information for the RICH detector.
    * @param name histogram name
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   static std::string EventEfficiencyStatisticsRichToString(
      boost::property_tree::ptree* pt,
      const std::string& name);

   /* Calculates integrated efficiencies and forms string with electron identification statistic information.
    * @param name histogram name
    * @param opt if 'event' than forms string for event statistics, if 'final' than forms the string with final statistics. */
   static std::string EventEfficiencyStatisticsElIdToString(
      boost::property_tree::ptree* pt,
      const std::string& name);

   static std::string EventDetAccElStatisticsToString(
      int row,
      boost::property_tree::ptree* pt,
      const std::string& effName,
      const std::string& name);

   /* Calculates integrated efficiencies for different polar angles
    * and returns a string with statistics.
    * @param name histogram name */
   static std::string PolarAngleEfficiencyToString(
      boost::property_tree::ptree* pt,
      const std::string& name);

   /* Return string with CSS style for the HTML page */
   static std::string HtmlHeadString();
};

#endif /* CBMLITRECONSTRUCTIONQAHTML_H_ */
