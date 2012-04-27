/**
 * \file CbmLitPropertyTree.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \since 2012
 */
#include "CbmLitPropertyTree.h"
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


//template int X<int>::v(int);
//template map<string, Double_t> CbmLitPropertyTree<Double_t>::GetByPattern(const& string);
