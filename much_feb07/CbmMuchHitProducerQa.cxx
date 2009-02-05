// -------------------------------------------------------------------------
// -----                  CbmMuchHitProducerQa source file             -----
// -----                   Created 16/11/07 by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmMuchHitProducerQa.h"
#include "CbmRootManager.h"
#include "CbmMuchPoint.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"
#include "CbmMuchHit.h"

#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmGeoMuchPar.h"
#include "CbmMuchDigiPar.h"
#include "CbmMuchDigiScheme.h"
#include "CbmMuchStation.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"

#include "TCanvas.h"
#include "TH1D.h"
#include "TMath.h"

ClassImp(CbmMuchHitProducerQa)


// -------------------------------------------------------------------------
CbmMuchHitProducerQa::CbmMuchHitProducerQa(const char *name, Int_t verbose)
 :CbmTask(name,verbose){
  fVerbose = verbose;
  fDigiScheme  = CbmMuchDigiScheme::Instance();
  fEvent=0;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchHitProducerQa::~CbmMuchHitProducerQa(){
     if ( fDigiScheme ) fDigiScheme->FreeInstance();
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
InitStatus CbmMuchHitProducerQa::Init()
{
  CbmRootManager* fManager = CbmRootManager::Instance();
  fPoints      = (TClonesArray*) fManager->GetObject("MuchPoint");
  fHits        = (TClonesArray*) fManager->GetObject("MuchHit");
  fDigis       = (TClonesArray*) fManager->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) fManager->GetObject("MuchDigiMatch");
  fClusters    = (TClonesArray*) fManager->GetObject("MuchCluster");
  fDigiScheme->Init(fGeoPar, fDigiPar);
  fNstations = fDigiScheme->GetNStations();
  fhOccupancyR = new TH1D*[fNstations];
  fhPadsTotalR = new TH1D*[fNstations];
  fhPadsFiredR = new TH1D*[fNstations];
  for (Int_t i=0; i<fNstations; i++){
    CbmMuchStation* station = fDigiScheme->GetStation(i);
    Double_t rMax = station->GetRmax();
    fhPadsTotalR[i] = new TH1D(Form("hPadsTotal%i",i+1),Form("Number of  pads vs radius: station %i",i+1),100,0,1.2*rMax);
    fhPadsFiredR[i] = new TH1D(Form("hPadsFired%i",i+1),Form("Number of fired pads vs radius: station %i",i+1),100,0,1.2*rMax);
    fhOccupancyR[i] = new TH1D(Form("hOccupancy%i",i+1),Form("Occupancy vs radius: station %i",i+1),100,0,1.2*rMax);
    Int_t nSectors = station->GetNSectors();
    for (Int_t j=0;j<nSectors;j++){
      CbmMuchSector* sector = station->GetSector(j);
      for (Int_t iPad=0;iPad<sector->GetNChannels();iPad++){
        CbmMuchPad* pad = sector->GetPad(iPad);
        Double_t x0 = pad->GetX0();
        Double_t y0 = pad->GetY0();
	Double_t r0 = TMath::Sqrt(x0*x0+y0*y0);
        fhPadsTotalR[i]->Fill(r0);
      }
    }
  }

  fhPoolX = new TH1D("hPoolX","Pool distribution;(x_{RC} - x_{MC}) / dx_{RC};Entries",100,-5,5);
  fhPoolY = new TH1D("hPoolY","Pool distribution;(y_{RC} - y_{MC}) / dy_{RC};Entries",100,-5,5);

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitProducerQa::SetParContainers() {
  // Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get MUCH geometry parameter container
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");

  // Get MUCH digitisation parameter container
  fDigiPar = (CbmMuchDigiPar*) db->getContainer("CbmMuchDigiPar");
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitProducerQa::Exec(Option_t * option){
  Bool_t verbose = (fVerbose>1);
  fEvent++;
  printf("CbmMuchHitProducerQa::Exec()");
  printf("  Event:%i\n",fEvent);

  // Filling occupancy plots
  for (Int_t i=0;i<fDigis->GetEntriesFast();i++){
    CbmMuchPad* pad = fDigiScheme->GetPad((CbmMuchDigi*) fDigis->At(i));
    Double_t x0 = pad->GetX0();
    Double_t y0 = pad->GetY0();
    Double_t r0 = TMath::Sqrt(x0*x0+y0*y0);
    fhPadsFiredR[pad->GetStationNr()-1]->Fill(r0);
  }

  // Filling residuals and pools for hits at the second station
  for (Int_t i=0;i<fHits->GetEntriesFast();i++){
    CbmMuchHit* hit = (CbmMuchHit*) fHits->At(i);
    // Select hits from the second station only
    Int_t stationNr = hit->GetStationNr();
    if (stationNr<2) continue;
    if (stationNr>2) break;
    if (verbose) printf("   Hit %i, station %i ",i,stationNr);
    // Select hits which are unique in the corresponding cluster
    Bool_t hit_unique=1;
    Int_t clusterId = hit->GetCluster();
    for (Int_t j=i+1;j<fHits->GetEntriesFast();j++){
      CbmMuchHit* hit1 = (CbmMuchHit*) fHits->At(j);
      if (hit1->GetStationNr()>stationNr) break;
      if (hit1->GetCluster()==clusterId) { hit_unique=0; break;}
    }
    if (verbose) printf("hit_unique=%i",hit_unique);
    if (!hit_unique) {if (verbose) printf("\n"); continue;}

    // Select hits with clusters formed by only one MC Point
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(clusterId);
    Bool_t point_unique=1;
    Int_t  pointId=-1;
    for(Int_t digiId=0;digiId<cluster->GetNDigis();digiId++){
      Int_t index = cluster->GetDigiIndex(digiId);
      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(index);
      // Not unique if the pad has several mcPoint references
      if (verbose) printf(" n=%i",match->GetNPoints());
      if (match->GetNPoints()>1) { point_unique=0; break; }
      Int_t currentPointId = match->GetRefIndex(0);
      if (digiId==0) { pointId=currentPointId; continue; }
      // Not unique if mcPoint references differ for diferent digis
      if (currentPointId!=pointId) {point_unique=0; break; }
    }
    if (verbose) printf(" point_unique=%i",point_unique);
    if (!point_unique) {if (verbose) printf("\n"); continue;}

    if (verbose) printf(" pointId=%i",pointId);
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(pointId);

    Double_t xMC  = 0.5*(point->GetXIn()+point->GetXOut());
    Double_t yMC  = 0.5*(point->GetYIn()+point->GetYOut());

    Double_t xRC  = hit->GetX();
    Double_t yRC  = hit->GetY();
    Double_t dx   = hit->GetDx();
    Double_t dy   = hit->GetDy();

    if (dx<1.e-10) { printf("Anomalously small dx\n"); continue;}
    if (dy<1.e-10) { printf("Anomalously small dy\n"); continue;}
    fhPoolX->Fill((xRC-xMC)/dx);
    fhPoolY->Fill((yRC-yMC)/dy);
    if (verbose) printf("\n");
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitProducerQa::Finish(){
  for (Int_t i=0;i<fNstations;i++) {
    fhPadsTotalR[i]->Sumw2();
    fhPadsFiredR[i]->Sumw2();
    fhPadsFiredR[i]->Scale(1./fEvent);
    fhOccupancyR[i]->Divide(fhPadsFiredR[i],fhPadsTotalR[i]);
  }
  fhPoolX->Sumw2();
  fhPoolY->Sumw2();
  fhPoolX->Fit("gaus","q");
  fhPoolY->Fit("gaus","q");


  if (fVerbose>1) {
    TCanvas* c1 = new TCanvas("c1","All pad distributions",1500,900);
    c1->Divide(5,3);
    for (Int_t i=0;i<15;i++) {
      c1->cd(5*(i%3)+i/3+1);
      fhPadsTotalR[i]->Draw();
    }
    c1->cd();

    TCanvas* c2 = new TCanvas("c2","Fired pad distributions",1500,900);
    c2->Divide(5,3);
    for (Int_t i=0;i<15;i++) {
      c2->cd(5*(i%3)+i/3+1);
      fhPadsFiredR[i]->Draw();
    }
    c2->cd();

    TCanvas* c3 = new TCanvas("c3","Occupancy plots",1500,900);
    c3->Divide(5,3);
    for (Int_t i=0;i<15;i++) {
      c3->cd(5*(i%3)+i/3+1);
      fhOccupancyR[i]->Draw();
    }
    c3->cd();

    TCanvas* c4 = new TCanvas("c4","Pools",800,400);
    c4->Divide(2,1);
    c4->cd(1);
    fhPoolX->Draw();
    c4->cd(2);
    fhPoolY->Draw();
    c4->cd();
  }
}
// -------------------------------------------------------------------------
