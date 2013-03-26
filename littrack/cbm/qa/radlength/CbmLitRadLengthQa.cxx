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
#include "cbm/base/CbmLitTrackingGeometryConstructor.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TGeoManager.h"

#include "TRegexp.h"

#include "boost/assign/list_of.hpp"
#include <boost/algorithm/string.hpp>

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
   fRadLen(NULL),
   fDet()
{

}

CbmLitRadLengthQa::~CbmLitRadLengthQa()
{
   if (fHM) delete fHM;
}

InitStatus CbmLitRadLengthQa::Init()
{
   fHM = new CbmHistManager();
   fDet.DetermineSetup();
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

   ExecDetector(".+", "Total");
   ExecDetector("/cave_1/pipevac1_0/mvdstation.+", "Mvd");
   ExecDetector("/cave_1/STS.+", "Sts");
   ExecDetector("/cave_1/rich.+", "Rich");
   ExecDetector("/cave_1/trd.+", "Trd");
   ExecDetector("/cave_1/much.+", "Much");
   ExecDetector("/cave_1/tof.+", "Tof");
   ExecDetector("Mvd", CbmLitRadLengthQa::GetMvdStationId);
   ExecDetector("Sts", CbmLitRadLengthQa::GetStsStationId);
   ExecDetector("Trd", CbmLitRadLengthQa::GetTrdStationId);
   ExecDetector("Much", CbmLitRadLengthQa::GetMuchStationId);
}

void CbmLitRadLengthQa::Finish()
{
   fHM->WriteToFile();
   CbmSimulationReport* report = new CbmLitRadLengthQaReport();
   report->Create(fHM, fOutputDir);
   delete report;
   Draw();
   SaveMaterialBudgetToFile();
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
   vector<string> detNames = list_of("Total")("Mvd")("Sts")("Rich")("Trd")("Much")("Tof");
   Int_t nofDetNames = detNames.size();
   for (Int_t iDet = 0; iDet < nofDetNames; iDet++) {
      string detName = detNames[iDet];
      Bool_t createHistograms = detName == "Total" || (detName == "Mvd" && fDet.GetDet(kMVD)) || (detName == "Sts" && fDet.GetDet(kSTS))
             || (detName == "Rich" && fDet.GetDet(kRICH)) || (detName == "Trd" && fDet.GetDet(kTRD))
             || (detName == "Much" && fDet.GetDet(kMUCH)) || (detName == "Tof" && fDet.GetDet(kTOF));
      if (!createHistograms) continue; // Create histograms only for the detectors wich are in the setup
      string rtname = "hrl_RadThickness_" + detName;
      fHM->Add(rtname + "_H1", new TH1D(string(rtname + "_H1").c_str(), string(rtname + "_H1;Radiation thickness [%];Entries").c_str(), nofBins, 0, 0));
      fHM->Add(rtname + "_P2", new TProfile2D(string(rtname + "_P2").c_str(), string(rtname + "_P2;X [cm];Y [cm];Radiation thickness [%]").c_str(), nofBins, 0., 0., nofBins, 0., 0.));
      string tname = "hrl_Thickness_" + detName;
      fHM->Add(tname + "_H1", new TH1D(string(tname + "_H1").c_str(), string(tname + "_H1;Thickness [cm];Entries").c_str(), nofBins, 0, 0));
      fHM->Add(tname + "_P2", new TProfile2D(string(tname + "_P2").c_str(), string(tname + "_P2;X [cm];Y [cm];Thickness [cm]").c_str(), nofBins, 0., 0., nofBins, 0., 0.));
      string tsname = "hrl_ThicknessSilicon_" + detName;
      fHM->Add(tsname + "_H1", new TH1D(string(tsname + "_H1").c_str(), string(tsname + "_H1;Thickness [cm];Entries").c_str(), nofBins, 0, 0));
      fHM->Add(tsname + "_P2", new TProfile2D(string(tsname + "_P2").c_str(), string(tsname + "_P2;X [cm];Y [cm];Thickness [cm]").c_str(), nofBins, 0., 0., nofBins, 0., 0.));
   }

   // Additional histograms for each station in tracking detectors
   vector<string> trackingDetNames = list_of("Mvd")("Sts")("Trd")("Much");
   Int_t nofTrackingDetNames = trackingDetNames.size();
   for (Int_t iDet = 0; iDet < nofTrackingDetNames; iDet++) {
      string dname = trackingDetNames[iDet];
      Int_t nofStations = (dname == "Mvd") ? CbmLitTrackingGeometryConstructor::Instance()->GetNofMvdStations() :
                          (dname == "Sts") ? CbmLitTrackingGeometryConstructor::Instance()->GetNofStsStations() :
                          (dname == "Trd") ? CbmLitTrackingGeometryConstructor::Instance()->GetNofTrdStations() :
                          (dname == "Much") ? CbmLitTrackingGeometryConstructor::Instance()->GetNofMuchStations() : 0;
      for (Int_t iStation = 0; iStation < nofStations; iStation++) {
         string name = "hrl_RadThickness_" + dname + "_" + ToString<Int_t>(iStation) + "_H1";
         fHM->Add(name, new TH1D(name.c_str(), string(name + ";Radiation thickness [%];Entries").c_str(), nofBins, 0, 0));
         name = "hrl_RadThickness_" + dname + "_" + ToString<Int_t>(iStation) + "_P2";
         fHM->Add(name, new TProfile2D(name.c_str(), string(name + ";X [cm];Y [cm];Radiation thickness [%]").c_str(), nofBins, 0., 0., nofBins, 0., 0.));
         name = "hrl_Thickness_" + dname + "_" + ToString<Int_t>(iStation) + "_H1";
         fHM->Add(name, new TH1D(name.c_str(), string(name + ";Thickness [cm];Entries").c_str(), nofBins, 0, 0));
         name = "hrl_Thickness_" + dname + "_" + ToString<Int_t>(iStation) + "_P2";
         fHM->Add(name, new TProfile2D(name.c_str(), string(name + ";X [cm];Y [cm];Thickness [cm]").c_str(), nofBins, 0., 0., nofBins, 0., 0.));
         name = "hrl_ThicknessSilicon_" + dname + "_" + ToString<Int_t>(iStation) + "_H1";
         fHM->Add(name, new TH1D(name.c_str(), string(name + ";Thickness [cm];Entries").c_str(), nofBins, 0, 0));
         name = "hrl_ThicknessSilicon_" + dname + "_" + ToString<Int_t>(iStation) + "_P2";
         fHM->Add(name, new TProfile2D(name.c_str(), string(name + ";X [cm];Y [cm];Thickness [cm]").c_str(), nofBins, 0., 0., nofBins, 0., 0.));
      }
   }

   std::cout << fHM->ToString();
}

