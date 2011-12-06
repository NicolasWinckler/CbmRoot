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
   CbmLitTrackingQaPTreeCreator(CbmLitHistManager* hm) : fHM(hm){}

   /**
    * \brief Destructor.
    */
   ~CbmLitTrackingQaPTreeCreator(){}

   /**
    * \brief Print statistics to property tree and return property tree.
    */
   boost::property_tree::ptree PrintPTree();

   bool fIsElectronSetup; // If "electron" setup detected than true
   bool fIsMvd; // If MVD detected than true
   bool fIsSts; // If STS detected than true
   bool fIsRich; // If RICH detected than true
   bool fIsTrd; // If TRD detected than true
   bool fIsMuch; // If MUCH detected than true
   bool fIsTof; // If TOF detected than true
   string fOutputDir; // Output directory for images
   float fMinAngle; // Minimum polar angle [grad]
   float fMaxAngle; // Maximum polar angle [grad]
   int fNofBinsAngle; // Number of bins for efficiency vs. polar angle histogram

private:
   /**
    * \brief TODO ADD COMMENTS
    */
   TH1F* H1(
      const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   TH2F* H2(
      const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   TH3F* H3(
      const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   void NofStatisticsToPTree(
      boost::property_tree::ptree* pt,
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
         boost::property_tree::ptree* pt,
         const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventEfficiencyStatisticsToPTree(
         boost::property_tree::ptree* pt,
         const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventEfficiencyStatisticsRichToPTree(
         boost::property_tree::ptree* pt,
         const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventEfficiencyStatisticsElIdToPTree(
         boost::property_tree::ptree* pt,
         const string& name);

   /**
    * \brief TODO ADD COMMENTS
    */
   void EventDetAccElStatisticsToPTree(
         boost::property_tree::ptree* pt,
         const string& name,
         const string& hmc,
         const string& hacc,
         const string& hrec);

   /**
    * \brief TODO ADD COMMENTS
    */
   void PolarAngleEfficiencyToPTree(
         boost::property_tree::ptree* pt,
         const string& name,
         const string& hist);

   // Pointer to histogram manager
   CbmLitHistManager* fHM;
};

#endif /* CBMLITTRACKINGQAPTREECREATOR_H_ */
