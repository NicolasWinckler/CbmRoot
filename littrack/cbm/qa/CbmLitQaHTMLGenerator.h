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

#include "CbmLitQaBaseGenerator.h"
#include <string>

/**
 * \class CbmLitQaHTMLGenerator
 *
 * \brief Creates HTML page out of property trees.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaHTMLGenerator : public CbmLitQaBaseGenerator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaHTMLGenerator() {}

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
    * \param[in] rowName Name of the row in the table.
    * \param[in] detName Name of the detector "Mvd", "Sts", "Trd" etc.
    * \return std::string with number of objects.
    */
   std::string PrintRowNofObjects(
      int row,
      const std::string& rowName,
      const std::string& detName);

   /**
    * \brief Prints hits histograms statistics (number of all, true, fake hits in track or ring).
    *
    * \param[in] row Row number in the table.
    * \param[in] rowName Name of the row in table.
    * \param[in] histName Histogram name.
    * \return std::string with hits histogram statistics.
    */
   std::string PrintRowHits(
      int row,
      const std::string& rowName,
      const std::string& histName);

   /**
    * \brief Prints efficiency statistics to string (without RICH detector).
    *
    * \param[in] row Row number in the table.
    * \param[in] rowName Name of the row in table.
    * \param[in] histName Histogram name.
    * \return std::string with efficiencies.
    */
   std::string PrintRowEfficiency(
         int row,
         const std::string& rowName,
         const std::string& histName);

   /**
    * \brief Prints efficiency statistics to string (with RICH detector).
    *
    * \param[in] row Row number in the table.
    * \param[in] rowName Name of the row in table.
    * \param[in] histName Histogram name.
    * \return std::string with efficiencies.
    */
   std::string PrintRowEfficiencyRich(
         int row,
         const std::string& rowName,
         const std::string& histName);

   /**
    * \brief Prints efficiency statistics for electron identification to string.
    *
    * \param[in] row Row number in the table.
    * \param[in] rowName Name of the row in table.
    * \param[in] histName Histogram name.
    * \return std::string with efficiencies.
    */
   std::string PrintRowEfficiencyElId(
         int row,
         const std::string& rowName,
         const std::string& histName);

   /**
    * \brief Prints detector acceptance for primary electrons to string.
    *
    * \param[in] row Row number in the table.
    * \param[in] rowName Name of the row in table.
    * \param[in] histName Histogram name.
    * \return std::string with detector acceptance.
    */
   std::string PrintRowDetAccEl(
      int row,
      const std::string& rowName,
      const std::string& histName);

   /**
    * \brief Prints efficiencies for different polar angles to string.
    *
    * \param[in] histName Histogram name.
    * \return std::string with efficiencies.
    */
   std::string PrintPolarAngleEfficiency(
      const std::string& histName);

};
#endif /* CBMLITQAHTMLGENERATOR_H_ */
