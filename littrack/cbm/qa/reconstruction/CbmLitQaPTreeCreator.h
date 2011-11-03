/*
 * CbmLitQaPTreeCreator.h
 *
 *  Created on: 17.10.2011
 *      Author: slebedev
 */

#ifndef CBMLITQAPTREECREATOR_H_
#define CBMLITQAPTREECREATOR_H_

#include "TObject.h"

#include <string>
#include <boost/property_tree/ptree.hpp>

class CbmLitQaHistManager;
class TH1F;
class TH2F;
class TH3F;

using namespace std;

class CbmLitQaPTreeCreator
{
public:
   CbmLitQaPTreeCreator(CbmLitQaHistManager* hm){fHM = hm;}
   ~CbmLitQaPTreeCreator(){;}

   /**
    * \brief Print statistics to property tree and return property tree.
    */
   boost::property_tree::ptree PrintPTree();

   Bool_t fIsElectronSetup; // If "electron" setup detected than true
   Bool_t fIsMvd; // If MVD detected than true
   Bool_t fIsSts; // If STS detected than true
   Bool_t fIsRich; // If RICH detected than true
   Bool_t fIsTrd; // If TRD detected than true
   Bool_t fIsMuch; // If MUCH detected than true
   Bool_t fIsTof; // If TOF detected than true
   string fOutputDir; // Output directory for images
   Double_t fMinAngle; // Minimum polar angle [grad]
   Double_t fMaxAngle; // Maximum polar angle [grad]
   Int_t fNofBinsAngle; // Number of bins for efficiency vs. polar angle histogram

private:
   CbmLitQaHistManager* fHM;

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
};

#endif /* CBMLITQAPTREECREATOR_H_ */
