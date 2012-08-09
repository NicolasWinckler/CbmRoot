/*
 * CbmMuchFindHitsGem.cxx
 */
#include "CbmMuchFindHitsGem.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModule.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPad.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigi.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TClonesArray.h"

// -------------------------------------------------------------------------
CbmMuchFindHitsGem::CbmMuchFindHitsGem(const char* digiFileName) 
  : FairTask("MuchFindHitsAdvancedGem", 1) ,
    fAlgorithm(3),
    fDigiFile(""),
    fClusterSeparationTime(100.),
    fThresholdRatio(0.1),
    fEvent(0),
    fDigis(NULL),
    fClusters(new TClonesArray("CbmMuchCluster", 1000)),
    fHits(new TClonesArray("CbmMuchPixelHit", 1000)),
    fGeoScheme(CbmMuchGeoScheme::Instance()),
    fDigiIndices(),
    fFiredPads(),
    fDigiIndexMap(),
    fClusterMap(),
    fLocalMax(),
    fClusterNx(0),
    fClusterNy(0)
{
}
// -------------------------------------------------------------------------


// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindHitsGem::Init() {
  FairRootManager* ioman = FairRootManager::Instance();
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
  ioman->Register("MuchCluster", "Cluster in MUCH", fClusters, kTRUE);
  ioman->Register("MuchPixelHit", "Hit in MUCH", fHits, kTRUE);
  
  // Initialize GeoScheme
  TFile* oldfile = gFile;
  TFile* file = new TFile(fDigiFile);
  TObjArray* stations = (TObjArray*) file->Get("stations");
  file->Close();
  file->Delete();
  gFile = oldfile;
  fGeoScheme->Init(stations);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmMuchFindHitsGem::Exec(Option_t* opt) {
  TStopwatch timer;
  timer.Start();
  fEvent++;

  // Clear output array
  if (fHits) fHits->Clear();
  if (fClusters) fClusters->Delete();//Clear(); // Delete because of memory escape

  // Find clusters
  FindClusters();

  vector<CbmMuchCluster*> clusters;
  for (Int_t iCluster = 0; iCluster < fClusters->GetEntriesFast(); ++iCluster) {
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(iCluster);
    switch (fAlgorithm) {
    // Hit
    case 0: 
      // One hit per pad
    case 1: {
      // One hit per cluster
      CreateHits(cluster, iCluster);
      break;
    }
    case 2: {
      // Simple cluster deconvolution
      clusters.clear();
      ExecClusteringSimple(cluster, clusters);
      CreateHits(clusters, iCluster);
      break;
    }
    case 3: {
      // Peak finder
//      clusters.clear();
      ExecClusteringPeaks(cluster,iCluster);
//      CreateHits(clusters, iCluster);
//      CreateHits(cluster, iCluster);
      break;
    }
    default: {
      Fatal("CbmMuchFindHitsGem::Exec:","The algorithm index does not exist.");
      break;
    }
    }
  }
  clusters.clear();
  fDigiIndices.clear();
  fFiredPads.clear();

  timer.Stop();
  printf("-I- %s:",fName.Data());
  printf(" digis: %i",fDigis->GetEntriesFast());
  printf(" clusters: %i",fClusters->GetEntriesFast());
  printf(" hits: %i",fHits->GetEntriesFast());
  printf(" time: %f s\n",timer.RealTime());
}
// -------------------------------------------------------------------------


