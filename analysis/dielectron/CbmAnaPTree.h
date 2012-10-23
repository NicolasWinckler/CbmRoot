/**
 * \file CbmAnaPTree.h
 * \brief Wrapper for boost property tree.
 * Sometimes it is not possible to use ptree directly due to problems
 * with dictionary compilation.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \since 2012
 */

#ifndef CBM_ANA_P_TREE
#define CBM_ANA_P_TREE

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//#include <boost/regex.hpp>

using std::string;
using boost::property_tree::ptree;

/**
 * \class CbmAnaPTree
 * \brief Wrapper for boost property tree.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \since 2012
 */
class CbmAnaPTree
{
public:

	/**
	 * \brief Constructor.
	 */
 CbmAnaPTree() : fPt() { }

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmAnaPTree(){ }

	template <class T> void Put(
			const string& path,
			const T& value) {
	   fPt.put(path, value);
	};

	template <class T> T Get(
			const string& path,
			const T& defaultValue) const {
	   return fPt.get(path, defaultValue);
	};

	void Write(
	      const string& path)
	{
	   if (path != "") {
	      //gSystem->mkdir(path.c_str(), true);
	      write_json(path.c_str(), fPt);
	   }
	}

private:
   ptree fPt; // Property tree
};


#endif
