/**
 * \file CbmLitClusteringQaPTreeCreator.h
 * \brief Create property tree for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQAPTREECREATOR_H_
#define CBMLITCLUSTERINGQAPTREECREATOR_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
using std::string;
class CbmLitHistManager;

/**
 * \file CbmLitClusteringQaPTreeCreator.h
 * \brief Create property tree for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitClusteringQaPTreeCreator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQaPTreeCreator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaPTreeCreator();

   /**
    * \brief Create and return property for CbmLitHistManager.
    * \return property tree.
    */
   ptree Create(
         CbmLitHistManager* histManager);

private:

   void NofStatisticsToPTree(
      ptree& pt,
      const string& mvd,
      const string& sts,
      const string& rich,
      const string& trd,
      const string& muchP,
      const string& muchS,
      const string& tof) const;

   // Pointer to histogram manager
   CbmLitHistManager* fHM;
};

#endif /* CBMLITCLUSTERINGQAPTREECREATOR_H_ */