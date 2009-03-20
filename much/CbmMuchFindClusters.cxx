/** CbmMuchFindClusters.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 31.01.08
 *@version 1.0
 **
 ** CBM task class for finding clusters in the MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchCluster.
 **/

#include "CbmMuchFindClusters.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchModule.h"
#include "CbmMuchPad.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigi.h"
#include "CbmMuchHit.h"
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
CbmMuchFindClusters::CbmMuchFindClusters() :
  CbmMuchTask("MuchFindClusters", 1) {
  fDigis = NULL;
  fDigiMatches = NULL;
  fHits = NULL;
  fDigiFile = NULL;
  fPrimaryClusters = NULL;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchFindClusters::CbmMuchFindClusters(Int_t iVerbose) :
  CbmMuchTask("MuchFindClusters", iVerbose) {
  fDigis = NULL;
  fDigiMatches = NULL;
  fDigiFile = NULL;
  fPrimaryClusters = NULL;
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchFindClusters::CbmMuchFindClusters(const char* name, TFile* digiFile,
    Int_t iVerbose) :
  CbmMuchTask(name, iVerbose) {
  fDigis = NULL;
  fDigiMatches = NULL;
  fDigiFile = digiFile;
  fPrimaryClusters = NULL;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchFindClusters::~CbmMuchFindClusters() {
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmMuchFindClusters::Exec(Option_t* opt) {
  fTimer.Start();

  // Clear output array
  if (fHits)
    fHits->Clear();
  if (fPrimaryClusters)
    fPrimaryClusters->Delete();//Clear(); // Delete because of memory escape

  Int_t nHits = 0;

  // Find primary clusters
  FindClusters();

  Int_t nSecClusters = 0;
  // Find secondary clusters
  Int_t nClusters = fPrimaryClusters->GetEntriesFast();
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {
    CbmMuchCluster* cluster = (CbmMuchCluster*) fPrimaryClusters->At(
        iCluster);
    vector<CbmMuchCluster*> secondaryClusters;
    if (cluster)
      ApplyThreshold(cluster, secondaryClusters);
    nSecClusters += secondaryClusters.size();

    // Produce hits from secondary clusters
    for (vector<CbmMuchCluster*>::iterator it = secondaryClusters.begin(); it
        != secondaryClusters.end(); it++) {
      CbmMuchCluster* secCluster = (*it);
      Double_t x, y, z;
      Double_t xq = 0.0;
      Double_t yq = 0.0;
      Double_t dx = 1e5;
      Double_t dy = 1e5;
      Int_t nClusterDigis = secCluster->GetNDigis();
      CbmMuchPad* pad = NULL;
      for (Int_t iDigi = 0; iDigi < nClusterDigis; iDigi++) {
        Int_t digiIndex = secCluster->GetDigiIndex(iDigi);
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

      CbmMuchModule* module = fGeoScheme->GetModuleByDetId(
          pad->GetDetectorId());
      x = xq / secCluster->GetCharge();
      y = yq / secCluster->GetCharge();
      z = module->GetPosition()[2];

      TVector3 pos, dpos;
      pos.SetXYZ(x, y, z);
      dpos.SetXYZ(sigmaX, sigmaY, 0.);
      Int_t detId = module->GetDetectorId();
      Int_t planeId = fGeoScheme->GetLayerSideNr(detId);

      new ((*fHits)[nHits++]) CbmMuchHit(detId, pos,
         dpos, 0, iCluster,planeId);

      if (planeId>30) printf("planeId=%i\n",planeId);

      if (secCluster)
        delete secCluster;
    }
    secondaryClusters.clear();
  }

  fTimer.Stop();
  if (fVerbose) {
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Active channels           : " << fDigis->GetEntriesFast()
        << endl;
    cout << "    Hits created              : " << fHits->GetEntriesFast()
        << endl;
    cout << "    Real time                 : " << fTimer.RealTime() << endl;
    cout << "    Clusters                  : " << nClusters << endl;
    //    for(Int_t i=0; i< fHits->GetEntriesFast(); ++i){
    //      CbmMuchHit* h = (CbmMuchHit*) fHits->At(i);
    //      printf("StationNr = %i\n", h->GetStationNr());
    //    }
  } else {
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
        << fixed << right << fTimer.RealTime() << " s, digis "
        << fDigis->GetEntriesFast() << ", hits: "
        << fHits->GetEntriesFast() << endl;
  }
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void CbmMuchFindClusters::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindClusters::Init() {
  // Get input arrays
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");

  // Initialize GeoScheme
  TObjArray* stations = (TObjArray*) fDigiFile->Get("stations");
  fGeoScheme->Init(stations);
  fGeoScheme->InitGrid();

  // Register output arrays
  fPrimaryClusters = new TClonesArray("CbmMuchCluster", 1000);
  ioman->Register("MuchCluster", "Cluster in MUCH", fPrimaryClusters, kTRUE);
  fHits = new TClonesArray("CbmMuchHit", 1000);
  ioman->Register("MuchHit", "Hit in MUCH", fHits, kTRUE);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchFindClusters::ReInit() {
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method FindChannelDigiMap  ------------------------------
void CbmMuchFindClusters::FillChannelDigiMap() {
  fChannelDigiMap.clear();
  Int_t nDigis = fDigis->GetEntriesFast();
  for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    if (!digi)
      continue;

    // Skip if digi was generated by straw digitizer
    CbmMuchStation* station = fGeoScheme->GetStationByDetId(digi->GetDetectorId());
    if(station->GetDetectorType()!=1) continue;

    // Unique channel id within the MUCH
    pair<Int_t, Int_t>
        uniqueId(digi->GetDetectorId(), digi->GetChannelId());
    if (fChannelDigiMap.find(uniqueId) == fChannelDigiMap.end())
      fChannelDigiMap[uniqueId] = iDigi;
  }
}
// -------------------------------------------------------------------------

// -----   Private method FindClusters  ------------------------------------
void CbmMuchFindClusters::FindClusters() {
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
    new ((*fPrimaryClusters)[nClusters]) CbmMuchCluster(digiIndices,
        sumCharge, qMax);
    // Match digis to the parent cluster
    SetDigiClusterMatch(digiIndices, nClusters);
    nClusters++;
  }
}
// -------------------------------------------------------------------------

// -----   Private method SetDigiClusterMatch  -----------------------------
void CbmMuchFindClusters::SetDigiClusterMatch(vector<Int_t> digiIndices,
    Int_t clusterIndex) {
  for (vector<Int_t>::iterator it = digiIndices.begin(); it
      != digiIndices.end(); it++) {
    Int_t idx = *it;
    CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fDigiMatches->At(idx);
    if (!digiMatch)
      continue;
    digiMatch->SetClusterIndex(clusterIndex);
  }
}
// -------------------------------------------------------------------------

// -----   Private method CreateCluster  -----------------------------------
void CbmMuchFindClusters::CreateCluster(Int_t iDigi,
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
    if (fChannelDigiMap.find(uniqueId) == fChannelDigiMap.end())
      continue;

    Int_t digiIndex = fChannelDigiMap[uniqueId];
    CbmMuchDigi* neighbourDigi = (CbmMuchDigi*) fDigis->At(digiIndex);
    if (!neighbourDigi)
      continue;
    Int_t digiCharge = neighbourDigi->GetCharge();
    if (digiCharge <= qThreshold)
      continue;

    // Selection
    if (fSelectedDigis.find(digiIndex) != fSelectedDigis.end())
      continue;

    CreateCluster(digiIndex, digiIndices, sumCharge, qMax, qThreshold);
  }
}
// -------------------------------------------------------------------------

// -----   Private method ApplyThreshold  ----------------------------------
void CbmMuchFindClusters::ApplyThreshold(CbmMuchCluster* cluster, vector<
    CbmMuchCluster*> &newClusters) {
  fSelectedDigis.clear();
  Int_t qThreshold = cluster->GetMaxCharge() / 10;
  Int_t dummy = 0;
  for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++) {
    Int_t digiIndex = cluster->GetDigiIndex(iDigi);
    Int_t charge = 0;
    CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);
    if (!pad)
      continue;
    if (charge <= qThreshold)
      continue;

    // Selection
    if (fSelectedDigis.find(digiIndex) != fSelectedDigis.end())
      continue;

    vector<Int_t> digiIndices;
    Int_t sumCharge = 0;
    CreateCluster(digiIndex, digiIndices, sumCharge, dummy, qThreshold);
    newClusters.push_back(new CbmMuchCluster(digiIndices, sumCharge));
  }
}
// -------------------------------------------------------------------------

// -----   Private method GetPadByDigi  ------------------------------------
CbmMuchPad* CbmMuchFindClusters::GetPadByDigi(Int_t digiIndex, Int_t &charge) {
  CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(digiIndex);
  if (digi) {
    charge = digi->GetCharge();
    Int_t detectorId = digi->GetDetectorId();
    Int_t channelId = digi->GetChannelId();
    CbmMuchPad* pad = fGeoScheme->GetPadByDetId(detectorId, channelId);
    if (pad){
      assert(pad->GetDetectorId() == digi->GetDetectorId());
      assert(pad->GetChannelId() == digi->GetChannelId());
    }
    return pad;
  }
  return NULL;
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchFindClusters)