void CbmLitRadLengthQa::ExecDetector(
      const string& pathPattern,
      const string& detName)
{
    if (!(detName == "Total" || (detName == "Mvd" && fDet.GetDet(kMVD)) || (detName == "Sts" && fDet.GetDet(kSTS))
                || (detName == "Rich" && fDet.GetDet(kRICH)) || (detName == "Trd" && fDet.GetDet(kTRD))
                || (detName == "Much" && fDet.GetDet(kMUCH)) || (detName == "Tof" && fDet.GetDet(kTOF)))) return;

   map<Int_t, Double_t> radThicknessOnTrack; // track ID -> sum of radiation thickness on track
   map<Int_t, Double_t> thicknessOnTrack; // track ID -> sum of track lengthens on track
   map<Int_t, Double_t> thicknessSiliconOnTrack; // track ID -> sum of track lengthens on track in silicon equivalent

   Double_t x, y;
   for (Int_t iRL = 0; iRL < fRadLen->GetEntriesFast(); iRL++) {
      FairRadLenPoint* point = (FairRadLenPoint*) fRadLen->At(iRL);

      TVector3 pos = point->GetPosition();
      TVector3 posOut = point->GetPositionOut();
      TVector3 res = posOut - pos;
      TVector3 middle = (pos + posOut) * 0.5;
      //x = middle.X();
      //y = middle.Y();

      TGeoNode* nodeOut = gGeoManager->FindNode(posOut.X(), posOut.Y(), posOut.Z());
      Bool_t isOutside = gGeoManager->IsOutside();
      TGeoNode* node = gGeoManager->FindNode(middle.X(), middle.Y(), middle.Z());
      TString path = gGeoManager->GetPath();
      if (!isOutside && path.Contains(TRegexp(pathPattern.c_str()))) {
         x = posOut.X();
         y = posOut.Y();
         const Double_t thickness = res.Mag();
         const Double_t radThickness = 100 * thickness / point->GetRadLength();
         const Double_t thicknessSilicon = (SILICON_RAD_LENGTH / point->GetRadLength()) * thickness;
         radThicknessOnTrack[point->GetTrackID()] += radThickness;
         thicknessOnTrack[point->GetTrackID()] += thickness;
         thicknessSiliconOnTrack[point->GetTrackID()] += thicknessSilicon;
      }
   }

   map<Int_t, Double_t>::const_iterator it;
   for (it = radThicknessOnTrack.begin(); it != radThicknessOnTrack.end(); it++) {
      Double_t rl = (*it).second;
      fHM->H1("hrl_RadThickness_" + detName + "_H1")->Fill(rl);
      fHM->P2("hrl_RadThickness_" + detName + "_P2")->Fill(x, y, rl);
   }

   for (it = thicknessOnTrack.begin(); it != thicknessOnTrack.end(); it++) {
      Double_t tl = (*it).second;
      fHM->H1("hrl_Thickness_" + detName + "_H1")->Fill(tl);
      fHM->P2("hrl_Thickness_" + detName + "_P2")->Fill(x, y, tl);
   }

   for (it = thicknessSiliconOnTrack.begin(); it != thicknessSiliconOnTrack.end(); it++) {
      Double_t tl = (*it).second;
      fHM->H1("hrl_ThicknessSilicon_" + detName + "_H1")->Fill(tl);
      fHM->P2("hrl_ThicknessSilicon_" + detName + "_P2")->Fill(x, y, tl);
   }
}

