/**
 * \file CbmLitPropertyTree.h
 * \brief Wrapper for boost property tree.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \since 2012
 */

#ifndef CBMLITPROPERTYTREE_H_
#define CBMLITPROPERTYTREE_H_

#include "TObject.h"
#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/regex.hpp>

using std::string;
using std::map;
using boost::property_tree::ptree;

/**
 * \class CbmLitPropertyTree
 * \brief Wrapper for boost property tree.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \since 2012
 *
 * Helps to search for properties based on regular expressions.
 */
class CbmLitPropertyTree
{
public:
	/**
	 * \brief Constructor.
	 */
	CbmLitPropertyTree(
			const ptree& pt);

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmLitPropertyTree();

	template <class T> void Put(
			const string& path,
			const T& value) {};

	template <class T> T Get(
			const string& path,
			const T& defaultValue) const { return defaultValue; };

	string GetNumberAsString(
			const string& path,
			Double_t defaultValue) const { return ""; };

	template <class T> map<string, T> GetByPattern(
	      const string& pattern) const;

private:
   /**
    * \brief Build recursively map out of property tree.
    * \param[in] path Initial path. Must be "".
    * \param[in] pt Input property tree.
    * \param[out] mymap Output map<"property name", "value">.
    */
   void PropertyTreeToMap(
         const string& path,
         const ptree& pt,
         map<string, Double_t>& mymap) const;

   map<string, Double_t> fPM; // Property map, used to store property tree as a map
   ptree fPT; // Property tree
};


template <class T>
map<string, T> CbmLitPropertyTree::GetByPattern(
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

#endif /* CBMLITPROPERTYTREE_H_ */
