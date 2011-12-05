/*
 * CbmLitQaHistCreator.cxx
 *
 *  Created on: 17.10.2011
 *      Author: slebedev
 */
#include "qa/reconstruction/CbmLitQaHistCreator.h"
#include "qa/reconstruction/CbmLitQaHistNames.h"
#include "qa/base/CbmLitHistManager.h"

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
      const string& xTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin,
      LitQaNameOption opt)
{
   const vector<string>& type = CbmLitQaHistNames::GetTypes(opt);
   const vector<string>& cat = CbmLitQaHistNames::GetCategories(opt);
   string yTitle = "Yield";
   for (Int_t i = 0; i < cat.size(); i++) {
      for (Int_t j = 0; j < type.size(); j++) {
         string histName = name + "_" + cat[i] + "_" + type[j];
         if (type[j] == "Eff") yTitle = "Efficiency [%]";
         fHM->Add(histName, new TH1F(histName.c_str(),
               (histName+";"+xTitle+";"+yTitle).c_str(), nofBins, minBin, maxBin));
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
         fHM->Add(histName, new TH3F(histName.c_str(),
               (histName+";P [GeV/c];Rapidity;P_{t} [GeV/c]").c_str(),
               fNofBinsMom, fMinMom, fMaxMom,
               fNofBinsY, fMinY, fMaxY,
               fNofBinsPt, fMinPt, fMaxPt));
      }
   }
}


void CbmLitQaHistCreator::Create1DHist(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin)
{
   TH1F* h = new TH1F(name.c_str(),
         (name+";"+xTitle+";"+yTitle).c_str(), nofBins, minBin, maxBin);
   fHM->Add(name, h);
}

