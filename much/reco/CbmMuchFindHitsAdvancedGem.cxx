/*
 * CbmMuchFindHitsAdvancedGem.cxx
 *
 *  Created on: Jun 17, 2007
 *      Author: M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *
 *  Produces hits in GEM-like modules using different
 *  clustering algorithms. Clustering algorithms can be chosen
 *  by the switch SetAlgorithm(Int_t iAlgorithm)
 *  Several clustering algorithms are supported
 *  0 - Spatial separation (clusters are sets of neighbor fired pads)
 *  1 - Simple
 *  2 - Ward (fuzzy clustering)
 *  3 - Simple + Ward
 *  4 - Divisive clustering
 *  5 - Simple + divisive clustering
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
  fDigiFile = NULL;
  fClusters = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fThresholdRatio = 0.1;
  fDistanceLimit = 0.5;
  fAlgorithm = 0;

//  fDebug = 0;
  fEvent=0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchFindHitsAdvancedGem::CbmMuchFindHitsAdvancedGem(Int_t iVerbose) :
  FairTask("MuchFindHitsAdvancedGem", iVerbose) {
  fDigis = NULL;
  fDigiMatches = NULL;
  fDigiFile = NULL;
  fClusters = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fThresholdRatio = 0.1;
  fDistanceLimit = 0.5;
  fAlgorithm = 0;

//  fDebug = 0;
  fEvent=0;
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchFindHitsAdvancedGem::CbmMuchFindHitsAdvancedGem(const char* name, const char* digiFileName,
    Int_t iVerbose) : FairTask(name, iVerbose) {
  fDigis = NULL;
  fDigiMatches = NULL;
  fDigiFile    = new TFile(digiFileName);
  fClusters = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fThresholdRatio = 0.1;
  fDistanceLimit = 0.5;
  fAlgorithm = 0;

//  fDebug = 0;
  fEvent=0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchFindHitsAdvancedGem::~CbmMuchFindHitsAdvancedGem() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
  if ( fClusters ) {
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
  for(Int_t iCluster = 0; iCluster<fClusters->GetEntriesFast(); ++iCluster){
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(iCluster);
//    if(fAnalysisClusters.find(iCluster) == fAnalysisClusters.end()) continue;
//    CbmMuchCluster* cluster = fAnalysisClusters[iCluster];
    vector<CbmMuchCluster*> clusters;
    if (cluster) {
      switch(fAlgorithm){
        //------------ Spatial separation ----------
        case 0:{
          CreateHits(cluster, iCluster);
          break;
        }
        //------------ Simple ----------------------
        case 1:{
          ExecClusteringSimple(cluster, clusters);
          CreateHits(clusters, iCluster);
          break;
        }
        //------------ Ward ------------------------
        case 2:{
          ExecClusteringWard(cluster, clusters);
          CreateHits(clusters, iCluster);
          break;
        }
        //------------ Simple+Ward -----------------
        case 3:{
          ExecClusteringSimple(cluster, clusters);
          for(vector<CbmMuchCluster*>::iterator it = clusters.begin(); it!=clusters.end(); ++it){
            CbmMuchCluster* cl = (*it);
            vector<CbmMuchCluster*> cls;
            ExecClusteringWard(cl, cls);
            CreateHits(cls, iCluster);
            if(cl) delete cl;
          }
          clusters.clear();
          break;
        }
        //------------ Divisive --------------------
        case 4:{
          //printf("clusters.size() = %i\n",clusters.size());
          ExecClusteringDivisive(cluster, clusters);
          //printf("clusters.size() = %i\n",clusters.size());
          CreateHits(clusters, iCluster);
          break;
        }
        //------------ Simple+Divisive -------------
        case 5:{
          ExecClusteringSimple(cluster, clusters);
          for(vector<CbmMuchCluster*>::iterator it = clusters.begin(); it!=clusters.end(); ++it){
            CbmMuchCluster* cl = (*it);
            vector<CbmMuchCluster*> cls;
            ExecClusteringDivisive(cl, cls);
            CreateHits(cls, iCluster);
            if(cl) delete cl;
          }
          clusters.clear();
          break;
        }

        default:{
          Fatal("CbmMuchFindHitsAdvancedGem::Exec:", "The algorithm index does not exist.");
          break;
        }
      }
    }
  }

  fTimer.Stop();
  if (fVerbose) {
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Active channels           : " << fDigis->GetEntriesFast() << endl;
    cout << "    Hits created              : " << fHits->GetEntriesFast() << endl;
    cout << "    Real time                 : " << fTimer.RealTime() << "s" << endl;
//    cout << "    Clusters                  : " << fAnalysisClusters.size() << endl;
    cout << "    Clusters                  : " << fClusters->GetEntriesFast() << endl;
  } else {
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
    << fixed << right << fTimer.RealTime() << " s, digis "
    << fDigis->GetEntriesFast() << ", hits: "
    << fHits->GetEntriesFast() << endl;
  }
  StatInfo();
}
// -------------------------------------------------------------------------

void CbmMuchFindHitsAdvancedGem::FinishTask(){
  printf("----------------------- Some statistics -----------------------------\n");
  // Loop over stations
  for(Int_t iStation=0; iStation < fGeoScheme->GetNStations(); ++iStation){
    printf("Station %i: losts = %f%, fakes = %f%\n", iStation+1, fLosts.at(iStation)/fEvent*100, fFakes.at(iStation)/fEvent*100);
  }
  printf("---------------------------------------------------------------------\n");
}

// -------------------------------------------------------------------------
void CbmMuchFindHitsAdvancedGem::SetClusterDistanceLimits(Int_t iStation, Int_t clusterSize, Double_t *distanceLimits){
  // Fill vector of distances for the given station and cluster size
  vector<Double_t> distances;
  for(Int_t iDistance=0; iDistance<*distanceLimits; ++iDistance){
    Double_t distance = distanceLimits[iDistance+1];
    distances.push_back(distance);
  }

  // Put the vector into appropriate place in the staionXclusterSize matrix
  if(fDistanceLimits.find(iStation) == fDistanceLimits.end()){
    map<Int_t, vector<Double_t> > stationDistances;
    stationDistances[clusterSize]= distances;
    fDistanceLimits[iStation] = stationDistances;
  }
  else{
    fDistanceLimits[iStation][clusterSize] = distances;
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchFindHitsAdvancedGem::SetClusterCharges(Int_t iStation, Int_t clusterSize, Int_t *padCharges){
  // Fill vector of charges for the given station and cluster size
  vector<Int_t> charges;
  for(Int_t iCharge=0; iCharge<*padCharges; ++iCharge){
    Int_t charge = padCharges[iCharge+1];
    charges.push_back(charge);
  }

  // Put the vector into appropriate place in the staionXclusterSize matrix
  if(fChargeLimits.find(iStation) == fChargeLimits.end()){
    map<Int_t, vector<Int_t> > stationCharges;
    stationCharges[clusterSize]= charges;
    fChargeLimits[iStation] = stationCharges;
  }
  else{
    fChargeLimits[iStation][clusterSize] = charges;
  }
}
// -------------------------------------------------------------------------

// -----   Public method StatInfo  -----------------------------------------
void CbmMuchFindHitsAdvancedGem::StatInfo(){
  Int_t nHitsTotal = 0; // Total number of hits
  Int_t nPointsTotal = 0; // Total number of processed points
  Double_t frFakes = 0.; // Fraction of fake hits
  Double_t frLosts = 0.; // Fraction of lost points
  // Loop over stations
  for(Int_t iStation=0; iStation < fGeoScheme->GetNStations(); ++iStation){
    Double_t frStationFakes = 0.; // Fraction of fake hits in the station
    Double_t frStationLosts = 0.; // Fraction of lost points in the station
    Int_t nStationPointsTotal = 0; // Total number of processed points in the station
    Int_t nStationHitsTotal = 0;   // Total number of hits in the station

    // Sort hits by cluster indices for each station
    map<Int_t, vector<CbmMuchPixelHit*> > stationHits;
    for(Int_t iHit = 0; iHit<fHits->GetEntriesFast(); ++iHit){
      CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fHits->At(iHit);
      if(CbmMuchGeoScheme::GetStationIndex(hit->GetDetectorId()) != iStation) continue;
      Int_t iCluster = hit->GetRefId();
      if(stationHits.find(iCluster) == stationHits.end()){
        vector<CbmMuchPixelHit*> clusterHits;
        stationHits[iCluster] = clusterHits;
      }
      stationHits[iCluster].push_back(hit);
    }

    // Loop over clusters
    for(map<Int_t, vector<CbmMuchPixelHit*> >::iterator it = stationHits.begin();
      it!=stationHits.end(); ++it){

      // Number of hits in the cluster
      vector<CbmMuchPixelHit*> clusterHits = (*it).second;
      Int_t nHits = clusterHits.size();
      nStationHitsTotal += nHits;
      nHitsTotal += nHits;

      // Loop over cluster digis
      Int_t iCluster = (*it).first;
      CbmMuchCluster* cluster = (CbmMuchCluster*)fClusters->At(iCluster);
      set<Int_t> points;
      for(Int_t i=0; i<cluster->GetNDigis(); ++i){
        Int_t iDigi = cluster->GetDigiIndex(i);
        CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(iDigi);
        for(Int_t j=0; j<match->GetNPoints(); ++j){
          Int_t iPoint = match->GetRefIndex(j);
          if(points.find(iPoint) == points.end()) points.insert(iPoint);
        }
      }

      Int_t nPoints = points.size();
      nStationPointsTotal += nPoints;
      nPointsTotal += nPoints;

      if(nPoints > nHits) {
        frStationLosts += nPoints - nHits;
        frLosts += nPoints - nHits;
      }
      if(nHits > nPoints) {
        frStationFakes += nHits - nPoints;
        frFakes += nHits - nPoints;
      }
    }

    frStationLosts /= nStationPointsTotal;
    frStationFakes /= nStationHitsTotal;
//    printf("Station %i: losts = %f%, fakes = %f%\n", iStation+1, frStationLosts*100, frStationFakes*100);
//    printf("nStationPointsTotal = %i, nStationHitsTotal = %i\n", nStationPointsTotal, nStationHitsTotal);

    if(fFakes.size() == iStation){
      fFakes.push_back(0);
    }
    fFakes.at(iStation) += frStationFakes;

    if(fLosts.size() == iStation){
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
void CbmMuchFindHitsAdvancedGem::CreateHits(vector<CbmMuchCluster*> clusters, Int_t iCluster){
  // Produce hits
  for (vector<CbmMuchCluster*>::iterator it = clusters.begin(); it != clusters.end(); it++) {
    CbmMuchCluster* cl = (*it);
    CreateHits(cl, iCluster);
    if (cl) delete cl;
  }
  clusters.clear();
}
// -------------------------------------------------------------------------

// -----   Private method CreateHits  --------------------------------------
void CbmMuchFindHitsAdvancedGem::CreateHits(CbmMuchCluster* cluster, Int_t iCluster){
  // Produce a hit
  Double_t x, y, z;
  Double_t xq = 0.0;
  Double_t yq = 0.0;
  Double_t dx = std::numeric_limits<Double_t>::max();
  Double_t dy = std::numeric_limits<Double_t>::max();
  Int_t nClusterDigis = cluster->GetNDigis();
  if(!nClusterDigis) return;
  CbmMuchPad* pad = NULL;
  for (Int_t iDigi = 0; iDigi < nClusterDigis; iDigi++) {
    Int_t digiIndex = cluster->GetDigiIndex(iDigi);
    Int_t charge = 0;
    pad = GetPadByDigi(digiIndex, charge);
    if (!pad) continue;
    xq += pad->GetX0() * charge;
    yq += pad->GetY0() * charge;
    if (dx > pad->GetLx()) dx = pad->GetLx();
    if (dy > pad->GetLy()) dy = pad->GetLy();
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
  for(Int_t i = 0; i < cluster->GetNDigis(); ++i){
    Int_t iDigi = cluster->GetDigiIndex(i);
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    time += digi->GetTime();
    dtime += TMath::Power(digi->GetDTime(), 2);
  }
  time /= cluster->GetNDigis();
  dtime = TMath::Sqrt(dtime)/cluster->GetNDigis();

  new ((*fHits)[iHit]) CbmMuchPixelHit(detId, pos, dpos, 0, iCluster, planeId, time, dtime);

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
    Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");

  // Initialize GeoScheme
  TObjArray* stations = (TObjArray*) fDigiFile->Get("stations");
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
    if (!digi) continue;

    // Skip if digi was generated by straw digitizer
    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(digi->GetDetectorId());
    if(module->GetDetectorType()!=1) continue;

    // Unique channel id within the MUCH
    pair<Int_t, Int_t> uniqueId(digi->GetDetectorId(), digi->GetChannelId());
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
    vector<Int_t> &digiIndices, Int_t &sumCharge, Int_t &qMax,
    Int_t qThreshold) {
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
    pair<Int_t, Int_t> uniqueId(neighbourPad->GetDetectorId(),
        neighbourPad->GetChannelId());
    if (fChannelDigiMap.find(uniqueId) == fChannelDigiMap.end()) continue;

    Int_t digiIndex = fChannelDigiMap[uniqueId];
    CbmMuchDigiMatch* neighbourMatch = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);
    if (!neighbourMatch) continue;
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
CbmMuchPad* CbmMuchFindHitsAdvancedGem::GetPadByDigi(Int_t digiIndex, Int_t &charge) {
  CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(digiIndex);
  CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);
  if (digi) {
    charge = match->GetTotalCharge();

    Int_t detectorId = digi->GetDetectorId();
    Int_t channelId = digi->GetChannelId();

    CbmMuchModule* mod = (CbmMuchModule*) fGeoScheme->GetModuleByDetId(detectorId);
    assert(mod->GetDetectorType() == 1);

    CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(detectorId);
    CbmMuchPad* pad = module->GetPad(channelId);
    if (pad){
      assert(pad->GetDetectorId() == digi->GetDetectorId());
      assert(pad->GetChannelId() == digi->GetChannelId());
    }
    return pad;
  }
  return NULL;
}
// -------------------------------------------------------------------------

// -----   Private method ExecClusteringSimple  ----------------------------
void CbmMuchFindHitsAdvancedGem::ExecClusteringSimple(CbmMuchCluster* cluster, vector<CbmMuchCluster*> &clusters) {
  fSelectedDigis.clear();
  Int_t qThreshold = (Int_t) fThresholdRatio*cluster->GetMaxCharge();
  Int_t dummy = 0;
  for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++) {
    Int_t digiIndex = cluster->GetDigiIndex(iDigi);
    Int_t charge = 0;
    CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);
    if (!pad) continue;
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);
    charge = match->GetTotalCharge();
    if (charge <= qThreshold) continue;

    // Selection
    if (fSelectedDigis.find(digiIndex) != fSelectedDigis.end()) continue;

    vector<Int_t> digiIndices;
    Int_t sumCharge = 0;
    CreateCluster(digiIndex, digiIndices, sumCharge, dummy, qThreshold);
    clusters.push_back(new CbmMuchCluster(digiIndices, sumCharge));
  }
}
// -------------------------------------------------------------------------

// -----   Private method ExecClusteringWard  ------------------------------
void CbmMuchFindHitsAdvancedGem::ExecClusteringWard(CbmMuchCluster* cluster, vector<CbmMuchCluster*> &clusters){

  TArrayI digiIndices = cluster->GetDigiIndices();

  // Create list of groups of digis for the cluster
  vector<vector<Int_t> > digiGroups;
  for(Int_t i=0; i<digiIndices.GetSize(); ++i){
    // Only one digi per group on the first stage of recursion
    vector<Int_t> digiGroup;
    digiGroup.push_back(digiIndices.At(i));
    digiGroups.push_back(digiGroup);
  }

  // Calculate ESS for each group in the cluster
  vector<Double_t> essVector; // List of ESS for each group
  for(Int_t iGroup=0; iGroup<digiGroups.size(); ++iGroup){
    vector<Int_t> digiGroup = digiGroups.at(iGroup);
    essVector.push_back(GetESS(digiGroup));
  }
  essVector.push_back(0.0); // The last element is the total ESS for the groups
  assert(essVector.size() == digiGroups.size()+1);

  // Disentangle the cluster into groups of neighbour digis
  GroupDigisWard(digiGroups, essVector);

  // Create CbmMuchcluster objects
  clusters = GetDigiGroupsClusters(digiGroups);
}
// -------------------------------------------------------------------------

// -----   Private method GroupDigisWard  ----------------------------------
void CbmMuchFindHitsAdvancedGem::GroupDigisWard(vector<vector<Int_t> > &digiGroups, vector<Double_t> &essVector) {
  Int_t group1 = -1; // Group index of the first cluster to merge
  Int_t group2 = -1; // Group index of the second cluster to merge
  Double_t totalEssOld = essVector.at(essVector.size() - 1);  // Total ESS of the previous configuration
  Double_t deltaEssMin = std::numeric_limits<Double_t>::max(); // Minimum ESS increment
  Double_t mergedEss = 0;  // ESS of the two groups will be merged

  for(Int_t iGroup = 0; iGroup < digiGroups.size(); ++iGroup){
    vector<Int_t> digiGroup1 = digiGroups.at(iGroup);
    Double_t essI = essVector.at(iGroup);
    for(Int_t jGroup = iGroup+1; jGroup < digiGroups.size(); ++jGroup){
      vector<Int_t> digiGroup2 = digiGroups.at(jGroup);
      Double_t essIJsingle = essI + essVector.at(jGroup); // Sum of the ESS for the two current single groups
      Double_t essIJmerged = GetESS(digiGroup1, digiGroup2); // ESS for the two current groups if the are merged

      // ESS increment
      Double_t deltaEss = essIJmerged - essIJsingle;
      //assert(deltaEss >= 0);

      // Search for the minimal ESS increment
      if(deltaEss < deltaEssMin){
        deltaEssMin = deltaEss;
        group1 = iGroup;
        group2 = jGroup;
        mergedEss = essIJmerged;
      }
    } // Other digi groups loop
  } // Current digi group loop

  // Merge groups, if ESS increment is less than some upper limit
  if(deltaEssMin < fDistanceLimit){
    //assert(group2 < digiGroups.size());
    //assert(group2 < digiGroups.size());

    // Update the list of grouped digi indices
    vector<Int_t> digiGroup2 = digiGroups.at(group2);
    for(Int_t iGroup=0;iGroup<digiGroup2.size(); ++iGroup){
      digiGroups.at(group1).push_back(digiGroup2.at(iGroup));
    }
    digiGroups.erase(digiGroups.begin()+group2);

    // Update the list of ESS
    essVector.at(essVector.size()-1) = totalEssOld - essVector.at(group1) - essVector.at(group2) + mergedEss;
    essVector.at(group1) = mergedEss;
    essVector.erase(essVector.begin() + group2);

    // Make next step (recursion)
    GroupDigisWard(digiGroups, essVector);
  }
}
// -------------------------------------------------------------------------

// -----   Private method GetESS  ------------------------------------------
Double_t CbmMuchFindHitsAdvancedGem::GetESS(vector<Int_t> digiGroup1, vector<Int_t> digiGroup2){
  // Calculates ESS for two groups if they are united
  Double_t ess = 0.;
  Double_t coef = 1./(digiGroup1.size() + digiGroup2.size());
  Double_t sumX = 0.;
  Double_t sumY = 0.;
  Int_t sumQ = 0;
  // Calculate ESS for each group
  for(Int_t i=0;i<2;++i){
    vector<Int_t> digiGroup = i==0 ? digiGroup1 : digiGroup2;
    for(vector<Int_t>::iterator it = digiGroup.begin(); it!=digiGroup.end(); ++it){
      Int_t iDigi = (*it);
      CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
      Int_t q = -1;
      CbmMuchPad* pad = GetPadByDigi(iDigi, q);
      assert(pad);
      Double_t x = pad->GetX0();
      Double_t y = pad->GetY0();

      ess += x*x + y*y + digi->GetADCCharge()*digi->GetADCCharge();
      sumX += x;
      sumY += y;
      sumQ += digi->GetADCCharge();
    }
  }
  ess -= coef*(sumX*sumX + sumY*sumY + sumQ*sumQ);
  return ess;
}
// -------------------------------------------------------------------------

// -----   Private method GetESS  ------------------------------------------
Double_t CbmMuchFindHitsAdvancedGem::GetESS(vector<Int_t> digiGroup){
  Double_t ess = 0.;
  Double_t coef = 1./digiGroup.size();
  Double_t sumX = 0.;
  Double_t sumY = 0.;
  Int_t sumQ = 0;
  // Calculate ESS for each group
  for(vector<Int_t>::iterator it = digiGroup.begin(); it!=digiGroup.end(); ++it){
    Int_t iDigi = (*it);
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    Int_t q = -1;
    CbmMuchPad* pad = GetPadByDigi(iDigi, q);
    assert(pad);
    Double_t x = pad->GetX0();
    Double_t y = pad->GetY0();

    ess += x*x + y*y + digi->GetADCCharge()*digi->GetADCCharge();
    sumX += x;
    sumY += y;
    sumQ += digi->GetADCCharge();
  }
  ess -= coef*(sumX*sumX + sumY*sumY + sumQ*sumQ);
  return ess;
}
// -------------------------------------------------------------------------

// -----   Private method ExecClusteringDivisive  --------------------------
void CbmMuchFindHitsAdvancedGem::ExecClusteringDivisive(CbmMuchCluster* cluster, vector<CbmMuchCluster*> &clusters){
  TArrayI digiIndices = cluster->GetDigiIndices();

  // Create one group of digis from the given cluster on the first stage
  vector<vector<Int_t> > digiGroups;
  vector<Int_t> digiGroup;
  for(Int_t i=0; i<digiIndices.GetSize(); ++i){
    digiGroup.push_back(digiIndices.At(i));
  }
  digiGroups.push_back(digiGroup);
//  if(digiGroups.at(0).size() == 3 && digiGroups.at(0).at(0) == 1) fDebug=1;
////  if(digiGroups.at(0).size() > 1) {
////    printf("size = %i\n", digiGroups.at(0).size());
////    printf("iDigi = %i\n", digiGroups.at(0).at(0));
////  }
//  else fDebug = 0;
  // Disentangle the cluster into groups of neighbour digis
  GroupDigisDivisive(digiGroups, 0);

  // Create CbmMuchcluster objects
  clusters = GetDigiGroupsClusters(digiGroups);
}
// -------------------------------------------------------------------------

// -----   Private method GroupDigisDivisive  ------------------------------
void CbmMuchFindHitsAdvancedGem::GroupDigisDivisive(vector<vector<Int_t> > &digiGroups, Int_t iGroup) {
  vector<Int_t> digiGroupCurrent = digiGroups.at(iGroup);
  if(!ShouldDivideDivisive(digiGroupCurrent)) return;

  assert(iGroup<digiGroups.size());
  vector<Int_t> digiGroupNew; // New group
  Int_t jGroup = digiGroups.size();
  digiGroups.push_back(digiGroupNew);

//  vector<Int_t> digiGroupCurrent = digiGroups.at(iGroup);
  if(digiGroupCurrent.size() == 1) return;

  assert(jGroup < digiGroups.size());
  digiGroupNew = digiGroups.at(jGroup);

//  if(fDebug){
//    printf("Begin\n");
//    printf("current group size = %i\n",digiGroups.at(iGroup).size());
//    printf("new group size = %i\n",digiGroups.at(jGroup).size());
//    for(Int_t i=0; i<digiGroups.at(iGroup).size(); ++i){
//      printf("%i = %i\n", i, digiGroups.at(iGroup).at(i));
//    }
//  }
  // Move digis from one group to another one (if needed)
  MoveDigisDivisive(digiGroups, iGroup, jGroup);
//  if(fDebug){
//    printf("Finish\n");
//    printf("current group size = %i\n",digiGroups.at(iGroup).size());
//    printf("new group size = %i\n",digiGroups.at(jGroup).size());
//  }

  //if(digiGroups.at(iGroup).size() == 0 || digiGroups.at(jGroup).size() == 0) return;

  // Recursion
  GroupDigisDivisive(digiGroups, jGroup);
  GroupDigisDivisive(digiGroups, iGroup);
}
// -------------------------------------------------------------------------

// -----   Private method ShouldDivideDivisive  ----------------------------
Bool_t CbmMuchFindHitsAdvancedGem::ShouldDivideDivisive(vector<Int_t> digiGroup){
  if(digiGroup.size() == 0) return false; // Don't consider empty group

  const Double_t defaultDistance = 100;
  Double_t distance = defaultDistance; // Distance threshold (100 by default)
  Int_t iStation = -1; // Index of the station
  Int_t nPads = digiGroup.size(); // Cluster size
  Int_t meanPadCharge = 0; // Mean pad-charge in the cluster

  // Calculate mean-pad charge
  for(Int_t i=0; i<digiGroup.size(); ++i){
    Int_t iDigi = digiGroup.at(i);
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    if (iStation<0) iStation = fGeoScheme->GetStationIndex(digi->GetDetectorId());
    meanPadCharge += digi->GetADCCharge();
  }
  meanPadCharge /= nPads;

  // Get user-distance corresponding to this cluster pad-charge and size
  if(fDistanceLimits.find(iStation) != fDistanceLimits.end()){
    assert(fChargeLimits.find(iStation) != fChargeLimits.end());
    map<Int_t, vector<Double_t> > clusterDistances = fDistanceLimits[iStation];
    map<Int_t, vector<Int_t> > clusterCharges = fChargeLimits[iStation];
    if(clusterDistances.find(nPads) != clusterDistances.end()){
      assert(clusterCharges.find(nPads) != clusterCharges.end());
      vector<Double_t> distances = clusterDistances[nPads];
      vector<Int_t> charges = clusterCharges[nPads];
      assert(charges.size() == distances.size());
      Int_t i;
      for(i=0; i<charges.size(); i++)
        if(meanPadCharge < charges.at(i)) break;

      distance = i < distances.size() ? distances.at(i) : defaultDistance;
    }
  }

//  if(GetMeanDistance(digiGroup) > fDistanceLimit) return true;
  if(GetMeanDistance(digiGroup) > distance) return true;
  return false;
}
// -------------------------------------------------------------------------

// -----   Private method MoveDigisDivisive  -------------------------------
void CbmMuchFindHitsAdvancedGem::MoveDigisDivisive(vector<vector<Int_t> > &digiGroups, Int_t iGroup, Int_t jGroup){
  vector<Int_t> digiGroupCurrent = digiGroups.at(iGroup);
  vector<Int_t> digiGroupNew = digiGroups.at(jGroup);

  Double_t maxDeltaDist = std::numeric_limits<Double_t>::min();
  Int_t index = -1; // Position in the first group of the digi which has maximal
                    // difference between mean distances to digis in the first
                    // group and in the current one

  for(Int_t i=0; i<digiGroupCurrent.size(); ++i){
    assert(i < digiGroupCurrent.size());
    Int_t iDigi = digiGroupCurrent.at(i);
    // Get mean dist. between current digi and digis in the first group
    Double_t distance1 = GetMeanDistance(digiGroupCurrent, iDigi);
    // Get mean dist. between current digi and digis in the new group
    Double_t distance2 = GetMeanDistance(digiGroupNew, iDigi);
    Double_t deltaDist = digiGroups.at(jGroup).size()==0 ? distance1 : distance1 - distance2;
    // Search maximum distance difference
    if(deltaDist > maxDeltaDist){
      maxDeltaDist = deltaDist;
      index = i;
    }
//    if(fDebug){
//      printf("iDigi = %i, distance1 = %f, distance2 = %f, deltaDist = %f\n", iDigi, distance1, distance2, deltaDist);
//    }
  }
//  if(fDebug){
//    printf("maxDeltaDist = %f\n", maxDeltaDist);
//  }
  if(index < 0) return;

  // Put digi with the max distance into another group
  if(maxDeltaDist > 0) {
    assert(iGroup < digiGroups.size());
    assert(index < digiGroups.at(iGroup).size());
    Int_t digiIndex = digiGroups.at(iGroup).at(index);
    digiGroups.at(jGroup).push_back(digiIndex);  // Put dig in the new group
//    if(fDebug) {
//      printf("Moving %i to the new group\n", digiIndex);
//    }
    assert(index < digiGroups.at(iGroup).size());
    vector<Int_t>::iterator it = digiGroups.at(iGroup).begin();
    digiGroups.at(iGroup).erase(it+index);    // Delete digi from the current group
//    if(fDebug){
//      printf("Size of the current group: %i\n", digiGroups.at(iGroup).size());
//      printf("Size of the new group: %i\n", digiGroups.at(jGroup).size());
//    }

    MoveDigisDivisive(digiGroups, iGroup, jGroup);
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchFindHitsAdvancedGem::GetMeanDistance(CbmMuchCluster* cluster){
  Double_t distance = 0;
  for(Int_t i=0; i<cluster->GetNDigis(); ++i){
    Int_t iDigi = cluster->GetDigiIndex(i);
    for(Int_t j=0; j<cluster->GetNDigis(); ++j){
      Int_t jDigi = cluster->GetDigiIndex(j);
      distance += GetDistance(iDigi, jDigi);
    }
  }
  return distance/cluster->GetNDigis();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchFindHitsAdvancedGem::GetMeanDistance(vector<Int_t> digiGroup){
  Double_t distance = 0;
  for(Int_t i=0; i<digiGroup.size(); ++i){
    Int_t iDigi = digiGroup.at(i);
    for(Int_t j=0; j<digiGroup.size(); ++j){
      Int_t jDigi = digiGroup.at(j);
      distance += GetDistance(iDigi, jDigi);
    }
  }
  return distance/digiGroup.size();
}
// -------------------------------------------------------------------------

// -----   Private method GetMeanDistance  ---------------------------------
Double_t CbmMuchFindHitsAdvancedGem::GetMeanDistance(vector<Int_t> digiGroup, Int_t iDigi){
  // Calculates mean distance between the current digi and others for the given group
  Double_t distance = 0;
  for(vector<Int_t>::iterator it = digiGroup.begin(); it!=digiGroup.end(); ++it){
    Int_t jDigi = (*it);

    distance += GetDistance(iDigi, jDigi);
  }

  return digiGroup.size()!=0 ? distance/digiGroup.size() : 0;
}
// -------------------------------------------------------------------------

// -----   Private method GetDistance  -------------------------------------
Double_t CbmMuchFindHitsAdvancedGem::GetDistance(Int_t iDigi, Int_t jDigi){
  CbmMuchDigi* digi1 = (CbmMuchDigi*) fDigis->At(iDigi);
  CbmMuchDigi* digi2 = (CbmMuchDigi*) fDigis->At(jDigi);
  Int_t deltaCharge = digi1->GetADCCharge() - digi2->GetADCCharge();
  Int_t q1=0;
  Int_t q2=0;
  CbmMuchPad* pad1 = GetPadByDigi(iDigi, q1);
  CbmMuchPad* pad2 = GetPadByDigi(jDigi, q2);
  // Normalized space distances (because of different pad sizes)
  Double_t deltaX = 2*(pad1->GetX0() - pad2->GetX0())/(
  pad1->GetLx() + pad2->GetLx());
  Double_t deltaY = 2*(pad1->GetY0() - pad2->GetY0())/(pad1->GetLy() + pad2->GetLy());
  return TMath::Sqrt(deltaX*deltaX + deltaY*deltaY + deltaCharge*deltaCharge);
}
// -------------------------------------------------------------------------

// -----   Private method GetDigiGroupsClusters  ---------------------------
vector<CbmMuchCluster*> CbmMuchFindHitsAdvancedGem::GetDigiGroupsClusters(vector<vector<Int_t> > digiGroups){
  vector<CbmMuchCluster*> clusters;
  for(Int_t iGroup=0; iGroup < digiGroups.size(); ++iGroup){
    // Create cluster object from each group of digis
    vector<Int_t> digiGroup = digiGroups.at(iGroup);
    Int_t sumCharge = 0;
    Int_t qMax = std::numeric_limits<Int_t>::min();
    for(vector<Int_t>::iterator it = digiGroup.begin(); it!=digiGroup.end(); ++it){
      Int_t iDigi = (*it);
      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*)fDigiMatches->At(iDigi);
      sumCharge += match->GetTotalCharge();
      qMax = std::max((Int_t)match->GetTotalCharge(), qMax);
    }
    clusters.push_back(new CbmMuchCluster(digiGroup, sumCharge, qMax));
  }
  return clusters;
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchFindHitsAdvancedGem)
