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

class CbmLitHistManager;
class TH1F;
class TH2F;
class TH3F;

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
         CbmLitHistManager* histManager);

private:
   /**
    * \brief TODO ADD COMMENTS
    */
   void NofStatisticsToPTree(
      ptree* pt,
      const string& mvd,
      const string& sts,
      const string& rich,
      const string& trd,
      const string& muchP,
      const string& muchS,
      const string& tof);

   /**
    * \brief TODO ADD COMMENTS
    */
   void HitsHistosToPTree(
         ptree* pt,
         const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventEfficiencyStatisticsToPTree(
         ptree* pt,
         const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventEfficiencyStatisticsRichToPTree(
         ptree* pt,
         const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventEfficiencyStatisticsElIdToPTree(
         ptree* pt,
         const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventDetAccElStatisticsToPTree(
         ptree* pt,
         const string& name,
         const string& hmc,
         const string& hacc,
         const string& hrec);

   /**
    * \brief TODO ADD COMMENTS
    */
   void PolarAngleEfficiencyToPTree(
         ptree* pt,
         const string& name,
         const string& hist);

   CbmLitHistManager* fHM; // Pointer to histogram manager
};

#endif /* CBMLITTRACKINGQAPTREECREATOR_H_ */
