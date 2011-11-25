/*
 * CbmLitQaHistCreator.h
 *
 *  Created on: 17.10.2011
 *      Author: slebedev
 */

#ifndef CBMLITQAHISTCREATOR_H_
#define CBMLITQAHISTCREATOR_H_

#include "TObject.h"

#include "qa/reconstruction/CbmLitQaHistNames.h"

#include <string>

class CbmLitQaHistManager;
class TFile;

using namespace std;

class CbmLitQaHistCreator
{
public:

   CbmLitQaHistCreator(){;}

   virtual ~CbmLitQaHistCreator(){;}

   void CreateEffHist(
         const string& name,
         const string& xTitle,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin,
         LitQaNameOption opt);

   void CreateEffHist3D(
         const string& name,
         LitQaNameOption opt);

   void Create1DHist(
         const string& name,
         const string& xTitle,
         const string& yTitle,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin);

   void Create2DHist(
         const string& name,
         Int_t nofBinsX,
         Double_t minBinX,
         Double_t maxBinX,
         Int_t nofBinsY,
         Double_t minBinY,
         Double_t maxBinY);

   CbmLitQaHistManager* ReadFromFile(
         TFile* file);

   CbmLitQaHistManager* Create();

   Double_t fMinMom; // Minimum momentum for tracks for efficiency calculation [GeV/c]
   Double_t fMaxMom; // Maximum momentum for tracks for efficiency calculation [GeV/c]
   Int_t fNofBinsMom; // Number of bins for efficiency vs. momentum histogram
   Double_t fMinPt; // Minimum Pt for tracks for efficiency calculation [GeV/c]
   Double_t fMaxPt; // Maximum Pt for tracks for efficiency calculation [GeV/c]
   Int_t fNofBinsPt; // Number of bins for efficiency vs. Pt histogram
   Double_t fMinY; // Minimum rapidity for tracks for efficiency calculation [GeV/c]
   Double_t fMaxY; // Maximum rapidity for tracks for efficiency calculation [GeV/c]
   Int_t fNofBinsY; // Number of bins for efficiency vs. rapidity histogram
   Double_t fMinAngle; // Minimum polar angle [grad]
   Double_t fMaxAngle; // Maximum polar angle [grad]
   Int_t fNofBinsAngle; // Number of bins for efficiency vs. polar angle histogram

private:
   CbmLitQaHistManager* fHM;


};

#endif /* CBMLITQAHISTCREATOR_H_ */
