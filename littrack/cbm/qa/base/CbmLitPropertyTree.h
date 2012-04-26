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

	map<string, Double_t> PropertyByPattern(
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

   map<string, Double_t> fPM; // Property map
};

#endif /* CBMLITPROPERTYTREE_H_ */