void CbmLitRadLengthQa::ExecDetector(
      const string& detName,
      Int_t (*getStationId)(const TString&))
{
   if (!((detName == "Mvd" && fDet.GetDet(kMVD)) || (detName == "Sts" && fDet.GetDet(kSTS))
           || (detName == "Trd" && fDet.GetDet(kTRD)) || (detName == "Much" && fDet.GetDet(kMUCH)))) return;

   // track ID -> TRD station ID -> parameter
   map<Int_t, map<Int_t, Double_t> > radThicknessOnTrack; // track ID -> sum of radiation thickness on track
   map<Int_t, map<Int_t, Double_t> > thicknessOnTrack; // track ID -> sum of thickness on track
   map<Int_t, map<Int_t, Double_t> > thicknessSiliconOnTrack; // track ID -> sum of thickness on track

   Double_t x, y;
   for (Int_t iRL = 0; iRL < fRadLen->GetEntriesFast(); iRL++) {
      FairRadLenPoint* point = (FairRadLenPoint*) fRadLen->At(iRL);
      Int_t trackId = point->GetTrackID();
      Int_t detId = point->GetDetectorID();

      TVector3 pos = point->GetPosition();
      TVector3 posOut = point->GetPositionOut();
      TVector3 res = posOut - pos;
      TVector3 middle = (pos + posOut) * 0.5;
      //x = posOut.X();
      //y = posOut.Y();

      TGeoNode* node = gGeoManager->FindNode(middle.X(), middle.Y(), middle.Z());
      TString path = gGeoManager->GetPath();
      Int_t stationId = getStationId(path);

      // Check if node exists in one of the geometry versions
      if (stationId >= 0) {
         x = posOut.X();
         y = posOut.Y();
         const Double_t thickness = res.Mag();
         const Double_t radThickness = 100 * thickness / point->GetRadLength();
         const Double_t thicknessSilicon = (SILICON_RAD_LENGTH / point->GetRadLength()) * thickness;
         radThicknessOnTrack[trackId][stationId] += radThickness;
         thicknessOnTrack[trackId][stationId] += thickness;
         thicknessSiliconOnTrack[trackId][stationId] += thicknessSilicon;
      }
   }

   FillHistosDetector(radThicknessOnTrack, "hrl_RadThickness_" + detName + "_", x, y);
   FillHistosDetector(thicknessOnTrack, "hrl_Thickness_" + detName + "_", x, y);
   FillHistosDetector(thicknessSiliconOnTrack, "hrl_ThicknessSilicon_" + detName + "_", x, y);
}

void CbmLitRadLengthQa::FillHistosDetector(
      const map<Int_t, map<Int_t, Double_t> >& parMap,
      const string& histName,
      Double_t x,
      Double_t y)
{
   map<Int_t, map<Int_t, Double_t> >::const_iterator it1;
   for (it1 = parMap.begin(); it1 != parMap.end(); it1++) {
      Int_t trackId = (*it1).first;
      map<Int_t, Double_t>::const_iterator it2;
      for (it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++) {
         Int_t station = (*it2).first;
         Double_t param = (*it2).second;
         string name = histName + ToString<Int_t>(station) + "_H1";
         fHM->H1(name)->Fill(param);
         name = histName + ToString<Int_t>(station) + "_P2";
         fHM->P2(name)->Fill(x, y, param);
      }
   }
}

Int_t CbmLitRadLengthQa::GetMvdStationId(
      const TString& nodePath)
{
   std::cout << "-W- CbmLitRadLengthQa::GetMvdStationId: function not implemented\n";
   return 0;
}

Int_t CbmLitRadLengthQa::GetStsStationId(
      const TString& nodePath)
{
   Int_t station = 0;
   Bool_t nodeExists = false;
   if (nodePath.Contains(TRegexp("/cave_1/STS_v[0-9][0-9][a-z]_0/Station[0-9][0-9]_.+"))) { // sts_v12x
      station = std::atoi(string((gGeoManager->GetPath() + 26), 2).c_str()); // 26-27th element is station number
      nodeExists = true;
   }
   return (nodeExists) ? (station - 1) : -1;
}

Int_t CbmLitRadLengthQa::GetTrdStationId(
      const TString& nodePath)
{
   Int_t station = 0;
   Int_t layer = 0;
   Bool_t nodeExists = false;
   if (nodePath.Contains(TRegexp("/cave_1/trd[1-3]_0/trd[1-3]mod[0-9]_[0-9][0-9][0-9][0-9]/trd[1-3]mod.+"))) { // trd_v10b and trd_v11c
      station = std::atoi(string(1, *(gGeoManager->GetPath() + 18)).c_str()); // 18th element is station number
      layer = std::atoi(string(1, *(gGeoManager->GetPath() + 24)).c_str()); // 24th element is layer number
      nodeExists = true;
   } else if (nodePath.Contains(TRegexp("/cave_1/trd1_0/trd1mod[0-9]_[0-9][0-9][0-9][0-9][0-9]/trd1mod.+"))) { // trd_v12x
      station = std::atoi(string(1, *(gGeoManager->GetPath() + 24)).c_str()); // 24th element is station number
      layer = std::atoi(string(1, *(gGeoManager->GetPath() + 25)).c_str()); // 25th element is layer number
      nodeExists = true;
   } else if (nodePath.Contains(TRegexp("/cave_1/trd_v13[a-z]_0/trd1mod[0-9]_[0-9][0-9][0-9][0-9][0-9]/trd1mod.+"))) { // trd_v13x
      station = std::atoi(string(1, *(gGeoManager->GetPath() + 28)).c_str()); // 28th element is station number
      layer = std::atoi(string(1, *(gGeoManager->GetPath() + 29)).c_str()); // 29th element is layer number
      nodeExists = true;
   }
   return (nodeExists) ? CbmLitTrackingGeometryConstructor::Instance()->ConvertTrdToAbsoluteStationNr(station - 1, layer - 1) : -1;
}

Int_t CbmLitRadLengthQa::GetMuchStationId(
      const TString& nodePath)
{
   Int_t station = 0;
   Int_t layer = 0;
   Bool_t nodeExists = false;
   if (nodePath.Contains(TRegexp("/cave_1/much_0/muchstation[0-9][0-9]_0/muchstation[0-9][0-9]layer[0-9]_0/.+"))) { // much_v11x
      station = std::atoi(string(gGeoManager->GetPath() + 42, 2).c_str()); // 42-43th elements are station number
      layer = std::atoi(string(1, *(gGeoManager->GetPath() + 49)).c_str()); // 49th element is layer number
      nodeExists = true;
   }
   return (nodeExists) ? CbmLitTrackingGeometryConstructor::Instance()->ConvertMuchToAbsoluteStationNr(station - 1, layer - 1) : -1;
}

void CbmLitRadLengthQa::SaveMaterialBudgetToFile()
{
   SaveDetectorMaterialBudgetToFile("Mvd");
   SaveDetectorMaterialBudgetToFile("Sts");
   SaveDetectorMaterialBudgetToFile("Rich");
   SaveDetectorMaterialBudgetToFile("Trd");
   SaveDetectorMaterialBudgetToFile("Much");
   SaveDetectorMaterialBudgetToFile("Tof");
}

void CbmLitRadLengthQa::SaveDetectorMaterialBudgetToFile(
      const string& detName)
{
   string pattern = (detName == "Mvd" || detName == "Sts" || detName == "Trd" || detName == "Much") ?
         "hrl_ThicknessSilicon_" + detName + "_.+_P2" : "hrl_ThicknessSilicon_" + detName + "_P2";
   vector<TH1*> histos = fHM->H1Vector(pattern);
   if (histos.empty()) return;
   TFile* oldFile = gFile;
   TDirectory* oldDirectory = gDirectory;
   TFile* file = new TFile(string(fOutputDir + "/" + boost::algorithm::to_lower_copy(detName) + ".silicon.root").c_str(), "RECREATE");
   for (vector<TH1*>::const_iterator it = histos.begin(); it != histos.end(); it++) {
      (*it)->Write();
   }
   file->Close();
   delete file;
   gFile = oldFile;
   gDirectory = oldDirectory;
}
ClassImp(CbmLitRadLengthQa);