// -----   Private method FindClusters  ------------------------------------
void CbmMuchFindHitsGem::FindClusters() {
  Int_t nClusters = 0;
  
  if (fAlgorithm==0){
    for (Int_t iDigi = 0; iDigi < fDigis->GetEntriesFast(); iDigi++) {
      fDigiIndices.clear();
      fDigiIndices.push_back(iDigi);
      new ((*fClusters)[nClusters++]) CbmMuchCluster(fDigiIndices);
    }
    return;
  }
  
  vector<CbmMuchModuleGem*> modules = fGeoScheme->GetGemModules();

  // Clear array of digis in the modules
  for (UInt_t m=0;m<modules.size();m++) modules[m]->ClearDigis();

  // Fill array of digis in the modules. Digis are automatically sorted in time
  for (Int_t iDigi = 0; iDigi < fDigis->GetEntriesFast(); iDigi++) {
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    Double_t time = digi->GetTime();
    Int_t detId = digi->GetDetectorId();
    fGeoScheme->GetModuleByDetId(detId)->AddDigi(time,iDigi);
  }
  
  // Find clusters module-by-module
  for (UInt_t m=0;m<modules.size();m++){
    CbmMuchModuleGem* module = modules[m];
    multimap<Double_t,Int_t> digis = modules[m]->GetDigis();
    multimap<Double_t,Int_t>::iterator it,itmin,itmax;

    // Split module digis into time slices according to fClusterSeparationTime
    vector<multimap<Double_t,Int_t>::iterator> slices;
    Double_t tlast = -10000;
//    slices.push_back(digis.begin());
    for (it=digis.begin();it!=digis.end();++it){
      Double_t t  = it->first;
      if (t> tlast + fClusterSeparationTime) slices.push_back(it);
      tlast = t;
    }
    slices.push_back(it);
    for (Int_t s=1;s<slices.size();s++){
      fFiredPads.clear();
      for (it=slices[s-1];it!=slices[s];it++){
        Int_t iDigi = it->second;
        CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
        CbmMuchPad* pad = module->GetPad(digi->GetChannelId());
        pad->SetDigiIndex(iDigi);
        fFiredPads.push_back(pad);
      }
      // Loop over fired pads in a time slice
      for (Int_t p=0;p<fFiredPads.size();p++){
        fDigiIndices.clear();
        CreateCluster(fFiredPads[p]);
        if (fDigiIndices.size()==0) continue;
        new ((*fClusters)[nClusters++]) CbmMuchCluster(fDigiIndices);
      }
      //return;
    }
  }
}
// -------------------------------------------------------------------------


// -----   Private method CreateCluster  -----------------------------------
void CbmMuchFindHitsGem::CreateCluster(CbmMuchPad* pad) {
  Int_t digiIndex = pad->GetDigiIndex();
  if (digiIndex<0) return;
  fDigiIndices.push_back(digiIndex);
  pad->SetDigiIndex(-1);
  vector<CbmMuchPad*> neighbours = pad->GetNeighbours();
  for (Int_t i=0;i<neighbours.size();i++) CreateCluster(neighbours[i]);
}
// -------------------------------------------------------------------------


// -----   Private method ExecClusteringSimple  ----------------------------
void CbmMuchFindHitsGem::ExecClusteringSimple(CbmMuchCluster* cluster,vector<CbmMuchCluster*> &clusters) {
  CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(cluster->GetDigiIndex(0));
  CbmMuchModule* m = fGeoScheme->GetModuleByDetId(digi->GetDetectorId());
  CbmMuchModuleGem* module = (CbmMuchModuleGem*) m;
  Int_t iStation = fGeoScheme->GetStationIndex(digi->GetDetectorId());
  UInt_t threshold = UInt_t(fThresholdRatio*cluster->GetMaxCharge());
  
  // Fire pads which are higher than threshold in a cluster
  fFiredPads.clear();
  for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++) {
    Int_t digiIndex = cluster->GetDigiIndex(iDigi);
    digi = (CbmMuchDigi*) fDigis->At(digiIndex);
    if (digi->GetADCCharge()<=threshold) continue;
    CbmMuchPad* pad = module->GetPad(digi->GetChannelId());
    pad->SetDigiIndex(digiIndex);
    fFiredPads.push_back(pad);
  }
  for (Int_t p=0;p<fFiredPads.size();p++){
    fDigiIndices.clear();
    CreateCluster(fFiredPads[p]);
    if (fDigiIndices.size()==0) continue;
    clusters.push_back(new CbmMuchCluster(fDigiIndices));
  }
}
// -------------------------------------------------------------------------


