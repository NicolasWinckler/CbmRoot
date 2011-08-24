/**
 * \file CbmLitQaHTMLGenerator.h
 *
 * \brief Creates HTML page out of property trees.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITQAHTMLGENERATOR_H_
#define CBMLITQAHTMLGENERATOR_H_

#include <string>
#include <boost/property_tree/ptree.hpp>

/**
 * \class CbmLitQaHTMLGenerator
 *
 * \brief Creates HTML page out of property trees.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaHTMLGenerator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaHTMLGenerator() {
      fErrorColor = "#FF0000";
      fWarningColor = "#FFFF00";
      fNormalColor = "#00FF00";
   }

   /**
    * \brief Destructor
    */
   virtual ~CbmLitQaHTMLGenerator(){}

   /**
    * \brief Creates HTML with final statistics.
    *
    * \param[out] out Output stream.
    * \param[in] qa Property tree with Qa results.
    * \param[in] ideal Property tree with ideal values.
    * \param[in] check Property tree with checked results.
    */
   void Create(
      std::ostream& out,
      const boost::property_tree::ptree* qa,
      const boost::property_tree::ptree* ideal,
      const boost::property_tree::ptree* check);

private:

   /**
    * \brief Prints number of points or hits or tracks or rings etc. to std::string.
    *
    * \param[in] row Row number in the table.
    * \param[in] name Name of the object i.e. Points, Hits, Tracks etc.
    * \param[in] mvd Property name which stores number of objects for MVD.
    * \param[in] sts Property name which stores number of objects for STS.
    * \param[in] rich Property name which stores number of objects for RICH.
    * \param[in] trd Property name which stores number of objects for TRD.
    * \param[in] muchP Property name which stores number of objects for MUCH pixels.
    * \param[in] muchS Property name which stores number of objects for MUCH straws.
    * \param[in] tof Property name which stores number of objects for TOF.
    * \return std::string with number of objects.
    */
   std::string PrintNofStatisticsToString(
      int row,
      const std::string& name,
      const std::string& mvd,
      const std::string& sts,
      const std::string& rich,
      const std::string& trd,
      const std::string& muchP,
      const std::string& muchS,
      const std::string& tof);

   /**
    * \brief Prints hits histograms statistics (number of all, true, fake hits in track or ring).
    *
    * \param[in] row Row number in the table.
    * \param[in] hist Histogram name.
    * \param[in] name Name of the row in table.
    * \return std::string with hits histogram statistics.
    */
   std::string PrintHitsStatisticsToString(
      int row,
      const std::string& hist,
      const std::string& name);

   /**
    * \brief Prints efficiency statistics to string (without RICH detector).
    *
    * \param[in] name Histogram name.
    * \return std::string with efficiencies.
    */
   std::string EventEfficiencyStatisticsToString(
      const std::string& name);

   /**
    * \brief Prints efficiency statistics to string (with RICH detector).
    *
    * \param[in] name Histogram name.
    * \return std::string with efficiencies.
    */
   std::string EventEfficiencyStatisticsRichToString(
      const std::string& name);

   /**
    * \brief Prints efficiency statistics for electron identification to string.
    *
    * \param[in] name Histogram name.
    * \return std::string with efficiencies.
    */
   std::string EventEfficiencyStatisticsElIdToString(
      const std::string& name);

   /**
    * \brief Prints detector acceptance for primary electrons to string.
    *
    * \param[in] row Row number in the table.
    * \param[in] effName Name of the row in the table.
    * \param[in] name Name of the histogram.
    * \return std::string with detector acceptance.
    */
   std::string EventDetAccElStatisticsToString(
      int row,
      const std::string& effName,
      const std::string& name);

   /**
    * \brief Prints efficiencies for different polar angles to string.
    *
    * \param[in] name Histogram name.
    * \return std::string with efficiencies.
    */
   std::string PolarAngleEfficiencyToString(
      const std::string& name);

   /**
    * \brief Print property value from property tree to std::string.
    *
    * Uniforms output style of properties. Colors background
    * in dependence on the correctness of the property.
    *
    * \param[in] valueTitle Property title which is printed before the property value.
    * \param[in] valueName Property name in the property tree.
    * \param[in] tag HTML tag which is used to print the property i.e. "p", "b", "td".
    * \return std::string with property in HTML format.
    */
   std::string PrintValue(
         const std::string& valueTitle,
         const std::string& valueName,
         const std::string& tag);

   /**
    * \brief Return string with CSS style for the HTML page.
    * \return std::string with CSS style for the HTML page.
    */
   std::string HtmlHeadString();

private:
   // Property tree of Qa results
   const boost::property_tree::ptree* fQa;
   // Property with ideal values
   const boost::property_tree::ptree* fIdeal;
   // Property tree with checked results
   const boost::property_tree::ptree* fCheck;
   // Background colors for error highlighting
   std::string fErrorColor; // error
   std::string fWarningColor; // warning
   std::string fNormalColor; // normal
};
#endif /* CBMLITQAHTMLGENERATOR_H_ */
