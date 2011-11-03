/**
 * \file CbmLitFieldQaHTMLGenerator.h
 *
 * \brief Creates HTML page out of property tree for field QA.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITFIELDQAHTMLGENERATOR_H_
#define CBMLITFIELDQAHTMLGENERATOR_H_

#include "reconstruction/CbmLitQaBaseGenerator.h"
#include <string>
#include <boost/property_tree/ptree.hpp>

/**
 * \class CbmLitFieldQaHTMLGenerator
 *
 * \brief Creates HTML page out of property tree for field QA.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitFieldQaHTMLGenerator : public CbmLitQaBaseGenerator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFieldQaHTMLGenerator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldQaHTMLGenerator();

   /**
    * \brief Main function which creates HTML.
    *
    * \param[out] out Output stream.
    * \param[in] qa Property tree with field Qa results.
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
    * \brief Prints summary table for field QA in HTML format.
    * \return String with HTML.
    */
   std::string PrintSummaryTable();

   /**
    * \brief Prints table row which spans over all columns.
    * \param[in] row Row counter. Used to highlight row.
    * \param[in] ncols Number of columns to span.
    * \param[in] name Name of the row.
    * \return std::string with table row in HTML format.
    */
   std::string PrintEmptyRow(
         int row,
         int ncols,
         const std::string& name);
};

#endif /* CBMLITFIELDQAHTMLGENERATOR_H_ */
