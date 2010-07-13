/*
 * CbmMuchFindHitsAdvancedGem.cxx
 *
 *  Created on: June 17, 2007
 *      Author: M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *
 *  Produces hits in GEM-like modules using different
 *  clustering algorithms. Clustering algorithms can be chosen
 *  by the switch SetAlgorithm(Int_t iAlgorithm)
 *  Several clustering algorithms are supported
 *  0 - Spatial separation (clusters are sets of neighbor fired pads)
 *  1 - Simple
 *  2 - Simple (threshold is set for each station separately)
 *  3 - Peaks algorithm
 *
 */

#include "CbmMuchFindHitsAdvancedGem.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPad.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigi.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchStation.h"
#include "FairRootManager.h"

#include "TMath.h"

#include <iostream>
#include <iomanip>
#include <cassert>
#include <limits>

using std::cout;
using std::endl;
using std::cerr;
using std::setw;
using std::fixed;
using std::left;
using std::right;
using std::setprecision;

// -----   Default constructor   ------------------------------------------
CbmMuchFindHitsAdvancedGem::CbmMuchFindHitsAdvancedGem() :
  FairTask("MuchFindHitsAdvancedGem", 1) {
  fDigis = NULL;
  fDigiMatches = NULL;
  fHits = NULL;
  fDigiFile = "";
  fClusters = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fThresholdRatio = 0.1;
  fDistanceLimit = 0.5;
  fAlgorithm = 0;

  //  fDebug = 0;
  fEvent = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchFindHitsAdvancedGem::CbmMuchFindHitsAdvancedGem(Int_t iVerbose) :
  FairTask("MuchFindHitsAdvancedGem", iVerbose) {
  fDigis = NULL;
  fDigiMatches = NULL;
  fDigiFile = "";
  fClusters = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fThresholdRatio = 0.1;
  fDistanceLimit = 0.5;
  fAlgorithm = 0;

  //  fDebug = 0;
  fEvent = 0;
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchFindHitsAdvancedGem::CbmMuchFindHitsAdvancedGem(const char* name,
    const char* digiFileName, Int_t iVerbose) :
  FairTask(name, iVerbose) {
  fDigis = NULL;
  fDigiMatches = NULL;
  fDigiFile = digiFileName;
  fClusters = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fThresholdRatio = 0.1;
  fDistanceLimit = 0.5;
  fAlgorithm = 0;

  //  fDebug = 0;
  fEvent = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchFindHitsAdvancedGem::~CbmMuchFindHitsAdvancedGem() {
  if (fHits) {
    fHits->Delete();
    delete fHits;
  }
  if (fClusters) {
    fClusters->Delete();
    delete fClusters;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmMuchFindHitsAdvancedGem::Exec(Option_t* opt) {
  fTimer.Start();
  fEvent++;

  // Clear output array
  if (fHits)
    fHits->Clear();
  if (fClusters)
    fClusters->Delete();//Clear(); // Delete because of memory escape

  // Find clusters
  FindClusters();

  Int_t nHits = 0;

  //  // ----------------------------------- Select specific clusters for analysis -----------------------------
  //  fAnalysisClusters.clear();
  //  // Look only for the specific cluster types
  //  for(Int_t iCluster=0; iCluster<fClusters->GetEntriesFast(); ++iCluster){
  //    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(iCluster);
  //    // Look for specific station
  //    Int_t iDigi = cluster->GetDigiIndex(0);
  //    CbmMuchDigi* digi = (CbmMuchDigi*)fDigis->At(iDigi);
  //    if(CbmMuchGeoScheme::GetStationIndex(digi->GetDetectorId()) != fAnalysisStationIndex) continue;
  //
  //    // Look for clusters with specific number of pads
  //    if(cluster->GetNDigis() != fAnalysisClusterSize) continue;
  //
  //    // Look for clusters with specific number of contributed MC-points
  //    set<Int_t> points;
  //    for(Int_t i=0; i<cluster->GetNDigis(); ++i){
  //      Int_t iDigi = cluster->GetDigiIndex(i);
  //      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(iDigi);
  //      for(Int_t j=0; j<match->GetNPoints(); ++j){
  //        Int_t iPoint = match->GetRefIndex(j);
  //        if(points.find(iPoint) == points.end()) points.insert(iPoint);
  //      }
  //    }
  //    if(points.size() != fAnalysisNPoints) continue;
  //
  //    fAnalysisClusters[iCluster] = cluster;
  //  }
  //  // -------------------------------------------------------------------------------------------------------


  // Disentangle clusters
  for (Int_t iCluster = 0; iCluster < fClusters->GetEntriesFast(); ++iCluster) {
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(iCluster);
    //    if(fAnalysisClusters.find(iCluster) == fAnalysisClusters.end()) continue;
    //    CbmMuchCluster* cluster = fAnalysisClusters[iCluster];
    vector<CbmMuchCluster*> clusters;
    if (cluster) {
      switch (fAlgorithm) {
      //------------ Spatial separation ----------
      case 0: {
        CreateHits(cluster, iCluster);
        break;
      }
        //------------ Simple ----------------------
      case 1:
      case 2: {
        ExecClusteringSimple(cluster, clusters);
        CreateHits(clusters, iCluster);
        break;
      }
      case 3: {
        ExecClusteringPeaks(cluster, clusters);
        CreateHits(clusters, iCluster);
        break;
      }
      default: {
        Fatal("CbmMuchFindHitsAdvancedGem::Exec:",
            "The algorithm index does not exist.");
        break;
      }
      }
    }
  }

  fTimer.Stop();
  if (fVerbose) {
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Active channels           : " << fDigis->GetEntriesFast()
         << endl;
    cout << "    Hits created              : " << fHits->GetEntriesFast()
         << endl;
    cout << "    Real time                 : " << fTimer.RealTime() << "s"
         << endl;
    cout << "    Clusters                  : " << fClusters->GetEntriesFast()
         << endl;
  } else {
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
        << fixed << right << fTimer.RealTime() << " s, digis "
        << fDigis->GetEntriesFast() << ", hits: " << fHits->GetEntriesFast()
        << endl;
  }
  StatInfo();
}
// -------------------------------------------------------------------------

void CbmMuchFindHitsAdvancedGem::FinishTask() {
  printf(
      "----------------------- Some statistics -----------------------------\n");
  // Loop over stations
  for (Int_t iStation = 0; iStation < fGeoScheme->GetNStations(); ++iStation) {
    printf("Station %i: losts = %f%, fakes = %f%\n", iStation + 1, fLosts.at(
        iStation) / fEvent * 100, fFakes.at(iStation) / fEvent * 100);
  }
  printf(
      "---------------------------------------------------------------------\n");
}

// -------------------------------------------------------------------------
void CbmMuchFindHitsAdvancedGem::SetNStations(Int_t nStations) {
  fNStations = nStations;
  fThresholdRatios.resize(fNStations);

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchFindHitsAdvancedGem::SetThresholdRatios(Double_t* thresholdRatios) {
  for (Int_t iStation = 0; iStation < fNStations; ++iStation) {
    if (iStation >= fNStations)
      Fatal("CbmMuchFindHitsAdvancedGem::SetThresholdRatios",
          "Station index is out of range.");
    fThresholdRatios.at(iStation) = thresholdRatios[iStation];
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchFindHitsAdvancedGem::SetClusterCharges(Int_t iStation,
    Int_t clusterSize, Int_t *padCharges) {
  // Fill vector of charges for the given station and cluster size
  vector<Int_t> charges;
  for (Int_t iCharge = 0; iCharge < *padCharges; ++iCharge) {
    Int_t charge = padCharges[iCharge + 1];
    charges.push_back(charge);
  }

  // Put the vector into appropriate place in the staionXclusterSize matrix
  if (fChargeLimits.find(iStation) == fChargeLimits.end()) {
    map<Int_t, vector<Int_t> > stationCharges;
    stationCharges[clusterSize] = charges;
    fChargeLimits[iStation] = stationCharges;
  } else {
    fChargeLimits[iStation][clusterSize] = charges;
  }
}
// -------------------------------------------------------------------------

// -----   Public method StatInfo  -----------------------------------------
void CbmMuchFindHitsAdvancedGem::StatInfo() {
  Int_t nHitsTotal = 0; // Total number of hits
  Int_t nPointsTotal = 0; // Total number of processed points
  Double_t frFakes = 0.; // Fraction of fake hits
  Double_t frLosts = 0.; // Fraction of lost points
  // Loop over stations
  for (Int_t iStation = 0; iStation < fGeoScheme->GetNStations(); ++iStation) {
    Double_t frStationFakes = 0.; // Fraction of fake hits in the station
    Double_t frStationLosts = 0.; // Fraction of lost points in the station
    Int_t nStationPointsTotal = 0; // Total number of processed points in the station
    Int_t nStationHitsTotal = 0; // Total number of hits in the station

    // Sort hits by cluster indices for each station
    map<Int_t, vector<CbmMuchPixelHit*> > stationHits;
    for (Int_t iHit = 0; iHit < fHits->GetEntriesFast(); ++iHit) {
      CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fHits->At(iHit);
      if (CbmMuchGeoScheme::GetStationIndex(hit->GetDetectorId()) != iStation)
        continue;
      Int_t iCluster = hit->GetRefId();
      if (stationHits.find(iCluster) == stationHits.end()) {
        vector<CbmMuchPixelHit*> clusterHits;
        stationHits[iCluster] = clusterHits;
      }
      stationHits[iCluster].push_back(hit);
    }

    // Loop over clusters
    for (map<Int_t, vector<CbmMuchPixelHit*> >::iterator it =
        stationHits.begin(); it != stationHits.end(); ++it) {

      // Number of hits in the cluster
      vector<CbmMuchPixelHit*> clusterHits = (*it).second;
      Int_t nHits = clusterHits.size();
      nStationHitsTotal += nHits;
      nHitsTotal += nHits;

      // Loop over cluster digis
      Int_t iCluster = (*it).first;
      CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(iCluster);
      set<Int_t> points;
      for (Int_t i = 0; i < cluster->GetNDigis(); ++i) {
        Int_t iDigi = cluster->GetDigiIndex(i);
        CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(iDigi);
        for (Int_t j = 0; j < match->GetNPoints(); ++j) {
          Int_t iPoint = match->GetRefIndex(j);
          if (points.find(iPoint) == points.end())
            points.insert(iPoint);
        }
      }

      Int_t nPoints = points.size();
      nStationPointsTotal += nPoints;
      nPointsTotal += nPoints;

      if (nPoints > nHits) {
        frStationLosts += nPoints - nHits;
        frLosts += nPoints - nHits;
      }
      if (nHits > nPoints) {
        frStationFakes += nHits - nPoints;
        frFakes += nHits - nPoints;
      }
    }

    frStationLosts /= nStationPointsTotal;
    frStationFakes /= nStationHitsTotal;
    //    printf("Station %i: losts = %f%, fakes = %f%\n", iStation+1, frStationLosts*100, frStationFakes*100);
    //    printf("nStationPointsTotal = %i, nStationHitsTotal = %i\n", nStationPointsTotal, nStationHitsTotal);

    if (fFakes.size() == iStation) {
      fFakes.push_back(0);
    }
    fFakes.at(iStation) += frStationFakes;

    if (fLosts.size() == iStation) {
      fLosts.push_back(0);
    }
    fLosts.at(iStation) += frStationLosts;
  }
  frLosts /= nPointsTotal;
  frFakes /= nHitsTotal;
  //  printf("Total: losts = %f%, fakes = %f%\n", frLosts*100, frFakes*100);
  //  printf("nPointsTotal = %i, nHitsTotal = %i\n", nPointsTotal, nHitsTotal);
  //  printf("---------------------------------------------------------------------\n");
}
// -------------------------------------------------------------------------

// -----   Private method CreateHits  --------------------------------------
void CbmMuchFindHitsAdvancedGem::CreateHits(vector<CbmMuchCluster*> clusters,
    Int_t iCluster) {
  // Produce hits
  for (vector<CbmMuchCluster*>::iterator it = clusters.begin(); it
      != clusters.end(); it++) {
    CbmMuchCluster* cl = (*it);
    CreateHits(cl, iCluster);
    if (cl)
      delete cl;
  }
  clusters.clear();
}
// -------------------------------------------------------------------------

// -----   Private method CreateHits  --------------------------------------
void CbmMuchFindHitsAdvancedGem::CreateHits(CbmMuchCluster* cluster,
    Int_t iCluster) {
  // Produce a hit
  Double_t x, y, z;
  Double_t xq = 0.0;
  Double_t yq = 0.0;
  Double_t dx = std::numeric_limits<Double_t>::max();
  Double_t dy = std::numeric_limits<Double_t>::max();
  Int_t nClusterDigis = cluster->GetNDigis();
  if (!nClusterDigis)
    return;
  CbmMuchPad* pad = NULL;
  for (Int_t iDigi = 0; iDigi < nClusterDigis; iDigi++) {
    Int_t digiIndex = cluster->GetDigiIndex(iDigi);
    Int_t charge = 0;
    pad = GetPadByDigi(digiIndex, charge);
    if (!pad)
      continue;
    xq += pad->GetX0() * charge;
    yq += pad->GetY0() * charge;
    if (dx > pad->GetLx())
      dx = pad->GetLx();
    if (dy > pad->GetLy())
      dy = pad->GetLy();
  }
  // Transform variances into global c.s.
  Double_t sigmaX = dx / TMath::Sqrt(12.);
  Double_t sigmaY = dy / TMath::Sqrt(12.);

  CbmMuchModule* module = fGeoScheme->GetModuleByDetId(pad->GetDetectorId());
  x = xq / cluster->GetCharge();
  y = yq / cluster->GetCharge();
  z = module->GetPosition()[2];

  TVector3 pos, dpos;
  pos.SetXYZ(x, y, z);
  dpos.SetXYZ(sigmaX, sigmaY, 0.);
  Int_t detId = module->GetDetectorId();
  Int_t planeId = fGeoScheme->GetLayerSideNr(detId);

  Int_t iHit = fHits->GetEntriesFast();

  // Calculate time stamp for hit
  Double_t time = 0;
  Double_t dtime = 0;
  for (Int_t i = 0; i < cluster->GetNDigis(); ++i) {
    Int_t iDigi = cluster->GetDigiIndex(i);
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    time += digi->GetTime();
    dtime += TMath::Power(digi->GetDTime(), 2);
  }
  time /= cluster->GetNDigis();
  dtime = TMath::Sqrt(dtime / cluster->GetNDigis());

  new ((*fHits)[iHit]) CbmMuchPixelHit(detId, pos, dpos, 0, iCluster, planeId,
      time, dtime);

  //  printf("time = %f, dtime = %f, size = %i\n", time, dtime, cluster->GetNDigis());
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void CbmMuchFindHitsAdvancedGem::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindHitsAdvancedGem::Init() {
  // Get input arrays
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("CbmMuchFindHitsAdvancedGem::Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");

  // Initialize GeoScheme
  TFile* oldfile = gFile;
  TFile* file = new TFile(fDigiFile);
  TObjArray* stations = (TObjArray*) file->Get("stations");
  file->Close();
  file->Delete();
  gFile = oldfile;
  fGeoScheme->Init(stations);

  // Register output arrays
  fClusters = new TClonesArray("CbmMuchCluster", 1000);
  ioman->Register("MuchCluster", "Cluster in MUCH", fClusters, kTRUE);
  fHits = new TClonesArray("CbmMuchPixelHit", 1000);
  ioman->Register("MuchPixelHit", "Hit in MUCH", fHits, kTRUE);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchFindHitsAdvancedGem::ReInit() {
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method FindChannelDigiMap  ------------------------------
void CbmMuchFindHitsAdvancedGem::FillChannelDigiMap() {
  fChannelDigiMap.clear();
  Int_t nDigis = fDigis->GetEntriesFast();
  for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    if (!digi)
      continue;

    // Skip if digi was generated by straw digitizer
    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(digi->GetDetectorId());
    if (module->GetDetectorType() != 1)
      continue;

    // Unique channel id within the MUCH
    pair<Int_t, Long64_t> uniqueId(digi->GetDetectorId(), digi->GetChannelId());
    if (fChannelDigiMap.find(uniqueId) == fChannelDigiMap.end())
      fChannelDigiMap[uniqueId] = iDigi;
  }
}
// -------------------------------------------------------------------------

// -----   Private method FindClusters  ------------------------------------
void CbmMuchFindHitsAdvancedGem::FindClusters() {
  FillChannelDigiMap();

  fSelectedDigis.clear();
  Int_t nClusters = 0;
  Int_t nDigis = fDigis->GetEntriesFast();
  Int_t dummyCharge = 0;
  for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
    // Selection
    if (fSelectedDigis.find(iDigi) != fSelectedDigis.end())
      continue;

    vector<Int_t> digiIndices;
    Int_t qMax = 0;
    Int_t sumCharge = 0;
    CreateCluster(iDigi, digiIndices, sumCharge, qMax);
    new ((*fClusters)[nClusters]) CbmMuchCluster(digiIndices, sumCharge, qMax);
    // Match digis to the parent cluster
    nClusters++;
  }
}
// -------------------------------------------------------------------------

// -----   Private method CreateCluster  -----------------------------------
void CbmMuchFindHitsAdvancedGem::CreateCluster(Int_t iDigi,
    vector<Int_t> &digiIndices, Int_t &sumCharge, Int_t &qMax, Int_t qThreshold) {
  digiIndices.push_back(iDigi);
  fSelectedDigis.insert(iDigi);
  Int_t charge = 0;
  CbmMuchPad* pad = GetPadByDigi(iDigi, charge);
  if (!pad)
    return;
  if (charge > qMax)
    qMax = charge;
  sumCharge += charge;
  vector<CbmMuchPad*> neighbourPads = pad->GetNeighbours();

  for (vector<CbmMuchPad*>::iterator it = neighbourPads.begin(); it
      != neighbourPads.end(); it++) {
    CbmMuchPad* neighbourPad = *it;
    pair<Int_t, Long64_t> uniqueId(neighbourPad->GetDetectorId(),
        neighbourPad->GetChannelId());
    if (fChannelDigiMap.find(uniqueId) == fChannelDigiMap.end())
      continue;

    Int_t digiIndex = fChannelDigiMap[uniqueId];
    CbmMuchDigiMatch* neighbourMatch = (CbmMuchDigiMatch*) fDigiMatches->At(
        digiIndex);
    if (!neighbourMatch)
      continue;
    Int_t digiCharge = neighbourMatch->GetTotalCharge();
    if (digiCharge <= qThreshold)
      continue;

    // Selection
    if (fSelectedDigis.find(digiIndex) != fSelectedDigis.end())
      continue;

    CreateCluster(digiIndex, digiIndices, sumCharge, qMax, qThreshold);
  }
}
// -------------------------------------------------------------------------

// -----   Private method GetPadByDigi  ------------------------------------
CbmMuchPad* CbmMuchFindHitsAdvancedGem::GetPadByDigi(Int_t digiIndex,
    Int_t &charge) {
  CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(digiIndex);
  CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);
  if (digi) {
    charge = match->GetTotalCharge();

    Int_t detectorId = digi->GetDetectorId();
    Long64_t channelId = digi->GetChannelId();

    CbmMuchModule* mod = (CbmMuchModule*) fGeoScheme->GetModuleByDetId(
        detectorId);
    assert(mod->GetDetectorType() == 1);

    CbmMuchModuleGem* module =
        (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(detectorId);
    CbmMuchPad* pad = module->GetPad(channelId);
    if (pad) {
      assert(pad->GetDetectorId() == digi->GetDetectorId());
      assert(pad->GetChannelId() == digi->GetChannelId());
    }
    return pad;
  }
  return NULL;
}
// -------------------------------------------------------------------------

// -----   Private method ExecClusteringSimple  ----------------------------
void CbmMuchFindHitsAdvancedGem::ExecClusteringSimple(CbmMuchCluster* cluster,
    vector<CbmMuchCluster*> &clusters) {
  fSelectedDigis.clear();

  Int_t iDig = cluster->GetDigiIndex(0);
  CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDig);
  Int_t iStation = fGeoScheme->GetStationIndex(digi->GetDetectorId());
  Double_t thresholdRatio = 0.;
  switch (fAlgorithm) {
  case 1:
    thresholdRatio = fThresholdRatio;
    break;
  case 2:
    if (iStation >= fNStations)
      Fatal("CbmMuchFindHitsAdvancedGem::ExecClusterSimple",
          "Station index is out of range.");
    thresholdRatio = fThresholdRatios.at(iStation);
    break;
  }

  Int_t qThreshold = Int_t(thresholdRatio * cluster->GetMaxCharge());
  Int_t dummy = 0;
  for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++) {
    Int_t digiIndex = cluster->GetDigiIndex(iDigi);
    Int_t charge = 0;
    CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);
    if (!pad)
      continue;
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);
    charge = match->GetTotalCharge();
    if (charge <= qThreshold)
      continue;

    // Selection
    if (fSelectedDigis.find(digiIndex) != fSelectedDigis.end())
      continue;

    vector<Int_t> digiIndices;
    Int_t sumCharge = 0;
    CreateCluster(digiIndex, digiIndices, sumCharge, dummy, qThreshold);
    clusters.push_back(new CbmMuchCluster(digiIndices, sumCharge));
  }
}
// -------------------------------------------------------------------------

// -----   Private method ExecClusteringPeaks   ----------------------------
void CbmMuchFindHitsAdvancedGem::ExecClusteringPeaks(CbmMuchCluster* cluster,
    vector<CbmMuchCluster*> &clusters) {
  fSelectedDigis.clear();
//  printf("Cluster contents:\n");
  map<Double_t, Int_t> chargesX; // map from x coordinate to pad charge
  map<Double_t, Int_t> chargesY; // map from y coordinate to pad charge
  Int_t nDigis = cluster->GetNDigis();
  for(Int_t iDigi = 0; iDigi < nDigis; ++iDigi){
    Int_t charge = 0;
    Int_t digiIndex = cluster->GetDigiIndex(iDigi);
    CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);

    // Collect x-side pads
    Double_t xPad = pad->GetX0();
    Double_t x;
    Bool_t exists = false;
    for(map<Double_t, Int_t>::iterator it = chargesX.begin(); it != chargesX.end(); ++it){
      x = (*it).first;
      if(TMath::Abs(x - xPad) < 1e-5) {
        exists = true;
        break;
      }
    }
    x = xPad;
    chargesX[x] = exists ? chargesX[x] + charge : charge;

    // Collect y-side pads
    Double_t yPad = pad->GetY0();
    Double_t y;
    exists = false;
    for(map<Double_t, Int_t>::iterator it = chargesY.begin(); it != chargesY.end(); ++it){
      y = (*it).first;
      if(TMath::Abs(y - yPad) < 1e-5) {
        exists = true;
        break;
      }
    }
    y = yPad;
    chargesY[y] = exists ? chargesY[y] + charge : charge;

    // Debug info
//    printf("\t%i) (x, y, charge) = (%4.3f, %4.3f, %i)\n", iDigi, xPad, yPad, charge);
  }

  // Debug info
//  printf("\tX-side:\n");
//  for(map<Double_t, Int_t>::iterator it = chargesX.begin(); it != chargesX.end(); ++it){
//    printf("\t\t (x, charge) = (%4.3f, %i)\n", (*it).first, (*it).second);
//  }
//  printf("\tY-side:\n");
//  for(map<Double_t, Int_t>::iterator it = chargesY.begin(); it != chargesY.end(); ++it){
//    printf("\t\t (y, charge) = (%4.3f, %i)\n", (*it).first, (*it).second);
//  }

  // Process x-side
  Int_t charge = 0;
  Bool_t increase = true;
  vector<Double_t> xMin, xMax;
  Double_t previous = std::numeric_limits<Double_t>::max();
  for(map<Double_t, Int_t>::iterator it = chargesX.begin(); it != chargesX.end(); ++it){
    Double_t x = (*it).first;
    Int_t q = (*it).second;
    if(increase != q>charge){
      assert(previous <= x);
      if(!increase) xMin.push_back(previous);
      else xMax.push_back(previous);
    }
    increase = q > charge;
    charge = q;

    previous = x;
  }
  if(increase) xMax.push_back(previous);

  // Debug info
//  printf("\tExtremums in X:\n");
//  for(Int_t iMax = 0; iMax < xMax.size(); ++iMax){
//    printf("\t\t%i maximum: %4.2f\n", iMax+1, xMax.at(iMax));
//  }
//  for(Int_t iMin = 0; iMin < xMin.size(); ++iMin){
//    printf("\t\t%i minimum: %4.2f\n", iMin+1, xMin.at(iMin));
//  }

  // Process y-side
  charge = 0;
  increase = true;
  vector<Double_t> yMin, yMax;
  previous = std::numeric_limits<Double_t>::max();
  for(map<Double_t, Int_t>::iterator it = chargesY.begin(); it != chargesY.end(); ++it){
    Double_t y = (*it).first;
    Int_t q = (*it).second;
    if(increase != q>charge){
      assert(previous <= y);
      if(!increase) yMin.push_back(previous);
      else yMax.push_back(previous);
    }
    increase = q > charge;
    charge = q;

    previous = y;
  }
  if(increase) yMax.push_back(previous);

  // Debug info
//  assert(xMin.size() == xMax.size()-1);
//  assert(yMin.size() == yMax.size()-1);
//  printf("\tExtremums in Y:\n");
//  for(Int_t iMax = 0; iMax < yMax.size(); ++iMax){
//    printf("\t\t%i maximum: %4.2f\n", iMax+1, yMax.at(iMax));
//  }
//  for(Int_t iMin = 0; iMin < yMin.size(); ++iMin){
//    printf("\t\t%i minimum: %4.2f\n", iMin+1, yMin.at(iMin));
//  }


  // Create clusters
  map<Double_t, Int_t>::iterator itX = chargesX.end();
  map<Double_t, Int_t>::iterator itY = chargesY.end();
  --itX; --itY;
  Int_t nGroups = xMax.size() * yMax.size();
  vector<vector<Int_t> > digiGroups(nGroups); // groups of digis (cluster candidates)
  vector<Int_t> groupCharges(nGroups, 0); // total charges of each digi group
  Bool_t xOutOfRange, yOutOfRange;
  for (Int_t iMax = 0; iMax < xMax.size(); ++iMax) {
    xOutOfRange = iMax == xMin.size();
    for (Int_t jMax = 0; jMax < yMax.size(); ++jMax) {
      yOutOfRange = jMax == yMin.size();
      Double_t x = xOutOfRange ? (*itX).first : xMin.at(iMax);
      Double_t y = yOutOfRange ? (*itY).first : yMin.at(jMax);
      for (Int_t iDigi = 0; iDigi < nDigis; ++iDigi) {
        charge = 0;
        Int_t digiIndex = cluster->GetDigiIndex(iDigi);
        // Selection
        if (fSelectedDigis.find(digiIndex) != fSelectedDigis.end())
          continue;
        CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);
        Double_t padX = pad->GetX0();
        Double_t padY = pad->GetY0();

        if (padX <= x && padY <= y){
          Int_t iGroup = jMax * xMax.size() + iMax;
          digiGroups.at(iGroup).push_back(digiIndex);
          groupCharges.at(iGroup) += charge;
//          groupCharges.at(iGroup) ++;
          fSelectedDigis.insert(digiIndex);
        }
      }
    }
  }

  for (Int_t iMax = 0; iMax < xMax.size(); ++iMax) {
    for (Int_t jMax = 0; jMax < yMax.size(); ++jMax) {
      Int_t iGroup = jMax * xMax.size() + iMax;
      // Debug info
//      printf("\t%i/%i group (contains %i pads, total charge %i):\n", iGroup+1, nGroups, digiGroups.at(iGroup).size(), groupCharges.at(iGroup));
//      for(Int_t i = 0; i < digiGroups.at(iGroup).size(); ++i){
//        Int_t digiIndex = digiGroups.at(iGroup).at(i);
//        CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);
//        Double_t padX = pad->GetX0();
//        Double_t padY = pad->GetY0();
//        printf("\t\t(x, y) = (%4.2f, %4.2f)\n", padX, padY);
//      }
      clusters.push_back(new CbmMuchCluster(digiGroups.at(iGroup), groupCharges.at(iGroup)));
    }
  }
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchFindHitsAdvancedGem)
