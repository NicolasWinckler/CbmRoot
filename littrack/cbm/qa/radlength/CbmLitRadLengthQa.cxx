/**
 * \file CbmLitRadLengthQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include "CbmLitRadLengthQa.h"
#include "CbmHistManager.h"
#include "FairRootManager.h"
#include "FairRadLenPoint.h"
#include "CbmDrawHist.h"
#include "CbmTrdDetectorId.h"
#include "std/utils/CbmLitUtils.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TGeoManager.h"

#include "TRegexp.h"

#include "boost/assign/list_of.hpp"

#include <string>
#include <map>
#include <cstdlib>

using std::map;
using std::atoi;
using std::string;
using boost::assign::list_of;
using lit::ToString;

CbmLitRadLengthQa::CbmLitRadLengthQa():
   fHM(NULL),
   fOutputDir(""),
   fRadLen(NULL)
{

}

CbmLitRadLengthQa::~CbmLitRadLengthQa()
{
   if (fHM) delete fHM;
}

InitStatus CbmLitRadLengthQa::Init()
{
   fHM = new CbmHistManager();
   CreateHistograms();
   ReadDataBranches();
   return kSUCCESS;
}

void CbmLitRadLengthQa::Exec(
    Option_t* opt)
{
   static Int_t eventNo = 0;
   eventNo++;
   if (0 == eventNo % 10000) {
      std::cout << "-I- CbmLitRadLengthQa::Exec: eventNo=" << eventNo << std::endl;
   }

   ExecTotal();
   ExecTrd();
}

void CbmLitRadLengthQa::Finish()
{
   fHM->WriteToFile();
   Draw();
}

void CbmLitRadLengthQa::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);
   fRadLen = (TClonesArray*) ioman->GetObject("RadLen");
}

void CbmLitRadLengthQa::CreateHistograms()
{
   const Int_t nofBins = 200;
   fHM->Add("hrl_RadThickness_Total_H1", new TH1D("hrl_RadThickness_Total_H1", "hrl_RadThickness_Total_H1;Radiation thickness [%];Entries", nofBins, 0, 0));
   fHM->Add("hrl_RadThickness_Total_P2", new TProfile2D("hrl_RadThickness_Total_P2", "hrl_RadThickness_Total_P2;X [cm];Y [cm];Radiation thickness [%]", nofBins, 0., 0., nofBins, 0., 0.));
   fHM->Add("hrl_Thickness_Total_H1", new TH1D("hrl_Thickness_Total_H1", "hrl_Thickness_Total_H1;Thickness [cm];Entries", nofBins, 0, 0));
   fHM->Add("hrl_Thickness_Total_P2", new TProfile2D("hrl_Thickness_Total_P2", "hrl_Thickness_Total_P2;X [cm];Y [cm];Thickness [cm]", nofBins, 0., 0., nofBins, 0., 0.));

   Int_t nofStations = 3;
   std::vector<Int_t> nofLayersPerStation = list_of(4)(4)(2);
   for (Int_t iStation = 0; iStation < nofStations; iStation++) {
      Int_t nofLayers = nofLayersPerStation[iStation];
      for (Int_t iLayer = 0; iLayer < nofLayers; iLayer++) {
         string name = "hrl_RadThickness_Trd_" + ToString<Int_t>(iStation  + 1) + "_" + ToString<Int_t>(iLayer + 1) + "_H1";
         fHM->Add(name, new TH1D(name.c_str(), string(name + ";Radiation thickness [%];Entries").c_str(), nofBins, 0, 0));
         name = "hrl_RadThickness_Trd_" + ToString<Int_t>(iStation + 1) + "_" + ToString<Int_t>(iLayer + 1) + "_P2";
         fHM->Add(name, new TProfile2D(name.c_str(), string(name + ";X [cm];Y [cm];Radiation thickness [%]").c_str(), nofBins, 0., 0., nofBins, 0., 0.));
         name = "hrl_Thickness_Trd_" + ToString<Int_t>(iStation  + 1) + "_" + ToString<Int_t>(iLayer + 1) + "_H1";
         fHM->Add(name, new TH1D(name.c_str(), string(name + ";Thickness [cm];Entries").c_str(), nofBins, 0, 0));
         name = "hrl_Thickness_Trd_" + ToString<Int_t>(iStation + 1) + "_" + ToString<Int_t>(iLayer + 1) + "_P2";
         fHM->Add(name, new TProfile2D(name.c_str(), string(name + ";X [cm];Y [cm];Thickness [cm]").c_str(), nofBins, 0., 0., nofBins, 0., 0.));
      }
   }

   std::cout << fHM->ToString();
}

void CbmLitRadLengthQa::ExecTotal()
{
   map<Int_t, Double_t> radThicknessOnTrack; // track ID -> sum of radiation thickness on track
   map<Int_t, Double_t> thicknessOnTrack; // track ID -> sum of track lengthens on track

   Double_t x, y;
   for (Int_t iRL = 0; iRL < fRadLen->GetEntriesFast(); iRL++) {
      FairRadLenPoint* point = (FairRadLenPoint*) fRadLen->At(iRL);

      TVector3 pos = point->GetPosition();
      TVector3 posOut = point->GetPositionOut();
      TVector3 res = posOut - pos;
      x = pos.X();
      y = pos.Y();

      const Double_t radThick = res.Mag() / point->GetRadLength();
      radThicknessOnTrack[point->GetTrackID()] += radThick;
      thicknessOnTrack[point->GetTrackID()] += res.Mag();
   }

   map<Int_t, Double_t>::const_iterator it;
   for (it = radThicknessOnTrack.begin(); it != radThicknessOnTrack.end(); it++) {
      Double_t rl = (*it).second * 100;
      fHM->H1("hrl_RadThickness_Total_H1")->Fill(rl);
      fHM->P2("hrl_RadThickness_Total_P2")->Fill(x, y, rl);
   }

   for (it = thicknessOnTrack.begin(); it != thicknessOnTrack.end(); it++) {
      Double_t tl = (*it).second;
      fHM->H1("hrl_Thickness_Total_H1")->Fill(tl);
      fHM->P2("hrl_Thickness_Total_P2")->Fill(x, y, tl);
   }
}

void CbmLitRadLengthQa::ExecTrd()
{
   // track ID -> TRD station ID -> TRD Layer ID -> parameter
   map<Int_t, map<Int_t, map<Int_t, Double_t> > > radThicknessOnTrack; // track ID -> sum of radiation thickness on track
   map<Int_t, map<Int_t, map<Int_t, Double_t> > > thicknessOnTrack; // track ID -> sum of thickness on track

   Double_t x, y;
   for (Int_t iRL = 0; iRL < fRadLen->GetEntriesFast(); iRL++) {
      FairRadLenPoint* point = (FairRadLenPoint*) fRadLen->At(iRL);
      Int_t trackId = point->GetTrackID();
      Int_t detId = point->GetDetectorID();

      TVector3 pos = point->GetPosition();
      TVector3 posOut = point->GetPositionOut();
      TVector3 res = posOut - pos;
      TVector3 middle = (pos + posOut) * 0.5;
      x = pos.X();
      y = pos.Y();

      TGeoNode* node = gGeoManager->FindNode(middle.X(), middle.Y(), middle.Z());
      TString name = node->GetName();
      TString path = gGeoManager->GetPath();
      TRegexp e("/cave_1/trd1_0/trd1mod[0-9]_[0-9][0-9][0-9][0-9][0-9]/trd1mod.+");
      if (path.Contains(e)) {
         Int_t station = std::atoi(string(1, *(gGeoManager->GetPath() + 24)).c_str()); // 24th element is station number
         Int_t layer = std::atoi(string(1, *(gGeoManager->GetPath() + 25)).c_str()); // 25th element is layer number
         const Double_t thickness = res.Mag();
         const Double_t radThickness = (thickness / point->GetRadLength()) * 100;
         radThicknessOnTrack[trackId][station][layer] += radThickness;
         thicknessOnTrack[trackId][station][layer] += thickness;
      }
   }

   FillHistosTrd(radThicknessOnTrack, "hrl_RadThickness_Trd_", x, y);
   FillHistosTrd(thicknessOnTrack, "hrl_Thickness_Trd_", x, y);
}

void CbmLitRadLengthQa::FillHistosTrd(
      const map<Int_t, map<Int_t, map<Int_t, Double_t> > >& parMap,
      const string& histName,
      Double_t x,
      Double_t y)
{
   map<Int_t, map<Int_t, map<Int_t, Double_t> > >::const_iterator it1;
   for (it1 = parMap.begin(); it1 != parMap.end(); it1++) {
      Int_t trackId = (*it1).first;
      map<Int_t, map<Int_t, Double_t> >::const_iterator it2;
      for (it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++) {
         Int_t station = (*it2).first;
         map<Int_t, Double_t>::const_iterator it3;
         for (it3 = (*it2).second.begin(); it3 != (*it2).second.end(); it3++) {
            Int_t layer = (*it3).first;
            Double_t param = (*it3).second;
            string name = histName + ToString<Int_t>(station) + "_" + ToString<Int_t>(layer) + "_H1";
            fHM->H1(name)->Fill(param);
            name = histName + ToString<Int_t>(station) + "_" + ToString<Int_t>(layer) + "_P2";
            fHM->P2(name)->Fill(x, y, param);
         }
      }
   }
}

void CbmLitRadLengthQa::Draw()
{
   TCanvas* canvas1 = new TCanvas("hrl_Thickness_Total_H1", "hrl_Thickness_Total_H1", 1000, 1000);
   DrawH1(fHM->H1("hrl_Thickness_Total_H1"));

   TCanvas* canvas2 = new TCanvas("hrl_Thickness_Total_P2", "hrl_Thickness_Total_P2", 1000, 1000);
   DrawH2(fHM->P2("hrl_Thickness_Total_P2"));

   TCanvas* canvas3 = new TCanvas("hrl_RadThickness_Total_H1", "hrl_RadThickness_Total_H1", 1000, 1000);
   DrawH1(fHM->H1("hrl_RadThickness_Total_H1"));

   TCanvas* canvas4 = new TCanvas("hrl_RadThickness_Total_P2", "hrl_RadThickness_Total_P2", 1000, 1000);
   DrawH2(fHM->P2("hrl_RadThickness_Total_P2"));

   DrawH1ByPattern("hrl_RadThickness_Trd_.+_.+_H1");
   DrawP2ByPattern("hrl_RadThickness_Trd_.+_.+_P2");
   DrawH1ByPattern("hrl_Thickness_Trd_.+_.+_H1");
   DrawP2ByPattern("hrl_Thickness_Trd_.+_.+_P2");
}

void CbmLitRadLengthQa::DrawH1ByPattern(
      const string& pattern)
{
   std::vector<TH1*> histos = fHM->H1Vector(pattern);
   Int_t nofHistos = histos.size();
   for (Int_t i = 0; i < nofHistos; i++) {
      TH1* hist = histos[i];
      TCanvas* canvas = new TCanvas(hist->GetName(), hist->GetName(), 1000, 1000);
      DrawH1(hist);
   }
}

void CbmLitRadLengthQa::DrawP2ByPattern(
      const string& pattern)
{
   std::vector<TProfile2D*> profiles = fHM->P2Vector(pattern);
   Int_t nofProfiles = profiles.size();
   for (Int_t i = 0; i < nofProfiles; i++) {
      TProfile2D* profile = profiles[i];
      TCanvas* canvas = new TCanvas(profile->GetName(), profile->GetName(), 1000, 1000);
      DrawH2(profile);
   }
}

ClassImp(CbmLitRadLengthQa);
