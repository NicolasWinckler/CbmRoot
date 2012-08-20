/**
 * \file CbmStsSimulationQaHistCreator.cxx
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */

#include "CbmStsSimulationQaHistCreator.h"
#include "CbmHistManager.h"
#include "TH1F.h"
#include "TH2F.h"
#include <cassert>
#include <sstream>

CbmStsSimulationQaHistCreator::CbmStsSimulationQaHistCreator():
   fHM(NULL)
{

}

CbmStsSimulationQaHistCreator::~CbmStsSimulationQaHistCreator()
{

}

void CbmStsSimulationQaHistCreator::Create(
      CbmHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   Int_t nofStsStations = 8; // TODO: Get number of STS stations from TGeo!

   // Number of STS points per station
   CreateH1F("hss_NofObjects_Sts_Points_All_Station", "Station number", "Entries", 100, 0., 100.);
   CreateH1F("hss_NofObjects_Sts_Points_Primary_Station", "Station number", "Entries", 100, 0., 100.);

   // XY distributions for STS points
   for (Int_t iStation = 0; iStation < nofStsStations; iStation++) {
      std::stringstream histName;
      histName << "hss_XY_Sts_Points_All_Station_" << (iStation + 1);
      string histTitle = histName.str() + ";X [cm];Y [cm]";
      fHM->Add(histName.str(), new TH2F(histName.str().c_str(), histTitle.c_str(), 100, -50., 50., 100, -50., 50.));
   }

   // Number of points per STS track
   CreateH1F("hss_NofPointsPerTrack_Sts_Primary", "Number of points", "Entries", 15, 0., 15.);
   CreateH1F("hss_NofPointsPerTrack_Sts_Reference", "Number of points", "Entries", 15, 0., 15.);

   // ---> Momentum for primary, accepted and reference tracks
   CreateH1F("hss_Sts_All_p", "P [Gev/c]", "Entries", 100, 0., 25.);
   CreateH1F("hss_Sts_Acc_p", "P [Gev/c]", "Entries", 100, 0., 25.);
   CreateH1F("hss_Sts_Ref_p", "P [Gev/c]", "Entries", 100, 0., 25.);

   // ---> Momentum for primary, accepted and reference tracks
   CreateH1F("hss_Sts_All_pt", "Pt [Gev/c]", "Entries", 100, 0., 25.);
   CreateH1F("hss_Sts_Acc_pt", "Pt [Gev/c]", "Entries", 100, 0., 25.);
   CreateH1F("hss_Sts_Ref_pt", "Pt [Gev/c]", "Entries", 100, 0., 25.);

   // ---> Start z position of secondary tracks
   CreateH1F("hss_StartSecondaries", "Z [cm]", "Entries", 100, -10., 110.);

   // Histogram stores number of events
   CreateH1F("hss_EventNo_StsSimQa", "", "", 1, 0, 1.);
}

void CbmStsSimulationQaHistCreator::CreateH1F(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin)
{
   TH1F* h = new TH1F(name.c_str(), (name + ";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
   fHM->Add(name, h);
}