void CbmLitQaHistCreator::Create2DHist(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      const string& zTitle,
      Int_t nofBinsX,
      Double_t minBinX,
      Double_t maxBinX,
      Int_t nofBinsY,
      Double_t minBinY,
      Double_t maxBinY)
{
   TH2F* h = new TH2F(name.c_str(),
         (name+";"+xTitle+";"+yTitle+";"+zTitle).c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
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
   CreateEffHist("hStsNp", "Number of points", nBinsNofPoints, minNofPoints, maxNofPoints, kTracking);
   CreateEffHist("hStsAngle", "Polar angle [grad]", fNofBinsAngle, fMinAngle, fMaxAngle, kTracking);
   CreateEffHist3D("hHalfGlobal3D", kTracking);
   CreateEffHist3D("hHalfGlobal3DNormGlobal", kTracking);
   CreateEffHist3D("hGlobal3D", kTracking);
   CreateEffHist3D("hRec3D", kTracking);
   CreateEffHist("hRecNp", "Number of points",nBinsNofPoints, minNofPoints, maxNofPoints, kTracking);
   CreateEffHist("hRecAngle", "Polar angle [grad]", fNofBinsAngle, fMinAngle, fMaxAngle, kTracking);
   CreateEffHist3D("hTof3D", kTracking);
   CreateEffHist("hTofAngle", "Polar angle [grad]", fNofBinsAngle, fMinAngle, fMaxAngle, kTracking);

   CreateEffHist3D("hRich3D", kRich);
   CreateEffHist("hRichNh", "Number of hits", nBinsNofPoints, minNofPoints, maxNofPoints, kRich);

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
   Create1DHist("hStsGhostNh", "Nof ghosts", "Yield", nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hRecGhostNh", "Nof ghosts", "Yield",nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hRichGhostNh", "Nof ghosts", "Yield",nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hRichGhostStsMatchingNh", "Nof ghosts", "Yield",nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hRichGhostElIdNh", "Nof ghosts", "Yield",nBinsNofPoints, minNofPoints, maxNofPoints);
   Create1DHist("hStsGhostRichMatchingNh", "Nof ghosts", "Yield",nBinsNofPoints, minNofPoints, maxNofPoints);

   const UInt_t maxNofStations = 30;
   Create1DHist("hMvdNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);
   Create1DHist("hStsNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);
   Create1DHist("hTrdNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);
   Create1DHist("hMuchNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);
   Create1DHist("hTofNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);

   const UInt_t nofHitsHistos = 5;
   string hittype[] = { "All", "True", "Fake", "TrueOverAll", "FakeOverAll" };
   Double_t hitmin[] = {0, 0, 0, -0.1, -0.1};
   Double_t hitmax[] = {20, 20, 20, 1.1, 1.1};
   Int_t hitbins[] = {20, 20, 20, 12, 12};
   Double_t hitmaxrich[] = {50, 50, 50, 1.1, 1.1};
   Int_t hitbinsrich[] = {50, 50, 50, 12, 12};
   string xTitle = "Number of hits";
   for(UInt_t i = 0; i < nofHitsHistos; i++) {
      string histName1 = "hMvdTrackHits_" + hittype[i];
      string histName2 = "hStsTrackHits_" + hittype[i];
      string histName3 = "hTrdTrackHits_" + hittype[i];
      string histName4 = "hMuchTrackHits_" + hittype[i];
      string histName5 = "hRichRingHits_" + hittype[i];

      if (i == 3 || i == 4) xTitle = "Ratio";
      Create1DHist(histName1.c_str(), xTitle, "Counter", hitbins[i], hitmin[i], hitmax[i]);
      Create1DHist(histName2.c_str(), xTitle, "Counter", hitbins[i], hitmin[i], hitmax[i]);
      Create1DHist(histName3.c_str(), xTitle, "Counter", hitbins[i], hitmin[i], hitmax[i]);
      Create1DHist(histName4.c_str(), xTitle, "Counter", hitbins[i], hitmin[i], hitmax[i]);
      Create1DHist(histName5.c_str(), xTitle, "Counter", hitbinsrich[i], hitmin[i], hitmaxrich[i]);
   }

   Int_t nofBinsC = 1000;
   Double_t maxXC = 50000.;
   Create1DHist("hNofGlobalTracks", "Tracks per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsTracks", "Tracks per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdTracks", "Tracks per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchTracks", "Tracks per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofRichRings", "Rings per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofRichProjections", "Projections per event", "Counter", nofBinsC, 1., maxXC);

   Create1DHist("hNofMvdHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofRichHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchPixelHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchStrawHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofTofHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);

   Create1DHist("hNofMvdPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofRichPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofTofPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);

   Create1DHist("hNofMvdDigis", "Digis per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsDigis", "Digis per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchDigis", "Digis per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdDigis", "Digis per event", "Counter", nofBinsC, 1., maxXC);

   Create1DHist("hNofMvdClusters", "Clusters per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofStsClusters", "Clusters per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofMuchClusters", "Clusters per event", "Counter", nofBinsC, 1., maxXC);
   Create1DHist("hNofTrdClusters", "Clusters per event", "Counter", nofBinsC, 1., maxXC);

   Create1DHist("hStsChiprim", "#chi^{2}_{vertex}", "Yield", 150, 0., 15.);
   Create2DHist("hStsMomresVsMom", "P [GeV/c]", "dP/P", "Counter", 120, 0., 12., 100, -15., 15.);
   Create1DHist("hTrackLength", "dL/L [%]", "Yield", 40, -2., 2.);

   // MC momentum vs. polar angle histograms
   string cat[6];
   cat[0] = "All"; cat[1] = "Ref"; cat[2] = "Prim";
   cat[3] = "Sec"; cat[4] = "Mu"; cat[5] = "El";
   for (Int_t i = 0; i < 6; i++) {
      string histName = "hMCMomVsAngle_" + cat[i];
      Create2DHist(histName.c_str(), "P [GeV/c]", "Polar angle [grad]", "Counter",
            fNofBinsMom, fMinMom, fMaxMom, 10, 0., 35.);
   }

   // Histogram stores number of events
   Create1DHist("hEventNo", "", "",1, 0, 1.);

   return fHM;
}
