/**
 * \file CbmLitReconstructionQaChecker.h
 *
 * \brief Automatic checker for reconstruction quality results.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
#ifndef CBMLITRECONSTRUCTIONQACHECKER_H_
#define CBMLITRECONSTRUCTIONQACHECKER_H_

#include <boost/property_tree/ptree.hpp>
#include <map>

/**
 * \class CbmLitReconstructionQaChecker
 *
 * \brief Automatic checker for reconstruction quality results.
 *
 * Checks reconstruction quality results.
 * The check is done by comparing two property trees:
 * "reconstruction quality" property tree vs.
 * "ideal results" property tree. Results are
 * stored in the third property tree as "0" - "1"
 * if value was compared to the ideal result
 * or "-1" if value was not compared.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitReconstructionQaChecker {
public:
   /**
    * \brief Constructor.
    */
   CbmLitReconstructionQaChecker();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitReconstructionQaChecker();

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

#endif /* CBMLITRECONSTRUCTIONQACHECKER_H_ */
