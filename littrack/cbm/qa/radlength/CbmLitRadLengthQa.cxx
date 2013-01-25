/**
 * \file CbmLitRadLengthQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include "CbmLitRadLengthQa.h"
#include "CbmLitRadLengthQaReport.h"
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
   CbmSimulationReport* report = new CbmLitRadLengthQaReport();
   report->Create(fHM, fOutputDir);
   delete report;
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

   fHM->Add("hrl_ThicknessSilicon_Total_H1", new TH1D("hrl_ThicknessSilicon_Total_H1", "hrl_ThicknessSilicon_Total_H1;Thickness [cm];Entries", nofBins, 0, 0));
   fHM->Add("hrl_ThicknessSilicon_Total_P2", new TProfile2D("hrl_ThicknessSilicon_Total_P2", "hrl_ThicknessSilicon_Total_P2;X [cm];Y [cm];Thickness [cm]", nofBins, 0., 0., nofBins, 0., 0.));

   Int_t nofStations = 3;
   std::vector<Int_t> nofLayersPerStation = list_of(4)(4)(4);
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
         name = "hrl_ThicknessSilicon_Trd_" + ToString<Int_t>(iStation  + 1) + "_" + ToString<Int_t>(iLayer + 1) + "_H1";
         fHM->Add(name, new TH1D(name.c_str(), string(name + ";Thickness [cm];Entries").c_str(), nofBins, 0, 0));
         name = "hrl_ThicknessSilicon_Trd_" + ToString<Int_t>(iStation + 1) + "_" + ToString<Int_t>(iLayer + 1) + "_P2";
         fHM->Add(name, new TProfile2D(name.c_str(), string(name + ";X [cm];Y [cm];Thickness [cm]").c_str(), nofBins, 0., 0., nofBins, 0., 0.));
      }
   }

   std::cout << fHM->ToString();
}

void CbmLitRadLengthQa::ExecTotal()
{
   map<Int_t, Double_t> radThicknessOnTrack; // track ID -> sum of radiation thickness on track
   map<Int_t, Double_t> thicknessOnTrack; // track ID -> sum of track lengthens on track
   map<Int_t, Double_t> thicknessSiliconOnTrack; // track ID -> sum of track lengthens on track in silicon equivalent

   Double_t x, y;
   for (Int_t iRL = 0; iRL < fRadLen->GetEntriesFast(); iRL++) {
      FairRadLenPoint* point = (FairRadLenPoint*) fRadLen->At(iRL);

      TVector3 pos = point->GetPosition();
      TVector3 posOut = point->GetPositionOut();
      TVector3 res = posOut - pos;
      x = pos.X();
      y = pos.Y();

      const Double_t thickness = res.Mag();
      const Double_t radThickness = 100 * thickness / point->GetRadLength();
      const Double_t thicknessSilicon = (SILICON_RAD_LENGTH / point->GetRadLength()) * thickness;
      radThicknessOnTrack[point->GetTrackID()] += radThickness;
      thicknessOnTrack[point->GetTrackID()] += thickness;
      thicknessSiliconOnTrack[point->GetTrackID()] += thicknessSilicon;
   }

   map<Int_t, Double_t>::const_iterator it;
   for (it = radThicknessOnTrack.begin(); it != radThicknessOnTrack.end(); it++) {
      Double_t rl = (*it).second;
      fHM->H1("hrl_RadThickness_Total_H1")->Fill(rl);
      fHM->P2("hrl_RadThickness_Total_P2")->Fill(x, y, rl);
   }

   for (it = thicknessOnTrack.begin(); it != thicknessOnTrack.end(); it++) {
      Double_t tl = (*it).second;
      fHM->H1("hrl_Thickness_Total_H1")->Fill(tl);
      fHM->P2("hrl_Thickness_Total_P2")->Fill(x, y, tl);
   }

   for (it = thicknessSiliconOnTrack.begin(); it != thicknessSiliconOnTrack.end(); it++) {
      Double_t tl = (*it).second;
      fHM->H1("hrl_ThicknessSilicon_Total_H1")->Fill(tl);
      fHM->P2("hrl_ThicknessSilicon_Total_P2")->Fill(x, y, tl);
   }
}

void CbmLitRadLengthQa::ExecTrd()
{
   // track ID -> TRD station ID -> TRD Layer ID -> parameter
   map<Int_t, map<Int_t, map<Int_t, Double_t> > > radThicknessOnTrack; // track ID -> sum of radiation thickness on track
   map<Int_t, map<Int_t, map<Int_t, Double_t> > > thicknessOnTrack; // track ID -> sum of thickness on track
   map<Int_t, map<Int_t, map<Int_t, Double_t> > > thicknessSiliconOnTrack; // track ID -> sum of thickness on track

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
      Int_t station = 0;
      Int_t layer = 0;
      Bool_t nodeExists = false;

      if (path.Contains(TRegexp("/cave_1/trd[1-3]_0/trd[1-3]mod[0-9]_[0-9][0-9][0-9][0-9]/trd[1-3]mod.+"))) { // trd_v10b and trd_v11c
         station = std::atoi(string(1, *(gGeoManager->GetPath() + 18)).c_str()); // 18th element is station number
         layer = std::atoi(string(1, *(gGeoManager->GetPath() + 24)).c_str()); // 24th element is layer number
         nodeExists = true;
      } else if (path.Contains(TRegexp("/cave_1/trd1_0/trd1mod[0-9]_[0-9][0-9][0-9][0-9][0-9]/trd1mod.+"))) { // trd_v12x
         station = std::atoi(string(1, *(gGeoManager->GetPath() + 24)).c_str()); // 24th element is station number
         layer = std::atoi(string(1, *(gGeoManager->GetPath() + 25)).c_str()); // 25th element is layer number
         nodeExists = true;
      } else if (path.Contains(TRegexp("/cave_1/trd_v13[a-z]_0/trd1mod[0-9]_[0-9][0-9][0-9][0-9][0-9]/trd1mod.+"))) { // trd_v13x
         station = std::atoi(string(1, *(gGeoManager->GetPath() + 28)).c_str()); // 28th element is station number
         layer = std::atoi(string(1, *(gGeoManager->GetPath() + 29)).c_str()); // 29th element is layer number
         nodeExists = true;
      }

      // Check if node exists in one of the geometry versions
      if (nodeExists) {
         const Double_t thickness = res.Mag();
         const Double_t radThickness = 100 * thickness / point->GetRadLength();
         const Double_t thicknessSilicon = (SILICON_RAD_LENGTH / point->GetRadLength()) * thickness;
         radThicknessOnTrack[trackId][station][layer] += radThickness;
         thicknessOnTrack[trackId][station][layer] += thickness;
         thicknessSiliconOnTrack[trackId][station][layer] += thicknessSilicon;
      }
   }

   FillHistosTrd(radThicknessOnTrack, "hrl_RadThickness_Trd_", x, y);
   FillHistosTrd(thicknessOnTrack, "hrl_Thickness_Trd_", x, y);
   FillHistosTrd(thicknessSiliconOnTrack, "hrl_ThicknessSilicon_Trd_", x, y);
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

ClassImp(CbmLitRadLengthQa);
