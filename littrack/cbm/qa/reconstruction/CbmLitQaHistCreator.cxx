/*
 * CbmLitQaHistCreator.cxx
 *
 *  Created on: 17.10.2011
 *      Author: slebedev
 */
#include "qa/reconstruction/CbmLitQaHistCreator.h"
#include "qa/reconstruction/CbmLitQaHistNames.h"
#include "qa/CbmLitHistManager.h"

#include "TH1.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TClass.h"

#include <iostream>

using namespace std;

void CbmLitQaHistCreator::CreateEffHist(
      const string& name,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin,
      LitQaNameOption opt)
{
   const vector<string>& types = CbmLitQaHistNames::GetTypes(opt);
   const vector<string>& cat = CbmLitQaHistNames::GetCategories(opt);

   for (Int_t i = 0; i < cat.size(); i++) {
      for (Int_t j = 0; j < types.size(); j++) {
         string histName = name + "_" + cat[i] + "_" + types[j];
         fHM->Add(histName, new TH1F(histName.c_str(), histName.c_str(), nofBins, minBin, maxBin));
      }
   }
}

void CbmLitQaHistCreator::CreateEffHist3D(
      const string& name,
      LitQaNameOption opt)
{
   const vector<string>& types = CbmLitQaHistNames::GetTypes(opt);
   const vector<string>& cat = CbmLitQaHistNames::GetCategories(opt);

   for (Int_t i = 0; i < cat.size(); i++) {
      for (Int_t j = 0; j < types.size(); j++) {
         string histName = name + "_" + cat[i] + "_" + types[j];
         fHM->Add(histName, new TH3F(histName.c_str(), histName.c_str(),
               fNofBinsMom, fMinMom, fMaxMom,
               fNofBinsY, fMinY, fMaxY,
               fNofBinsPt, fMinPt, fMaxPt));
      }
   }
}


void CbmLitQaHistCreator::Create1DHist(
      const string& name,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin)
{
   TH1F* h = new TH1F(name.c_str(), name.c_str(), nofBins, minBin, maxBin);
   fHM->Add(name, h);
}