// -----   Private method CreateHits  --------------------------------------
void CbmMuchFindHitsGem::CreateHits(vector<CbmMuchCluster*> clusters, Int_t iCluster) {
  // Produce hits
  for (Int_t i=0;i<clusters.size();i++){
    CbmMuchCluster* cl = clusters[i];
    CreateHits(cl,iCluster);
    if (cl) delete cl;
  }
}
// -------------------------------------------------------------------------


// -----   Private method CreateHits  --------------------------------------
void CbmMuchFindHitsGem::CreateHits(CbmMuchCluster* cluster, Int_t iCluster) {
  Int_t nDigis = cluster->GetNDigis();
  Double_t sumq=0, sumx=0, sumy=0, sumt=0, sumdx2=0, sumdy2=0, sumdxy2=0, sumdt2=0;
  Double_t q=0,x=0,y=0,t=0,z=0,dx=0,dy=0,dxy=0,dt=0;
  Int_t detId = 0;
  Int_t planeId = 0;
  CbmMuchModuleGem* module = NULL;
  
  for (Int_t i=0;i<nDigis;i++) {
    Int_t iDigi = cluster->GetDigiIndex(i);
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    if (i==0) {
      detId   = digi->GetDetectorId();
      planeId = fGeoScheme->GetLayerSideNr(detId);
      module  = (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(detId);
      z       = module->GetPosition()[2];
    }
    CbmMuchPad* pad = module->GetPad(digi->GetChannelId());
    x   = pad->GetX();
    y   = pad->GetY();
    t   = digi->GetTime();
    q   = digi->GetADCCharge();
    dx  = pad->GetDx();
    dy  = pad->GetDy();
    dxy = pad->GetDxy();
    dt = digi->GetDTime();
    sumq    += q;
    sumx    += q*x;
    sumy    += q*y;
    sumt    += q*t;
    sumdx2  += q*q*dx*dx;
    sumdy2  += q*q*dy*dy;
    sumdxy2 += q*q*dxy*dxy;
    sumdt2  += q*q*dt*dt;
  }
  
  x   = sumx/sumq;
  y   = sumy/sumq;
  t   = sumt/sumq;
  dx  = sqrt(sumdx2/12)/sumq;
  dy  = sqrt(sumdy2/12)/sumq;
  dxy = sqrt(sumdxy2/12)/sumq;
  dt = sqrt(sumdt2)/sumq;
  Int_t iHit = fHits->GetEntriesFast();
  new ((*fHits)[iHit]) CbmMuchPixelHit(detId,x,y,z,dx,dy,0,dxy,iCluster,planeId,t,dt);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchFindHitsGem::ExecClusteringPeaks(CbmMuchCluster* cluster,Int_t iCluster) {
  if (!CreateClusterMap(cluster)) { CreateHits(cluster,iCluster);  return; }
  if (FindLocalMaxima()<2) { CreateHits(cluster,iCluster); return; }
  // Create clusters from single digi corresponding to each local maximum
  for (Int_t i=0; i<fClusterNx; i++) { 
    for (Int_t j=0; j<fClusterNy; j++) {
      if (fLocalMax[i][j]==1) {
        fDigiIndices.clear();
        fDigiIndices.push_back(fDigiIndexMap[i][j]);
        CbmMuchCluster* newcluster = new CbmMuchCluster(fDigiIndices);
        CreateHits(newcluster,iCluster); 
      }
    }
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Bool_t CbmMuchFindHitsGem::CreateClusterMap(CbmMuchCluster* cluster){
  // Skip small clusters
  if (cluster->GetNDigis()<=2) return kFALSE; 
  
  // Determine cluster dimensions
  Int_t xmin = std::numeric_limits<Int_t>::max();
  Int_t ymin = std::numeric_limits<Int_t>::max();
  Int_t xmax = 0;
  Int_t ymax = std::numeric_limits<Int_t>::min();

  for (Int_t i=0;i<cluster->GetNDigis();i++){
    Int_t iDigi = cluster->GetDigiIndex(i);
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    Long64_t channelId = digi->GetChannelId();
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(digi->GetDetectorId());
    Int_t n = module->GetNSectors();
    Int_t x = CbmMuchModuleGem::GetSectorIndex(channelId);
    Int_t y = CbmMuchModuleGem::GetChannelIndex(channelId)-n/2;
    if (x<xmin) xmin = x;
    if (y<ymin) ymin = y;
    if (x>xmax) xmax = x;
    if (y>ymax) ymax = y;
  }
  
  fClusterNx = xmax-xmin+1;
  fClusterNy = ymax-ymin+1;
  // Skip small clusters
  if (fClusterNx<3 && fClusterNy<3) return kFALSE;
  
  // Skip too large clusters
  if (fClusterNx>MAXCLUSTERSIZE) return kFALSE;
  if (fClusterNy>MAXCLUSTERSIZE) return kFALSE;
  
  // Clear cluster map and local maxima
  for (Int_t i=0;i<fClusterNx;i++) {
    for (Int_t j=0;j<fClusterNy;j++) {
      fDigiIndexMap[i][j]=0;
      fClusterMap[i][j]=0;
      fLocalMax[i][j]=0;
    }
  }

  // Fill cluster map
  for (Int_t i=0;i<cluster->GetNDigis();i++){
    Int_t iDigi = cluster->GetDigiIndex(i);
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    Long64_t channelId = digi->GetChannelId();
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(digi->GetDetectorId());
    Int_t n = module->GetNSectors();
    Int_t x = CbmMuchModuleGem::GetSectorIndex(channelId);
    Int_t y = CbmMuchModuleGem::GetChannelIndex(channelId)-n/2;
    Int_t c = digi->GetADCCharge();
    fDigiIndexMap[x-xmin][y-ymin]=iDigi;
    fClusterMap[x-xmin][y-ymin]=c;
  }
  
  return kTRUE;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmMuchFindHitsGem::FindLocalMaxima(){
  // Flag local maxima
  for (Int_t i=0; i<fClusterNx;i++) {
    for (Int_t j=0; j<fClusterNy;j++) {
      if (fLocalMax[i][j]!=0) continue;
      FlagLocalMax(i,j);
    }
  }
  // Count local maxima
  Int_t nMax = 0;
  for (Int_t i=0; i<fClusterNx; i++) 
    for (Int_t j=0; j<fClusterNy; j++) 
      if (fLocalMax[i][j]==1) nMax++;
  return nMax;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchFindHitsGem::FlagLocalMax(Int_t i, Int_t j){
  Int_t c = fClusterMap[i][j];
  for (Int_t i1=i-1;i1<=i+1;i1++) {
    if (i1<0 || i1>=fClusterNx) continue;
    for (Int_t j1=j-1;j1<=j+1;j1++) {
      if (j1<0 || j1>=fClusterNy) continue;
      if (i==i1 && j==j1) continue;
      Int_t c1 = fClusterMap[i1][j1];
      if      (c < c1) { fLocalMax[i][j] = -1; return; }
      else if (c > c1)   fLocalMax[i1][j1] = -1;
      else { // the same charge
        fLocalMax[i][j] = 1; 
        if (fLocalMax[i1][j1] == 0) {
          FlagLocalMax(i1,j1);
          if (fLocalMax[i1][j1] < 0) { fLocalMax[i][j] = -1; return; }
          else fLocalMax[i1][j1] = -1;
        }
      } 
    }
  }
  fLocalMax[i][j] = 1;
}
// -------------------------------------------------------------------------



ClassImp(CbmMuchFindHitsGem)
