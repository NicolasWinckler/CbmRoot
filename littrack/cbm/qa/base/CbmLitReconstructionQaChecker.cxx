/**
 * \file CbmLitReconstructionQaChecker.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitReconstructionQaChecker.h"

#include <map>

CbmLitReconstructionQaChecker::CbmLitReconstructionQaChecker()
{

}

CbmLitReconstructionQaChecker::~CbmLitReconstructionQaChecker()
{

}

void CbmLitReconstructionQaChecker::DoCheck(
      const boost::property_tree::ptree& qa,
      const boost::property_tree::ptree& ideal,
      boost::property_tree::ptree& out)
{
   // Build map out of property tree for convenience.
   std::map<std::string, float> mymap;
   PropertyTreeToMap("", qa, mymap);

   // Iterate over the map, get each property and compare it to ideal.
   for (std::map<std::string, float>::const_iterator it = mymap.begin(); it != mymap.end(); it++) {
      std::map<std::string, float>::value_type v = *it;

      boost::optional<float> vmin = ideal.get_optional<float>(v.first + ".min");
      boost::optional<float> vmax = ideal.get_optional<float>(v.first + ".max");

      // Check if value exists in ideal
      if (!vmin || !vmax) {
//         std::cout << "Value " << v.first << " doesn't exist in ideal property tree." << std::endl;
         // -1 in the output tree indicates that value was not checked because
         // it was not found in ideal property tree
         out.put(v.first, -1.f);
         continue;
      } else {
         // Check that qa value lays within (min, max) limits
         if (v.second >= vmin && v.second <= vmax) {
            // Qa value is within limits
//            std::cout << "Value " << v.first << " is within limits." << std::endl;
            out.put(v.first, 1.f);
         } else {
            // Qa value is out of range
//            std::cout << "Value " << v.first << " is out of range." << std::endl;
            out.put(v.first, 0.f);
         }
      }
   }
}

void CbmLitReconstructionQaChecker::PropertyTreeToMap(
      const std::string& path,
      const boost::property_tree::ptree& pt,
      std::map<std::string, float>& mymap)
{
   if (pt.size() == 0) {
      mymap.insert(std::pair<std::string, float>(path, pt.get_value(-1.f)));
      return;
   }
   for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++) {
      boost::property_tree::ptree::value_type v = *it;
      std::string path1 = (path != "") ? (path + "." + v.first) : v.first;
      PropertyTreeToMap(path1, v.second, mymap);
   }
}