void CbmLitQaHistCreator::Create2DHist(
      const string& name,
      Int_t nofBinsX,
      Double_t minBinX,
      Double_t maxBinX,
      Int_t nofBinsY,
      Double_t minBinY,
      Double_t maxBinY)
{
   TH2F* h = new TH2F(name.c_str(), name.c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
   fHM->Add(name, h);
}


CbmLitQaHistManager* CbmLitQaHistCreator::ReadFromFile(
      TFile* file)
{
   cout<< "ReadAllHistosFromFile" <<endl;
   fHM = new CbmLitQaHistManager();

   TDirectory* dir = gDirectory;
   TIter nextkey( dir->GetListOfKeys());
   TKey *key;
   Int_t c = 0;
   while (key = (TKey*) nextkey()) {
      TObject* obj = key->ReadObj();
      if (obj->IsA()->InheritsFrom (TH1::Class())) {
         TH1* h = (TH1*) obj;
         TH1* h1 = (TH1*)file->Get(h->GetName());
         fHM->Add(h->GetName(), h1);
         cout << c++ << " " << h->GetName()<< endl;
      }
   }
   return fHM;
}

CbmLitQaHistManager* CbmLitQaHistCreator::Create()
{
   fHM = new CbmLitQaHistManager();

   // Number of points distributions
   Double_t minNofPoints =  0.;
   Double_t maxNofPoints = 100.;
   Int_t nBinsNofPoints = 100;

   // Reconstruction efficiency histograms
   CreateEffHist3D("hSts3D", kTracking);
   CreateEffHist3D("hSts3DNormHalfGlobal", kTracking);
   CreateEffHist3D("hSts3DNormGlobal", kTracking);
   CreateEffHist("hStsNp", nBinsNofPoints, minNofPoints, maxNofPoints, kTracking);
   CreateEffHist("hStsAngle", fNofBinsAngle, fMinAngle, fMaxAngle, kTracking);
   CreateEffHist3D("hHalfGlobal3D", kTracking);
   CreateEffHist3D("hHalfGlobal3DNormGlobal", kTracking);
   CreateEffHist3D("hGlobal3D", kTracking);
   CreateEffHist3D("hRec3D", kTracking);
   CreateEffHist("hRecNp", nBinsNofPoints, minNofPoints, maxNofPoints, kTracking);
   CreateEffHist("hRecAngle", fNofBinsAngle, fMinAngle, fMaxAngle, kTracking);
   CreateEffHist3D("hTof3D", kTracking);
   CreateEffHist("hTofAngle", fNofBinsAngle, fMinAngle, fMaxAngle, kTracking);

   CreateEffHist3D("hRich3D", kRich);
   CreateEffHist("hRichNh", nBinsNofPoints, minNofPoints, maxNofPoints, kRich);

   CreateEffHist3D("hSts3DNormStsRich", kRich);
   CreateEffHist3D("hStsRich3D", kRich);
   CreateEffHist3D("hStsRichNoMatching3D", kRich);

   CreateEffHist3D("hSts3DNormStsRichTrd", kRich);
   CreateEffHist3D("hStsRich3DNormStsRichTrd", kRich);
   CreateEffHist3D("hStsRichTrd3D", kRich);

   CreateEffHist3D("hSts3DNormStsRichTrdTof", kRich);
   CreateEffHist3D("hStsRich3DNormStsRichTrdTof", kRich);
   CreateEffHist3D("hStsRichTrd3DNormStsRichTrdTof", kRich);
   CreateEffHist3D("hStsRichTrdTof3D", kRich);

   //Electron identification efficiency histograms
   CreateEffHist3D("hStsTrd3DElId", kElid);
   CreateEffHist3D("hStsTrd3DElIdNormStsTrdTof", kElid);
   CreateEffHist3D("hStsTrdTof3DElId", kElid);
   CreateEffHist3D("hStsRich3DElId", kElid);
   CreateEffHist3D("hStsRich3DElIdNormStsRichTrd", kElid);
   CreateEffHist3D("hStsRichTrd3DElId", kElid);
   CreateEffHist3D("hStsRich3DElIdNormStsRichTrdTof", kElid);
   CreateEffHist3D("hStsRichTrd3DElIdNormStsRichTrdTof", kElid);
   CreateEffHist3D("hStsRichTrdTof3DElId", kElid);

   // Detector acceptance histogramm
   CreateEffHist3D("hStsDetAcc3D", kDetAcc);
   CreateEffHist3D("hStsRichDetAcc3D", kDetAcc);
   CreateEffHist3D("hStsTrdDetAcc3D", kDetAcc);
   CreateEffHist3D("hStsTofDetAcc3D", kDetAcc);
   CreateEffHist3D("hStsRichTrdDetAcc3D", kDetAcc);
   CreateEffHist3D("hStsRichTrdTofDetAcc3D", kDetAcc);
   CreateEffHist3D("hStsTrdTofDetAcc3D", kDetAcc);

   //Create histograms for ghost tracks
   Create1DHist("hStsGhostNh", nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hRecGhostNh", nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hRichGhostNh", nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hRichGhostStsMatchingNh", nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hRichGhostElIdNh", nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hStsGhostRichMatchingNh", nBinsNofPoints, minNofPoints, maxNofPoints);

   const UInt_t maxNofStations = 30;
   Create1DHist("hMvdNofHitsInStation", maxNofStations, 0, maxNofStations);
   Create1DHist("hStsNofHitsInStation", maxNofStations, 0, maxNofStations);
   Create1DHist("hTrdNofHitsInStation", maxNofStations, 0, maxNofStations);
   Create1DHist("hMuchNofHitsInStation", maxNofStations, 0, maxNofStations);
   Create1DHist("hTofNofHitsInStation", maxNofStations, 0, maxNofStations);

   const UInt_t nofHitsHistos = 5;
   std::string hittype[] = { "All", "True", "Fake", "TrueOverAll", "FakeOverAll" };
   Double_t hitmin[] = {0, 0, 0, -0.1, -0.1};
   Double_t hitmax[] = {20, 20, 20, 1.1, 1.1};
   Int_t hitbins[] = {20, 20, 20, 12, 12};
   Double_t hitmaxrich[] = {50, 50, 50, 1.1, 1.1};
   Int_t hitbinsrich[] = {50, 50, 50, 12, 12};
   for(UInt_t i = 0; i < nofHitsHistos; i++) {
      string histName1 = "hMvdTrackHits_" + hittype[i];
      string histName2 = "hStsTrackHits_" + hittype[i];
      string histName3 = "hTrdTrackHits_" + hittype[i];
      string histName4 = "hMuchTrackHits_" + hittype[i];
      string histName5 = "hRichRingHits_" + hittype[i];

      Create1DHist(histName1.c_str(), hitbins[i], hitmin[i], hitmax[i]);
      Create1DHist(histName2.c_str(), hitbins[i], hitmin[i], hitmax[i]);
      Create1DHist(histName3.c_str(), hitbins[i], hitmin[i], hitmax[i]);
      Create1DHist(histName4.c_str(), hitbins[i], hitmin[i], hitmax[i]);
      Create1DHist(histName5.c_str(), hitbinsrich[i], hitmin[i], hitmaxrich[i]);
   }

   Int_t nofBinsC = 1000;
   Double_t maxXC = 50000.;
   Create1DHist("hNofGlobalTracks", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsTracks", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdTracks", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchTracks", nofBinsC, 1., maxXC);
   Create1DHist("hNofRichRings", nofBinsC, 1., maxXC);
   Create1DHist("hNofRichProjections", nofBinsC, 1., maxXC);

   Create1DHist("hNofMvdHits", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsHits", nofBinsC, 1., maxXC);
   Create1DHist("hNofRichHits", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdHits", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchPixelHits", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchStrawHits", nofBinsC, 1., maxXC);
   Create1DHist("hNofTofHits", nofBinsC, 1., maxXC);

   Create1DHist("hNofMvdPoints", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsPoints", nofBinsC, 1., maxXC);
   Create1DHist("hNofRichPoints", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdPoints", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchPoints", nofBinsC, 1., maxXC);
   Create1DHist("hNofTofPoints", nofBinsC, 1., maxXC);

   Create1DHist("hNofMvdDigis", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsDigis", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchDigis", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdDigis", nofBinsC, 1., maxXC);

   Create1DHist("hNofMvdClusters", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsClusters", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchClusters", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdClusters", nofBinsC, 1., maxXC);

   Create1DHist("hStsChiprim", 150, 0., 15.);
   Create2DHist("hStsMomresVsMom", 120, 0., 12., 100, -15., 15.);
   Create1DHist("hTrackLength", 40, -2., 2.);

   // MC momentum vs. polar angle histograms
   std::string cat[6];
   cat[0] = "All"; cat[1] = "Ref"; cat[2] = "Prim";
   cat[3] = "Sec"; cat[4] = "Mu"; cat[5] = "El";
   for (Int_t i = 0; i < 6; i++) {
      string histName = "hMCMomVsAngle_" + cat[i];
      Create2DHist(histName.c_str(), fNofBinsMom, fMinMom, fMaxMom, 10, 0., 35.);
   }

   // Histogram stores number of events
   Create1DHist("hEventNo", 1, 0, 1.);

   return fHM;
}
