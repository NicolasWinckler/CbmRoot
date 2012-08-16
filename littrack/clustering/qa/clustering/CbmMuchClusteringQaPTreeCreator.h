/**
 * \file CbmMuchClusteringQaPTreeCreator.h
 * \brief Create property tree for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMMUCHCLUSTERINGQAPTREECREATOR_H_
#define CBMMUCHCLUSTERINGQAPTREECREATOR_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
using std::string;
class CbmHistManager;

/**
 * \file CbmLitClusteringQaPTreeCreator.h
 * \brief Create property tree for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmMuchClusteringQaPTreeCreator
{
public:
   /**
    * \brief Constructor.
    */
   CbmMuchClusteringQaPTreeCreator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMuchClusteringQaPTreeCreator();

   /**
    * \brief Create and return property for CbmLitHistManager.
    * \return property tree.
    */
   ptree Create(
         CbmHistManager* histManager);

private:

   void NofObjectsToPTree(
      ptree& pt);

   // Pointer to histogram manager
   CbmHistManager* fHM;
};

#endif /* CBMMUCHCLUSTERINGQAPTREECREATOR_H_ */
