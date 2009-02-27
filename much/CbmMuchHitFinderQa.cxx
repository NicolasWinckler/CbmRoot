// -------------------------------------------------------------------------
// -----                  CbmMuchHitProducerQa source file             -----
// -----                   Created 16/11/07 by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmMuchHitFinderQa.h"
#include "FairRootManager.h"
#include "CbmMuchPoint.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"
#include "CbmMuchHit.h"

#include "CbmMuchStation.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"

#include "CbmMCTrack.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"


#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "CbmMuchPointInfo.h"
#include "CbmMuchDigitize.h"
#include "TStyle.h"
#include "TArrayI.h"
#include "CbmMuchGeoScheme.h"
#include "TObjArray.h"

#include "FairRuntimeDb.h"
#include "CbmGeoMuchPar.h"
#include "TGraph.h"

#include <map>
#include <vector>
#include <algorithm>

using std::map;
using std::vector;

// -------------------------------------------------------------------------
CbmMuchHitFinderQa::CbmMuchHitFinderQa(const char* name, Int_t verbose)
 :FairTask(name,verbose){
  fGeoScheme   = CbmMuchGeoScheme::Instance();
  fGeoFileName = "much.digi.root";
  fFileName = "performance.root";
  fVerbose = verbose;
  fEvent = 0;
  fSignalPoints = fSignalHits = 0;
  fPointInfos = new TClonesArray("CbmMuchPointInfo",10);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchHitFinderQa::~CbmMuchHitFinderQa(){}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
InitStatus CbmMuchHitFinderQa::Init()
{
  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks    = (TClonesArray*) fManager->GetObject("MCTrack");
  fPoints      = (TClonesArray*) fManager->GetObject("MuchPoint");
  fHits        = (TClonesArray*) fManager->GetObject("MuchHit");
  fDigis       = (TClonesArray*) fManager->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) fManager->GetObject("MuchDigiMatch");
  fClusters    = (TClonesArray*) fManager->GetObject("MuchCluster");

  TFile* f = new TFile(fGeoFileName,"R");
  TObjArray* stations = (TObjArray*) f->Get("stations");
  fGeoScheme->Init(stations);
  fGeoScheme->InitGrid();
  fNstations = fGeoScheme->GetNStations();
  printf("Init: fNstations = %i\n", fNstations);

  fNall       = new Int_t[fNstations];
  fNpr        = new Int_t[fNstations];
  fNpi        = new Int_t[fNstations];
  fNel        = new Int_t[fNstations];
  fNmu        = new Int_t[fNstations];
  fNka        = new Int_t[fNstations];
  fNprimary   = new Int_t[fNstations];
  fNsecondary = new Int_t[fNstations];

//  Int_t nChannelsTotal = 0;
//  for (Int_t i=0;i<fNstations;i++){
//    CbmMuchStation* station = fDigiScheme->GetStation(i);
//    Int_t nChannels=station->GetNChannels();
//    nChannelsTotal+=nChannels;
//    printf("Station:%i channels:%i\n",i+1,nChannels);
//  }
//  printf("Total channels:%i\n",nChannelsTotal);

  // Reset counters
  for (Int_t i=0;i<fNstations;i++){
    fNall[i] = 0;
    fNpr[i] = 0;
    fNpi[i] = 0;
    fNel[i] = 0;
    fNmu[i] = 0;
    fNka[i] = 0;
    fNprimary[i] = 0;
    fNsecondary[i] = 0;
  }

  fPointsTotal=0;
  fPointsUnderCounted=0;
  fPointsOverCounted=0;

  #define BINNING_CHARGE        100,    0,   3.0
  #define BINNING_LENGTH        100,    0,   2.5
  #define BINNING_CHARGE_LOG    100,    4,     8
  #define BINNING_ENERGY_LOG    100, -0.5,   4.5
  #define BINNING_ENERGY_LOG_EL 100, -0.5,   4.5

  fhCharge              = new TH1D("hCharge",
                                   "Charge distribution from tracks",
                                   BINNING_CHARGE);

  fhChargeLog           = new TH1D("hChargeLog",
                                   "Charge (log.) distribution from tracks",
                                   BINNING_CHARGE_LOG);

  fhChargeEnergyLog     = new TH2D("hChargeEnergyLog"    ,
                                   "Charge vs energy (log.) for all tracks",
                                   BINNING_ENERGY_LOG,BINNING_CHARGE);

  fhChargeEnergyLogPi   = new TH2D("hChargeEnergyLogPi",
                                   "Charge vs energy (log.) for pions",
                                   BINNING_ENERGY_LOG,BINNING_CHARGE);

  fhChargeEnergyLogPr   = new TH2D("hChargeEnergyLogPr",
                                   "Charge vs energy (log.) for protons",
                                   BINNING_ENERGY_LOG,BINNING_CHARGE);

  fhChargeEnergyLogEl   = new TH2D("hChargeEnergyLogEl",
                                   "Charge vs energy (log.) for electrons",
                                   BINNING_ENERGY_LOG_EL,BINNING_CHARGE);

  fhChargeTrackLength   = new TH2D("hChargeTrackLength",
                                   "Charge vs length for all tracks",
                                   BINNING_LENGTH,BINNING_CHARGE);

  fhChargeTrackLengthPi = new TH2D("hChargeTrackLengthPi",
                                   "Charge vs length for pions",
                                   BINNING_LENGTH,BINNING_CHARGE);
  fhChargeTrackLengthPr = new TH2D("hChargeTrackLengthPr",
                                   "Charge vs length for proton",
                                   BINNING_LENGTH,BINNING_CHARGE);

  fhChargeTrackLengthEl = new TH2D("hChargeTrackLengthEl",
                                   "Charge vs length for electrons",
                                   BINNING_LENGTH,BINNING_CHARGE);

  fhChargePr_1GeV_3mm = new TH1D("hChargePr_1GeV_3mm",
                                 "Charge for 1 GeV protons",
                                 BINNING_CHARGE);

  fhPointsInCluster = new TH1I("hPointsInCluster",
                               "Number of tracks in cluster",
                                10,0,10);

  fhDigisInCluster  = new TH1I("hDigisInCluster",
                               "Number of digis in cluster",
                               10,0,10);

  fhHitsInCluster   = new TH1I("hHitsInCluster",
                               "Number of hits in cluster",
                               10,0,10);

  fhCharge   ->GetXaxis()->SetTitle("Charge [10^{6} electrons]");
  fhChargeLog->GetXaxis()->SetTitle("Lg (Charge) [Number of electrons]");
  fhChargePr_1GeV_3mm->GetXaxis()->SetTitle("Charge [10^{6} electrons]");


  fChargeHistos = new TObjArray();
  fChargeHistos->Add(fhChargeEnergyLog    );
  fChargeHistos->Add(fhChargeEnergyLogEl  );
  fChargeHistos->Add(fhChargeEnergyLogPi  );
  fChargeHistos->Add(fhChargeEnergyLogPr  );
  fChargeHistos->Add(fhChargeTrackLength  );
  fChargeHistos->Add(fhChargeTrackLengthEl);
  fChargeHistos->Add(fhChargeTrackLengthPi);
  fChargeHistos->Add(fhChargeTrackLengthPr);
  fChargeHistos->Add(fhCharge);
  fChargeHistos->Add(fhChargeLog);

  for (Int_t i=0;i<8;i++){
    TH2D* histo = (TH2D*) fChargeHistos->At(i);
    histo->SetStats(0);
    histo->GetYaxis()->SetDecimals(kTRUE);
    histo->GetYaxis()->SetTitleOffset(1.4);
    histo->GetYaxis()->CenterTitle();
    histo->GetYaxis()->SetTitle("Charge [10^{6} electrons]");
    if (i<4) histo->GetXaxis()->SetTitle("Lg E_{kin} [MeV]");
    else     histo->GetXaxis()->SetTitle("Track length [cm]");
  }

  fhOccupancyR = new TH1D*[fNstations];
  fhPadsTotalR = new TH1D*[fNstations];
  fhPadsFiredR = new TH1D*[fNstations];

  for (Int_t i=0; i<fNstations; i++){
    CbmMuchStation* station = fGeoScheme->GetStation(i);
    Double_t rMax = station->GetRmax();
    fhPadsTotalR[i] = new TH1D(Form("hPadsTotal%i",i+1),Form("Number of  pads vs radius: station %i;Radius [cm]",i+1),100,0,1.2*rMax);
    fhPadsFiredR[i] = new TH1D(Form("hPadsFired%i",i+1),Form("Number of fired pads vs radius: station %i;Radius [cm]",i+1),100,0,1.2*rMax);
    fhOccupancyR[i] = new TH1D(Form("hOccupancy%i",i+1),Form("Occupancy vs radius: station %i;Radius [cm];Occupancy",i+1),100,0,1.2*rMax);
  }
  vector<CbmMuchPad*> pads = fGeoScheme->GetPads();
  for (Int_t p=0;p<pads.size();p++){
    CbmMuchPad* pad = pads[p];
    Int_t stationId = fGeoScheme->GetStationIndex(pad->GetDetectorId());
    Double_t x0 = pad->GetX0();
    Double_t y0 = pad->GetY0();
    Double_t r0 = TMath::Sqrt(x0*x0+y0*y0);
    fhPadsTotalR[stationId]->Fill(r0);
  } // pads

  Double_t xmax = fGeoScheme->GetStation(0)->GetSigmaXmax();
  Double_t xmin = fGeoScheme->GetStation(0)->GetSigmaXmin();
  Double_t ymax = fGeoScheme->GetStation(0)->GetSigmaYmax();
  Double_t ymin = fGeoScheme->GetStation(0)->GetSigmaYmin();
  fnPadSizesX = Int_t(TMath::Log2(xmax/xmin));
  fnPadSizesY = Int_t(TMath::Log2(ymax/ymin));
  fhPullXpads1 = new TH1D*[fnPadSizesX];
  fhPullYpads1 = new TH1D*[fnPadSizesY];
  fhPullXpads2 = new TH1D*[fnPadSizesX];
  fhPullYpads2 = new TH1D*[fnPadSizesY];
  fhPullXpads3 = new TH1D*[fnPadSizesX];
  fhPullYpads3 = new TH1D*[fnPadSizesY];

  for (Int_t i=0;i<fnPadSizesX;i++){
    fhPullXpads1[i] = new TH1D(Form("hPullXpads1%i",i),Form("Pull distribution X. Npads = 1 Size =%i; (x_{RC} - x_{MC}) / dx_{RC}",i),100,-5,5);
    fhPullXpads2[i] = new TH1D(Form("hPullXpads2%i",i),Form("Pull distribution X. Npads = 2 Size =%i; (x_{RC} - x_{MC}) / dx_{RC}",i),100,-5,5);
    fhPullXpads3[i] = new TH1D(Form("hPullXpads3%i",i),Form("Pull distribution X. Npads = 3 Size =%i; (x_{RC} - x_{MC}) / dx_{RC}",i),100,-5,5);
  }

  for (Int_t i=0;i<fnPadSizesY;i++){
    fhPullYpads1[i] = new TH1D(Form("hPullYpads1%i",i),Form("Pull distribution Y. Npads = 1 Size =%i; (y_{RC} - y_{MC}) / dy_{RC}",i),100,-5,5);
    fhPullYpads2[i] = new TH1D(Form("hPullYpads2%i",i),Form("Pull distribution Y. Npads = 2 Size =%i; (y_{RC} - y_{MC}) / dy_{RC}",i),100,-5,5);
    fhPullYpads3[i] = new TH1D(Form("hPullYpads3%i",i),Form("Pull distribution Y. Npads = 3 Size =%i; (y_{RC} - y_{MC}) / dy_{RC}",i),100,-5,5);
  }



  fhPullX = new TH1D("hPullX","Pull distribution X;(x_{RC} - x_{MC}) / dx_{RC}",100,-5,5);
  fhPullY = new TH1D("hPullY","Pull distribution Y;(y_{RC} - y_{MC}) / dy_{RC}",100,-5,5);

  fhNpadsVsS = new TH2D("hNpadsVsS","Number of fired pads vs pad area:area:n pads",10,-5,0,10,0.5,10.5);

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::SetParContainers() {
  // Get run and runtime database
  // FairRuntimeDb* db = FairRuntimeDb::instance();
  // if ( ! db ) Fatal("SetParContainers", "No runtime database");
  // Get MUCH geometry parameter container
  // fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------




// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::Exec(Option_t * option){
  fEvent++;
  Info("Exec",Form("Event:%i",fEvent));

  //PullsQa();
  //DigitizerQa();
  OccupancyQa();
  //StatisticsQa();
  //ClusterDeconvQa();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::FinishTask(){
  gStyle->SetPaperSize(20,20);

  Double_t errors[100];
  for (Int_t i=0;i<100;i++) errors[i]=0;

  for (Int_t i=0;i<fNstations;i++) {
    fhPadsTotalR[i]->Sumw2();
    fhPadsTotalR[i]->SetError(errors);
    fhPadsFiredR[i]->Sumw2();
    fhPadsFiredR[i]->Scale(1./fEvent);
    fhOccupancyR[i]->Divide(fhPadsFiredR[i],fhPadsTotalR[i]);
  }

  if (fVerbose>3) {
    TCanvas* c4 = new TCanvas("c4","Pulls",800,400);
    c4->Divide(2,1);
    c4->cd(1);
    fhPullX->Sumw2();
    fhPullX->Fit("gaus");
    fhPullX->GetFunction("gaus")->SetLineWidth(2);
    fhPullX->GetFunction("gaus")->SetLineColor(kBlue);
    fhPullX->Draw();
    gPad->Print(".gif");
    gPad->Print(".eps");
    c4->cd(2);
    fhPullY->Sumw2();
    fhPullY->Fit("gaus");
    fhPullY->GetFunction("gaus")->SetLineWidth(2);
    fhPullY->GetFunction("gaus")->SetLineColor(kBlue);
    fhPullY->Draw();
    gPad->Print(".gif");
    gPad->Print(".eps");
    c4->cd();
  }

  if (fVerbose>3) {
    TCanvas* c4x = new TCanvas("c4x","Pulls",fnPadSizesX*300,3*300);
    c4x->Divide(fnPadSizesX,3);
    for (Int_t i=0;i<fnPadSizesX;i++){
      c4x->cd(i+1);
      fhPullXpads1[i]->Sumw2();
      fhPullXpads1[i]->Fit("gaus");
      fhPullXpads1[i]->GetFunction("gaus")->SetLineWidth(2);
      fhPullXpads1[i]->GetFunction("gaus")->SetLineColor(kBlue);
      fhPullXpads1[i]->Draw();
      gPad->Print(".gif");
      gPad->Print(".eps");

      c4x->cd(fnPadSizesX+i+1);
      fhPullXpads2[i]->Sumw2();
      fhPullXpads2[i]->Fit("gaus");
      fhPullXpads2[i]->GetFunction("gaus")->SetLineWidth(2);
      fhPullXpads2[i]->GetFunction("gaus")->SetLineColor(kBlue);
      fhPullXpads2[i]->Draw();
      gPad->Print(".gif");
      gPad->Print(".eps");

      c4x->cd(2*fnPadSizesX+i+1);
      fhPullXpads3[i]->Sumw2();
      fhPullXpads3[i]->Fit("gaus");
      fhPullXpads3[i]->GetFunction("gaus")->SetLineWidth(2);
      fhPullXpads3[i]->GetFunction("gaus")->SetLineColor(kBlue);
      fhPullXpads3[i]->Draw();
      gPad->Print(".gif");
      gPad->Print(".eps");
    }
  }

  if (fVerbose>3) {
    TCanvas* c4y = new TCanvas("c4y","Pulls",fnPadSizesY*300,3*300);
    c4y->Divide(fnPadSizesY,3);
    for (Int_t i=0;i<fnPadSizesY;i++){
      c4y->cd(i+1);
      fhPullYpads1[i]->Sumw2();
      fhPullYpads1[i]->Fit("gaus");
      fhPullYpads1[i]->GetFunction("gaus")->SetLineWidth(2);
      fhPullYpads1[i]->GetFunction("gaus")->SetLineColor(kBlue);
      fhPullYpads1[i]->Draw();
      gPad->Print(".gif");
      gPad->Print(".eps");

      c4y->cd(fnPadSizesY+i+1);
      fhPullYpads2[i]->Sumw2();
      fhPullYpads2[i]->Fit("gaus");
      fhPullYpads2[i]->GetFunction("gaus")->SetLineWidth(2);
      fhPullYpads2[i]->GetFunction("gaus")->SetLineColor(kBlue);
      fhPullYpads2[i]->Draw();
      gPad->Print(".gif");
      gPad->Print(".eps");

      c4y->cd(2*fnPadSizesY+i+1);
      fhPullYpads3[i]->Sumw2();
      fhPullYpads3[i]->Fit("gaus");
      fhPullYpads3[i]->GetFunction("gaus")->SetLineWidth(2);
      fhPullYpads3[i]->GetFunction("gaus")->SetLineColor(kBlue);
      fhPullYpads3[i]->Draw();
      gPad->Print(".gif");
      gPad->Print(".eps");
    }
  }

  if (1) {
    TCanvas* c3 = new TCanvas("c3","Occupancy plots",1200,800);
    c3->Divide(3,2);
    for (Int_t i=0;i<fNstations;i++) {
      c3->cd(i+1);
      fhOccupancyR[i]->SetStats(0);
      fhOccupancyR[i]->Draw();
      gPad->Print(".gif");
      gPad->Print(".eps");
    }
    c3->cd();
  }

  if (fVerbose>3) {

    TCanvas* c1 = new TCanvas("c1","All pad distributions",1200,800);
    c1->Divide(3,2);
    for (Int_t i=0;i<fNstations;i++) {
      c1->cd(i+1);
      fhPadsTotalR[i]->SetStats(0);
      fhPadsTotalR[i]->Draw("hist");
      gPad->Print(".gif");
      gPad->Print(".eps");
    }
    c1->cd();

    TCanvas* c2 = new TCanvas("c2","Fired pad distributions",1200,800);
    c2->Divide(3,2);
    for (Int_t i=0;i<fNstations;i++) {
      c2->cd(i+1);
      fhPadsFiredR[i]->SetStats(0);
      fhPadsFiredR[i]->Draw();
      gPad->Print(".gif");
      gPad->Print(".eps");
    }
    c2->cd();

    TF1* fit_el = new TF1("fit_el",LandauMPV,-0.5,4.5,1);
    fit_el->SetParameter(0,0.511);
    fit_el->SetLineWidth(2);

    TF1* fit_pi = new TF1("fit_pi",LandauMPV,-0.5,4.5,1);
    fit_pi->SetParameter(0,139.57);
    fit_pi->SetLineWidth(2);

    TF1* fit_pr = new TF1("fit_pr",LandauMPV,-0.5,4.5,1);
    fit_pr->SetParameter(0,938.27);
    fit_pr->SetLineWidth(2);


    TH1D* hLength   = fhChargeTrackLength  ->ProjectionX();
    TH1D* hLengthEl = fhChargeTrackLengthEl->ProjectionX();;
    TH1D* hLengthPi = fhChargeTrackLengthPi->ProjectionX();;
    TH1D* hLengthPr = fhChargeTrackLengthPr->ProjectionX();;
    hLength  ->SetTitle("Track length for all tracks");
    hLengthEl->SetTitle("Track length for electrons");
    hLengthPi->SetTitle("Track length for pions");
    hLengthPr->SetTitle("Track length for protons");

    fChargeHistos->Add(hLength);
    fChargeHistos->Add(hLengthEl);
    fChargeHistos->Add(hLengthPi);
    fChargeHistos->Add(hLengthPr);

    TCanvas* c5 = new TCanvas("c5","Charge vs energy and length",1600,1200);
    c5->Divide(4,3);
    for (Int_t i=0;i<8;i++){
      c5->cd(i+1);
      gPad->Range(0,0,200,200);
      gPad->SetBottomMargin(0.11);
      gPad->SetRightMargin(0.14);
      gPad->SetLeftMargin(0.12);
      gPad->SetLogz();
      TH2D* histo = (TH2D*) fChargeHistos->At(i);
      histo->Draw("colz");
      if (i==1) fit_el->Draw("same");
      if (i==2) fit_pi->Draw("same");
      if (i==3) fit_pr->Draw("same");
      gPad->Print(".gif");
      gPad->Print(".eps");
    }

    for (Int_t i=10;i<14;i++){
      c5->cd(i-1);
      gPad->SetLogy();
      gStyle->SetOptStat(1110);
      TH1D* histo = (TH1D*) fChargeHistos->At(i);
      histo->Draw();
      gPad->Print(".gif");
      gPad->Print(".eps");
    }


    TCanvas* c6 = new TCanvas("c6","Charge distribution",1200,400);
    c6->Divide(3,1);
    c6->cd(1);
    fhCharge->Draw();
    gPad->Print(".gif");
    gPad->Print(".eps");

    c6->cd(2);
    fhChargeLog->Draw();
    gPad->Print(".gif");
    gPad->Print(".eps");

    c6->cd(3);
    fhChargePr_1GeV_3mm->Draw();
    gPad->Print(".gif");
    gPad->Print(".eps");

    TCanvas* c7 = new TCanvas("c7","Cluster statistics",1200,400);
    c7->Divide(3,1);
    c7->cd(1);
    gStyle->SetOptStat(1110);
    gPad->SetLogy();
    fhPointsInCluster->Draw();
    gPad->Print(".gif");
    gPad->Print(".eps");
    c7->cd(2);
    gStyle->SetOptStat(1110);
    gPad->SetLogy();
    fhDigisInCluster->Draw();
    gPad->Print(".gif");
    gPad->Print(".eps");
    c7->cd(3);
    gStyle->SetOptStat(1110);
    gPad->SetLogy();
    fhHitsInCluster->Draw();
    gPad->Print(".gif");
    gPad->Print(".eps");


    TCanvas* c8 = new TCanvas("c8","Square vs nPads",800,400);
    c8->Divide(2,1);
    c8->cd(1);
    fhNpadsVsS->Draw("colz");
    Double_t nMean[11];
    Double_t s[11];
    for (Int_t iS=1;iS<=10;iS++){
      nMean[iS]=0;
      s[iS]=-5.25+0.5*iS;
      Double_t total=0;
      for (Int_t iN=1;iN<=10;iN++){
        nMean[iS]+=iN*fhNpadsVsS->GetBinContent(iS,iN);
        total+=fhNpadsVsS->GetBinContent(iS,iN);
      }
      if (total>0) nMean[iS]/=total;
      //printf("%f %f\n",s[iS],nMean[iS]);
    }
    c8->cd(2);
    TGraph* gNvsS = new TGraph(11,s,nMean);
    //gNvsS->DrawClone();

  }


  TFile* performanceFile = new TFile(fFileName, "recreate");

   for (Int_t i=0;i<fNstations;i++) {
     fhPadsTotalR[i]->Write();
     fhPadsFiredR[i]->Write();
     fhOccupancyR[i]->Write();
   }

   fhPullX->Write();
   fhPullY->Write();

   fhPointsInCluster->Write();
   fhDigisInCluster->Write();
   fhHitsInCluster->Write();

   for (Int_t i=0;i<10;i++) fChargeHistos->At(i)->Write();

   fhNpadsVsS->Write();
   performanceFile->Close();

   printf("All tracks: ;");
   for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNall[i]);
   printf("\n");
   printf("------------;");
   for (Int_t i=0;i<fNstations;i++) printf("---------");
   printf("\n");
   printf("Primary:    ;");
   for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNprimary[i]);
   printf("\n");
   printf("Secondary:  ;");
   for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNsecondary[i]);
   printf("\n");
   printf("-------------");
   for (Int_t i=0;i<fNstations;i++) printf("---------");
   printf("\n");
   printf("Protons:    ;");
   for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNpr[i]);
   printf("\n");
   printf("Pions:      ;");
   for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNpi[i]);
   printf("\n");
   printf("Electrons:  ;");
   for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNel[i]);
   printf("\n");
   printf("Muons:      ;");
   for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNmu[i]);
   printf("\n");
   printf("Kaons:      ;");
   for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNka[i]);
   printf("\n");

   printf("Total number of points: %i\n",fPointsTotal);
   printf("Points overcounted: %i\n",fPointsOverCounted);
   printf("Points undercounted: %i\n",fPointsUnderCounted);

   printf("Signal points: %i\n", fSignalPoints);
   printf("Signal hits: %i\n", fSignalHits);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t LandauMPV(Double_t *lg_x, Double_t *par) {
  Double_t gaz_gain_mean=1.e+4;
  Double_t scale=1.e+6;
  gaz_gain_mean/=scale;
  Double_t mass = par[0]; // mass in MeV
  Double_t x = TMath::Power(10,lg_x[0]);
  return gaz_gain_mean*CbmMuchDigitize::MPV_n_e(x,mass);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::DigitizerQa(){
  Bool_t verbose = (fVerbose>2);
  TVector3 vIn;   // in  position of the track
  TVector3 vOut;  // out position of the track
  TVector3 p;     // track momentum

  fPointInfos->Clear();

  for (Int_t i=0;i<fPoints->GetEntriesFast();i++){
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(i);
    Int_t stId = fGeoScheme->GetStationIndex(point->GetDetectorID());

    // Check if the point corresponds to a certain  MC Track
    Int_t trackId = point->GetTrackID();
    if (trackId < 0) {
        new ((*fPointInfos)[i]) CbmMuchPointInfo(0,0,0,0,0);
        continue;
    }
    CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(trackId);
    if (!mcTrack) {
        new ((*fPointInfos)[i]) CbmMuchPointInfo(0,0,0,0,0);
        continue;
    }

    Int_t motherId = mcTrack->GetMotherId();

    // Get mass
    Int_t pdgCode = mcTrack->GetPdgCode();
    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
    if(!particle ||
       pdgCode == 22 ||  // photons
       pdgCode == 2112)  // neutrons
    {
       new ((*fPointInfos)[i]) CbmMuchPointInfo(0,0,0,0,0);
       continue;
    }

    if (fGeoScheme->GetLayerIndex(point->GetDetectorID())==0){
      fNall[stId]++;

      if      (pdgCode==2212)                 fNpr[stId]++;
      else if (pdgCode==-211 || pdgCode==211) fNpi[stId]++;
      else if (pdgCode==-11  || pdgCode==11 ) fNel[stId]++;
      else if (pdgCode==-13  || pdgCode==13 ) fNmu[stId]++;
      else if (pdgCode==-321 || pdgCode==321) fNka[stId]++;

      if (motherId==-1) fNprimary[stId]++;
      else              fNsecondary[stId]++;
    }

    Double_t mass = particle->Mass();

    point->PositionIn(vIn);
    point->PositionOut(vOut);
    point->Momentum(p);
    Double_t length = (vOut-vIn).Mag();              // Track length
    Double_t kine = sqrt(p.Mag2()+mass*mass) - mass; // Kinetic energy
    // Get mother pdg code
    Int_t motherPdg = 0;
    CbmMCTrack* motherTrack = NULL;
    if(motherId != -1) motherTrack = (CbmMCTrack*)fMCTracks->At(motherId);
    if(motherTrack) motherPdg = motherTrack->GetPdgCode();
    new ((*fPointInfos)[i]) CbmMuchPointInfo(pdgCode,motherPdg,kine,length,stId);
  }


  // Filling generated charge for each point
  for (Int_t i=0;i<fDigiMatches->GetEntriesFast();i++){
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(i);
    // Get pad area
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(i);
    CbmMuchPad* pad = fGeoScheme->GetPadByDetId(digi->GetDetectorId());
    Double_t area = pad->GetLx()*pad->GetLy();
    for (Int_t pt=0;pt<match->GetNPoints();pt++){
      Int_t pointId = match->GetRefIndex(pt);
      Int_t charge  = match->GetCharge(pt);
      CbmMuchPointInfo* info = (CbmMuchPointInfo*) fPointInfos->At(pointId);
      if (info->GetPdgCode()==0) continue;
      info->AddCharge(charge);
      info->AddArea(area);
    }
  }

  //Filling digitizer performance plots
  for (Int_t i=0;i<fPointInfos->GetEntriesFast();i++){
    CbmMuchPointInfo* info = (CbmMuchPointInfo*) fPointInfos->At(i);
    if (verbose) {printf("%i",i); info->Print();}
    Double_t length     = info->GetLength();
    Double_t kine       = 1000*(info->GetKine());
    Double_t charge     = info->GetCharge();
    Int_t    pdg        = info->GetPdgCode();
    if (pdg==0) continue;
    if (charge<=0) continue;
    Double_t log_kine   = TMath::Log10(kine);
    Double_t log_charge = TMath::Log10(charge);
    charge = charge/1.e+6;  // measure charge in 10^6 electrons

    Int_t    nPads = info->GetNPads();
    Double_t area  = info->GetArea()/nPads;
    //printf("%f %i\n",TMath::Log2(area),nPads);
    fhNpadsVsS->Fill(TMath::Log2(area),nPads);
    fhCharge->Fill(charge);
    fhChargeLog->Fill(log_charge);
    fhChargeEnergyLog->Fill(log_kine,charge);
    fhChargeTrackLength->Fill(length,charge);

    if (pdg == 2212)                   fhChargeEnergyLogPr->Fill(log_kine,charge);
    else if (pdg == 211 || pdg ==-211) fhChargeEnergyLogPi->Fill(log_kine,charge);
    else if (pdg == 11  || pdg ==-11 ) fhChargeEnergyLogEl->Fill(log_kine,charge);

    if (pdg == 2212)                   fhChargeTrackLengthPr->Fill(length,charge);
    else if (pdg == 211 || pdg ==-211) fhChargeTrackLengthPi->Fill(length,charge);
    else if (pdg == 11  || pdg ==-11 ) fhChargeTrackLengthEl->Fill(length,charge);

    if (pdg == 2212 && length>0.3 && length<0.32 && kine>1000 && kine < 1020)
      fhChargePr_1GeV_3mm->Fill(charge);
  }

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::OccupancyQa(){
  Bool_t verbose = (fVerbose>2);
  // Filling occupancy plots
  for (Int_t i=0;i<fDigis->GetEntriesFast();i++){
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(i);
    Long64_t detId = digi->GetDetectorId();
    CbmMuchPad* pad = fGeoScheme->GetPadByDetId(detId);
    Double_t x0 = pad->GetX0();
    Double_t y0 = pad->GetY0();
    Double_t r0 = TMath::Sqrt(x0*x0+y0*y0);
    fhPadsFiredR[fGeoScheme->GetStationIndex(detId)]->Fill(r0);
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::StatisticsQa(){
  Bool_t verbose = (fVerbose>2);
  Int_t nClusters = fClusters->GetEntriesFast();
  TArrayI hitsInCluster;
  TArrayI pointsInCluster;
  hitsInCluster.Set(nClusters);
  pointsInCluster.Set(nClusters);

  for (Int_t i=0;i<nClusters;i++) hitsInCluster[i]=0;
  for (Int_t i=0;i<nClusters;i++) pointsInCluster[i]=0;

  for (Int_t i=0;i<fHits->GetEntriesFast();i++){
    CbmMuchHit* hit = (CbmMuchHit*) fHits->At(i);
    Int_t clusterId = hit->GetCluster();
    hitsInCluster[clusterId]++;
  }

  for (Int_t i=0;i<nClusters;i++){
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(i);
    map<Int_t,Int_t> map_points;
    Int_t nDigis = cluster->GetNDigis();
    fhDigisInCluster->Fill(nDigis);
    for(Int_t digiId=0;digiId<nDigis;digiId++){
      Int_t index = cluster->GetDigiIndex(digiId);
      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(index);
      Int_t nPoints = match->GetNPoints();
      for (Int_t iRefPoint=0;iRefPoint<nPoints;iRefPoint++){
        Int_t pointId = match->GetRefIndex(iRefPoint);
        map_points[pointId]=1;
      }
    }
    pointsInCluster[i]=map_points.size();
    map_points.clear();
  }


  for (Int_t i=0;i<nClusters;i++){
    Int_t nPts  = pointsInCluster[i];
    Int_t nHits = hitsInCluster[i];
    fhPointsInCluster->Fill(nPts);
    fhHitsInCluster->Fill(nHits);
    if (nPts>nHits) fPointsUnderCounted+=(nPts-nHits);
    if (nHits>nPts) fPointsOverCounted+=(nHits-nPts);
    fPointsTotal+=nPts;
  }

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::PullsQa(){
  Bool_t verbose = (fVerbose>2);
  vector<CbmMuchPad*> pads = fGeoScheme->GetPads();
  // Filling residuals and pools for hits at the first layer
  for (Int_t i=0;i<fHits->GetEntriesFast();i++){
    CbmMuchHit* hit = (CbmMuchHit*) fHits->At(i);
    // Select hits from the second station only
    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(hit->GetDetectorId());
    Int_t iLayer   = CbmMuchGeoScheme::GetLayerIndex(hit->GetDetectorId());
    //if(!(iStation == 3 && iLayer == 0)) continue;
    if (verbose) printf("   Hit %i, station %i, layer %i ",i,iStation, iLayer);
    // Select hits which are unique in the corresponding cluster
    Bool_t hit_unique=1;
    Int_t clusterId = hit->GetCluster();
    for (Int_t j=i+1;j<fHits->GetEntriesFast();j++){
      CbmMuchHit* hit1 = (CbmMuchHit*) fHits->At(j);
      //if (hit1->GetStationNr()>stationNr) break;
      if (hit1->GetCluster()==clusterId) { hit_unique=0; break;}
    }
    if (verbose) printf("hit_unique=%i",hit_unique);
    if (!hit_unique) {if (verbose) printf("\n"); continue;}

    // Select hits with clusters formed by only one MC Point
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(clusterId);
    Bool_t point_unique=1;
    Int_t  pointId=-1;

    Double_t xmax = 0;
    Double_t xmin = 0;
    Double_t ymax = 0;
    Double_t ymin = 0;
    Double_t dxmin = 0;
    Double_t dymin = 0;

//    if (cluster->GetNDigis()>1) {if (verbose) printf("\n"); continue;}
    for(Int_t digiId=0;digiId<cluster->GetNDigis();digiId++){
      Int_t index = cluster->GetDigiIndex(digiId);
      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(index);
      // Not unique if the pad has several mcPoint references
      if (verbose) printf(" n=%i",match->GetNPoints());
      if (match->GetNPoints()>1) { point_unique=0; break; }
      Int_t currentPointId = match->GetRefIndex(0);
      CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(index);
      CbmMuchPad* pad = fGeoScheme->GetPadByDetId(digi->GetDetectorId());
      Double_t x = pad->GetX0();
      Double_t y = pad->GetY0();
      Double_t dx = pad->GetLx();
      Double_t dy = pad->GetLy();
      if (digiId==0 || dxmin>dx   ) dxmin=dx;
      if (digiId==0 || dymin>dy   ) dymin=dy;
      if (digiId==0 || xmin>x-dx/2) xmin=x-dx/2;
      if (digiId==0 || xmax<x+dx/2) xmax=x+dx/2;
      if (digiId==0 || ymin>y-dy/2) ymin=y-dy/2;
      if (digiId==0 || ymax<y+dy/2) ymax=y+dy/2;
      if (digiId==0) { pointId=currentPointId; continue; }
      // Not unique if mcPoint references differ for diferent digis
      if (currentPointId!=pointId) {point_unique=0; break; }
    }
    if (verbose) printf(" point_unique=%i",point_unique);
    if (!point_unique) {if (verbose) printf("\n"); continue;}
    //printf(" %f %f %f %f %f %f\n",xmin,xmax,ymin,ymax,dxmin,dymin);
    Int_t nPadsX=Int_t((xmax-xmin)/dxmin);
    Int_t nPadsY=Int_t((ymax-ymin)/dymin);
    //printf("nPadsX=%i nPadsY=%i\n",nPadsX,nPadsY);

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
    fhPullX->Fill((xRC-xMC)/dx);
    fhPullY->Fill((yRC-yMC)/dy);
    if (verbose) printf("\n");

    Int_t index = cluster->GetDigiIndex(0);
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(index);
    CbmMuchStation* station = fGeoScheme->GetStationByDetId(digi->GetDetectorId());
    Double_t pad_xmin = TMath::Sqrt(12)*station->GetSigmaXmin();
    Double_t pad_ymin = TMath::Sqrt(12)*station->GetSigmaYmin();
    Int_t padSizeX = Int_t(TMath::Log2(dxmin/pad_xmin));
    Int_t padSizeY = Int_t(TMath::Log2(dymin/pad_ymin));
    if (padSizeX>=fnPadSizesX || padSizeX<0) { printf("wrong x pad size\n"); continue; }
    if (padSizeY>=fnPadSizesY || padSizeY<0) { printf("wrong y pad size\n"); continue; }
    if (nPadsX==1 && nPadsY==1) fhPullXpads1[padSizeX]->Fill((xRC-xMC)/dx);
    if (nPadsY==1 && nPadsX==1) fhPullYpads1[padSizeY]->Fill((yRC-yMC)/dy);
    if (nPadsX==2 && nPadsY==1) fhPullXpads2[padSizeX]->Fill((xRC-xMC)/dx);
    if (nPadsY==2 && nPadsX==1) fhPullYpads2[padSizeY]->Fill((yRC-yMC)/dy);
    if (nPadsX==3 && nPadsY==1) fhPullXpads3[padSizeX]->Fill((xRC-xMC)/dx);
    if (nPadsY==3 && nPadsX==1) fhPullYpads3[padSizeY]->Fill((yRC-yMC)/dy);

  }

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::ClusterDeconvQa(){
  Int_t nPoints = fPoints->GetEntriesFast();
  Int_t nMatches = fDigiMatches->GetEntriesFast();
  Int_t nClusters = fClusters->GetEntriesFast();
  vector<Int_t> pIndices;
  vector<Int_t>::iterator it;

  for(Int_t iPoint = 0; iPoint < nPoints; ++iPoint){
    if(IsSignalPoint(iPoint)) fSignalPoints++;
  }

  for(Int_t iCluster = 0; iCluster < nClusters; ++iCluster){
    CbmMuchCluster* cluster = (CbmMuchCluster*)fClusters->At(iCluster);
    if(!cluster) continue;
    Int_t nDigis = cluster->GetNDigis();
    for(Int_t i = 0; i < nDigis; ++i){
      Int_t iDigi = cluster->GetDigiIndex(i);
      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(iDigi);
      if(!match) continue;
      for(Int_t i=0; i<match->GetNPoints();++i){
	Int_t iPoint = match->GetRefIndex(i);
	it = find(pIndices.begin(), pIndices.end(), iPoint);
	if(it != pIndices.end()) continue;
	pIndices.push_back(iPoint);
	if(IsSignalPoint(iPoint)) fSignalHits++;
      }
    }
  }
}
// -------------------------------------------------------------------------

Bool_t CbmMuchHitFinderQa::IsSignalPoint(Int_t iPoint){
  Int_t nPoints = fPoints->GetEntriesFast();
  Int_t nTracks = fMCTracks->GetEntriesFast();
  CbmMuchPoint* point = (iPoint<0 || iPoint>nPoints-1) ? NULL : (CbmMuchPoint*) fPoints->At(iPoint);
  if(!point) return kFALSE;
  Int_t iTrack = point->GetTrackID();
  CbmMCTrack* track = (iTrack<0 || iTrack>nTracks-1) ? NULL : (CbmMCTrack*)fMCTracks->At(iTrack);
  if(!track) return kFALSE;
  if(iTrack != 0 && iTrack != 1) return kFALSE; // Signal tracks must be fist ones
  // Verify if it is a signal muon
  if(track->GetMotherId() < 0){                 // No mother for signal
    Int_t pdgCode = track->GetPdgCode();
    if(TMath::Abs(pdgCode) == 13){              // If it is a muon
      return kTRUE;
    }
  }
  return kFALSE;
}


ClassImp(CbmMuchHitFinderQa)
