/** CbmMuchSegmentSector.cxx
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 05.02.12
 *@version 1.0
 *
 * class for making parameter file for MUCH digitizer
 * sector version
 */

#include "CbmMuchSegmentSector.h"
#include "CbmGeoMuchPar.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModuleSector.h"
#include "CbmMuchRadialSector.h"

#include "FairRuntimeDb.h"

#include "TFile.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TArc.h"
#include "TColor.h"
#include "TSystem.h"

#include <stdio.h>
#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchSegmentSector::CbmMuchSegmentSector(){
  fDigiFileName = (char*)"much.digi.root";
  fDebug = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchSegmentSector::CbmMuchSegmentSector(char* inputFileName, char* digiFileName){
  fInputFileName = inputFileName;
  fDigiFileName = digiFileName;
  fDebug = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor  ----------------------------------------------------
CbmMuchSegmentSector::~CbmMuchSegmentSector() {
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers  --------------------------------
void CbmMuchSegmentSector::SetParContainers() {
  // Get runtime database
  FairRuntimeDb* db = FairRuntimeDb::instance();
  if ( ! db ) Fatal("Init", "No runtime database");
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------

// -----   Private method Init ---------------------------------------------
InitStatus CbmMuchSegmentSector::Init(){
  printf("\n=============================  Inputs segmentation parameters  ================================\n");
  ReadInputFile();
  printf("\n===============================================================================================\n");

  // Get MUCH geometry parameter container
  fStations = fGeoPar->GetStations();
  if(!fStations) Fatal("Init", "No input array of MUCH stations.");
  if(fStations->GetEntries() != fNStations) Fatal("Init", "Incorrect number of stations.");

  if(fDebug){
    for(Int_t iStation = 0; iStation < fNStations; ++iStation){
      printf("Station %i\n", iStation+1);
      for(Int_t iRegion = 0; iRegion < fNRegions[iStation]; ++iRegion)
        printf("  Region %i: fAngles=%4.1f\n", iRegion+1, fAngles[iStation][iRegion]);
    }
  }

  // Segment MuCh
  SegmentMuch();
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method SegmentMuch  --------------------------------------
void CbmMuchSegmentSector::SegmentMuch(){
  for(Int_t iStation=0; iStation<fStations->GetEntries(); ++iStation){
    CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);

    Int_t nLayers = station->GetNLayers();
    for(Int_t iLayer = 0; iLayer < nLayers; ++iLayer){
      CbmMuchLayer* layer = station->GetLayer(iLayer);
      if(!layer) Fatal("SegmentMuch", "Incomplete layers array.");
      // Segment layer sides
      SegmentLayerSide(layer->GetSideF());
      SegmentLayerSide(layer->GetSideB());
    }
    printf("Station %i segmented\n",iStation+1);
  }

  // Save parameters
  TFile* oldfile = gFile;
  TFile* f = new TFile(fDigiFileName, "RECREATE");
  fStations->Write("stations",1);

  f->Close();
  gFile=oldfile;
  
  DrawSegmentation();
}
// -------------------------------------------------------------------------

// -----   Private method SegmentLayerSide  --------------------------------
void CbmMuchSegmentSector::SegmentLayerSide(CbmMuchLayerSide* layerSide){
  if(!layerSide) Fatal("SegmentLayerSide", "Incomplete layer sides array.");
  Int_t nModules = layerSide->GetNModules();
  for(Int_t iModule = 0; iModule < nModules; iModule++){
    CbmMuchModule* module = layerSide->GetModule(iModule);
    if(module->GetDetectorType()!=3) continue;
    CbmMuchModuleSector* mod = (CbmMuchModuleSector*)module;
    if(nModules > 1) SegmentModule(mod, true); // Module design
    else SegmentModule(mod, false);            // Monolithic design
  }
}
// -------------------------------------------------------------------------

// -----   Private method SegmentSector  -----------------------------------
void CbmMuchSegmentSector::SegmentModule(CbmMuchModuleSector* module, Bool_t useModuleDesign){
  Int_t detectorId = module->GetDetectorId();
  Int_t iStation = CbmMuchGeoScheme::GetStationIndex(detectorId);
  Int_t iModule  = CbmMuchGeoScheme::GetModuleIndex(detectorId);
  Int_t iLayer   = CbmMuchGeoScheme::GetLayerIndex(detectorId);
  Int_t iSide    = CbmMuchGeoScheme::GetLayerSideIndex(detectorId);
  CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);
  Double_t phi0 = module->GetPosition().Phi()/TMath::Pi()*180.;
  Double_t r0   = module->GetPosition().Perp();
  Double_t dx1  = module->GetDx1();
  Double_t dx2  = module->GetDx2();
  Double_t dy   = module->GetDy();
  Double_t rMin = sqrt((r0-dy)*(r0-dy)+dx1*dx1);
  Double_t rMax = r0+dy;

  Double_t t  = 2*dy/(dx2-dx1);
  Double_t b= (r0-dy)-t*dx1;
  Double_t b2 = b*b;
  Double_t bt = b*t;
  Double_t a  = t*t+1;
 
  Double_t r1 = rMin;
  Double_t r2 = rMin;
//  printf("Debug: %i %i %i %i\n",iStation,iLayer,iSide,iModule);
  Int_t iSector=0;
  for (Int_t i=0;i<fNRegions[iStation];i++){
    Double_t angle = fAngles[iStation][i];
    while (r2<fRadii[iStation][i] && r2<rMax){
      r2 = r1+r1*angle*TMath::Pi()/180.;
      Double_t phiMax = 180./TMath::Pi()*TMath::ASin((-bt+sqrt(a*r2*r2-b2))/a/r2); 
      Int_t nPads = 2*Int_t(phiMax/angle);
      if (iStation==0 && iLayer==0 && iSide==0 && iModule==0) printf("Sector: %f-%f %i phiMax=%f\n",r1,r2,nPads,phiMax);
      module->AddSector(new CbmMuchRadialSector(detectorId,iSector,r1,r2,phi0,angle,nPads));
      r1 = r2;
      iSector++;
    }
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchSegmentSector::ReadInputFile(){
  ifstream infile;
  infile.open(fInputFileName);
  if(!infile){
    Fatal("ReadInputFile","Error: Cannot open the input file.");
  }

  Int_t index;
  string str;
  vector<string> strs;

  // Set number of stations
  OmitDummyLines(infile, str);
  strs = Split(str, ' ');
  index = strs.size()-1;
  Int_t nStations;
  StrToNum(strs.at(index), nStations);
  fNStations=nStations;
  printf("Number of stations: \t%i", fNStations);

  // Set number of regions
  OmitDummyLines(infile, str);
  strs = Split(str, ' ');
  printf("\nNumber of regions: ");
  for(Int_t iStation = 0; iStation < fNStations; ++iStation){
    index = strs.size() - fNStations + iStation;
    Int_t nRegions;
    StrToNum(strs.at(index), nRegions);
    fNRegions[iStation]=nRegions;
    fRadii[iStation].resize(nRegions);
    fAngles[iStation].resize(nRegions);
    printf("\t%i", fNRegions[iStation]);
  }

  for(Int_t iStation = 0; iStation < fNStations; ++iStation){
    // Set region radii
    OmitDummyLines(infile, str);
    strs = Split(str, ' ');
    printf("\nStation %i", iStation + 1);
    printf("\n   Region radii [cm]: ");
    for(Int_t iRegion = 0; iRegion < fNRegions[iStation]; ++iRegion){
      index = strs.size() - fNRegions[iStation] + iRegion;
      StrToNum(strs.at(index),fRadii[iStation][iRegion]);
      printf("\t%4.2f",fRadii[iStation][iRegion]);
    }
    // Set pad angles
    OmitDummyLines(infile, str);
    strs = Split(str, ' ');
    printf("\n   Pad angles [deg]: ");
    for(Int_t iRegion = 0; iRegion < fNRegions[iStation]; ++iRegion){
      index = strs.size() - fNRegions[iStation] + iRegion;
      StrToNum(strs.at(index), fAngles[iStation][iRegion]);
      printf("\t%4.2f", fAngles[iStation][iRegion]);
    }
  }
  printf("\n");
  infile.close();
}

void CbmMuchSegmentSector::DrawSegmentation(){
  // Change file extension
  char txtfile[100];
  Int_t length = strlen(fDigiFileName);
  Int_t iChar;
  for(iChar = length-1; iChar >= 0; --iChar){
    if(fDigiFileName[iChar] == '.') break;
  }
  strncpy(txtfile, fDigiFileName, iChar+1);
  txtfile[iChar+1] = '\0';
  strcat(txtfile, "txt");

  FILE* outfile;
  outfile = fopen(txtfile, "w");
  Int_t colors[] = {kGreen, kMagenta, kCyan, kRed, kBlue, kYellow, kTeal,
                    kPink, kAzure, kOrange, kViolet, kSpring,
                    kGreen+2, kMagenta+2, kCyan+2, kRed+2, kBlue+2, kYellow+2, kTeal+2,
                    kPink+2, kAzure+2, kOrange+2, kViolet+2, kSpring+2,
                    kGreen+4, kMagenta+4, kCyan+4, kRed+4, kBlue+4, kYellow+4, kTeal+4,
                    kPink+4, kAzure+4, kOrange+4, kViolet+4, kSpring+4};
  for (Int_t iStation=0;iStation<fStations->GetEntriesFast();++iStation){
//  for (Int_t iStation=0;iStation<1;++iStation){
    fprintf(outfile, "===========================================================================\n");
    fprintf(outfile, "Station %i\n", iStation+1);
    fprintf(outfile, "Sector size, cm   Sector position, cm   Number of pads   Side   Pad size, cm\n");
    fprintf(outfile, "----------------------------------------------------------------------------\n");
    TCanvas* c1 = new TCanvas(Form("station%i",iStation+1),Form("station%i",iStation+1),1000,1000);
    c1->SetFillColor(0);
    c1->Range(-200,-200,200,200);
    CbmMuchStation* station = (CbmMuchStation*) fStations->At(iStation);
    CbmMuchLayer* layer = station->GetLayer(0);
    for (Int_t iSide=1;iSide>=0;iSide--){
      CbmMuchLayerSide* layerSide = layer->GetSide(iSide);
      for (Int_t iModule=0;iModule<layerSide->GetNModules();++iModule) {
        CbmMuchModule* mod = layerSide->GetModule(iModule);
        mod->SetFillStyle(0);
//        mod->Draw();
        if(mod->GetDetectorType() != 3) continue;
        CbmMuchModuleSector* module = (CbmMuchModuleSector*)mod;
        for (Int_t iSector=0;iSector<module->GetNSectors();++iSector){
          CbmMuchRadialSector* sector = module->GetSector(iSector);
//          sector->SetFillColor(iSide ? TColor::GetColorDark(colors[i+j]) : colors[i+j]);
//          sector->Draw("f");
//          sector->Draw();
          sector->AddPads();
          sector->DrawPads();
        } // sectors
      } // modules
    } // sides

    // Draw a hole
    TArc* holeArc = new TArc(0.,0.,station->GetRmin());
    holeArc->Draw();

    for(Int_t iRegion=0; iRegion < fNRegions[iStation]; ++iRegion){
      TArc* arc = new TArc(0.,0.,fRadii[iStation].at(iRegion));
      arc->SetLineColor(kBlack);
      arc->SetLineWidth(2);
      arc->SetFillStyle(0);
      arc->Draw();
    }

    c1->Print(Form("%s/station%i.eps",gSystem->DirName(fDigiFileName), iStation+1));
    c1->Print(Form("%s/station%i.png",gSystem->DirName(fDigiFileName), iStation+1));
  }//stations
  fclose(outfile);
}


//double x0 = -a*c/(a*a+b*b),  y0 = -b*c/(a*a+b*b);
//if (c*c > r*r*(a*a+b*b)+EPS)
//    puts ("no points");
//else {
//    double d = r*r - c*c/(a*a+b*b);
//    double mult = sqrt (d / (a*a+b*b));
//    double ax,ay,bx,by;
//    ax = x0 + b * mult;
//    bx = x0 - b * mult;
//    ay = y0 - a * mult;
//    by = y0 + a * mult;
//}

ClassImp(CbmMuchSegmentSector)




