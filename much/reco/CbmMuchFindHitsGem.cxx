/*
 * CbmMuchFindHitsGem.cxx
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

#include "CbmMuchFindHitsGem.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"
#include "CbmMuchPad.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchDigi.h"
#include "CbmMuchModule.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPoint.h"

#include "CbmMCTrack.h"
#include "FairRootManager.h"

#include "TMath.h"
#include "TFile.h"

#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>

using std::endl;
using std::setw;
using std::setprecision;
using std::left;
using std::fixed;
using std::cout;
using std::right;
using std::multimap;

// -----   Default constructor   ------------------------------------------
CbmMuchFindHitsGem::CbmMuchFindHitsGem() 
  : FairTask("MuchFindHitsAdvancedGem", 1) ,
    fGeoScheme( CbmMuchGeoScheme::Instance()),
    fDigiFile(""),
    fNStations(0),
    fHits(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fClusters(NULL),
    fPoints(NULL),
    fMCTracks(NULL),
    fChannelDigiMap(),
    fSelectedDigis(),
    fThresholdRatio(0.1),
    fThresholdRatios(),
    fAlgorithm(3),
    fTimer(),
    fEpoch(kFALSE),
    fClusterSeparationTime(0.),
    fFakes(),
    fLosts(),
    fSignalMuonsAll(),
    fSignalMuons(),
    fStationEff(),
    fEvent(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchFindHitsGem::CbmMuchFindHitsGem(Int_t iVerbose) 
  : FairTask("MuchFindHitsAdvancedGem", iVerbose),
    fGeoScheme( CbmMuchGeoScheme::Instance()),
    fDigiFile(""),
    fNStations(0),
    fHits(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fClusters(NULL),
    fPoints(NULL),
    fMCTracks(NULL),
    fChannelDigiMap(),
    fSelectedDigis(),
    fThresholdRatio(0.1),
    fThresholdRatios(),
    fAlgorithm(3),
    fTimer(),
    fEpoch(kFALSE),
    fClusterSeparationTime(2.),
    fFakes(),
    fLosts(),
    fSignalMuonsAll(),
    fSignalMuons(),
    fStationEff(),
    fEvent(0)
{
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchFindHitsGem::CbmMuchFindHitsGem(const char* name,
    const char* digiFileName, Int_t iVerbose) 
  : FairTask(name, iVerbose),
    fGeoScheme( CbmMuchGeoScheme::Instance()),
    fDigiFile(digiFileName),
    fNStations(0),
    fHits(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fClusters(NULL),
    fPoints(NULL),
    fMCTracks(NULL),
    fChannelDigiMap(),
    fSelectedDigis(),
    fThresholdRatio(0.1),
    fThresholdRatios(),
    fAlgorithm(3),
    fTimer(),
    fEpoch(kFALSE),
    fClusterSeparationTime(2.),
    fFakes(),
    fLosts(),
    fSignalMuonsAll(),
    fSignalMuons(),
    fStationEff(),
    fEvent(0)

{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchFindHitsGem::~CbmMuchFindHitsGem() {
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
void CbmMuchFindHitsGem::Exec(Option_t* opt) {
  fTimer.Start();
  fEvent++;

  fSignalMuons.clear();
  fSignalMuonsAll.clear();

  // Clear output array
  if (fHits) fHits->Clear();
  if (fClusters) fClusters->Delete();//Clear(); // Delete because of memory escape

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
        //------------ Peaks  ----------------------
      case 3: {
        ExecClusteringPeaks(cluster, clusters);
        CreateHits(clusters, iCluster);
        break;
      }
      default: {
        Fatal("CbmMuchFindHitsGem::Exec:",
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
  }
  else {
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
        << fixed << right << fTimer.RealTime() << " s, digis "
        << fDigis->GetEntriesFast() << ", hits: " << fHits->GetEntriesFast()
        << endl;
  }
  StatInfo();
}
// -------------------------------------------------------------------------

void CbmMuchFindHitsGem::FinishTask() {
  printf(
      "----------------------- Some statistics -----------------------------\n");
  // Loop over stations
  for (Int_t iStation = 0; iStation < fGeoScheme->GetNStations(); ++iStation) {
    vector<Double_t> efficiencies = fStationEff[iStation];
    Double_t eff = 0.0;
    for (vector<Double_t>::iterator it = efficiencies.begin(); it
        != efficiencies.end(); ++it) {
      eff += (*it);
    }
    eff /= efficiencies.size();
    printf("Station %i: losts = %f, fakes = %f, resolved signal = %f\n",
        iStation + 1, fLosts.at(iStation) / fEvent * 100, fFakes.at(iStation)
            / fEvent * 100, eff * 100);
  }
  printf(
      "---------------------------------------------------------------------\n");
}

// -------------------------------------------------------------------------
void CbmMuchFindHitsGem::SetNStations(Int_t nStations) {
  fNStations = nStations;
  fThresholdRatios.resize(fNStations);

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchFindHitsGem::SetThresholdRatios(Double_t* thresholdRatios) {
  for (Int_t iStation = 0; iStation < fNStations; ++iStation) {
    if (iStation >= fNStations) Fatal(
        "CbmMuchFindHitsGem::SetThresholdRatios",
        "Station index is out of range.");
    fThresholdRatios.at(iStation) = thresholdRatios[iStation];
  }
}
// -------------------------------------------------------------------------


// -----   Public method StatInfo  -----------------------------------------
void CbmMuchFindHitsGem::StatInfo() {
  for (Int_t iStation = 0; iStation < fGeoScheme->GetNStations(); ++iStation) {
    Int_t nStationPoints = 0; // Number of MC points in a station
    Int_t nStationHits = 0; // Number of hits in the station
    Int_t nStationLosts = 0; // Number of lost MC points in the station
    Int_t nStationFakes = 0; // Number of fake hits in the station

    // Sort hits by cluster indices in the station
    map<Int_t, vector<CbmMuchPixelHit*> > stationHits;
    for (Int_t iHit = 0; iHit < fHits->GetEntriesFast(); ++iHit) {
      CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fHits->At(iHit);
      if (CbmMuchGeoScheme::GetStationIndex(hit->GetDetectorId()) != iStation) continue;
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
      Int_t iCluster = (*it).first;
      vector<CbmMuchPixelHit*> hits = (*it).second;
      CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(iCluster);
      set<Int_t> pointIndices;
      for (Int_t i = 0; i < cluster->GetNDigis(); ++i) {
        Int_t iDigi = cluster->GetDigiIndex(i); // Digi index
        CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(iDigi);
        for (Int_t k = 0; k < match->GetNPoints(); ++k) {
          Int_t iPoint = match->GetRefIndex(k); // Point index

          if (!fEpoch) {
            // Debug
            CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(iPoint);
            assert(iStation == fGeoScheme->GetStationIndex(point->GetDetectorId()));
          }
          
          if (pointIndices.find(iPoint) == pointIndices.end()) pointIndices.insert(
              iPoint);
        }
      }

      Int_t nPoints = pointIndices.size();
      Int_t nHits = hits.size();
      nStationPoints += nPoints;
      nStationHits += nHits;
      nStationLosts += nPoints > nHits ? nPoints - nHits : 0;
      nStationFakes += nPoints < nHits ? nHits - nPoints : 0;
    }

    Double_t frStationLosts = (Double_t) nStationLosts / nStationPoints;
    Double_t frStationFakes = (Double_t) nStationFakes / nStationHits;
    //    printf("Station %i: losts = %4.2f%, fakes = %4.2f%\n", iStation + 1,
    //        frStationLosts * 100, frStationFakes * 100);

    if (fFakes.size() == iStation) {
      fFakes.push_back(0);
    }
    fFakes.at(iStation) += frStationFakes;

    if (fLosts.size() == iStation) {
      fLosts.push_back(0);
    }
    fLosts.at(iStation) += frStationLosts;

    // Signal muons efficiency in each station
    if (fSignalMuonsAll[iStation].size() == 0) {
      printf(
          "Station %i: Total number of signal = %i, Number of resolved signal = %i\n",
          iStation + 1, (Int_t)fSignalMuonsAll[iStation].size(),
          (Int_t)fSignalMuons[iStation].size());
    }
    else {
      Double_t frSignal = (Double_t) fSignalMuons[iStation].size()
          / fSignalMuonsAll[iStation].size();
      if (fStationEff.find(iStation) == fStationEff.end()) {
        vector<Double_t> efficiencies;
        fStationEff[iStation] = efficiencies;
      }
      fStationEff[iStation].push_back(frSignal);
      printf(
          "Station %i: Total number of signal = %i, Number of resolved signal = %i, signal efficiency = %f \n",
          iStation + 1, (Int_t)fSignalMuonsAll[iStation].size(),
          (Int_t)fSignalMuons[iStation].size(), frSignal * 100);
    }
  }

}
// -------------------------------------------------------------------------

// -----   Private method CreateHits  --------------------------------------
void CbmMuchFindHitsGem::CreateHits(vector<CbmMuchCluster*> clusters,
    Int_t iCluster) {
  // Produce hits
  for (vector<CbmMuchCluster*>::iterator it = clusters.begin(); it
      != clusters.end(); it++) {
    CbmMuchCluster* cl = (*it);
    CreateHits(cl, iCluster);
    if (cl) delete cl;
  }
  clusters.clear();
}
// -------------------------------------------------------------------------

// -----   Private method CreateHits  --------------------------------------
void CbmMuchFindHitsGem::CreateHits(CbmMuchCluster* cluster,
    Int_t iCluster) {
  // Produce a hit
  Double_t x, y, z;
  Double_t xq = 0.0;
  Double_t yq = 0.0;
  Double_t dx = std::numeric_limits<Double_t>::max();
  Double_t dy = std::numeric_limits<Double_t>::max();
  Int_t nClusterDigis = cluster->GetNDigis();
  if (!nClusterDigis) return;

  CbmMuchPad* pad = NULL;
  Double_t time = 0;
  Double_t dtime = 0;

  set<Int_t> points;
  Int_t iPoint, iStation;
  Bool_t isSignalMuon, isIncluded;
  CbmMuchPoint* point;
  for (Int_t i = 0; i < nClusterDigis; i++) {
    Int_t iDigi = cluster->GetDigiIndex(i);
    UInt_t charge = 0;
    pad = GetPadByDigi(iDigi, charge);
    if (!pad) continue;
    xq += pad->GetX0() * charge;
    yq += pad->GetY0() * charge;
    if (dx > pad->GetLx()) dx = pad->GetLx();
    if (dy > pad->GetLy()) dy = pad->GetLy();

    // Calculate time stamp for hit
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    time += digi->GetTime();
    dtime += TMath::Power(digi->GetDTime(), 2);

    if (fEpoch) continue;
    // Find signal muons
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(iDigi);
    for (Int_t k = 0; k < match->GetNPoints(); ++k) {
      iPoint = match->GetRefIndex(k);
      Bool_t isProcessedPoint = points.find(iPoint) != points.end();
      if (!isProcessedPoint) points.insert(iPoint);
      point = (CbmMuchPoint*) fPoints->At(iPoint);
      iStation = fGeoScheme->GetStationIndex(point->GetDetectorId());
      if (fSignalMuons.find(iStation) == fSignalMuons.end()) {
        set<Int_t> muonIndices;
        fSignalMuons[iStation] = muonIndices;
      }
      if (fSignalMuonsAll.find(iStation) == fSignalMuonsAll.end()) {
        set<Int_t> muonIndices;
        fSignalMuonsAll[iStation] = muonIndices;
      }

      // Check if it is a signal muon
      CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(point->GetTrackID());
      isSignalMuon = track->GetMotherId() < 0
          && TMath::Abs(track->GetPdgCode()) == 13;
      Bool_t isIncludedAll = fSignalMuonsAll[iStation].find(iPoint)
          != fSignalMuonsAll[iStation].end();
      if (isSignalMuon && !isIncludedAll) {
        fSignalMuonsAll[iStation].insert(iPoint);
      }
    }
  }

  Bool_t isOnlyPoint = points.size() == 1;
  isIncluded = fSignalMuons[iStation].find(iPoint)
      != fSignalMuons[iStation].end();
  if (isSignalMuon && !isIncluded && isOnlyPoint) fSignalMuons[iStation].insert(
      iPoint);

  time /= cluster->GetNDigis();
  dtime = TMath::Sqrt(dtime / cluster->GetNDigis());

  //printf("time = %f, dtime = %f, size = %i\n", time, dtime, cluster->GetNDigis());

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

  new ((*fHits)[iHit]) CbmMuchPixelHit(detId, pos, dpos, 0, iCluster, planeId,
      time, dtime);
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void CbmMuchFindHitsGem::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindHitsGem::Init() {
  // Get input arrays
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman) Fatal("CbmMuchFindHitsGem::Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
  
  if (!fEpoch){
    fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  }
  
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
InitStatus CbmMuchFindHitsGem::ReInit() {
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method FindChannelDigiMap  ------------------------------
void CbmMuchFindHitsGem::FillChannelDigiMap() {
  fChannelDigiMap.clear();
  Int_t nDigis = fDigis->GetEntriesFast();
  for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    if (!digi) continue;

    // Skip if digi was generated by straw digitizer
    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(digi->GetDetectorId());
    if (module->GetDetectorType() != 1) continue;

    // Unique channel id within the MUCH
    pair<Int_t, Long64_t> uniqueId(digi->GetDetectorId(), digi->GetChannelId());
    fChannelDigiMap[uniqueId] = iDigi;
  }
}
// -------------------------------------------------------------------------

// -----   Private method FindClusters  ------------------------------------
void CbmMuchFindHitsGem::FindClusters() {
  Int_t nClusters = 0;
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules();

  // Clear array of digis in the modules
  for (UInt_t i=0;i<modules.size();i++){
    if (modules[i]->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) modules[i];
    module->ClearDigis();
  }

  // Fill array of digis in the modules. Digis are automatically sorted in time
  for (Int_t iDigi = 0; iDigi < fDigis->GetEntriesFast(); iDigi++) {
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    Double_t time = digi->GetTime();
    Int_t detId = digi->GetDetectorId();
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(detId);
    module->AddDigi(time,iDigi);  
  }
  
  // Find clusters module-by-module
  for (UInt_t m=0;m<modules.size();m++){
    if (modules[m]->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) modules[m];
    multimap<Double_t,Int_t> digis = module->GetDigis();
    multimap<Double_t,Int_t>::iterator it = digis.begin();
    vector<Int_t> time_slice;
    Double_t last_time = -1.e+10;
    while (it!=digis.end() || time_slice.size()>0){
      // This condition is true only at the end of digi loop or in case time slice is separated in the epoch approach
      if (it==digis.end() ? 1 : !fEpoch ? 0 : last_time>0 ? it->first-last_time > fClusterSeparationTime : 0){
        if (!time_slice.size()) continue;
        // create clusters in the time slice

        // Fill channel-to-digi-map
        fChannelDigiMap.clear();
        for (Int_t i=0; i<time_slice.size();i++) {
          Int_t iDigi = time_slice[i];
          CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
          pair<Int_t, Long64_t> uniqueId(digi->GetDetectorId(), digi->GetChannelId());
          fChannelDigiMap[uniqueId] = iDigi;
        }

        // Create clusters
        fSelectedDigis.clear();
        for (Int_t i=0; i<time_slice.size();i++) {
          Int_t iDigi = time_slice[i];
          if (fSelectedDigis.find(iDigi) != fSelectedDigis.end()) continue;
          vector<Int_t> digiIndices;
          UInt_t qMax = 0;
          UInt_t sumCharge = 0;
          CreateCluster(iDigi, digiIndices, sumCharge, qMax);
          new ((*fClusters)[nClusters++]) CbmMuchCluster(digiIndices, sumCharge, qMax);
        }
        
        // Clear time_slice
        time_slice.clear();
      }
      if (it==digis.end()) continue;
      last_time = it->first;
      time_slice.push_back(it->second);
      it++;
    }
  }

}
// -------------------------------------------------------------------------

// -----   Private method CornersNotFired ----------------------------------
Bool_t CbmMuchFindHitsGem::CornersNotFired(CbmMuchPad *pad,
    CbmMuchPad *neighbourPad, Double_t deltaX, Double_t deltaY) {
  Int_t nDx = Int_t((TMath::Abs(deltaX) + 1e-5)/deltaX);
  Int_t nDy = Int_t((TMath::Abs(deltaY) + 1e-5)/deltaY);

  // Debug
  if(deltaX < 0) assert(nDx == -1);
  else assert(nDx == 1);
  if(deltaY < 0) assert(nDy == -1);
  else assert(nDy == 1);

  Bool_t result1 = kFALSE;
  Bool_t result2 = kFALSE;

  vector<CbmMuchPad*> neighbours = neighbourPad->GetNeighbours();

  for (vector<CbmMuchPad*>::iterator it = neighbours.begin(); it
      != neighbours.end(); ++it) {
    CbmMuchPad* p = *it;
    // Omit main pad
    if (p->GetDetectorId() == pad->GetDetectorId() &&
        p->GetChannelId() == pad->GetChannelId()) continue;

    // Omit fired pads
    pair<Int_t, Long64_t> uniqueId(p->GetDetectorId(), p->GetChannelId());
    Bool_t isFired = fChannelDigiMap.find(uniqueId) != fChannelDigiMap.end();
    if (isFired) continue;

    // Verify two pads near the corner
    Bool_t isSameHSide = TMath::Abs(neighbourPad->GetX0() - nDx
        * neighbourPad->GetLx() / 2 - (p->GetX0() - nDx * p->GetLx() / 2.)) < 1e-5;
    Bool_t isV = nDy * (neighbourPad->GetY0() - p->GetY0()) > 0;
    Bool_t isSameVSide = TMath::Abs(neighbourPad->GetY0() - nDy
        * neighbourPad->GetLy() / 2 - (p->GetY0() - nDy * p->GetLy() / 2.)) < 1e-5;
    Bool_t isH = nDx * (neighbourPad->GetX0() - p->GetX0()) > 0;

    if (!result1) result1 = isSameHSide && isV;
    if (!result2) result2 = isSameVSide && isH;
  }

  return result1 && result2;
}

// -------------------------------------------------------------------------

// -----   Private method IsCornerBorder -----------------------------------
Bool_t CbmMuchFindHitsGem::IsCornerBorder(CbmMuchPad *pad,
    CbmMuchPad *neighbourPad) {
  Double_t deltaX = neighbourPad->GetX0() - pad->GetX0();
  Double_t deltaY = neighbourPad->GetY0() - pad->GetY0();
  Double_t sumLx = pad->GetLx() / 2 + neighbourPad->GetLx() / 2;
  Double_t sumLy = pad->GetLy() / 2 + neighbourPad->GetLy() / 2;
  Bool_t isCornerNeighbour = TMath::Abs(TMath::Abs(deltaX) - sumLx) < 1e-5
      && TMath::Abs(TMath::Abs(deltaY) - sumLy) < 1e-5;
  if (!isCornerNeighbour) return kFALSE;

  return CornersNotFired(pad, neighbourPad, deltaX, deltaY);
}
// -------------------------------------------------------------------------

// -----   Private method CreateCluster  -----------------------------------
void CbmMuchFindHitsGem::CreateCluster(Int_t iDigi,
    vector<Int_t> &digiIndices, UInt_t &sumCharge, UInt_t &qMax, UInt_t qThreshold) {
  digiIndices.push_back(iDigi);
  fSelectedDigis.insert(iDigi);
  UInt_t charge = 0;
  CbmMuchPad* pad = GetPadByDigi(iDigi, charge);
  if (!pad) return;
  if (charge > qMax) qMax = charge;
  // Overflow proof
  Bool_t isOverflow = std::numeric_limits<UInt_t>::max() - charge < sumCharge;
  sumCharge = isOverflow ? std::numeric_limits<UInt_t>::max() : sumCharge + charge;
  vector<CbmMuchPad*> neighbourPads = pad->GetNeighbours();

  // Processed neighbour pads
  for (vector<CbmMuchPad*>::iterator it = neighbourPads.begin(); it
      != neighbourPads.end(); it++) {
    CbmMuchPad* neighbourPad = *it;
    pair<Int_t, Long64_t> uniqueId(neighbourPad->GetDetectorId(),
        neighbourPad->GetChannelId());

    if (fChannelDigiMap.find(uniqueId) == fChannelDigiMap.end()) continue; // pad not fired

    Int_t digiIndex = fChannelDigiMap[uniqueId];
    CbmMuchDigiMatch* neighbourMatch = (CbmMuchDigiMatch*) fDigiMatches->At(
        digiIndex);
    if (!neighbourMatch) continue;
    Int_t digiCharge = neighbourMatch->GetTotalCharge();
    if (digiCharge <= qThreshold) continue;

    // Selection
    if (fSelectedDigis.find(digiIndex) != fSelectedDigis.end()) continue; // pad already included

    if (IsCornerBorder(pad, neighbourPad)) continue;

    CreateCluster(digiIndex, digiIndices, sumCharge, qMax, qThreshold);
  }
}
// -------------------------------------------------------------------------

// -----   Private method GetPadByDigi  ------------------------------------
CbmMuchPad* CbmMuchFindHitsGem::GetPadByDigi(Int_t digiIndex, UInt_t &charge) {
  CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(digiIndex);
  CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);
  if (digi) {
//    charge = match->GetTotalCharge();
    charge = digi->GetADCCharge();

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
void CbmMuchFindHitsGem::ExecClusteringSimple(CbmMuchCluster* cluster,
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
    if (iStation >= fNStations) Fatal(
        "CbmMuchFindHitsGem::ExecClusterSimple",
        "Station index is out of range.");
    thresholdRatio = fThresholdRatios.at(iStation);
    break;
  }

  UInt_t qThreshold = UInt_t(thresholdRatio * cluster->GetMaxCharge());
  UInt_t dummy = 0;
  for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++) {
    Int_t digiIndex = cluster->GetDigiIndex(iDigi);
    UInt_t charge = 0;
    CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);
    if (!pad) continue;
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);
    charge = match->GetTotalCharge();
    if (charge <= qThreshold) continue;

    // Selection
    if (fSelectedDigis.find(digiIndex) != fSelectedDigis.end()) continue;

    vector<Int_t> digiIndices;
    UInt_t sumCharge = 0;
    CreateCluster(digiIndex, digiIndices, sumCharge, dummy, qThreshold);
    clusters.push_back(new CbmMuchCluster(digiIndices, sumCharge));
  }
}
// -------------------------------------------------------------------------

// -----   Private method ExecClusteringPeaks   ----------------------------
void CbmMuchFindHitsGem::ExecClusteringPeaks(CbmMuchCluster* cluster,
    vector<CbmMuchCluster*> &clusters) {
  fSelectedDigis.clear();
  //  printf("Cluster contents:\n");
  map<Double_t, UInt_t> chargesX; // map from x coordinate to pad charge
  map<Double_t, UInt_t> chargesY; // map from y coordinate to pad charge
  Int_t nDigis = cluster->GetNDigis();
  for (Int_t iDigi = 0; iDigi < nDigis; ++iDigi) {
    UInt_t charge = 0;
    Int_t digiIndex = cluster->GetDigiIndex(iDigi);
    CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);

    // Collect x-side pads
    Double_t xPad = pad->GetX0();
    Double_t x;
    Bool_t exists = false;
    for (map<Double_t, UInt_t>::iterator it = chargesX.begin(); it != chargesX.end(); ++it) {
      x = (*it).first;
      if (TMath::Abs(x - xPad) < 1e-5) {
        exists = true;
        break;
      }
    }
    x = xPad;
    if(!exists){
      chargesX[x] = 0;
    }
    // Overflow proof
    Bool_t isOverflow = std::numeric_limits<UInt_t>::max() - charge < chargesX[x];
    chargesX[x] = isOverflow ? std::numeric_limits<UInt_t>::max() : chargesX[x] + charge;

    // Collect y-side pads
    Double_t yPad = pad->GetY0();
    Double_t y;
    exists = false;
    for (map<Double_t, UInt_t>::iterator it = chargesY.begin(); it
        != chargesY.end(); ++it) {
      y = (*it).first;
      if (TMath::Abs(y - yPad) < 1e-5) {
        exists = true;
        break;
      }
    }
    y = yPad;
    if(!exists){
      chargesY[y] = 0;
    }
    // Overflow proof
    isOverflow = std::numeric_limits<UInt_t>::max() - charge < chargesY[y];
    chargesY[y] = isOverflow ? std::numeric_limits<UInt_t>::max() : chargesY[y] + charge;

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
  UInt_t charge = 0;
  Bool_t increase = true;
  vector<Double_t> xMin, xMax;
  Double_t previous = std::numeric_limits<Double_t>::max();
  for (map<Double_t, UInt_t>::iterator it = chargesX.begin(); it!= chargesX.end(); ++it) {
    Double_t x = (*it).first;
    UInt_t q = (*it).second;
    if (increase != q >= charge) {
      assert(previous <= x);
      if (!increase) xMin.push_back(previous);
      else xMax.push_back(previous);
    }
    increase = q >= charge;
    charge = q;

    previous = x;
  }
  if (increase) xMax.push_back(previous);

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
  for (map<Double_t, UInt_t>::iterator it = chargesY.begin(); it
      != chargesY.end(); ++it) {
    Double_t y = (*it).first;
    Int_t q = (*it).second;
    if (increase != q >= charge) {
      assert(previous <= y);
      if (!increase) yMin.push_back(previous);
      else yMax.push_back(previous);
    }
    increase = q >= charge;
    charge = q;

    previous = y;
  }
  if (increase) yMax.push_back(previous);

  // Debug info
  assert(xMin.size() == xMax.size()-1);
  assert(yMin.size() == yMax.size()-1);
  //  printf("\tExtremums in Y:\n");
  //  for(Int_t iMax = 0; iMax < yMax.size(); ++iMax){
  //    printf("\t\t%i maximum: %4.2f\n", iMax+1, yMax.at(iMax));
  //  }
  //  for(Int_t iMin = 0; iMin < yMin.size(); ++iMin){
  //    printf("\t\t%i minimum: %4.2f\n", iMin+1, yMin.at(iMin));
  //  }


  // Create clusters
  map<Double_t, UInt_t>::iterator itX = chargesX.end();
  map<Double_t, UInt_t>::iterator itY = chargesY.end();
  --itX;
  --itY;
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
        if (fSelectedDigis.find(digiIndex) != fSelectedDigis.end()) continue;
        CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);
        Double_t padX = pad->GetX0();
        Double_t padY = pad->GetY0();

        if (padX <= x && padY <= y) {
          Int_t iGroup = jMax * xMax.size() + iMax;
          digiGroups.at(iGroup).push_back(digiIndex);
          // Overflow proof
          Bool_t isOverflow = std::numeric_limits<UInt_t>::max() - charge < groupCharges.at(iGroup);
          groupCharges.at(iGroup) = isOverflow ? std::numeric_limits<UInt_t>::max() :  groupCharges.at(iGroup) + charge;
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
      clusters.push_back(new CbmMuchCluster(digiGroups.at(iGroup),
          groupCharges.at(iGroup)));
    }
  }
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchFindHitsGem)
