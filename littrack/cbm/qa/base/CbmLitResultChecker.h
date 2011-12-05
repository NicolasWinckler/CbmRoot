/**
 * \file CbmLitResultChecker.h
 * \brief Automatic checker of QA results.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
#ifndef CBMLITRESULTCHECKER_H_
#define CBMLITRESULTCHECKER_H_

#include <boost/property_tree/ptree.hpp>
#include <map>

/**
 * \class CbmLitResultChecker
 *
 * \brief Automatic checker of QA results.
 *
 * Checking procedure compares two property trees:
 * "quality" property tree obtained from QA procedure vs.
 * "ideal results" property tree. Results are
 * stored in the third property tree as "0" - "1"
 * if value was compared to the ideal result
 * or "-1" if value was not compared.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitResultChecker
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitResultChecker();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitResultChecker();

   /**
    * \brief Checks reconstruction quality results.
    *
    * \param[in] qa Reconstruction quality results stored as property tree.
    * \param[in] ideal Ideal reconstruction quality results as property tree.
    * \param[out] out Output property tree.
    */
   void DoCheck(
         const boost::property_tree::ptree& qa,
         const boost::property_tree::ptree& ideal,
         boost::property_tree::ptree& out);

private:
   /**
    * \brief Builds recursively std::map out of property tree.
    *
    * \param[in] path Initial path. Must be "".
    * \param[in] pt Input property tree.
    * \param[out] mymap Output std::map<"property name", "value">.
    */
   void PropertyTreeToMap(
         const std::string& path,
         const boost::property_tree::ptree& pt,
         std::map<std::string, float>& mymap);
};

#endif /* CBMLITRESULTCHECKER_H_ */
