/**
 * \file CbmLitPropertyTree.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \since 2012
 */
#include "CbmLitPropertyTree.h"
#include <boost/regex.hpp>
using std::pair;

CbmLitPropertyTree::CbmLitPropertyTree(
		const ptree& pt)
{
	PropertyTreeToMap("", pt, fPM);
}

CbmLitPropertyTree::~CbmLitPropertyTree()
{

}

void CbmLitPropertyTree::PropertyTreeToMap(
      const string& path,
      const ptree& pt,
      map<string, Double_t>& mymap) const
{
   if (pt.size() == 0) {
      mymap.insert(pair<string, Double_t>(path, pt.get_value(-1.f)));
      return;
   }
   for (ptree::const_iterator it = pt.begin(); it != pt.end(); it++) {
      ptree::value_type v = *it;
      string path1 = (path != "") ? (path + "." + v.first) : v.first;
      PropertyTreeToMap(path1, v.second, mymap);
   }
}

map<string, Double_t> CbmLitPropertyTree::PropertyByPattern(
      const string& pattern) const
{
   map<string, Double_t> properties;
   map<string, Double_t>::const_iterator it;

   try {
		const boost::regex e(pattern);
		for (it = fPM.begin(); it != fPM.end(); it++) {
			if (boost::regex_match(it->first, e)) properties.insert(*it);
		}
   } catch (std::exception& ex) {
	   std::cout << "Exception in CbmLitPropertyTree::PropertyByPattern: " << ex.what() << std::endl;
   }
   return properties;
}
