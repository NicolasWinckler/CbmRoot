/**
 * \file CbmLitTrackingQaPTreeCreator.h
 * \brief Create property tree for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITTRACKINGQAPTREECREATOR_H_
#define CBMLITTRACKINGQAPTREECREATOR_H_

#include <string>
#include <boost/property_tree/ptree.hpp>

class CbmHistManager;

using std::string;
using boost::property_tree::ptree;

/**
 * \class CbmLitTrackingQaPTreeCreator
 * \brief Create property tree for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitTrackingQaPTreeCreator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackingQaPTreeCreator();

   /**
    * \brief Destructor.
    */
   ~CbmLitTrackingQaPTreeCreator();

   /**
    * \brief Print statistics to property tree and return property tree.
    */
   ptree Create(
         CbmHistManager* histManager);

private:

   void NofObjectsToPTree(
      ptree& pt);

   void TrackHitsToPTree(
         ptree& pt);

   void TrackingEfficiencyToPTree(
   		ptree& pt);

   void NofGhostsToPTree(
   		ptree& pt);

   CbmHistManager* fHM; // Pointer to histogram manager
};

#endif /* CBMLITTRACKINGQAPTREECREATOR_H_ */
